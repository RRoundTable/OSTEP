# 08 Scheduling: The Multi-Level Feedback Queue

이번파트에서는 Scheduling에서의 Multi Level Feedback Queue에 대해서 다룰것이다.
MLFO가 해결하고자하는 문제는 Two Fold이다.
먼저 Turnaround Time을 최적화한다.
이는 짧은 작업부터 처리하면 된다.
가장 대표적인 알고리즘으로 SJF가 있다.
하지만 Response Time역시 컴퓨터 시스템에서 중요한 이슈이다.
따라서 Turnaround Time를 최소화하면서 Response Time도 너무 커지면 안된다.
참고로 Response Time을 줄이기 위해서 Round Robin을 사용하게되면 Turnaround Time은 증가하게 된다.

어떻게 Scheduler가 Turnaround Time과 Response Time을 적절히 조절할 수 있을까?
후에 자세히 설명하겠지만 과거 데이터로 부터 미래를 예측하여 이를 최적화한다.

## 08-01 MLFQ: Basic Rules

Multi-Level Feedback Queue의 기본적인 알고리즘에 대해서 알아보겠다.

MLFQ는 여러 우선순위 수준의 Queue를 가진다.
Ready 상태의 작업들은 우선순위 수준에 따라서 하나의 Queue에 쌓여있다.
MLFQ는 어떤 작업을 우선순위를 바탕으로 어떤 작업을 먼저 실행시킬지 결정한다.

물론 Queue에 담긴 모든 작업들은 모두 동일한 우선순위를 가질 수 있다.
이런 경우 단순히 Round Robin Scheduling을 사용하여 실행시킨다.

다음은 MLFQ의 두 가지 규칙이다.
- Rule 1: 만약 `Priority(A) > Priority(B)`라면 A를 실행시킨다.
- Rule 2: 만약 'Priority(A) = Priority(B)`라면 A와 B를 Round Robin으로 실행시킨다.

위에서 알 수 있듯이 결국 MLFQ에서 가장 중요한 것은 Scheduler가 어떻게 우선순위를 부여하는지이다.
MLFQ는 고정된 우선순위를 부여하는 것이 아니라 작업의 관측되는 행동양상에 따라서 우선순위를 변경한다.
예를 들어 키보드 입력을 기다릴 때마다 CPU 사용을 포기하는 작업이 있을 경우 MLFQ는 해당 작업의 높은 우선순위를 부여한다.
왜냐하면 키보드로 입력을 받는 작업은 사용자와 상호작용하는 테스크기 때문이다.
반면에 CPU를 집중적으로 사용하는 작업의 경우 MLFQ는 해당작업의 우선순위를 낮춘다.
이런 방법들로 MLFQ는 Process의 작업형태를 학습하고 Process의 히스토리를 통해서 Process가 어떻게 행동할지 예측한다. 

![](../assets/images/08-Scheduling:%20The%20Multi-Level%20Feedback%20Queue/figure-8-1-MLFQ-example-.png)

위의 이미지는 MLFQ의 작업예시이다.
작업 A, B, C, D가 존재하며 서로 다른 우선순위를 가진다.
또한 A, B의 경우 같은 Q8에 졵재하기 때문에 Round Robin Scheduler를 통해서 동작한다.


## 08-02 Attempt #1: How To Change Prior

그렇다면 MLFQ는 어떻게 작업이 진행되는동안 작업의 우선순위를 변경할 수 있을까?
작업의 우선순위를 결정하는 가장 중요한 요소는 Workload이다.
CPU를 짧게 사용하는 사용자와 상호작용하는 작업과 CPU를 집중적을 사용하는 작업이 있다고 해보자.
우리는 다음과 같은 규칙을 더 추가할 수 있다.

- Rule3: 작업이 새롭게 시스템에 추가될 때 가장 높은 우선순위를 부여한다.
- Rule4a: 작업이 CPU를 Time Slice전체를 사용한다면 우선순위를 낮춘다.
- Rule4b: 작업이 Time Slice안에 CPU 점유를 포기한다면 우선순위를 유지한다.


### Example 1: A Single Long-Running Job

CPU를 길게 사용하는 작업의 경우 위의 MLFQ의 규칙에 따르면 어떻게 진행되는지 살펴보자.

![](../assets/images/08-Scheduling:%20The%20Multi-Level%20Feedback%20Queue/figure-8-2-Long-running-Job-Over-Time.png)

위의 그림에서 볼 수 있듯이 작업이 처음에 시스템에 들어오면 가장 높은 우선순위인 Q2로 간다.
한번의 Time Slice동안 해당작업은 CPU를 계속 사용했기 때문에 작업의 우선순위가 Q1으로 낮아졌다.
그 다음 Time Slice동안에도 해당작업은 CPU를 계속 사용한다.
따라서 우선순위가 Q0로 낮아졌다.

### Example 2: Along Came A Short Job

이번 예시는 조금 더 복잡하다.
작은 크기의 작업들이 있고 MLFQ가 어떻게 SJF를 근사하는지 살펴볼 것이다.

A작업은 CPU를 작업시간동안 집중적으로 사용하며 긴 작업이다.
B작업은 사용자와 상호작용하는 작업이며 짧은 작업이다.
A작업이 먼저 실행중이고 B작업이 도착했다고 가정하겠다.

![](../assets/images/08-Scheduling:%20The%20Multi-Level%20Feedback%20Queue/figure-8-3-Along-Came-An-Interactive-Job.png)

위의 그림은 해당 시나리오를 시각화한 것이다.
A는 검은색 박스이고 B는 회색 박스이다.

A작업은 실행하면서 Time Slice가 끝날때마다 우선순위가 낮아지며 B는 T = 100일때 삽입되기 때문에 가장 높은 우선순위를 부여받는다.

이 예시를 통해서 MLFQ의 메카니즘을 이해할 수 있다.
MLFQ는 작업이 들어오면 이 작업이 긴작업인지 짧은작업인지 알 수 없어 짧은 작업일 것이라고 생각하고 높은 우선순위를 부여한다.
그리고 실제로 짧은 작업이면 우선순위를 유지하고 그렇지 않으면 우선순위를 낮춘다.
이렇게 되면 실제로 SJF와 유사한 방법으로 Process를 실행하게 된다.

### Example 3: What About I/O?

Rule4b에서 볼 수 있듯이 Process가 Time Slice중간에 CPU 점유를 포기하게 되면 우선순위 수준을 유지한다.
Rule4b의 의도는 상호작용하는 작업의 경우(i.g. I/O Request, 키보드 입력을 기다리는 것) CPU를 Time Slice중간에 점유를 포기한다.
이런 경우 우선순위 수준을 그대로 유지시킨다.

![](../assets/images/08-Scheduling:%20The%20Multi-Level%20Feedback%20Queue/fiture-8-4-A-Mixed-IO-intensive-and-CPU-intensitve-Workload.png)

위의 예시는 I/O 작업이 있는 경우 어떤식으로 작업이 진행되는지 보여준다.
A(검은색)는 CPU를 계속사용하는 긴 작업이고 B(회색)는 상호작용하는 작업이다.
B는 CPU 점유를 주기적으로 포기한다. 상호작용 작업이기 때문에 MLFQ는 B의 우선순위를 높게 유지한다. B는 CPU 점유를 주기적으로 포기한다. 상호작용 작업이다.


### Problems With Our Current MLFQ

얼핏봤을 때 MLFQ는 이상적으로 작동할 것 같지만 몇가지 결점이 있다.
한번 어떤 Edge Case가 있는지 살펴보자.

첫 번째 문제는 상호작용 성격의 작업이 계속 늘어오면 CPU를 집중적으로 사용하는 긴 작업이 계속해서 미뤄진다는 것이다.
시스템은 이런 상황에서 긴 작업을 어느정도 진척시켜야한다.

두 번째 문제는 공격에 취약하다는 것이다.
Time Slice가 끝나기전에 I/O Request를 보내면 CPU 점유를 포기하게 된다.
이렇게 하면 해당 작업은 같은 우선순위 수준의 Queue에 있게 된다.
그렇게 되면 CPU를 거의 독점하는 상황이 발생한다.

세 번째 문제는 Program은 시간이 지남에 따라서 작업양상이 변한다는 것이다.
Program의 생애주기동안 상호작용 성격과 CPU를 집중적으로 사용하는 작업의 성격을 모두 가질 수 있으며 수시로 바뀔 수 있다.


## 08-03 Attempt #2: The Priority Boost

`상호작용 성격의 작업이 계속 늘어오면 CPU를 집중적으로 사용하는 긴 작업이 계속해서 미뤄진다는 것` 문제를 해결하기 위해서 무엇을 해볼 수 있을까?

간단한 아이디어는 주기적으로 모든 작업의 우선순위 수준을 올리는 것이다.

- Rule5: S주기마다 시스템에 있는 모든 작업의 우선순위를 가장 높게 올린다.

이렇게 되면 두 가지 문제를 동시에 해결할 수 있다.

- `상호작용 성격의 작업이 계속 늘어오면 CPU를 집중적으로 사용하는 긴 작업이 계속해서 미뤄진다는 것`
  모든 작업이 일정주기마다 최고 우선순위 수준으로 올라오기 때문에 특정작업이 계속해서 밀리는 일을 예방할 수 있다.
- `Program은 시간이 지남에 따라서 작업양상이 변한다는 것`
  CPU를 집중적으로 사용하는 작업이 상호작용 성격의 작업으로 변경되면 Priority Boost에 의해서 최고 수준의 우선순위를 부여받기 때문에 높은 우선순위 수준을 유지할 수 있다.

![](../assets/images/08-Scheduling:%20The%20Multi-Level%20Feedback%20Queue/fiture-8-4-A-Mixed-IO-intensive-and-CPU-intensitve-Workload.png)
위의 그림을 보면 Priority Boost를 적용한 것과 그렇지 않은 것을 비교한다.
Priority Boost가 없는 경우 CPU를 집중적으로 사용하는 긴 작업은 계속 낮은 우선순위 수준에 머물러 있으며 작업이 한동안 진행되지 않을 것이다.
반면에 Priority Boost를 적용하면 특정주기마다 우선순위 수준이 제일 높게 변경되기 때문에 어느정도 작업이 진행된다.

물론 어느주기마다 Prioriry Boost를 적용해야하는지는 Trade-Off가 있다.
주기가 너무 길다면 긴 작업은 느리게 진행될 것이고 너무 짧다면 Response Time이 길어지게 될 것이다.


## 08-04 Attempt #3: Better Accounting

`공격에 취약하다는 것: I/O Request를 Time Slice안에 보내는 과정이 주기적으로 있는 작업을 계속보낼 때` 에 대해서 어떻게 대처할  수 있을까?

해결책은 Process가 각 MLFQ의 Queue에서 CPU Time이 어느정도였는지 기억하는 것이다.
Process가 이미 할당된 CPU Time을 모두 사용하면 다음 Priority Queue로 강등된다.
한번에 CPU를 많이 사용하던지 혹은 나누어 사용하던지 중요하지 않다.
따라서 앞에서 Rule4a. Rule4b를 다시 작성하면 다음과 같다.

- Rule4: 작업이 주어진 우선순위 레벨의 CPU Time을 모두 사용하면 다음 Priority Queue로 강등된다.

![](../assets/images/08-Scheduling:%20The%20Multi-Level%20Feedback%20Queue/fiture-8-4-A-Mixed-IO-intensive-and-CPU-intensitve-Workload.png)

왼쪽의 경우 Time Slice안에 I/O Request를 주기적으로 하는 Process가 높은 우선순위를 계속해서 유지한다.
반면에 Rule4를 적용한 오른쪽의 경우 해당 프로세스는 CPU Time을 모두 사용하면 우선순위 수준이 낮아지는 것을 확인할 수 있다.


## 08-05 Tuning MLFQ And Other Issues

아직까지 MLFQ의 메카니즘에 대해서 알아보았다.
이제 중요한 질문은 어떻게 다음과 같은 파라미터를 튜닝할 것인가이다.
- Time Slice
- Priority Boost 주기
- 한 Process가 각 우선순위 수준에서 부여받을 CPU Time

튜닝의 예로 MLFQ는 각 우선순위 수준마다 Time Slice를 다르게 적용할 수 있다.
대게 높은 우선순위 수준의 Queue일수록 더 짧은 Time Slice를 가진다.
짧은 Time Slice를 가지게 되면 중요한 작업들을 빠르게 변경할 수 있기 때문에 공평하게 실행한다.
반면에 낮은 우선순위 수준의 Queue는 긴 Time Slice를 가진다.

![](../assets/images/08-Scheduling:%20The%20Multi-Level%20Feedback%20Queue/figure-8-7-Lower-Priority-Longer-Quanta.png)

Solaris MLFQ의 Time-Sharing Scheduling의 Class는 쉽게 설정할 수 있다.
Process의 우선순위를 설정할 수 있는 Table 집합을 제공하며 변경될 수 있다.
- Time Slice 설정
- Priority Boost 주기 설정
- etc

관리자는 설정을 통해서 다른 Scheduler를 만들 수 있었다.

다른 MLFQ의 경우 Table이나 명시적인 규칙을 적용하지 않았다.
오히려 수식을 통해서 우선순위를 설정하였다.
예를 들어 FReeBSD의 경우 현재 우선순위 레벨을 계산하기 위해서 수식을 계산한다.


마지막으로 많은 Scheduler들은 여러 기능을 가진다.
어떤 Scheduler의 경우 OS System작업에 가장 높은 우선순위를 부여한다.
따라서 사용자는 가장 높은 우선순위 수준을 얻지 못한다.
어떤 시스템의 경우에는 사용자에게 우선순위를 높이는 방법을 알려주기도 한다.
예를 들면 `nice`라는 명령어를 통해서 우선순위를 높이거나 줄일 수 있다.
