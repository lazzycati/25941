#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

extern char **environ;

int my_execvpe(const char *file, char *const argv[], char *const envp[]) 
{
    if (strchr(file, '/') != NULL) 
    {
        execve(file, argv, envp);
        return -1; 
    }
    const char *path = NULL;
    for (char *const *e = envp; *e != NULL; e++) 
    {
        if (strncmp(*e, "PATH=", 5) == 0) 
        {
            path = *e + 5;
            break;
        }
    }
    if (path == NULL) path = getenv("PATH");
    char *path_copy = strdup(path); 
    if (path_copy == NULL) return -1;
    int new_errno = ENOENT;
    char *dir = strtok(path_copy, ":");
    while (dir != NULL) 
    {
        size_t len = strlen(dir) + 1 + strlen(file) + 1;
        char *full = malloc(len);
        if (full == NULL) { 
            free(path_copy); 
            return -1; 
        }
        snprintf(full, len, "%s/%s", dir, file);
        execve(full, argv, envp);
        if (errno != ENOENT) new_errno = errno; 
        free(full);
        dir = strtok(NULL, ":");
    }
    free(path_copy);
    errno = new_errno;
    return -1;
}

int main(int argc, char *argv[]) 
{
    char *cmd;
    char **cmd_argv;
    char *envp[3];
    int use_new_env = 0;
    if (argc >= 4 && strcmp(argv[1], "--env") == 0) 
    {
        use_new_env = 1;
        const char *new_path = argv[2];
        cmd = argv[3];
        cmd_argv = &argv[3];
        char path_buf[1024];
        snprintf(path_buf, sizeof(path_buf), "PATH=%s", new_path);
        envp[0] = path_buf;
        envp[1] = "TESTVAR=OK";
        envp[2] = NULL;
    } 
    else if (argc >= 2) 
    {
        cmd = argv[1];
        cmd_argv = &argv[1];
    } 
    else 
    {
        fprintf(stderr,
            "Usage:\n"
            "  %s command [args...]\n"
            "  %s --env PATH command [args...]\n", argv[0], argv[0]);
        return 1;
    }
    pid_t pid = fork();
    if (pid == -1) 
    {
        perror("fork");
        return 1;
    }
    else if (pid == 0) 
    {
        if (use_new_env)
            my_execvpe(cmd, cmd_argv, envp);
        else
            my_execvpe(cmd, cmd_argv, environ);
        perror("my_execvpe");
        exit(1);
    }
    else 
    {
        int status;
        if (waitpid(pid, &status, 0) == -1) 
        {
            perror("waitpid");
            return 1;
        }
        if (WIFEXITED(status))
            printf("Код завершения: %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("Убит сигналом: %d\n", WTERMSIG(status));
    }
    return 0;
}
