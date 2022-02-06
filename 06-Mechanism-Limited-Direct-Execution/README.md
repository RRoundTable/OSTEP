# 06-Mechanism: Limited Direct Execution

CPU를 가상화하기 위해서는 컴퓨터에서 동시에 여러 Process를 운영할 수 있어야 하고 이를 위해서는 Time Sharing 기법이 필요하다.
간단히 말하면 한 Process가 끝날때까지 CPU를 점유하는 것이 아니라 여러 Process가 돌아가면서 CPU를 사용하는 것이라고 볼 수 있다.

성공적인 Time Sharing을 위해서는 두 가지 이슈가 있다.

- 성능: CPU를 가상화하면서 시스템에 추가적인 오버헤드가 없거나 적어야한다.
- 컨트롤: Time sharing시 Process가 제대로 완료되어야하고 Process가 접근할 수 있는 정보범위도 관리해줘야 한다.

## 06-01 Basic Technique: Limited Direct Execution

Program이 예상한대로 빠르게 작동하기 위한 방법으로 OS 개발자들은 Directed Execution을 떠올린다.
바로 CPU에 Program을 실행시키는 것이다.

Directed Execution은 다음과 같이 전개된다.

- OS는 Program이 실행되기를 기대하고 Process List에 Process를 생성한다.
- 그 후 Process에 해당하는 Memory를 할당하며 Program Code를 로딩한다.
- 그 후 Stack Argc/Argv를 로딩하고 Register를 Clear한다.
- Code를 실행하고 Return을 받는다.
- Process에 해당하는 Memory를 해제하면 Process List에서 해당 Process는 제거된다.


Directed Execution는 간단해보이지만 CPU가상화시 몇 가지문제를 야기할 수 있다.

첫 번째 문제는 어떻게 OS가 Program이 OS가 제한한 것을 하고 있는지 안하고 있는지 확인하기 어렵다는 것이다.
두 번째 문제는 Process가 실행중일때, OS가 어떻게 Process를 중지하고 다른 Process를 실행할 수 있는지이다.

해당 문제는 Limited Directed Execution으로 해결가능하며 아래에 더 자세히 서술할 것이다.



## 06-02 Problem #1: Restricted Operation

Direct Execution(CPU에 직접접근하여 실행)하는 것은 성능상 이점이 있다.
하지만 Direct Execution은 문제가 있다.
만약 Program이 제한된 오퍼레이션을 수행하는 부분을 포함한다면 전체 시스템에 악영향을 끼칠 수 있다.
예를 들면, I/O Request나 시스템에서 사용중인 CPU나 Memory에 접근할 수도 있다.

Process가 그들이 의도한 모든 작업을 수행하게 할 수 있다.
하지만 그렇게 되면 많은 종류의 시스템의 설계가 불가능할 수 있다.
예를 들어 파일에 접근하기 전에 유저의 접근권한을 확인하는 파일시스템 만든다고 가정해보자.
그렇게되면 Process에서 I/O 접근을 하지못하도록 막아야한다.
만약 Process에서 I/O 접근이 가능하다면 파일시스템 의도와 다르게 어떤 Process에서든 접근권한이 없더라도 파일시스템에 있는 정보에 접근할 수 있을 것이다.

OS에서는 이런 문제를 해결하기 위해서 User Mode를 사용한다.
User Mode에서 실행되는 Program은 그 권한에 따라 할 수 있는 것이 제한된다.\
예를 들어 I/O Request를 할 수 없는 User Mode에서 Process를 실행했다고 가정해보자.(참고로 I/O Request는 Kernel Mode에서 가능하다.)
만약 Process가 I/O Request를 할려고 한다면 Exception이 발생할 것이고 OS는 해당 Process를 Kill할 것이다.
User Mode와 반대되는 것은 Kernel Mode(OS가 동작하는 모드)이다.
해당 모드에서는 모든 것을 할 수 있다.
Privileged Operation이나 I/O Request 그리고 모든 종류의 제한된 Instruction들이 가능하다.

만약 User Process가 Previleged Operation을 수행하고 싶다면 어떻게 해야할까?
이를 위해서 현대 하드웨어는 User Program이 System Call을 수행할 수 있도록 한다.

System Call을 실행하기 위해서 Program은 특별한 Trap Instruction을 수행해야한다.
이 Execution은 Kenel에 접근하여 Process를 Kernel Mode로 승격시킨다.
그렇게되면 Process는 Privileged Operation을 수행할 수 있게 된다.
작업이 종료되면 Trap Instruction으로 부터 Return을 받으며 User Program에 전달한다.
그와 동시에 Privilege Level를 User Mode로 복구한다.

하드웨어는 Trap Instruction을 수행할 때 주의해야 한다.
하드웨어는 Caller의 Register가 충분히 확보되었는지 확인해야한다.
이는 OS가 Return-From-Trap Instruction을 정확하게 수행할 수 있도록 하기 위함이다.
예를 들어 X86에서 Processor가 Program Counter, Flags, 그리고 다른 Register 정보들을 Process당 Kernel Stack에 Push할 것이다.
Return-From-Trap은 이런 값들을 Stack에서 Pop할 것이다. 그리고 다시 User Mode Execution을 재개할 것이다.

몇가지 주요한 사항들이 남아있다.
어떻게 Trap은 어떤 Code를 돌릴지 알 수 있는가?
명확하게 Process를 Calling하는 것은 접근하는 Address를 특정할 수 없다.
접근하는 Address를 특정할 수 있다면 Program은 어떤 Kernel이든 접근할 수 있을 것이고 이것은 매우 안좋은 생각이다.
그러므로 Kernel은 Trap에서 수행되는 Execution을 아주 신중하게 관리해야한다.

어떻게 Trap은 어떤 Code를 돌릴지 알 수 있는가?
Kernel은 이를 위해서 부팅할 때 Trap Table을 설정한다.
머신이 부팅될 때 Kernel(Privileged) Mode에서 설정하며 머신 하드웨어를 직접 설정할 필요는 없다.
OS가 초기에 수행하는 작업중에 하나는 하드웨어에게 Exceptional Event가 발생할 때 어떤 코드가 동작하는지 말해주는 것이다.
예를 들어 하드디스크 인터럽트가 발생할 때, 키보드 인터럽트가 발생했을 때 혹은 System Call왔을 때 어떤 코드가 동작할지 설정해준다.
OS는 하드웨어에 Trap Hanlder의 위치를 알려준다.
하드웨어가 Trap Handler에 대한 정보를 받으면 머신이 리부팅되기전까지 Trap Handler의 위치를 기억한다.
그리고 이로 인해서 하드웨어는 System Call이나 다른 Exceptional Event가 발생하면 어떤 동작을 취해야하는지 알 수 있다.




System Call을 특정하기 위해서 System Call마다 System Call Number를 붙인다.
User Code는 Resigster혹은 Stack에 System Call Number를 넣어줘야한다.
Trap Handler가 System Call을 핸들링할 때 System Call Number를 확인하고 검증한다.
검증되었다면 System Call Number와 대응하는 System Call을 수행한다.
이런 직접적이지 않은 실행은 일종의 보호기능을 한다.
User Code는 직접 Address를 특정하지 않으며 System Call Number로 서비스를 요청해야한다.

하드웨어에게 Trap Table의 위치를 전달하는 Instruction을 수행하는 것은 매우 영향력이 크다.
아마도 해당 Instruction이 Privileged Operation이라고 생각할 수 있을 것이다. (그렇다.)
User Mode로 해당 Instruction을 실행하려고 하면 하드웨어는 허가하지 않는다.
그렇다면 만약 Trap Table을 직접 설치한다면 어떤 일이 벌어질까? 머신을 차지할 수 있을까?
다음은 Limited Direct Execution Protocal이다. 
각 Process는 Kernel Stack을 가졌고 Resgister가 하드웨어로부터 정보를 복원하고 하드웨어에 정보를 저장하는 역할을 한다고 가정했다.

![](https://t1.daumcdn.net/cfile/tistory/99408C4C5BC495B32C)


Limited Direct Execution에는 두 가지 부분이 있다.
첫 번째 부분은 부팅할 때이다.
Kernel은 Trap Table을 초기화하고 CPU는 Trap Table 위치를 기억한다.
그리고 Kernel은 이를 Privileged Operation을 통해서 수행한다.

두 번째 부분은 Process가 실행할 때이다. 
Return-From-Trap Instruction을 수행하기전에 Kernel은 몇 가지 설정을 한다.
- Node에 Process List 할당
- Memory 할당
Process가 System Call을 수행하려고 하면 OS에 전달하고 OS는 System Call을 핸들링하고 Return-From-Trap을 통해서 Process에 System Call 결과를 전달한다.
Process작업이 완료되면 OS는 Clean-UP(Memory 할당 제거, Process List에서 제거)한다.

## 06-03 Problem #2: Switching Between Processes

CPU의 Time Sharing을 위해서는 OS가 Process를 멈추고 다른 Process를 실행할 수 있어야한다.
하지만 이는 까다로운 작업이다.
만약 Process가 CPU에서 동작중이라면 OS는 동작중이지 않다.
OS가 동작중이지 않다면, OS가 어떻게 Process를 멈추고 다른 Process를 시작할까?
사실 그렇게 할 수 없다.
OS가 CPU에서 동작중이지 않을때는 OS의 어떤 동작도 실행할 수 없다.

그렇다면 OS는 어떻게 CPU 컨트롤을 다시 가져올 수 있을까?

### A Cooperative Approach: Wait For System Calls

과거에 사용되었던 하나의 방법은 Cooperative Approach이다.
이 방법에서는 OS는 System Process가 합리적으로 동작할 것이라고 신뢰한다.
오랜 시간동안 실행중인 Process는 주기적으로 CPU를 반납하고 이로인해 OS는 다른 작업을 돌릴 수 있다.

그렇다면 Process가 어떻게 CPU를 반환할까?
대부분의 Process는 CPU 컨트롤을 System Call을 통해서 OS에게 주기적으로 전달한다.
예를 들어 파일을 열거나 읽기위해서 혹은 메세지를 다른 머신으로 보내기 위해서 혹은 새로운 프로세스를 만들기 위해서 System Call을 사용한다.
위의 예시들은 모두 명시적인 System Call을 생성한다.
이는 OS에게 CPU 컨트롤을 전달하는 역할을 한다.
이로 인해 다른 Process를 수행할 수 있다.

어플리케이션도 허용되지 않은 것을 하게되면 OS에 Control을 전달한다.
예를 들어 어플리케이션이 0으로 나누거나 접근하면 안되는 메모리에 접근하게 되면 Trap을 만들게 된다.
그렇게되면 OS는 CPU에 대한 컨트롤을 얻는다.

정리해보면 Cooperative Scheduling System에서는 OS는 CPU 컨트롤을 System Call이나 허용되지 않은 동작을 할때 얻는다.
아마도 수동적인 방법론이라고 생각할 수 있겠다.
Cooperative Scheduling System은 만약 무한루프를 보는 Process가 생성되면 어떻게 처리할 수 있을까? (System Call을 만들지도 허용되지 않은 동작을 하지도 않는다.)

### A Non-Cooperative Approach: The OS Takes Control

하드웨어의 추가적인 도움없이 OS는 다음과 같은 상황에서 할 수 있는 것이 별로 없다.
만약 Process가 System Call을 만드는 것을 거부하여 OS에 컨트롤을 넘기지 않으면 어떻게 해야하는가?
사실 Cooperative Approach에서 Process가 무한루프에 빠지면 머신을 재부팅하는 것밖에 할게 없다.
그러므로 우리는 OS가 CPU 컨트롤을 얻는 방법에 대해서 다시 고민해봐야 한다.

위에 대한 해결책으로 Timer Interrupt가 있다.
Timer는 매 Milliseconds마다 Interrupt를 발생시키도록 프로그래밍된다.
Interrupt가 발생하게 되면 Process를 멈추고 미리 설정된 OS안에 Interrupt Handler가 동작한다.
OS는 다시 CPU에 대한 컨트롤을 얻게된다.
그 후에는 OS는 현재 Process를 멈추고 다른 것을 다시 시작할 수 있다.

OS는 하드웨어에게 Timer Interrupt가 발생할 때 어떤 코드를 동작해야하는지 알려줘야한다.
그러므로 OS는 부팅할 때 하드웨어에게 해당정보를 넘긴다.
그리고 Timer를 시작한다.

OS는 종종 잘못된 동작을 하는 Process들을 처리해야한다.
악의적인 디자인 혹은 버그는 허용되지 않은 동작을 할려한다.
현대 시스템에서는 OS가 이러한 잘못된 동작을 처리하기 위한 방법은 그들을 강제로 종료시키는 것이다.
하지만 Process가 메모리에 비정상적으로 접근하거나 Instruction을 실행하려고 할때 정지시켜야 한다면 어떻게 해야할까?

Timer가 시작할 때 OS는 컨트롤이 결국 컨트롤을 반환할 것이라고 생각한다.
그러므로 OS는 User Program을 돌릴 수 있다.
Timer는 꺼질수도 있는데 이는 Concurrency 부분에서 자세히 다루도록하겠다.

하드웨어가 Interrupt 발생시 해야할 일이 있다는 것을 기억해야한다.
특히 Interrupt발생했을때 동작중인 Program의 State를 보존하기 위해서 Return-From-Trap Instruction이 Program을 다시 정확하게 실행할 수 있을 것이다.
이러한 동작 집합들은 System Call 발생시 하드웨어가 하는 동작들과 매우 유사하다. (System Call into Kernal)

### Saving and Restoring Context

OS가 System Call후든 Timer Interrupt 후든 다시 CPU제어권을 가지고 있다고 해보자.
이제 다음과 같은 결정들이 이루어져야 한다.
- 현재 동작중인 Process를 다시 동작시킬지
- 다른 Process를 스위치할지

이 결정들은 OS의 Scheduler가 결정한다.
앞으로는 Scheduling Policy에 대해서 다룰 것이다.

만약 스위치하기로 결정했다면 OS는 Context Switch와 관련된 Low-Level의 코드를 동작시킨다.
Context Switch는 간단하다.
OS가 할일은 Register에 저장된 값(현재 동작중인 Process와 관련됨)을 보존하는 것이다.
그리고 곧 실행될 Process의 정보를 Kernel Stack으로 부터 Register에 복원시킨다.
그렇게 함으로써 OS는 Return-From-Trap이 발생이 실행되었을 때 동작중인 Process를 반환하는 것 대신에 System은 다른 Process를 실행할 수 있다.

현재 동작중인 Process의 Context를 저장하기 위해서 OS는 Low Level Assembly Code를 실행시킨다.
현재동작중인 Process의 Register, PC, Kernel Stack Pointer 정보를 저장하고,
곧 실행될 Process의 Kernel로 변경한다.
Stack을 Switching함으로써 Kernel은 곧 Interrupt될 Switch code의 Call로 들어가게 되고 곧 실행될 Process의 Context를 반환한다.
마지막으로 OS가 Return-From-Trap Instruction을 실행할때 곧 실행될 Process가 현재 동작중인 Process가 된다.

위의 과정들은 아래 이미지에서 표현되어 있다.

![](https://t1.daumcdn.net/cfile/tistory/99D073335BC49DD129)

이 예시에서는 Process A는 실행중이다.
그리고 Timer Interrupt에 의해서 Interrupted되었다.
하드웨어는 Process A의 Register를 Kernel Stack으로 저장하고 Kernel로 들어간다.
Timer Interrupt Handler에서는 OS는 Process A에서 Process B로 Switch할 것을 결정한다.
이 때, OS는 현재 Process의 Register의 정보를 저장하고 Process B의 정보를 Register에 복원하기 위해서 `switch()` Routine을 실행한다.
그런 후 Stack Pointer를 Process B의 Kernel Stack으로 변경하며 Context를 전환한다.
마지막으로 OS는 Return-From-Trap B의 Register를 복원하며 Process B를 시작한다.

Switch과정에서 Register를 저장/복원하는 과정이 포함되어 있다는 것을 주목해야한다.
첫 번째는 Timer Interrupt가 발생한다.
이 경우에는 실행중인 Process의 User Register는 Kernel Stack을 사용하여 하드웨어에 의해서 저장된다.
두 번째는 OS A에서 B로 바꾸기로 정한다.
이 경우에는 Kernel Register가 소프트웨어(OS)에 의해서 명시적으로 저장된다.
하지만 이번에는 Kernel Stack이 아닌 Memory에 저장한다.
그 후 System을 A에서 비롯된 Kernel에서 Trap이 된 것에서 B에서 비롯된 Kernel로 Trap된 것으로 변경한다.

## 06-05 Worried About Concurrency?

만약 System Call중에 Timer Interrupt가 발생한다면?
만약 Interrupt를 핸들링하고 있는중에 다른 Interrupt가 발생한다면?
이런 상황이 생기면 어떻게 처리해야할까?

OS는 이런 문제들에 대해서 대처할 수 있어야한다.
이 문제는 사실 Concurrency와 관련된 문제이다.

Concorrency 문제에 대해서 다뤄보면 OS가 위와 같은 상황에서 어떻게 대처하는지 알 수 있을 것이다.
하나 간단한 것은 Interrupt처리중에는 다른 Interrupt가 발생할 수 없게 하는 것이다.
그렇게함으로써 Interrupt가 핸들링되는 것을 보장할 수 있다.
다른 것들은 CPU에 전달되지 않을 것이다.
물론 OS는 이런 정책을 신중히 결정해야한다.
Interrupt를 긴시간동안 못하게하는 것은 좋지 못한 OS이다.

OS는 또한 복잡한 Concurrent Access에 대한 보호를 위해서 Locking Schema를 개발해야한다.
이 것은 복수의 활동들이 동시에 Kernel에서 동작가능하도록 해준다.
특히 Multiprocessor환경에서 유리하다.
다음 Concurrency부분에서 Locking이 복잡할 수 있으며 다양한 버그들을 유발하는 것을 볼 수 있을 것이다.

## Homework (Measurement)

1. System Call에 소요되는 시간을 측정해라

```
python systemcall.py
Average Time for System Call Open: 3.210067749023437e-05
```


2. Context Switch에 소요되는 시간을 측정해라

답: AVG 4.73 ms

```
# Run with 10 threads for 5 minutes to simulate the problem of multi-threaded switching
sudo sysbench --threads=10 --time=300 --events=10000000 --test=threads run

WARNING: the --test option is deprecated. You can pass a script name or path on the command line without any options.
sysbench 1.0.20 (using system LuaJIT 2.1.0-beta3)

Running the test with following options:
Number of threads: 10
Initializing random number generator from current time


Initializing worker threads...

Threads started!



General statistics:
    total time:                          300.0114s
    total number of events:              634055

Latency (ms):
         min:                                    0.31
         avg:                                    4.73
         max:                                  432.38
         95th percentile:                       23.52
         sum:                              2999052.67

Threads fairness:
    events (avg/stddev):           63405.5000/1123.37
    execution time (avg/stddev):   299.9053/0.00
```


- 참고: [Linux CPU context switching performance monitoring](https://programmer.group/linux-cpu-context-switching-performance-monitoring.html)
- 참고: [Measuring context switching and memory overheads for Linux threads](https://eli.thegreenplace.net/2018/measuring-context-switching-and-memory-overheads-for-linux-threads/)