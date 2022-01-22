#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int x;

    x = 100;

    int rc = fork();

    if (rc == 0) // Child
    {
        printf("Hello, I am child. \n");
    }
    else if (rc > 0) // Parent
    {
        printf("Goodby, I am parent. \n");
    }
    else // error
    {
        fprintf(stderr, "Error creating process");
    }
}