#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

#define MAX_LINE 40

static struct termios old_termios; 

void restore_terminal() 
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_termios);
}

void raw_mode() 
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &old_termios);
    atexit(restore_terminal);
    raw = old_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() 
{
    char line[MAX_LINE];
    int len = 0;         
    unsigned char c;
    raw_mode();
    while (read(STDIN_FILENO, &c, 1) == 1) 
    {
        if (c == 0x04) 
        {
            if (len == 0)
                break;
            continue;  
        }
        if (c == 0x7F || c == 0x08) 
        {
            if (len > 0) 
            {
                len--;
                write(STDOUT_FILENO, "\b \b", 3);
            }
            continue;
        }
        if (c == 0x15) 
        {
            while (len > 0) 
            {
                write(STDOUT_FILENO, "\b \b", 3);
                len--;
            }
            continue;
        }
        if (c == 0x17) 
        {
            while (len > 0 && line[len - 1] == ' ') 
            {
                write(STDOUT_FILENO, "\b \b", 3);
                len--;
            }
            while (len > 0 && line[len - 1] != ' ') 
            {
                write(STDOUT_FILENO, "\b \b", 3);
                len--;
            }
            continue;
        }
        if (c >= 0x20 && c < 0x7F) 
        {
            if (len >= MAX_LINE) 
            {
                int word_start = len;
                while (word_start > 0 && line[word_start - 1] != ' ')
                    word_start--;
                if (word_start > 0) 
                {
                    int word_len = len - word_start;
                    char word[MAX_LINE];
                    for (int i = 0; i < word_len; i++)
                        word[i] = line[word_start + i];
                    for (int i = 0; i < word_len; i++)
                        write(STDOUT_FILENO, "\b \b", 3);
                    len = word_start;
                    write(STDOUT_FILENO, "\n", 1);
                    for (int i = 0; i < word_len; i++) 
                    {
                        line[i] = word[i];
                        write(STDOUT_FILENO, &word[i], 1);
                    }
                    len = word_len;
                }
            }
            if (len < MAX_LINE) 
            {
                line[len++] = c;
                write(STDOUT_FILENO, &c, 1);
            }
            continue;
        }
        write(STDOUT_FILENO, "\a", 1);  
    }
    write(STDOUT_FILENO, "\n", 1);
    return 0;
}