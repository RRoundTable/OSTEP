# 07 Scheduling: Introduction

[06-Mechanism-Limited-Direct-Execution](06-Mechanism-Limited-Direct-Execution)에서는 OS가 CPU를 다루는 Low-Level 방법론을 살펴보았다. (Context Switching)

이번 장에서는 High-Level 방법론인 Scheduling Policy에 대해서 알아볼 것이다.

## 07-01 Workload Assumptions

Scheculing Policy에 대해서 본격적으로 다루기전에 Workload(Process를 실행시키는 상황)에 대해서 몇가지 가정을 하겠다.
Workload를 정하는 것에 따라서 Policy가 변경될 수 있으며 Workload에 대해서 잘 알수록 더 자세한 Policy를 설계할 수 있다.
지금 가정하는 Workload는 사실 비현실적이다.
하지만 후에 더 복잡한 시나리오에 대해서 다룰 것이므로 괜찮다. (이것을 Fully-Operational Scheduling Discipline이라고 부른다.)

**가정**

1. 각각의 작업은 동일한 시간만큼 필요하다.
2. 모든 작업은 동시에 도착한다.
3. 한번 시작하면 각각의 작업은 완료된다.
4. 모든 작업은 CPU만 사용하며 I/O에 대한 것은 하지 않는다.
5. 각 작업의 동작한 시간은 모두 알 수 있다.


## 07-02 Scheduling Metrics

Scheduling Policy를 평가하기 위해서는 Scheduling Metric이 필요하다.
Schduling Metric으로는 Turnaround Time을 사용한다.

$$
T_{\text{turnaround}} = T_{\text{completion}} - T_{\text{arrival}}
$$

위에서 모든작업은 동시에 도착하기 때문에 $T_{\text{arrival}}$는 0으로 간주할 수 있으므로 다음과 같이 정의할 수 있다.

$$
T_{\text{turnaround}} = T_{\text{completion}}
$$

주목할 점은 Turnaround Time은 Performance Metric이라는 것이다.
이번장에서는 주로 Turnaround Time을 다루지만 Scheduling Policy에서는 Fariness도 중요하다.
Performance와 Fairness는 주로 상충하는 Metric으로 추후에 자세히 다룰 것이다.


## 07-03 First In First Out (FIFO)

가장 기본적인 알고리즘은 First In, First Out Scheduling이다. (First Come First Served라고도 불린다.)
FIFO는 몇가지 긍적적인 특징이있다.
매우 간단하게 개발할 수 있으며 별이상없이 동작한다.

간단한 예시를 살펴보자.
작업 A,B,C가 동시에 도착했다.
FIFO에 의해서 특정작업을 먼저 수행한다. (무작위 혹은 내부정책에 따라서)
A, B, C를 수행한다.
그리고 각각의 작업은 10초동안 실행된다.
그렇다면 Average Turnaround Time은 어떻게 될까?

![](../assets/images/07-Scheduling:%20Introduction/FIFO_1.png)

- A: 10
- B: 20 (A가 완료된 후 실행)
- C: 30 (A, B가 완료된 후 실행)
- Average Turnaround Time: $\frac{10+20+30}{3}$

이제 가정중 하나를 깨보자.
만약 모든 작업이 동일한 실행을 가지지 않으면 어떻게 될까?
여기서 FIFO의 명확한 단점이 들어난다.
만약 A가 100초의 실행시간, B, C가 10초에 수행시간을 가지면 어떻게 될까?

![](../assets/images/07-Scheduling:%20Introduction/FIFO_2.png)

- A: 100
- B: 110 (A가 완료된 후 실행)
- C: 120 (A, B가 완료된 후 실행)
- Average Turnaround Time: $\frac{100+110+120}{3}$

이러한 상황을 [Convoy Effect](https://en.wikipedia.org/wiki/Lock_convoy)라고 부른다.
작은 리소소를 실행하는 작업들이 큰 리소스를 필요로하는 작업에 밀리는 현상이다.

## 07-04 Shortest Job First (SJF)

Convoy Effect를 해결하기 위해서 Shortest Job First Scheduling 방법론을 고려할 수 있다.
먼저 작은 크기의 작업을 수행하고 그 다음 작업을 수행한다.

SJF하에서 A가 100초의 실행시간, B, C가 10초에 수행시간을 가지면 어떻게 될까?

![](../assets/images/07-Scheduling:%20Introduction/SJF_1.png)


- B: 10 (B 먼저 수행)
- C: 20 (B가 완료된 후 실행)
- A: 120 (B, C가 완료된 후 실행)
- Average Turnaround Time: $\frac{10+20+120}{3}$

도착시간이 모두 같다라는 가정에서는 Shortest Job First가 최적의 알고리즘이다.
(여기서 증명을 하지 않는다.)

하지만 도착시간이 모두 같다라는 가정도 역시 비현실적이다.
만약 작업마다 도착시간이 모두 상이하다면 어떻게 될까?

A가 먼저 도착한후 10초 후에 B와 C가 도착한다면 다음과 같을 것이다.

![](../assets/images/07-Scheduling:%20Introduction/SJF_2.png)

- A: 100
- B: 110 - 10 (A가 완료된 후 실행)
- C: 120 - 10(A, B가 완료된 후 실행)
- Average Turnaround Time: $\frac{100+110+120}{3}$

## 07-05 Shortest Time-to-Completion First(STCF or Preemptive Shortest Job First)

위의 문제를 해결하기 위해서는 `3. 한번 시작하면 각각의 작업은 완료된다.` 가정을 깨야한다.
또한 Scheduler는 Timer Interrupt와 Context Switching을 할 필요가 있다.
그렇게되면 B와 C가 도착했을 때 계속 A를 실행시키는 것이 아니라 작은 작업인 B, C를 끝내고 A를 실행할 수 있다.

![](../assets/images/07-Scheduling:%20Introduction/STCF_1.png)

- B: 20 - 10
- C: 30 - 10
- A: 120 - 0
- Average Turnaround Time: $\frac{10 + 20 + 120}{3} = 50$

참고로 STCF(Preemptive Shortest Job First라고도 불린다.)는 선점 Scheduler이며 반면에 SJF는 비선점 Scheduler이다.
SJF 비선점 Scheduler이기 때문에 위에서 말한 문제를 가지고 있다.

모든 작업이 동시에 도착한다면 SJF가 최적의 알고리즘이다.
하지만 모든 작업이 다른시간에 도착한다면 STCF가 최적이다.

## 07-06 A New Metric: Response Time

작업의 길이를 알고 그 작업이 오직 CPU만 사용한다면 Turnaround Metric으로 Scheduler의 성능을 파악할 수 있다.
실제로 많은 컴퓨터 시스템에서 많이 활용되었다.
하지만 Time Shared Machine이 등장하면서 많은 것이 변화되었다.
이제 사용자는 터미널에 명령어를 치며 시스템에 Interactive Performance를 요구한다. (즉 컴퓨터에게 명령에 대한 답을 빨리 얻기를 바란다.)

이러한 요구사항에 적절한 Metric은 Response Time이다.

$$
T_{\text{response}} = T_{\text{firstturn}} - T_{\text{arrival}}
$$

아래의 상황에 대해서 Response Time을 구해보면 다음과 같다.

![](../assets/images/07-Scheduling:%20Introduction/STCF_1.png)

- A: 0
- B: 0
- C: 10
- Average Response Time: $\frac{0 + 0+ 10}{3} = 3.333$

STCF 알고리즘은 Response Time에 대해서 불리하다.
예를 들어 세개의 작업이 동시에 들어오면 세번째로 실행되는 작업은 앞의 작은 두 작업이 반응하고 나서야 반응할 수 있게된다.
비록 STCF는 Turnaround Time에 대해서는 좋은 성능을 보이지만 Response Time에서는 그렇지 않다.
터미널에 입력에 대해서 10초뒤에 반응이 온다고 상상해보아라. (끔찍하다.)
따라서 우리는 Response Time도 고려한 Scheduling이 필요하다.

## 07-07 Round Robin

위의 문제를 해결하기 위해서 Round Robin을 고려해볼 수 있다.
Round Robin은 작업을 완료까지 진행하는 것이 아니라 작업을 특정 시간단위로 분할하여 작업을 수행한다.
특정시간이 지나면 Queue에 쌓여있는 다른 작업을 진행한다.
이런 특징 때문에 Round Robin은 Time-Slicing이라고 불린다.
주목할 점은 Round Robin에서 사용되는 시간단위는 Timer Interrupt의 배수여야한다는 것이다.
예를 들어 Timer Interrupt가 10 MilliSecond이면 RoundRobin의 시간단위는 10MS, 20MS 등이 될 것이다.

이해를 돕기위해서 예시를 들어보겠다.
작업 A,B,C가 동시에 도착했다고 가정하겠다.
각각의 작업은 5초동안 동작한다.

SJF의 다음과 같이 진행된다.

![](../assets/images/07-Scheduling:%20Introduction/SJF_3.png)
SJF의 Average Response Time은 $\frac{0 + 5 + 10}{3} = 5$이다.

RR의 경우에는 다음과 같이 진행된다. (RR의 시간단위는 1초이다.)

![](../assets/images/07-Scheduling:%20Introduction/roundrobin_1.png)
RR의 Average Response Time은 $\frac{0 + 1+ 2}{3} = 1$이다.
여기서 알수있는이 Round Robin의 Time Slice단위에 따라 Response Time에 영향을 많이 준다.
하지만 Response Time을 줄이기 위해서 극단적으로 작은 단위의 Time Slice를 쓰는 것은 성능상 문제가 있다.
지나친 Context Switching을 유도하여 성능이 저하된다.
따라서 Time-Slice는 Response Time을 너무 길게하지 않는 적절한 길이를 유지하면서 Context Switching 비용을 너무 높이면 안된다.

Context Switching은 OS가 몇몇의 Register의 데이터를 저장하고 복원하는 비용뿐만 아니라 다른 비용도 야기한다.
Program이 실행되면 CPU Cache, TLB Branch Predictor, On-Chip Hardware에 많은 양의 State를 만든다.
다른 작업으로 변경하는 것은 이런 State들을 버리고 변경할 작업의 State를 다시 넣어줘야한다.
이는 성능상 꽤 큰 비용이다.

RR은 만약 Response Time만 중요한 메트릭이라면 적절한 Time Slice를 가지면 매우 휼륭한 Scheduler이다.
하지만 Turnaround관점에서 보면 어떨까?

- A: 13
- B: 14
- C: 15
- Average Turnaround Time: $\frac{13 + 14 + 15}{3} = 14$

매우 안좋은 성능이다.
FIFO보다 더 안좋은 Turnaound Time을 가질 수 있다.

하지만 Round Robin은 Fair하다.
Process끼리 CPU를 공평하게 시간단위로 나누어 사용한다.
이에 Turnaround Time에는 안좋다.
사실 이는 근본적으로 Trade-Off이다.
Fair할수록 Turnaround Time에서는 불리하고 Unfair할수록 Response Time에서 불리하다.

## 07-08 Incorporating I/O

이제 `4. 모든 작업은 CPU만 사용하며 I/O에 대한 것은 하지 않는다.` 가정을 없애보자.
Process가 I/O 작업을 요청한다면 그 동안은 CPU를 사용하지 않으며 I/O Request 결과를 기다린다.
즉, Process가 Hard Disk Drive에 I/O를 요청한다면 Milliseconds 혹은 더 길게 Block된다.
그러므로 스케쥴러는 이때 다른 Process(CPU를 사용해야하는)으로 바꿔주는 것이 필요하다.

또한 I/O가 완료되었을 때 Scheduler는 결정해야할 일이 있다.
I/O가 완료되었을 때 Interrupt가 발생하며 OS는 I/O에 의해서 Block된 Process를 동작시킨다.
혹은 아예 다른 Process를 실행시킬수도 있다.
OS는 어떻게 결정할까?

이해를 돕기위해서 예를 들어보자.
작업 A, B가 있다고 가정하자.
A, B는 모두 50 MS만큼 CPU를 사용해야한다.
하지만 A의 경우 10MS 만큼 동작하다가 I/O Requsest를 한다.(I/O Request는 10MS만큼 걸린다고 가정)
반면에 B는 I/O Request없이 50MS만큼 CPU를 사용한다.
Scheduler는 A를 먼저 실행시키며 B를 실행시킨다.

![](../assets/images/07-Scheduling:%20Introduction/poor_use_of_resource.png)

우리가 STCF Scheduler를 만들고 있다고 생각해보자.
A는 CPU 사용이 10 MS를 경계로 끊어지게 되고 B의 경우는 50MS 통째로 CPU를 사용한다.
이럴 경우 STCF는 어떻게 처리해야할까?
사실 B먼저 동작시키고 A를 동작시킨다면 I/O Request에 대한 고민을 할 필요가 없다.

일반적으로 A, B 작업을 10MS 단위로 독립적인 서브작업으로 나눈다.
시스템은 A의 서브작업을 동작시킬지 B의 서브작업를 동작시킬지 결정한다.
더 짧은 작업을 선택한다면 A의 서브작업을 선택할 것이다.
A의 첫번째 서브작업이 완료되면 그 다음 실행할 수 있는 것은 B의 서브작업뿐이다. (두 번째 A의 서브작업은 I/O Request를 기다려야한다.)
B의 서브작업이 10MS동안 동작하는 동안 Hard Disk Drive에서는 I/O Request를 처리하고 있는다.

![](../assets/images/07-Scheduling:%20Introduction/Overlap%20Allows%20Better%20Use%20Of%20Resources.png)

이는 A를 통째로 먼저 실행시켰을 때보다 효율적이다.


## Homework (Simulation)

This program, scheduler.py, allows you to see how different schedulers perform under scheduling metrics such as response time, turnaround time, and total wait time. See the README for details.


Questions
1. Compute the response time and turnaround time when running three jobs of length 200 with the SJF and FIFO schedulers.

답: SJF-400, FIFO-400

```
./scheduler.py -p SJF -l 200,200,200 -c
ARG policy SJF
ARG jlist 200,200,200

Here is the job list, with the run time of each job: 
  Job 0 ( length = 200.0 )
  Job 1 ( length = 200.0 )
  Job 2 ( length = 200.0 )
```

```
./scheduler.py -p FIFO -l 200,200,200 -c
ARG policy FIFO
ARG jlist 200,200,200

Here is the job list, with the run time of each job: 
  Job 0 ( length = 200.0 )
  Job 1 ( length = 200.0 )
  Job 2 ( length = 200.0 )
```

2. Now do the same but with jobs of different lengths: 100, 200, and 300.
   
답: SJF-333, FIFO-333

```
./scheduler.py -p SJF -l 100,200,300
ARG policy SJF
ARG jlist 100,200,300

Here is the job list, with the run time of each job: 
  Job 0 ( length = 100.0 )
  Job 1 ( length = 200.0 )
  Job 2 ( length = 300.0 )


Compute the turnaround time, response time, and wait time for each job.
When you are done, run this program again, with the same arguments,
but with -c, which will thus provide you with the answers. You can use
-s <somenumber> or your own job list (-l 10,15,20 for example)
to generate different problems for yourself.
```

```
./scheduler.py -p FIFO -l 100,200,300
ARG policy FIFO
ARG jlist 100,200,300

Here is the job list, with the run time of each job: 
  Job 0 ( length = 100.0 )
  Job 1 ( length = 200.0 )
  Job 2 ( length = 300.0 )


Compute the turnaround time, response time, and wait time for each job.
When you are done, run this program again, with the same arguments,
but with -c, which will thus provide you with the answers. You can use
-s <somenumber> or your own job list (-l 10,15,20 for example)
to generate different problems for yourself.

```

3. Now do the same, but also with the RR scheduler and a time-slice of 1.

답: 465

```
./scheduler.py -p RR -l 100,200,300 -q 1
ARG policy RR
ARG jlist 100,200,300

Here is the job list, with the run time of each job:
  Job 0 ( length = 100.0 )
  Job 1 ( length = 200.0 )
  Job 2 ( length = 300.0 )


Compute the turnaround time, response time, and wait time for each job.
When you are done, run this program again, with the same arguments,
but with -c, which will thus provide you with the answers. You can use
-s <somenumber> or your own job list (-l 10,15,20 for example)
to generate different problems for yourself.
```

5. For what types of workloads does SJF deliver the same turnaround times as FIFO?

답: 먼저들어올수록 작은길이의 작업인 경우

6. For what types of workloads and quantum lengths does SJF deliver the same response times as RR?

답: 200이상

7. What happens to response time with SJF as job lengths increase? Can you use the simulator to demonstrate the trend?

답: 작업의 길이가 커질수록 Response Time도 커진다. 사용할 수 없다.

8. What happens to response time with RR as quantum lengths in- crease? Can you write an equation that gives the worst-case re- sponse time, given N jobs?

답: response time은 N (jobs) * quantum length 만큼 늘어난다.
