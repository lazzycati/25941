#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <string.h>
#include <errno.h>

extern char **environ;

void Print_UID_GID() {
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());
    printf("Real GID: %d\n", getgid());
    printf("Effective GID: %d\n", getegid());
}

void Become_Leader() 
{
    if (setpgid(0, 0) == -1) perror("setpgid");
    else printf("Процесс стал лидером группы, PGID = %d\n", getpgrp());
}

void Print_PIDs() 
{
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());
    printf("PGID: %d\n", getpgrp());
}

void Print_ulimit() 
{
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == 0) 
    {
        printf("ulimit soft: %u\n", (unsigned)rl.rlim_cur);
        printf("ulimit hard: %u\n", (unsigned)rl.rlim_max);
    } 
    else perror("getrlimit");
}

void Change_ulimit(const char *arg) 
{
    if (arg[0] == '-') {
        fprintf(stderr, "Некорректное значение для -U: '%s'\n", arg);
        return;
    }
    char *end;
    errno = 0;
    unsigned long long val = strtoull(arg, &end, 10);
    if (errno != 0 || *end != '\0' || end == arg) 
    {
        fprintf(stderr, "Некорректное значение для -U: '%s'\n", arg);
        return;
    }
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == -1) 
    {
        perror("getrlimit");
        return;
    }
    rl.rlim_cur = val;
    if (setrlimit(RLIMIT_CORE, &rl) == -1) perror("setrlimit");
    else printf("ulimit изменен на %u\n", (unsigned)val);
}

void Print_core_size() 
{
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == 0) 
    {
        printf("Core file size: %u bytes\n", (unsigned)rl.rlim_cur);
    } 
    else perror("getrlimit");
}

void Change_core_size(const char *arg) 
{
    rlim_t newval = (rlim_t)atol(arg);
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == -1) 
    {
        perror("getrlimit");
        return;
    }
    rl.rlim_cur = newval;
    if (setrlimit(RLIMIT_CORE, &rl) == -1) perror("setrlimit");
    else printf("Размер core файла изменен на %lu\n", (unsigned long)newval);
}

void Print_cwd() {
    char buf[1024];
    if (getcwd(buf, sizeof(buf)) != NULL) printf("%s\n", buf);
    else perror("getcwd");
}

void Print_env() {
    for (char **ep = environ; *ep != NULL; ep++) printf("%s\n", *ep);
}

void Set_env(const char *arg) 
{
    char *copy = strdup(arg);          
    if (copy == NULL) 
    {
        perror("strdup");
        return;
    }
    char *p = strchr(copy, '=');
    if (p == NULL) 
    {
        fprintf(stderr, "Invalid format: use -Vname=value\n");
        free(copy);
        return;
    }
    *p = '\0';
    char *name = copy;
    char *value = p + 1;
    if (setenv(name, value, 1) == -1) perror("setenv");
    else printf("Set %s=%s\n", name, value);
    free(copy);
}

int main(int argc, char *argv[]) 
{
    if (argc == 1) 
    {
        printf("Программа вызвана без аргументов\n");
        return 0;
    }
    int c;
    while ((c = getopt(argc, argv, "ispduU:cC:vV:")) != -1) {
        switch (c) 
        {
            case 'i': 
                Print_UID_GID(); 
                break;
            
            case 's': 
                Become_Leader(); 
                break;
            
            case 'p': 
                Print_PIDs(); 
                break;
            
            case 'u': 
                Print_ulimit(); 
                break;
            
            case 'U': 
                Change_ulimit(optarg); 
                break;
            
            case 'c': 
                Print_core_size(); 
                break;
            
            case 'C': 
                Change_core_size(optarg); 
                break;
            
            case 'd': 
                Print_cwd(); 
                break;
            
            case 'v': 
                Print_env(); 
                break;
            
            case 'V': 
                Set_env(optarg); 
                break;
            
            case '?':
                if (optopt == 'U' || optopt == 'C' || optopt == 'V') fprintf(stderr, "Опция -%c без аргумента.\n", optopt);
                else  fprintf(stderr, "Неизвестная опция -%c\n", optopt);
                break;
            default:
                break;
        }
    }
    return 0;
}