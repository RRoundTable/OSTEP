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
        int w = wait(NULL);
        printf("-------------------------------\n");
        printf("Child process\n");
        
    }
    else if (rc > 0) // Parent
    {
        int w = wait(NULL);
        printf("-------------------------------\n");
        printf("Parent process\n");
        printf("%d", w);

    }
    else // error
    {
        fprintf(stderr, "Error creating process");
    }
    
}