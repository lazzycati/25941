#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s command [args...]\n", argv[0]);
        return 1;
    }
    int status;
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Ошибка при создании процесса");
        exit(1);
    }
    else if (pid == 0)
    {
        execvp(argv[1], &argv[1]);
        perror("execvp failed");
        exit(1);
    }
    else
    {
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid failed");
            exit(1);
        }
        if (WIFEXITED(status)) printf("Код завершения: %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status)) {
            printf("Процесс убит сигналом: %d\n", WTERMSIG(status));
        }
    }
    return 0;
}