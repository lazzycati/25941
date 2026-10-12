#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> 
#include <stdlib.h>
#define SIZE 32
#define TSIZE 1024

typedef struct Table {
    size_t offset;
    size_t len;
} Table;

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
        printf("pos = %zu, real_pos = %ld\n", pos, lseek(fd, 0L, SEEK_CUR));
    }
    if (pos > start && count_line < TSIZE)
    {
        table[count_line].offset = start;
        table[count_line].len = pos - start;
        count_line++;
    }
    printf("\n%-6s %-10s %-10s\n", "line", "offset", "len");
    printf("%-6s %-10s %-10s\n", "----", "------", "---");
    for (size_t i = 0; i < count_line; i++)
    {
        printf("%-6zu %-10zu %-10zu\n", i + 1, table[i].offset, table[i].len);
    }
    while(1){
        int num_line = 0;
        printf("\nВведите номер строки (0 - выход): ");
        scanf("%d", &num_line);
        if (num_line < 0)
        {
            fprintf(stderr, "number of the line must be >= 0\n");
            return 1;
        }
        if (num_line > count_line)
        {
            fprintf(stderr, "there is no such line\n");
            return 1;
        }
        if (num_line == 0) break;
        lseek(fd, table[num_line - 1].offset, SEEK_SET);
        read(fd, buf, table[num_line - 1].len);
        write(1, buf, table[num_line - 1].len);
    }
    close(fd);
    return 0;
}