#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int status;
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Ошибка при создании процесса");
        exit(1);
    }
    else if (pid == 0)
    {
        execlp("cat", "cat", "9.c", (char*)NULL);
        perror("execlp failed");
        exit(1);
    }
    else
    {
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid failed");
            exit(1);
        }
        printf("Родитель завершился.\n");
    }
    return 0;
}