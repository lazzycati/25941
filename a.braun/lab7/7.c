#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#define SIZE 32
#define TSIZE 1024

typedef struct Table {
    size_t offset;
    size_t len;
} Table;

static char* g_data = NULL;
static size_t g_size = 0;
int alarm_count = 0;

void on_alarm(int signo) 
{
    (void)signo;
    char msg[] = "\n[timeout]\n";
    write(1, msg, sizeof msg - 1);
    if (g_data != NULL && g_size > 0) 
    {
        size_t written = 0;
        while (written < g_size) 
        {
            ssize_t w = write(1, g_data + written, g_size - written);
            if (w <= 0) break;
            written += (size_t)w;
        }
    }
    _exit(0);
}

int main(int argc, char **argv)
{
    if (argc != 2) 
    {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        return 1;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) 
    {
        perror("fstat");
        close(fd);
        exit(1);
    }
    if (sb.st_size < 0) 
    {
        fprintf(stderr, "invalid file size\n");
        close(fd);
        exit(1);
    }
    g_size = (size_t)sb.st_size;
    g_data = NULL;
    if (g_size > 0) 
    {
        void *p = mmap(NULL, g_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (p == MAP_FAILED) 
        {
            perror("mmap");
            close(fd);
            exit(1);
        }
        g_data = (char*)p;
    }
    close(fd);
    Table table[TSIZE];
    size_t start = 0, pos = 0, count_line = 0;
    while (pos < g_size) 
    {
        if (g_data[pos] == '\n') 
        {
            if (count_line >= TSIZE) 
            {
                fprintf(stderr, "too many lines\n");
                if (g_data) munmap(g_data, g_size);
                return 1;
            }
            table[count_line].offset = start;
            table[count_line].len = pos - start + 1;
            start = pos + 1;
            count_line++;
        }
        pos++;
    }
    if (pos > start && count_line < TSIZE) {
        table[count_line].offset = start;
        table[count_line].len = pos - start;
        count_line++;
    }
    while (1) 
    {
        int num_line = 0;
        printf("\nВведите номер строки (0 - выход): ");
        if (!alarm_count) {
            signal(SIGALRM, on_alarm);
            alarm(5);
            alarm_count = 1;
        }
        int rc = scanf("%d", &num_line);
        alarm(0);
        if (rc != 1) {
            fprintf(stderr, "invalid input\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF) ;
            continue;
        }
        if (num_line == 0) break;
        if (num_line < 0) {
            fprintf(stderr, "number of the line must be >= 0\n");
            continue;
        }
        if ((size_t)num_line > count_line) 
        {
            fprintf(stderr, "there is no such line\n");
            continue;
        }
        size_t off = table[num_line - 1].offset;
        size_t len = table[num_line - 1].len;
        size_t written = 0;
        while (written < len) 
        {
            ssize_t w = write(1, g_data + off + written, len - written);
            if (w <= 0) 
            {
                perror("write");
                break;
            }
            written += (size_t)w;
        }
    }
    if (g_data != NULL && g_size > 0) munmap(g_data, g_size);
    return 0;
}