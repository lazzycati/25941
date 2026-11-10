#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("RUID = %d, EUID = %d\n", getuid(), geteuid());
    FILE *f = fopen("input.txt", "r");
    if (!f) perror("fopen");
    else fclose(f);
    if (setuid(getuid()) == -1) perror("setuid");
    printf("RUID = %d, EUID = %d\n", getuid(), geteuid());
    FILE *f2 = fopen("input.txt", "r");
    if (!f2) perror("fopen");
    else fclose(f2);
    return 0;
}