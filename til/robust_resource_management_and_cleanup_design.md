# C言語における堅牢なリソース管理とメモリ設計の4つの極意

## 📌 概要 (TL;DR)

動的確保 (`malloc`) がネスト（多重化）された構造体や配列を扱う際、**「いかに安全に確保し、いかに漏れなくクラッシュせずに解放（ロールバック）するか」** はC言語設計の要である。

本メモは、`wait_coders` や `data` などの多層構造体の初期化・解体において直面した課題から得られた**「一生使える堅牢なメモリ管理の設計思考（マインドセット）」**をまとめたものである。

---

## 💥 直面した課題とアンチパターン (Before)

現状のコードにおける課題・潜在バグ：

```c
// ❌ アンチパターン: 条件分岐が複雑で、NULLアクセスによるクラッシュやリークが発生する
void free_queue(t_SharedContext *shared_ctx, int i)
{
  int j = 0;
  if (i == 0) // ← 危険！ wait_coders 自体が NULL だと NULL->data でクラッシュ
  {
    free(shared_ctx->dongles[i].wait_coders->data);
    shared_ctx->dongles[i].wait_coders->data = NULL;
  }
  while (j < i) // ← 危険！ i 番目自身の wait_coders がリークする
  {
    free(shared_ctx->dongles[j].wait_coders->data);
    ...
  }
}
```

### 発生していた問題
1. **クラッシュ (SIGSEGV)**: `i = 0` で `wait_coders` の `malloc` 自体が失敗した時、`NULL->data` にアクセスしてしまう。
2. **メモリリーク**: `data` の `malloc` だけが失敗した際、失敗したインデックス自身の親構造体 (`wait_coders`) が `free` されずに残る。
3. **複雑化**: `i == 0` と `j < i` の条件分岐によりコードが見づらく、エッジケースの考慮漏れが発生する。

---

## 🥷 堅牢な設計を創る「4つの極意」

### 極意①：解体関数は「不完全な状態」でも死なないように作れ（防御的設計）

解体関数を呼ぶ側（呼び出し元）が「今どこまで確保成功したか」を気にする設計は危険である。

* **教訓**: **「ポインタが `NULL` だろうが未完成だろうが、安全にスルーしてくれる解体関数を作る」**

```c
// ガード文を先頭に入れることで、壊れたオブジェクトを渡されても絶対クラッシュしない
static void free_dongle_heap(t_Dongle *dongle)
{
  if (dongle == NULL || dongle->wait_coders == NULL)
    return ; // NULL なら安全に何もしない
  if (dongle->wait_coders->data != NULL)
  {
    free(dongle->wait_coders->data);
    dongle->wait_coders->data = NULL;
  }
  free(dongle->wait_coders);
  dongle->wait_coders = NULL;
}
```

---

### 極意②：「1要素の処理」と「ループ処理」は絶対に分けよ（単一責任の原則）

1つの関数に「1つの要素を消す処理」と「配列を回す処理」と「例外分岐」を同居させない。

* **教訓**: 
  - `free_dongle_heap`: 「1つの要素だけを綺麗に消す」ことに専念
  - `free_heapqueue`: 「0 から count までループして上の関数を呼ぶ」ことに専念

これにより、`if (i == 0)` のような複雑な例外分岐が**完全消滅**し、42のNorm（25行制限）にも自然に収まる。

---

### 極意③：確保と解放は「完全な鏡合わせ（対称性）」にせよ

メモリの `malloc` と `free` はマトリョーシカの開け閉めと同じ（逆順）にする。

* **確保（外から内へ）**:
  1. 親 `dongles[i]`
  2. 子 `wait_coders`
  3. 孫 `data`
* **解放（内から外へ）**:
  1. 孫 `data`
  2. 子 `wait_coders`
  3. (親 `dongles[i]`)

この**線対称の構造**を徹底するだけで、二重解放 (Double Free) や解放漏れは構造的に100%防止できる。

---

### 極意④：正常系（ハッピーパス）のコードを汚すな

エラー処理（ロールバック）を `malloc` の直後にごちゃごちゃと長文で書かない。

```c
// エラーが起きたら自分で片付けず、極意①で作った掃除屋に「どこまでやったか」を渡して一任する
d->wait_coders = malloc(sizeof(t_Heap));
if (d->wait_coders == NULL)
{
  free_heapqueue(shared_ctx, i); // 掃除屋に一任！
  return (cleanup_context(shared_ctx));
}
```

確保関数の中身は「上から下に順番に確保していくだけ」の綺麗な一本道のコード（ハッピーパス）になる。

---

## 🛠️ 改善後の完成コード (After)

```c
#include "codexion.h"
#include <stdlib.h>

// 1. 【極意①】1つの dongle の wait_coders と data を安全に解放（NULLガード付き）
static void	free_dongle_heap(t_Dongle *dongle)
{
	if (dongle == NULL || dongle->wait_coders == NULL)
		return ;
	if (dongle->wait_coders->data != NULL)
	{
		free(dongle->wait_coders->data);
		dongle->wait_coders->data = NULL;
	}
	free(dongle->wait_coders);
	dongle->wait_coders = NULL;
}

// 2. 【極意②】0 から count 番目までの dongle の wait_coders を安全に一括解放
void	free_heapqueue(t_SharedContext *shared_ctx, int count)
{
	int	j;

	if (shared_ctx == NULL || shared_ctx->dongles == NULL)
		return ;
	j = 0;
	while (j <= count) // j <= count にすることで失敗した i 番目自身も安全に巻き込んで解放
	{
		free_dongle_heap(&shared_ctx->dongles[j]);
		j++;
	}
}

// 3. 【極意④】全ドングルのヒープキューを確保（ハッピーパスが綺麗）
int	alloc_heapqueue(t_SharedContext *shared_ctx)
{
	int			i;
	t_Dongle	*d;

	i = 0;
	while (i < shared_ctx->coder)
	{
		d = &shared_ctx->dongles[i];
		d->wait_coders = malloc(sizeof(t_Heap));
		if (d->wait_coders == NULL)
		{
			free_heapqueue(shared_ctx, i);
			return (cleanup_context(shared_ctx));
		}
		d->wait_coders->data = malloc(sizeof(t_HeapDate) * 2);
		if (d->wait_coders->data == NULL)
		{
			free_heapqueue(shared_ctx, i);
			return (cleanup_context(shared_ctx));
		}
		i++;
	}
	return (0);
}
```

---

## 📝 新しい構造体を設計する際の 3 ステップ・チェックリスト

今後、新しい構造体やネストされた動的確保を書く際は、以下の手順で組み立てる：

1. [ ] **ステップ 1**: まず「1 要素を安全に消す関数 (`free_xxx_single`)」を `NULL` ガード文付きで書く。
2. [ ] **ステップ 2**: 次に「それを配列分ループして消す関数 (`free_xxx_array`)」を書く。
3. [ ] **ステップ 3**: 最後に「確保する関数 (`alloc_xxx`)」を書いて、エラー時にはステップ 2 の関数を呼んで一発破棄させる。
