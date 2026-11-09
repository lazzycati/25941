#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#define M 1024

struct Node {
    char* str;
    Node* next;
};

<<<<<<< HEAD
=======
bool Control(const char* s) 
{
    for (; *s; ++s)
        if ((unsigned char)*s < 0x20 && *s != '\n' && *s != '\t')
            return true;
    return false;
}

>>>>>>> 58644de66f2dab601a64d4813d28457c710c8625
Node* AddNode(Node* prev, char* buf)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        std::cerr << "Ошибка выделения памяти" << std::endl;
<<<<<<< HEAD
        exit(1);
=======
        return nullptr;
>>>>>>> 58644de66f2dab601a64d4813d28457c710c8625
    }
    size_t len = strlen(buf);
    newNode->str = (char*)malloc(len + 1);
    if (!newNode->str)
    {
        std::cerr << "Ошибка выделения памяти под строку" << std::endl;
        free(newNode);
<<<<<<< HEAD
        exit(1);
    }
    strcpy(newNode->str, buf);
    newNode->next = nullptr;
    if (prev != nullptr)
=======
        return nullptr;
    }
    strcpy(newNode->str, buf);
    newNode->next = nullptr;
    if (prev)
>>>>>>> 58644de66f2dab601a64d4813d28457c710c8625
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
<<<<<<< HEAD
        std::cout << head->str;
=======
        std::cout << head->str << '\n';
>>>>>>> 58644de66f2dab601a64d4813d28457c710c8625
        head = head->next;
    }
}

void FreeList(Node* head)
{
<<<<<<< HEAD
    if (head == nullptr)
    {
        std::cout << "Список пуст" << std::endl;
        return;
    }
=======
    if (head == nullptr) return;
>>>>>>> 58644de66f2dab601a64d4813d28457c710c8625
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
<<<<<<< HEAD
        printf("%s\n" "Введите строкую Для завершения программы введите '.'");
        if (!fgets(buf, M, stdin)) break;
        if (buf[0] == '.') break;
        prev = AddNode(prev, buf);
=======
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

>>>>>>> 58644de66f2dab601a64d4813d28457c710c8625
        if (head == nullptr) head = prev;
    }
    PrintList(head);
    FreeList(head);
    return 0;
}