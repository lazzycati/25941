#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#define SIZE 32
#define TSIZE 1024

typedef struct Table {
    size_t offset; 
    size_t len;
} Table;

static int g_fd = -1;

void on_alarm(int signo) {
    (void)signo;
    char msg[] = "\n[timeout]\n";
    write(1, msg, sizeof msg - 1);
    char buf[TSIZE];
    ssize_t r;
    lseek(g_fd, 0, SEEK_SET);
    while ((r = read(g_fd, buf, sizeof buf)) > 0)
        write(1, buf, r);
    _exit(0);
}

int main(int argc, char **argv)
{
    if (argc != 2) { 
        fprintf(stderr, "usage: %s file\n", argv[0]); 
        return 1; 
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    g_fd = fd;
    Table table[TSIZE];
    size_t start = 0, pos = 0, count_line = 0, n = 0;
    char buf[SIZE];
    while ((n = read(fd, buf, SIZE)) > 0)
    {
        for (size_t i = 0; i < n; i++)
        {
            if (buf[i] == '\n')
            {
                if (count_line >= TSIZE) {
                    fprintf(stderr, "to many lines\n");
                    return 1;
                }
                table[count_line].offset = start;
                table[count_line].len = pos - start + 1; 
                start = pos + 1;  
                count_line++;
            }
            pos++;
        }
    }
    if (pos > start && count_line < TSIZE)
    {
        table[count_line].offset = start;
        table[count_line].len = pos - start;
        count_line++;
    }
    while(1){
        int num_line = 0;
        signal(SIGALRM, on_alarm);
        printf("\nВведите номер строки (0 - выход): ");
        fflush(stdout);
        alarm(5);
        scanf("%d", &num_line);
        if (num_line == 0) break;
        if (num_line < 0)
        {
            fprintf(stderr, "number of the line must be >= 0\n");
            return 1;
        }
        if (num_line >= count_line)
        {
            fprintf(stderr, "there is no such line\n");
            return 1;
        }
        lseek(fd, table[num_line - 1].offset, SEEK_SET);
        read(fd, buf, table[num_line - 1].len);
        write(1, buf, table[num_line - 1].len);
    }
    close(fd);
    return 0;
}