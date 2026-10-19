#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#define M 1024

struct Node {
    char* str;
    Node* next;
};

bool Control(const char* s) 
{
    for (; *s; ++s)
        if ((unsigned char)*s < 0x20 && *s != '\n' && *s != '\t')
            return true;
    return false;
}

Node* AddNode(Node* prev, char* buf)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        std::cerr << "Ошибка выделения памяти" << std::endl;
        return nullptr;
    }
    size_t len = strlen(buf);
    newNode->str = (char*)malloc(len + 1);
    if (!newNode->str)
    {
        std::cerr << "Ошибка выделения памяти под строку" << std::endl;
        free(newNode);
        return nullptr;
    }
    strcpy(newNode->str, buf);
    newNode->next = nullptr;
    if (prev)
        prev->next = newNode;
    return newNode;
}

void PrintList(Node* head)
{
    if (head == nullptr)
    {
        std::cout << "Список пуст" << std::endl;
        return;
    }
    while (head != nullptr)
    {
        std::cout << head->str << '\n';
        head = head->next;
    }
}

void FreeList(Node* head)
{
    if (head == nullptr) return;
    Node* curr = head;
    while (curr != nullptr)
    {
        Node* t = curr->next;
        free(curr->str);
        free(curr);        
        curr = t;
    }
}

int main()
{
    char buf[M];
    Node* head = nullptr;
    Node* prev = head;
    while (true)
    {
        printf("%s\n", "Введите строку. Для завершения программы введите '.'");
        if (!fgets(buf, M, stdin)) break;
        if (!strchr(buf, '\n') && !feof(stdin)) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            std::cout << "Строка слишком длинная, обрезана\n";
        }
        buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, ".") == 0) break;
        if (Control(buf)) 
        {
            std::cout << "Недопустимые управляющие символы, повторите ввод\n";
            continue;
        }
        prev = AddNode(prev, buf);
        if (!prev) { 
            std::cerr << "Ошибка памяти\n"; 
            FreeList(head); 
            return 1; 
        }
        if (head == nullptr) head = prev;
    }
    PrintList(head);
    FreeList(head);
    return 0;
}