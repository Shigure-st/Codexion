<table>
	<thead>
    	<tr>
      		<th style="text-align:center">English</th>
      		<th style="text-align:center"><a href="README_ja.md">日本語</a></th>
    	</tr>
  	</thead>
</table>

*This project has been created as part of the 42 curriculum by tenomoto.*  

## 📖*Content*
1. [💡Description](#description)  
2. [✅Instructions](#instructions)  
3. [⛏Additional sections](#additional-sections)  
4. [🌈Resources](#resources)  

##  Description
Codexion is a high-performance concurrency and resource synchronization simulation written in C. It models a complex multi-threaded environment where multiple coders compete for a limited number of shared development resources (USB dongles) to compile quantum code without suffering from burnout.
Implemented POSIX threads, mutexes, condition variables, and advanced scheduling algorithms (FIFO/EDF) to control resource sharing, prevent deadlocks, and ensure fair access.

## Instructions
1. Compile the program to generate the executable.:

    `make`

2. Execute the program:
    
    `./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler`
    ### Arguments

   | Argument | Description | Example |
   | :--- | :--- | :--- |
   | `number_of_coders` | The number of coders and also the number of dongles | `5` |
   | `time_to_burnout` | if a coder did not start compiling within time_to_burnout milliseconds since the beginning of their last compile or the beginning of the simulation, they burn out | `1000` (ms) |
   | `time_to_compile` | The time it takes for a coder to compile. During that time, they must hold two dongles | `200` (ms) |
   | `time_to_debug` | The time a coder will spend debugging | `200` (ms) |
   | `time_to_refactor` | The time a coder will spend refactoring. After completing the refactoring phase, the coder will immediately attempt to acquire dongles and start compiling again | `200` (ms) |
   | `number_of_compiles_required` | If all coders have compiled at least this many times, the simulation stops. Otherwise, it stops when a coder burns out | `5` |
   | `dongle_cooldown` | After being released, a dongle is unavail-able until its cooldown has passed | `50` (ms) |
   | `scheduler` | The arbitration policy used by dongles to decide who gets them when multiple coders request them. The value must be exactly one of: fifo or edf. fifo means First In, First Out: the dongle is granted to the coder whose request arrived first. edf means Earliest Deadline First with deadline = `last_compile_start` + `time_to_burnout` (`fifo` or `efd`) | `fifo` |
   ### Example
   ```
   ./codexion 5 1000 200 200 200 5 50 fifo
   ```

### Additional Commands

* **Clean up generated object files:**
  ```
  make clean
  ```

* **Remove object files and executables:**  
  ```
  make fclean
  ```

* **Rebuild the project (clean all build artifacts and recompile):**  
  ```
  make re
  ```

## Additional sections

### 1. Blocking cases handled
#### **Deadlock Prevention & Coffman Conditions**
#### 📚 Reference: Coffman's 4 Necessary Conditions
A deadlock occurs only if all four conditions hold simultaneously. Breaking any single condition completely prevents deadlocks:
1. **Mutual Exclusion**: Resources cannot be shared; only one thread can occupy a resource at a time.
2. **Hold and Wait**: A thread holding at least one resource is waiting to acquire additional resources held by other threads.
3. **No Preemption**: Resources cannot be forcibly taken away; they must be voluntarily released by the thread holding them.
4. **Circular Wait**: A closed dependency chain of threads $\{P_1, P_2, \dots, P_n\}$ exists, where $P_1$ waits for a resource held by $P_2$, $P_2$ waits for $P_3$, ..., and $P_n$ waits for $P_1$.

#### 🛠️ Our Implementation: Breaking Circular Wait
* **Target Condition Broken**: Circular Wait.
- **Address-Ordered Locking**: When acquiring multiple dongles, threads compare memory addresses and lock them in strict ascending order.
- **Structural Loop Prevention**: Enforcing a global lock hierarchy physically prevents circular wait chains from forming.

#### **Starvation Prevention**
- **Heap-Based Prioritization**: The `dongle` struct embeds a binary heap, maintaining the coder closest to burnout at the root using last-compilation/deadline metrics as priority.
- **v1.5 Deterministic Tie-Breaker**: When priorities/deadlines collide, a tie-breaker rule resolves ordering deterministically by favoring the earlier-pushed entry (FIFO insertion order).

#### **Dongle Cooldown Handling**
- **Expiration Tracking**: The `dongle` structure stores a cooldown expiration timestamp, which the coder updates upon finishing a compilation phase.
- **Timed Wait**: When attempting to acquire the dongle, a coder inspects this timestamp. If the cooldown has not expired yet, the coder sleeps until the deadline using `pthread_cond_timedwait`.

#### **Precise Burnout Detection**
- **Non-busy-wait Monitoring**: Uses `pthread_cond_timedwait` instead of busy-waiting to prevent excessive CPU usage.
- **Burnout Criteria**: Tracks each coder's last compilation timestamp; burnout is triggered when `current_time - last_compile_time >= time_to_burnout`.
- **Targeted Wake-up**: The monitor thread wakes up dynamically at the burnout deadline of the coder closest to burning out. Due to scheduler overhead, detection typically occurs within **2–3 ms**.
- **Safe Termination**: Upon detection, the monitor immediately raises `stop_flag`. Coders check `stop_flag` before every action to exit cleanly without redundant work.
- **Race Condition Prevention**: Access/checks to `stop_flag` are serialized using a dedicated mutex.

#### **Log Serialization**
- Serialized log writes using a single log_mutex to prevent message interleaving during concurrent execution.


### 2. Thread synchronization mechanisms
#### **Constrained Decoding Mechanism**  

## Thread Synchronization Mechanisms
Data races are prevented by strict mutex protection (`pthread_mutex_t`) for shared state/timestamps and condition-variable timed waits (`pthread_cond_timedwait`) to eliminate CPU-wasting busy-waiting.

```c
typedef struct s_SharedContext
{
    pthread_mutex_t lock;      // Protects/serializes reads and writes of stop_flag
    pthread_mutex_t log_lock;  // Serializes concurrent log output
}   t_SharedContext;

typedef struct s_Dongle
{
    pthread_cond_t  *cond;     // Condition variable signaling the root coder in the priority heap
    pthread_mutex_t lock;      // Protects dongle acquisition state
}   t_Dongle;

typedef struct s_Coder
{
    pthread_cond_t  cond;      // Used with pthread_cond_timedwait to avoid busy-waiting
    pthread_mutex_t lock;      // Protects access to the last compilation timestamp
}   t_Coder;

typedef struct s_Monitor
{
    pthread_cond_t  cond;      // Used with pthread_cond_timedwait to avoid busy-waiting
    pthread_mutex_t lock;      // Mutex paired with cond for timed wait coordination
}   t_Monitor;
```

# Resources
- **NotebookLM:**  
Utilized for translating the assignment guidelines, identifying key learning objectives, clarifying unfamiliar technical terminology, and understanding LLM integration workflows.

- **Pthreadsプログラミング:**  
https://www.oreilly.co.jp/books/4900900664/

- **Gemini:**  
Providing sample code and usage examples for unfamiliar functions.

- **AntigravityCLI:**  
Advice on variable and function naming, and file modularization/splitting. Plus romantic advice, of course.
