# 04 The Abstraction: The Process

Process는 단순하게 보면 동작중인 Program으로 생각할 수 있다.
Program은 그 자체로 동작하지 않는다.
디스크에 Instruction의 형태로 저장되어 있을 뿐이다.

컴퓨터를 사용할 때 동시에 여러개의 Process를 돌리는 일은 이제 매우 일반적이다.
예를 들어 음악을 들으면서 인터넷 쇼핑을 하거나, 이메일을 보내면서 동시에 개발창을 띄워둘 수 있다.
사용자는 마치 많은 Processes(CPU수보다 많은)가 동시에 동작하는 것처럼 보인다.

어떻게 제한된 CPU수를 가지고 CPU수보다 많은 Processes가 동시에 동작하는 것처럼 보이게 할 수 있을까?

OS는 CPU Virtualization(CPU 가상화)를 통해서 'CPU수보다 많은 Processes가 동시에 동작하는 것'처럼 보이게 한다.
OS는 물리적 CPU가 하나일 때 복수의 가상의 CPU가 있는 것처럼 보이게 할 수 있다.
그런 기술을 **Time Sharing**이라고 한다.

**Time Sharing**을 하게 되면 하나의 CPU에서 복수의 가상 CPU가 있는 것처럼 동작한다.
(하지만 이는 CPU의 작업속도를 늦출 수 있다.)

CPU 가상화를 구현하기 위해서는 크게 두 가지가 필요하다.

- Time Sharing: Low-Level Machinery Mechanisms
- Scheduling Policy: High-Level Intelligence

**참고: Time Sharing VS Space Sharing**

<img src="https://slidetodoc.com/presentation_image_h/0089a4a5dc3ace11bcac80f3f48accf5/image-12.jpg" width="500" height="500" />


- [Time Sharing](https://en.wikipedia.org/wiki/Time-sharing)
  - 하나의 리소스를 시간별로 나누어 사용하는 방법
  - 예) 두 개의 Process가 하나의 CPU를 번갈아 가면 사용
- [Space Sharing](https://en.wikipedia.org/wiki/Shared_space)
  - 하나의 리소스를 공간별로 나누어 사용하는 방법
  - 예) 하나의 메모리공간을 Process A와 Process B가 나누어 사용


## 04-01 The Abstraction: A Process

Process를 구성하는 요소를 알기 위해서는 Machine State에 대한 이해가 필요하다.
(Machine State는 Program이 동작중에 읽고 업데이트하는 것이다.)

Process를 구성하는 Machine State의 중요한 부분은 Memory와 관련된다.
Instruction은 Memory에 위치한다.
Program이 읽고 쓰는 데이터또한 메모리에 위치한다.
그러므로 Address Space(Memory가 Process 위치를 추적하기 위한 공간) 또한 Process의 구성요소이다.

또 다른 부분은 Register이다.
Instruction은 Register에 있는 정보를 읽기도 업데이트하기도 한다.
Register는 Process 실행에 중요한 역할을 한다.
Resigster중에서 특히 Program Counter(PC)는 Machine State에 관해 중요한 역할을 한다.
PC는 다음에 실행할 Instruction의 주소를 가지고 있는다.
비슷하게 Stack Pointer도 Stack Function의 Parameter나 Local Variable, Return Address을 관리하기 위해 활용된다.

마지막으로 Process는 Persistance Volume Storage에 접근하기도 한다. (I/O Information의 경우 Process가 최근에 열었던 파일목록을 가지고 있는다.)


**참고: 폰노이만 구조**

<img src="https://slidetodoc.com/presentation_image/4910aada9725379e21e7cb7e161ff96f/image-7.jpg" width="500" height="500" />

- 메모리
  - 데이터메모리: 데이터에 대한 메모리
  - 명령어메모리: Instruction에 대한 메모리
- 중앙처리장치: 명령어를 실행하는 장치
  - 산술논리연산장치(ALU): 컴퓨터가 지원하는 저수준의 논리/산술 연산지원
  - 레지스터: CPU의 빠른 연산속도를 위해 존재하는 기억장치
  - 제어장치: 명령어해석의 역할을 수행한다.
- 입력/출력
  - 메모리 맵핑: 입력장치에서 오는 입력을 받을 메모리에 저장

## 04-02 Process API

Process는 다음과 같은 구조의 API를 가지고 있다.

- Create

OS는 새로운 Process를 만들 수 있다.
마우스 클릭이나 커멘트창에 입력을 통해서 새로운 Process가 만들어진다.

- Destroy

강제로 Process를 없앨 수 있다.
물론 대부분의 Process는 실행 후 완료되면 종료되지만 강제로 종료해야할 경우도 생긴다.

- Wait

실행을 멈추는 것을 기다리는 기능

- Miscellaneous Control

Process를 생성하고 제거하는 기능 외에 다양한 기능들이 필요하다.
대표적으로는 Process 실행을 잠시 멈췄다가 재개하는 것이다.

- Status

Process에 대한 상태를 나타낸다.


## 04-03 Process Creation: A Little More Detail

OS에서 Program을 실행하기 위해서는 다음과 같은 작업들이 필요하다.

1. Program의 Code와 Static Data를 메모리에 로드한다.
   참고로 Program Code는 디스크에 실행 가능한 형태로 저장되어 있다.
   초기 OS는 실행할 Program Code를 모두 미리 메모리에 실었지만, 현대 OS는 Program 실행에 필요한 부분만 그때그때 로딩한다.

2. Program의 Code와 Static Data를 메모리에 로드한 후 메모리는 Program의 Runtime Stack을 할당한다.
   Local Variables, Function Parameters, Return Addresses, Arguments

3. 메모리에 동적으로 데이터를 할당하기 위해서 Heap 구조를 활용하게 된다.
4. I/O 초기화 작업을 진행한다.


## 04-04: Process States
<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/8/83/Process_states.svg/800px-Process_states.svg.png" width="500" height="500" />


- Running: Process가 실행중인 상태, Instruction이 실행되고 있는 상태 
- Ready(Waiting): Process가 실행가능한 상태, 이 상태에서는 OS가 해당 Process를 선택하지 않고 있다.
- Blocked: Process가 Ready상태가 될 수 없는 작업을 실행한 상태, 예를 들면 I/O Request를 초기화하는 작업을 요청했으면 해당 요청이 완료될 때까지 Process는 기다려야한다.

## 04-05: Data Structure

아래는 OS에서 각 Process에 대해서 파악해야하는 데이터이다. (xv6 kernel)
Process State 부분을 보면 Running, Ready, Blocked State말고 더 다양한 State가 있을 수 있다.
- Initial: Process 생성중
- Zombie: Process가 완료되었으나 자원을 반환하지 않음
- Final: 주로 Parent에서 Child Process가 끝났다는 상태를 파악하고 Return Code를 실행한다.

```
# The xv6 Proc Structure
// the registers xv6 will save and restore
// to stop and subsequently restart a process
struct context {
  int eip;
  int esp;
  int ebx;
  int ecx;
  int edx;
  int esi;
  int edi;
  int ebp;
};
// the different states a process can be in
enum proc_state { UNUSED, EMBRYO, SLEEPING,
                  RUNNABLE, RUNNING, ZOMBIE };
// the information xv6 tracks about each process
// including its register context and state
struct proc {
 };
char *mem; // Start of process memory
uint sz; // Size of process memory
char *kstack; // Bottom of kernel stack
enum proc_state state; // Process state
int pid; // Process ID
struct proc *parent; // Parent process
void *chan; // If !zero, sleeping on chan
int killed; // If !zero, has been killed
struct file *ofile[NOFILE]; // Open files
struct inode *cwd; // Current directory
struct context context; // Switch here to run process
struct trapframe *tf; // Trap frame for the
// current interrupt
```

**참고: Process List**

OS에서는 중요한 데이터 구조가 있다.
그 중 Process List는 OS에서 중요한 역할을 한다.
Process List 정보다 있어야 OS가 안정적으로 동시에 여러개의 Process를 운영할 수 있다.
Process List는 [Process Control Block](https://en.wikipedia.org/wiki/Process_control_block#:~:text=A%20process%20control%20block%20(PCB,known%20as%20a%20process%20descriptor.&text=This%20specifies%20the%20process%20state,%2C%20running%2C%20waiting%20or%20terminated))으로 이루어져있다.


## Homework (Simulation)
This program, process-run.py, allows you to see how process states change as programs run and either use the CPU (e.g., perform an add instruction) or do I/O (e.g., send a request to a disk and wait for it to complete). See the README for details.
Questions

1. Run process-run.py with the following flags:-l 5:100,5:100. What should the CPU utilization be (e.g., the percent of time the CPU is in use?) Why do you know this? Use the -c and -p flags to see if you were right.

답: 100%, I/O process나 병목이 되는 것이 없다.

```
python process-run.py -l 5:100,5:100

Produce a trace of what would happen when you run these processes:
Process 0
  cpu
  cpu
  cpu
  cpu
  cpu

Process 1
  cpu
  cpu
  cpu
  cpu
  cpu

Important behaviors:
  System will switch when the current process is FINISHED or ISSUES AN IO
  After IOs, the process issuing the IO will run LATER (when it is its turn)
```

```
python process-run.py -l 5:100,5:100 -c -p
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1          
  2        RUN:cpu         READY             1          
  3        RUN:cpu         READY             1          
  4        RUN:cpu         READY             1          
  5        RUN:cpu         READY             1          
  6           DONE       RUN:cpu             1          
  7           DONE       RUN:cpu             1          
  8           DONE       RUN:cpu             1          
  9           DONE       RUN:cpu             1          
 10           DONE       RUN:cpu             1          

Stats: Total Time 10
Stats: CPU Busy 10 (100.00%)
Stats: IO Busy  0 (0.00%)
```

2. Now run with these flags: ./process-run.py -l 4:100,1:0. These flags specify one process with 4 instructions (all to use the CPU), and one that simply issues an I/O and waits for it to be done. How long does it take to complete both processes? Use -c and -p to find out if you were right.

    답: 11 Time = 4 (Process 0) + 7 (Process 1 IO)
    ```
    ./process-run.py -l 4:100,1:0 -c -p  
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2        RUN:cpu         READY             1          
    3        RUN:cpu         READY             1          
    4        RUN:cpu         READY             1          
    5           DONE        RUN:io             1          
    6           DONE       WAITING                           1
    7           DONE       WAITING                           1
    8           DONE       WAITING                           1
    9           DONE       WAITING                           1
    10           DONE       WAITING                           1
    11*          DONE   RUN:io_done             1          

    Stats: Total Time 11
    Stats: CPU Busy 6 (54.55%)
    Stats: IO Busy  5 (45.45%)
    ```

3. Switch the order of the processes: -l 1:0,4:100. What happens now? Does switching the order matter? Why? (As always, use -c and -p to see if you were right)

    답: 실행순서가 중요하다. 왜냐하면 I/O Request를 처리하는동안 Process 1이 동작할 수 있기 때문이다.

    ```
    ./process-run.py -l 1:0,4:100 -c -p
    Time        PID: 0        PID: 1           CPU           IOs
    1         RUN:io         READY             1          
    2        WAITING       RUN:cpu             1             1
    3        WAITING       RUN:cpu             1             1
    4        WAITING       RUN:cpu             1             1
    5        WAITING       RUN:cpu             1             1
    6        WAITING          DONE                           1
    7*   RUN:io_done          DONE             1          

    Stats: Total Time 7
    Stats: CPU Busy 6 (85.71%)
    Stats: IO Busy  5 (71.43%)
    ```

4. We’ll now explore some of the other flags. One important flag is -S, which determines how the system reacts when a process is- sues an I/O. With the flag set to SWITCH ON END, the system will NOT switch to another process while one is doing I/O, in- stead waiting until the process is completely finished. What happens when you run the following two processes (-l 1:0,4:100 -c -S SWITCH ON END), one doing I/O and the other doing CPU work?

    답: 순서를 바꿔도 동일한 실행시간을 가진다. 왜냐하면 I/O Request를 처리한 후 Process 1이 동작하기 때문이다.

    ```
    ./process-run.py -l 1:0,4:100 -c -S SWITCH_ON_END
    Time        PID: 0        PID: 1           CPU           IOs
    1         RUN:io         READY             1          
    2        WAITING         READY                           1
    3        WAITING         READY                           1
    4        WAITING         READY                           1
    5        WAITING         READY                           1
    6        WAITING         READY                           1
    7*   RUN:io_done         READY             1          
    8           DONE       RUN:cpu             1          
    9           DONE       RUN:cpu             1          
    10           DONE       RUN:cpu             1          
    11           DONE       RUN:cpu             1 
    ```

5. Now, run the same processes, but with the switching behavior set to switch to another process whenever one is WAITING for I/O (-l 1:0,4:100 -c -S SWITCH ON IO). What happens now? Use -c and -p to confirm that you are right.

    답: IO request시 Process 1이 동작할 수 있다.

    ```
    ./process-run.py -l 1:0,4:100 -c -S SWITCH_ON_IO  
    Time        PID: 0        PID: 1           CPU           IOs
    1         RUN:io         READY             1          
    2        WAITING       RUN:cpu             1             1
    3        WAITING       RUN:cpu             1             1
    4        WAITING       RUN:cpu             1             1
    5        WAITING       RUN:cpu             1             1
    6        WAITING          DONE                           1
    7*   RUN:io_done          DONE             1         
    ```

6. One other important behavior is what to do when an I/O completes. With -I IO RUN LATER, when an I/O completes, the process that issued it is not necessarily run right away; rather, whatever was running at the time keeps running. What happens when you run this combination of processes? (Run ./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH ON IO -I IO RUN LATER -c -p) Are system resources being effectively utilized?

    답: 먼저 실행된 Process0의 IO Request는 다른 Process들이 모두 끝난 후 처리된다. IO를 처리하는동안 CPU는 동작하지 못한다. 따라서 비효율적이다.

    ```
    ./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_LATER -c -p 
    Time        PID: 0        PID: 1        PID: 2        PID: 3           CPU           IOs
    1         RUN:io         READY         READY         READY             1          
    2        WAITING       RUN:cpu         READY         READY             1             1
    3        WAITING       RUN:cpu         READY         READY             1             1
    4        WAITING       RUN:cpu         READY         READY             1             1
    5        WAITING       RUN:cpu         READY         READY             1             1
    6        WAITING       RUN:cpu         READY         READY             1             1
    7*         READY          DONE       RUN:cpu         READY             1          
    8          READY          DONE       RUN:cpu         READY             1          
    9          READY          DONE       RUN:cpu         READY             1          
    10          READY          DONE       RUN:cpu         READY             1          
    11          READY          DONE       RUN:cpu         READY             1          
    12          READY          DONE          DONE       RUN:cpu             1          
    13          READY          DONE          DONE       RUN:cpu             1          
    14          READY          DONE          DONE       RUN:cpu             1          
    15          READY          DONE          DONE       RUN:cpu             1          
    16          READY          DONE          DONE       RUN:cpu             1          
    17    RUN:io_done          DONE          DONE          DONE             1          
    18         RUN:io          DONE          DONE          DONE             1          
    19        WAITING          DONE          DONE          DONE                           1
    20        WAITING          DONE          DONE          DONE                           1
    21        WAITING          DONE          DONE          DONE                           1
    22        WAITING          DONE          DONE          DONE                           1
    23        WAITING          DONE          DONE          DONE                           1
    24*   RUN:io_done          DONE          DONE          DONE             1          
    25         RUN:io          DONE          DONE          DONE             1          
    26        WAITING          DONE          DONE          DONE                           1
    27        WAITING          DONE          DONE          DONE                           1
    28        WAITING          DONE          DONE          DONE                           1
    29        WAITING          DONE          DONE          DONE                           1
    30        WAITING          DONE          DONE          DONE                           1
    31*   RUN:io_done          DONE          DONE          DONE             1          

    Stats: Total Time 31
    Stats: CPU Busy 21 (67.74%)
    Stats: IO Busy  15 (48.39%)
    ```


1. Now run the same processes,but with -I IO_RUN_IMMEDIATE set, which immediately runs the process that issued the I/O. How does this behavior differ? Why might running a process that just completed an I/O again be a good idea?

    답: IO가 바로 실행되는 동시에 중앙처리장치는 다른 Process를 수행한다. 따라서 CPU Utilization은 100%로 효율적이다.

    ```
    ./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_LATER -c -p -I IO_RUN_IMMEDIATE
    Time        PID: 0        PID: 1        PID: 2        PID: 3           CPU           IOs
    1         RUN:io         READY         READY         READY             1          
    2        WAITING       RUN:cpu         READY         READY             1             1
    3        WAITING       RUN:cpu         READY         READY             1             1
    4        WAITING       RUN:cpu         READY         READY             1             1
    5        WAITING       RUN:cpu         READY         READY             1             1
    6        WAITING       RUN:cpu         READY         READY             1             1
    7*   RUN:io_done          DONE         READY         READY             1          
    8         RUN:io          DONE         READY         READY             1          
    9        WAITING          DONE       RUN:cpu         READY             1             1
    10        WAITING          DONE       RUN:cpu         READY             1             1
    11        WAITING          DONE       RUN:cpu         READY             1             1
    12        WAITING          DONE       RUN:cpu         READY             1             1
    13        WAITING          DONE       RUN:cpu         READY             1             1
    14*   RUN:io_done          DONE          DONE         READY             1          
    15         RUN:io          DONE          DONE         READY             1          
    16        WAITING          DONE          DONE       RUN:cpu             1             1
    17        WAITING          DONE          DONE       RUN:cpu             1             1
    18        WAITING          DONE          DONE       RUN:cpu             1             1
    19        WAITING          DONE          DONE       RUN:cpu             1             1
    20        WAITING          DONE          DONE       RUN:cpu             1             1
    21*   RUN:io_done          DONE          DONE          DONE             1          

    Stats: Total Time 21
    Stats: CPU Busy 21 (100.00%)
    Stats: IO Busy  15 (71.43%)

    ```

8. Now run with some randomly generated processes:-s 1 -l 3:50,3:50 or -s 2 -l 3:50,3:50 or -s 3 -l 3:50,3:50. See if you can
predict how the trace will turn out. What happens when you use the flag -I IO_RUN_IMMEDIATE vs. -I IO_RUN_LATER? What hap-
pens when you use -S SWITCH_ON_IO vs. -S SWITCH_ON_END?


    답:
    `-s`: seed에 따라서 CPU Utilization이 달라진다.
    `-S SWITCH_ON_IO`일 때,  `-I IO_RUN_IMMEDIATE`, `-I IO_RUN_LATER`차이가 없다.
    `-S SWITCH_ON_END`일 때,  `-I IO_RUN_IMMEDIATE`와 `-I IO_RUN_LATER` 차이가 없다.
    `-S SWITCH_ON_IO`가 `-S SWITCH_ON_END`에 비해 효율적이다.

    ```
    # SWITCH_ON_IO
   ./process-run.py -s 3 -l 3:50,3:50 -S SWITCH_ON_IO -I IO_RUN_LATER -c -p                    
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        WAITING        RUN:io             1             1
    4        WAITING       WAITING                           2
    5        WAITING       WAITING                           2
    6        WAITING       WAITING                           2
    7        WAITING       WAITING                           2
    8*   RUN:io_done       WAITING             1             1
    9*       RUN:cpu         READY             1          
    10           DONE   RUN:io_done             1          
    11           DONE        RUN:io             1          
    12           DONE       WAITING                           1
    13           DONE       WAITING                           1
    14           DONE       WAITING                           1
    15           DONE       WAITING                           1
    16           DONE       WAITING                           1
    17*          DONE   RUN:io_done             1          
    18           DONE       RUN:cpu             1          

    Stats: Total Time 18
    Stats: CPU Busy 9 (50.00%)
    Stats: IO Busy  11 (61.11%)

    # SWITCH_ON_IO
    ./process-run.py -s 3 -l 3:50,3:50 -S SWITCH_ON_IO -I IO_RUN_IMMEDIATE -c -p 
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        WAITING        RUN:io             1             1
    4        WAITING       WAITING                           2
    5        WAITING       WAITING                           2
    6        WAITING       WAITING                           2
    7        WAITING       WAITING                           2
    8*   RUN:io_done       WAITING             1             1
    9*         READY   RUN:io_done             1          
    10          READY        RUN:io             1          
    11        RUN:cpu       WAITING             1             1
    12           DONE       WAITING                           1
    13           DONE       WAITING                           1
    14           DONE       WAITING                           1
    15           DONE       WAITING                           1
    16*          DONE   RUN:io_done             1          
    17           DONE       RUN:cpu             1          

    Stats: Total Time 17
    Stats: CPU Busy 9 (52.94%)
    Stats: IO Busy  11 (64.71%)


    # SWITCH_ON_END
    ./process-run.py -s 3 -l 3:50,3:50 -S SWITCH_ON_END -I IO_RUN_LATER -c -p 
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        WAITING         READY                           1
    4        WAITING         READY                           1
    5        WAITING         READY                           1
    6        WAITING         READY                           1
    7        WAITING         READY                           1
    8*   RUN:io_done         READY             1          
    9        RUN:cpu         READY             1          
    10           DONE        RUN:io             1          
    11           DONE       WAITING                           1
    12           DONE       WAITING                           1
    13           DONE       WAITING                           1
    14           DONE       WAITING                           1
    15           DONE       WAITING                           1
    16*          DONE   RUN:io_done             1          
    17           DONE        RUN:io             1          
    18           DONE       WAITING                           1
    19           DONE       WAITING                           1
    20           DONE       WAITING                           1
    21           DONE       WAITING                           1
    22           DONE       WAITING                           1
    23*          DONE   RUN:io_done             1          
    24           DONE       RUN:cpu             1          

    Stats: Total Time 24
    Stats: CPU Busy 9 (37.50%)
    Stats: IO Busy  15 (62.50%)

    ## SWITH_ON_END
    ./process-run.py -s 3 -l 3:50,3:50 -S SWITCH_ON_END -I IO_RUN_IMMEDIATE -c -p 
    Time        PID: 0        PID: 1           CPU           IOs
    1        RUN:cpu         READY             1          
    2         RUN:io         READY             1          
    3        WAITING         READY                           1
    4        WAITING         READY                           1
    5        WAITING         READY                           1
    6        WAITING         READY                           1
    7        WAITING         READY                           1
    8*   RUN:io_done         READY             1          
    9        RUN:cpu         READY             1          
    10           DONE        RUN:io             1          
    11           DONE       WAITING                           1
    12           DONE       WAITING                           1
    13           DONE       WAITING                           1
    14           DONE       WAITING                           1
    15           DONE       WAITING                           1
    16*          DONE   RUN:io_done             1          
    17           DONE        RUN:io             1          
    18           DONE       WAITING                           1
    19           DONE       WAITING                           1
    20           DONE       WAITING                           1
    21           DONE       WAITING                           1
    22           DONE       WAITING                           1
    23*          DONE   RUN:io_done             1          
    24           DONE       RUN:cpu             1          

    Stats: Total Time 24
    Stats: CPU Busy 9 (37.50%)
    Stats: IO Busy  15 (62.50%)
    ```