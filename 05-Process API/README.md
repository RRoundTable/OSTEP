# 05-Process API

이번장에서는 Unix System에서 Process를 생성해보는 것을 해볼 것이다.

`fork()`, `exec()`, `wait()`과 같은 System Call을 활용하여 새로운 Process를 만들 것이다.

## 05-01 The `fork()` System Call

`fork()`는 새로운 Process를 생성하는데 활용된다.
하지만, 미리경고하자면 아주 강한 Routine이기 때문에 조심히 사용해야한다.

아래와 같은 코드를 만들어낼 수 있다. (Calling fork() (p1.c))

```
# Calling fork() (p1.c)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) {
        // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
        } 
    else if (rc == 0) {
        //child (new processes)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
    }
    else{
        // parent goes down this path (main)
        printf("hello, I am parent of %d (pid:%d)\n", rc, (int) getpid());
    }
    return 0;
}   

```

Calling fork() (p1.c)은 다음과 같은 순서로 진행될 것이다.


1. "hello world (pid:27163)" print
2. fork() 실행 -> Process 생성
3. "hello, I am parent of 27164 (pid:27163)" print
4. "hello, I am child (pid: 27164)" print

참고로 3, 4번은 무엇이 먼저 실행될지 알 수 없다.
OS Scheduler의 상황에 따라서 3번이 먼저 실행될 수도 4번이 먼저 실행될 수도 있다.

```
prompt> ./p1
hello world (pid:29146)
hello, I am parent of 29147 (pid:29146)
hello, I am child (pid:29147)
prompt>


prompt> ./p1
hello world (pid:29146)
hello, I am child (pid:29147)
hello, I am parent of 29147 (pid:29146)
prompt>
```

`fork()`가 실행되면 p1 Process가 두 개인 것처럼 보일 것이다.
하지만 부모 Process와 자식 Process는 완전히 같지 않다.
자식 Process는 자신의 Address Space, Register, PC를 가진다.

## 05-02 The `wait()` System Call

아래코드는 Calling fork() (p1.c)에서 `wait()` 과정을 추가한 것이다.
이제 해당코드의 출력은 Deterministic하다.
즉, 몇번을 실행해도 같은 결과가 나온다.

```
# Calling fork() And wait() (p2.c)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) {
        // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
        } 
    else if (rc == 0) {
        //child (new processes)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
    }
    else{
        // parent goes down this path (main)
        int rc_wait = wait(Null);
        printf("hello, I am parent of %d (pid:%d)\n", rc, (int) getpid());
    }
    return 0;
}

```

```
prompt> ./p2
hello world (pid:29266)
hello, I am child (pid:29267)
hello, I am parent of 29267 (rc_wait:29267) (pid:29266)
prompt>

prompt> ./p2
hello world (pid:29266)
hello, I am child (pid:29267)
hello, I am parent of 29267 (rc_wait:29267) (pid:29266)
prompt>
```

## 05-02 The `exec()` System Call

`exec()`는 현재 Program속에서 다른 Program을 실행하는 역할을 한다.

```
# Calling fork(), wait(), And exec() (p3.c)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) {
        // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
        } 
    else if (rc == 0) {
        //child (new processes)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
        char *myargs[3];
        myargs[0] = strdup("wc");   // program: "wc" (word count)
        myargs[1] = strdup("p3.c"); // argument: file to count
        myargs[2] = NULL;           // marks end of array
        execvp(myargs[0], myargs);  // runs word count
        printf("this shouldn’t print out");
    }
    else{
        // parent goes down this path (main)
        int rc_wait = wait(Null);
        printf("hello, I am parent of %d (pid:%d)\n", rc, (int) getpid());
    }
    return 0;
}
```

`wc()` 는 Prgrogam의 Line, Words, Bytes, Filename을 출력한다.

```
prompt> ./p3
hello world (pid:29383)
hello, I am child (pid:29384)
      29     107    1030 p3.c
hello, I am parent of 29384 (rc_wait:29384) (pid:29383)
prompt>
```

## 05-04 Why? Motivating The API

Process를 생성하기 위해서는 왜 `fork()`, `wait()`, `exec()`와 같은 Interface들이 필요할까?
Unix Shell에서는 `fork()`와 `exec()`를 구분짓는 것이 큰 특징이다.
Unix Shell는 `fork()`후와 `exec()`전에 코드를 실행할 수 있다.(구분짓지 않았다면 사이에 코드를 넣을 수 없을 것이다.)
이 특징은 다양하고 흥미로운 개발을 가능하게 한다.

다음 예시를 살펴보자.

```
prompt> wc p3.c > newfile.txt
```

Program `wc()`를 실행 후 결과를 `newfile.txt`로 만든다.
Shell은 다음과 같이 작업한다.
자식 Process 생성하고 `exec()` 실행하기 전에 Shell은 Standard Output을 닫고 `newfile.txt`를 연다.
그렇게함으로써 Program `wc()`의 출력이 스크린출력 대신 파일이 된다.


위의 예시와 동일한 작업을 하는 코드예시이다.

```
# All Of The Above With Redirection (p4.c)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) {
        // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
        } 
    else if (rc == 0) {
        //child (new processes)
        close(STDOUT_FILENO);
        open("./p4.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
        
        char *myargs[3];
        myargs[0] = strdup("wc");   // program: "wc" (word count)
        myargs[1] = strdup("p4.c"); // argument: file to count
        myargs[2] = NULL;           // marks end of array
        execvp(myargs[0], myargs);  // runs word count
    }
    else{
        // parent goes down this path (main)
        int rc_wait = wait(Null);
    }
    return 0;
}
```

아래는 위의 프로그램의 출력이다.

```
prompt> ./p4
prompt> cat p4.output
      32     109     846 p4.c
prompt>
```

`p4.c`는 OS가 [File Descriptor](https://en.wikipedia.org/wiki/File_descriptor#:~:text=In%20Unix%20and%20Unix%2Dlike,a%20pipe%20or%20network%20socket.)를 다룬다는 가정 아래에서 동작한다.
Unix System은 Free File Descriptor(=0)를 찾는다.
이 경우에는 STDOUT_FOLENO가 첫번째 가능한 File Descriptor이다.
그리고 `open()`이 실행되면 할당될 것이다.
이어서 자식 Process에 의해서 Standard Output File Descriptor(`printf` 사용시)는 새롭게 열린 File Descriptor로 대체될 것이다. 

`p4.c`가 실행되면 스크린화면에는 어떤 것도 보이지 않을 것이다.
하지만 `newfile.txt`가 만들어졌을 것이다.


Unix Pipes는 위와 유사하게 설계되었다.
`pipe()`를 활용하여 이를 구현한다.
한 Process의 Output은 Kernel Pipe(i.e. queue)로 연결된다.
그리고 다른 Process의 Input은 같은 Pipe에 연결된다.
그렇게 함으로써 한 Process의 Output이 다른 Process의 Input이 될 수 있으며 이를 반복하여 체인처럼 만들 수 있다.

```
prompt> grep -o foo file | wc -l
```

## 05-05 Process Control And Users

`fork()`, `wait()`, `exec()`를 넘어서 UNIX System은 다양한 인터페이스를 갖추고 있다.
예를 들어  `kill()`은 Process에 시그널을 전달한다.
여기서 시그널은 Pause, Die등 다양한 표현을 할 수 있다.
사용성을 위해서 어떤 Keystroke 조합은 현재 작동중인 Process에 시그널을 전달한다.
예를 들어 `Ctrl + c`는 Interrupt 시그널을 전달하고 `Ctrl + z`는 Stop(후에 Resume 가능) 시그널을 전달한다.

그리고 각 Process는  `signal()` 인터페이스를 갖추고 있어 신호를 전달받을 수 있다.
시그널이 Process에 도착하면 원래하던 작업을 잠깐 중지하고 시그널을 처리한다.

시그널은 User의 권한에 따라서 각기 다르게 적용될 수 있다.
Root 권한의 경우 모든 시그널을 전달할 수 있지만 해당 Process에 권한이 없는 경우 시그널을 전달할 수 없다.

## 05-06 Useful Tools

생각보다 많은 Command Tool이 존재한다.
[Man Page](https://linux.die.net/man/)를 읽어보는 것을 추천한다.


## Homework(Simulation)

This simulation homework focuses on fork.py, a simple process cre- ation simulator that shows how processes are related in a single “familial” tree. Read the relevant README for details about how to run the simulator.


Questions

1. Run./fork.py -s 10 and see which actions aret aken. Can you predict what the process tree looks like at each step? Use the -c flag to check your answers. Try some different random seeds (-s) or add more actions (-a) to get the hang of it.

답: 할 수 없습니다. Seed에 따라서 다른 Action이 나옵니다.


```
python fork.py -s 10 -c

ARG seed 10
ARG fork_percentage 0.7
ARG actions 5
ARG action_list 
ARG show_tree False
ARG just_final False
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve True

                           Process Tree:
                               a

Action: a forks b
                               a
                               └── b
Action: a forks c
                               a
                               ├── b
                               └── c
Action: c EXITS
                               a
                               └── b
Action: a forks d
                               a
                               ├── b
                               └── d
Action: a forks e
                               a
                               ├── b
                               ├── d
                               └── e


python fork.py -s 11 -c

ARG seed 11
ARG fork_percentage 0.7
ARG actions 5
ARG action_list 
ARG show_tree False
ARG just_final False
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve True

                           Process Tree:
                               a

Action: a forks b
                               a
                               └── b
Action: b forks c
                               a
                               └── b
                                   └── c
Action: b forks d
                               a
                               └── b
                                   ├── c
                                   └── d
Action: d forks e
                               a
                               └── b
                                   ├── c
                                   └── d
                                       └── e
Action: b forks f
                               a
                               └── b
                                   ├── c
                                   ├── d
                                   │   └── e
                                   └── f
```


2. One control thes simulator gives you is the fork percentage, controlled by the -f flag. The higher it is, the more likely the next action is a fork; the lower it is, the more likely the action is an exit. Run the simulator with a large number of actions (e.g., -a 100) and vary the fork percentage from 0.1 to 0.9. What do you think the resulting final process trees will look like as the percentage changes? Check your answer with -c.

답: `-f` 값이 높을수록 트리관계가 복잡해진다.

```
python fork.py -s 11 -c -f 0.1

ARG seed 11
ARG fork_percentage 0.1
ARG actions 5
ARG action_list 
ARG show_tree False
ARG just_final False
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve True

                           Process Tree:
                               a

Action: a forks b
                               a
                               └── b
Action: b forks c
                               a
                               └── b
                                   └── c
Action: c EXITS
                               a
                               └── b
Action: b EXITS
                               a
Action: a forks d
                               a
                               └── d


python fork.py -s 11 -c -f 0.9

ARG seed 11
ARG fork_percentage 0.9
ARG actions 5
ARG action_list 
ARG show_tree False
ARG just_final False
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve True

                           Process Tree:
                               a

Action: a forks b
                               a
                               └── b
Action: b forks c
                               a
                               └── b
                                   └── c
Action: b forks d
                               a
                               └── b
                                   ├── c
                                   └── d
Action: d forks e
                               a
                               └── b
                                   ├── c
                                   └── d
                                       └── e
Action: b forks f
                               a
                               └── b
                                   ├── c
                                   ├── d
                                   │   └── e
                                   └── f
```

3. Now, switch the output by using the -t flag (e.g., run ./fork.py -t). Given a set of process trees, can you tell which actions were taken?

답: 네 screen에 보입니다.

```
python fork.py -t                      

ARG seed -1
ARG fork_percentage 0.7
ARG actions 5
ARG action_list 
ARG show_tree True
ARG just_final False
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve False

                           Process Tree:
                               a

Action?
                               a
                               └── b
Action?
                               a
                               └── b
                                   └── c
Action?
                               a
                               └── b
                                   └── c
                                       └── d
Action?
                               a
                               ├── b
                               │   └── c
                               │       └── d
                               └── e
Action?
                               a
                               ├── b
                               │   └── c
                               │       └── d
                               └── e
                                   └── f
```


4. One interesting thing to note is what happens when a child exits; what happens to its children in the process tree? To study this, let’s create a specific example: ./fork.py -A a+b,b+c,c+d,c+e,c-. This example has process ’a’ create ’b’, which in turn creates ’c’, which then creates ’d’ and ’e’. However, then, ’c’ exits. What do you think the process tree should like after the exit? What if you use the -R flag? Learn more about what happens to orphaned processes on your own to add more context.

답: c가 종료(완료가 아닌)되었다면 c의 자식 Process는 d,e의 결과값은 a에게 전달된다.
`-R`을 사용하게 되면 d, e의 결과값이 b에게 전달된다.

```
./fork.py -A a+b,b+c,c+d,c+e,c- -c 

ARG seed -1
ARG fork_percentage 0.7
ARG actions 5
ARG action_list a+b,b+c,c+d,c+e,c-
ARG show_tree False
ARG just_final False
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve True

                           Process Tree:
                               a

Action: a forks b
                               a
                               └── b
Action: b forks c
                               a
                               └── b
                                   └── c
Action: c forks d
                               a
                               └── b
                                   └── c
                                       └── d
Action: c forks e
                               a
                               └── b
                                   └── c
                                       ├── d
                                       └── e
Action: c EXITS
                               a
                               ├── b
                               ├── d
                               └── e



./fork.py -A a+b,b+c,c+d,c+e,c- -c -R 

ARG seed -1
ARG fork_percentage 0.7
ARG actions 5
ARG action_list a+b,b+c,c+d,c+e,c-
ARG show_tree False
ARG just_final False
ARG leaf_only False
ARG local_reparent True
ARG print_style fancy
ARG solve True

                           Process Tree:
                               a

Action: a forks b
                               a
                               └── b
Action: b forks c
                               a
                               └── b
                                   └── c
Action: c forks d
                               a
                               └── b
                                   └── c
                                       └── d
Action: c forks e
                               a
                               └── b
                                   └── c
                                       ├── d
                                       └── e
Action: c EXITS
                               a
                               └── b
                                   ├── d
                                   └── e
```

5. One last flag to explore is the -F flag, which skips intermediate steps and only asks to fill in the final process tree. Run ./fork.py -F and see if you can write down the final tree by looking at the series of actions generated. Use different random seeds to try this a few times.
   
답: 모두 같은 결과로 보인다.

```
./fork.py -A a+b,b+c,c+d,c+e,c- -F -c

ARG seed -1
ARG fork_percentage 0.7
ARG actions 5
ARG action_list a+b,b+c,c+d,c+e,c-
ARG show_tree False
ARG just_final True
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve True

                           Process Tree:
                               a

Action: a forks b
Action: b forks c
Action: c forks d
Action: c forks e
Action: c EXITS

                        Final Process Tree:
                               a
                               ├── b
                               ├── d
                               └── e
```

6. Finally, use both -t and -F together. This shows the final process tree, but then asks you to fill in the actions that took place. By looking at the tree, can you determine the exact actions that took place? In which cases can you tell? In which can’t you tell? Try some dif- ferent random seeds to delve into this question.

답: 없다. exit 되는 경우에는 tree에 모든 정보가 포함되지 않는다.

```
./fork.py -s 13 -t  -F -c

ARG seed 13
ARG fork_percentage 0.7
ARG actions 5
ARG action_list 
ARG show_tree True
ARG just_final True
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve True

                           Process Tree:
                               a

Action: a forks b
Action: b forks c
Action: a forks d
Action: a forks e
Action: b forks f

                        Final Process Tree:
                               a
                               ├── b
                               │   ├── c
                               │   └── f
                               ├── d
                               └── e

./fork.py -s 14 -t  -F -c

ARG seed 14
ARG fork_percentage 0.7
ARG actions 5
ARG action_list 
ARG show_tree True
ARG just_final True
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve True

                           Process Tree:
                               a

Action: a forks b
Action: b forks c
Action: a forks d
Action: c EXITS
Action: d forks e

                        Final Process Tree:
                               a
                               ├── b
                               └── d
                                   └── e

```


## Homework (Code)

In this homework, you are to gain some familiarity with the process management APIs about which you just read. Don’t worry – it’s even more fun than it sounds! You’ll in general be much better off if you find as much time as you can to write some code, so why not start now?


1. Write a program that calls fork(). Before calling fork(),have the main process access a variable (e.g., x) and set its value to something (e.g., 100). What value is the variable in the child process? What happens to the variable when both the child and parent change the value of x?

답: 각 프로세스의 variable이 각각의 값을 가진다. 부모 Process에서의 `x`는 100 -> 300, 자식 Process에서의 `x`는 100 -> 200이 된다.

```
./01-question
-------------------------------
Child process
-------------------------------
Value of X: 100
New Value of x: 200
-------------------------------
Parent process
-------------------------------
Value of X: 100
New Value of x: 300
```

2. Write a program that opens a file (with the open() system call) and then calls fork() to create a new process. Can both the child and parent access the file descriptor returned by open()? What happens when they are writing to the file concurrently, i.e., at the same time?

답: 자식 Process 부모 Process 모두 접근가능하다. 먼저 실행되는 순서에 따라서 파일에 작성된다.

[참고: `open()`](https://linux.die.net/man/3/open)

```
The open() function shall return a file descriptor for the named file that is the lowest file descriptor not currently open for that process. The open file description is new, and therefore the file descriptor shall not share it with any other process in the system. The FD_CLOEXEC file descriptor flag associated with the new file descriptor shall be cleared.
```

```
hello.txt
Hello, I am child process. Writing on you
Hello, I am parent process. Writing on you

```

3. Write another program using fork(). The child process should print “hello”; the parent process should print “goodbye”. You should try to ensure that the child process always prints first; can you do this without calling wait() in the parent?

답: 불가능하다. 하지만 [우회하는 방법](https://github.com/yorwos/OSTEP-Solution/blob/master/Chapter%205/question3.c)을 참고할 수 있다.

```
./03-question
Goodby, I am parent. 
Hello, I am child. 
```

4. Write a program that calls fork() and then calls some form of exec() to run the program /bin/ls. See if you can try all of the variants of exec(), including (on Linux) execl(), execle(), execlp(), execv(), execvp(), and execvpe(). Why do you think there are so many variants of the same basic call?

답: 결과값은 모두 동일하다. 다음과 같은 이유로 `exec()`의 Family가 존재한다.

[참고](https://stackoverflow.com/questions/20823371/what-is-the-difference-between-the-functions-of-the-exec-family-of-system-calls/20823413)


5. Now write a program that uses wait() to wait for the child process to finish in the parent. What does wait() return? What happens if you use wait() in the child?

답: 자식 Process에서 `wait()`를 할시에는 큰 특징없이 끝난다. 
`wait()`는 성공시에는 pid를 실패시에는 -1를 리턴한다.

[참고-wait()](https://www.google.com/search?q=wait()+linux&rlz=1C5CHFA_enKR960KR960&oq=wait()+linux&aqs=chrome..69i57j0i22i30l9.2881j1j4&sourceid=chrome&ie=UTF-8)
