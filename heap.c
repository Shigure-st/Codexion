#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct s_Data t_Data;
typedef struct s_HeapData t_HeapData;
typedef struct s_Queue t_Queue;

typedef struct s_Heap t_Heap;

struct s_HeapData
{
  int  data;
};

struct s_Heap
{
  t_HeapData  *data;
  int     size;
  int     capa;
};

void shift_up(t_Heap *queue)
{
  int parent;
  int curent;
  t_HeapData tmp;

  curent = queue->size;
  parent = (curent - 1) / 2;
  while (curent != 0 && queue->data[parent].data > queue->data[curent].data)
  {
    tmp = queue->data[parent];
    queue->data[parent] = queue->data[curent];
    queue->data[curent] = tmp;
    curent = parent;
    parent = (curent - 1) / 2;
  }
}

static int get_min_child(t_Heap *queue, int left, int right)
{
  if (right >= queue->size)
    return left;
  else
  {
    if (queue->data[left].data < queue->data[right].data)
      return left;
    else
      return right;
  }
}

void shift_down(t_Heap *queue)
{
  int parent;
  t_HeapData tmp;
  int child;

  parent = 0;
  while((parent * 2) + 1 < queue->size)
  {
    // left_child = (parent * 2) + 1;
    // right_child = (parent * 2) + 2;
    //
    // if (right_child >= queue->size)
    //   child = left_child;
    // else
    // {
    //   if (queue->data[left_child].data < queue->data[right_child].data)
    //     child = left_child;
    //   else
    //     child = right_child;
    // }
    child = get_min_child(queue, (parent * 2) + 1, (parent * 2) + 2);
    if (queue->data[parent].data > queue->data[child].data)
    {
      tmp = queue->data[parent];
      queue->data[parent] = queue->data[child];
      queue->data[child] = tmp;
      parent = child;
    }
    else
      break;
  }
}

int heap_pop(t_Heap *queue, t_HeapData *ret)
{
  if (queue->size == 0)
  {
    printf("queue is emptyh\n");
    return -1;
  }
  *ret = queue->data[0];
  queue->data[0] = queue->data[queue->size - 1];
  queue->size--;
  shift_down(queue);
  return 0;
}

void heap_push(t_Heap *queue, t_HeapData *push_data)
{
  queue->data[queue->size] = *push_data;
  shift_up(queue);
  queue->size++;
}


int main(void)
{
    t_Heap heap;
    t_HeapData input;
    t_HeapData ret;

    int values[] = {6, 5, 4, 3, 2, 1};
    int count = sizeof(values) / sizeof(values[0]);

    heap.capa = count;
    heap.size = 0;
    heap.data = malloc(sizeof(t_HeapData) * heap.capa);

    if (heap.data == NULL)
        return 1;

    /* push */
    for (int i = 0; i < count; i++)
    {
        input.data = values[i];
        heap_push(&heap, &input);
    }

    printf("Heap after push:\n");
    for (int i = 0; i < heap.size; i++)
        printf("%d ", heap.data[i].data);
    printf("\n");

    /* pop */
    printf("Pop order:\n");
    while (heap.size > 0)
    {
        if (heap_pop(&heap, &ret) == -1)
        {
            printf("pop failed\n");
            break;
        }

        printf("%d ", ret.data);
    }
    printf("\n");

    free(heap.data);

    return 0;
}









// struct s_Queue
// {
//   struct s_Data *arr;
//   int           tail;
//   int           head;
//   int        size;
// };
//
// struct s_Data
// {
//   int  data;
//   int     priority;
// };
//
// void  enqueue(t_Queue *queue, int  element) {
//   if(((queue->tail) + 2) % queue->size == queue->head)
//   {
//     printf("Queue is full so can't ENQUEUE\n");
//     return;
//
//   }
//   queue->arr[(queue->tail + 1) % queue->size].data = element;
//   queue->tail = (queue->tail + 1) % queue->size;
//   printf("[DEBUG]heap queue:%d\n", queue->arr[(queue->tail) % queue->size].data);
//   printf("[DEBUG]tail:%d\n", queue->tail);
// }
//
// int  dequeue(t_Queue *queue)
// {
//   int ret;
//
//   if((queue->tail + 1) % queue->size == queue->head)
//   {
//     printf("Queue is empty\n");
//     return -1;
//   }
//   ret = queue->arr[queue->head].data;
//   queue->head = (queue->head + 1) % queue->size;
//   return ret;
//
// }
//
// int main()
// {
//     struct s_Data dates[6] = {0};
//     // struct s_Data dates[5] = {
//     //     {1, 1},
//     //     {2, 2},
//     //     {3, 3},
//     //     {4, 4},
//     //     {5, 5}
//     // };
//
//     struct  s_Queue heap_queue;
//     heap_queue.arr = dates;
//     heap_queue.size = 6;
//     heap_queue.head = 0;
//     heap_queue.tail = -1;
//
//     enqueue(&heap_queue, 1);
//     enqueue(&heap_queue, 2);
//     enqueue(&heap_queue, 3);
//     enqueue(&heap_queue, 4);
//     enqueue(&heap_queue, 5);
//     printf("heap queue1:%d\n", heap_queue.arr[heap_queue.head % heap_queue.size].data);
//     printf("heap queue2:%d\n", heap_queue.arr[(heap_queue.head + 1) % heap_queue.size].data);
//     printf("heap queue3:%d\n", heap_queue.arr[(heap_queue.head + 2) % heap_queue.size].data);
//     printf("heap queue4:%d\n", heap_queue.arr[(heap_queue.head + 3) % heap_queue.size].data);
//     printf("heap queue5:%d\n", heap_queue.arr[(heap_queue.head + 4) % heap_queue.size].data);
//     printf("heap size:%d\n", heap_queue.size);
//
//     enqueue(&heap_queue, 10);
//     printf("queue5:%d\n", heap_queue.arr[4].data);
//     printf("DEQUEUE:%d\n", dequeue(&heap_queue));
//
// }
