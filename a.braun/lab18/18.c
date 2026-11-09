#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
//mode_t — целочисленный тип (обычно unsigned int), в котором упакованы тип файла
//  и права. В st_mode младшие 12 бит — права, старшие — тип. 
// Макросы S_IRUSR и т.п. — это маски с одним установленным битом.
static void print_perm(mode_t mode, mode_t r, mode_t w, mode_t x, char *buf, int *pos) {
    buf[(*pos)++] = (mode & r) ? 'r' : '-';
    buf[(*pos)++] = (mode & w) ? 'w' : '-';
    buf[(*pos)++] = (mode & x) ? 'x' : '-';
}

static void mode_to_string(mode_t mode, char out[11]) 
{
    int pos = 0;
    if (S_ISDIR(mode)) out[pos++] = 'd';
    else if (S_ISREG(mode)) out[pos++] = '-';
    else out[pos++] = '?';
    print_perm(mode, S_IRUSR, S_IWUSR, S_IXUSR, out, &pos);
    print_perm(mode, S_IRGRP, S_IWGRP, S_IXGRP, out, &pos);
    print_perm(mode, S_IROTH, S_IWOTH, S_IXOTH, out, &pos);
    out[pos] = '\0';
}

static const char *my_basename(const char *path) 
{
    const char *sl = strrchr(path, '/');
    if (sl && sl[1] != '\0')
        return sl + 1;
    return path;
}

static int print_entry(const char *path) 
{
    struct stat st;
    if (stat(path, &st) == -1) {
        fprintf(stderr, "stat: %s: %s\n", path, strerror(errno));
        return -1;
    }
    char modebuf[11];
    mode_to_string(st.st_mode, modebuf);
    struct passwd *pw = getpwuid(st.st_uid);
    char uidbuf[16];
    const char *owner;
    if (pw) 
    {
        owner = pw->pw_name;
    } 
    else {
        snprintf(uidbuf, sizeof uidbuf, "%u", (unsigned)st.st_uid);
        owner = uidbuf;
    }
    struct group *gr = getgrgid(st.st_gid);
    char gidbuf[16];
    const char *group;
    if (gr) 
    {
        group = gr->gr_name;
    } 
    else {
        snprintf(gidbuf, sizeof gidbuf, "%u", (unsigned)st.st_gid);
        group = gidbuf;
    }
    char sizebuf[32] = "";
    if (S_ISREG(st.st_mode)) {
        snprintf(sizebuf, sizeof sizebuf, "%ld", (long)st.st_size);
    }
    char timebuf[64];
    struct tm *tm = localtime(&st.st_mtime);
    if (tm) {
        strftime(timebuf, sizeof timebuf, "%b %e %H:%M", tm);
    } 
    else {
        snprintf(timebuf, sizeof timebuf, "??? ?? ??:??");
    }
    printf("%-10s %3lu %-8s %-8s %8s %s %s\n",
        modebuf,
        (unsigned long)st.st_nlink,
        owner,
        group,
        sizebuf,
        timebuf,
        my_basename(path));
    return 0;
}

int main(int argc, char **argv) 
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file...\n", argv[0]);
        return 1;
    }
    int status = 0;
    for (int i = 1; i < argc; i++) {
        if (print_entry(argv[i]) == -1) status = 1;
    }
    return status;
}