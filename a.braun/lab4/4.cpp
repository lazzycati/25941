#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#define M 1024

struct Node {
    char* str;
    Node* next;
};

Node* AddNode(Node* prev, char* buf)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        std::cerr << "Ошибка выделения памяти" << std::endl;
        exit(1);
    }
    size_t len = strlen(buf);
    newNode->str = (char*)malloc(len + 1);
    if (!newNode->str)
    {
        std::cerr << "Ошибка выделения памяти под строку" << std::endl;
        free(newNode);
        exit(1);
    }
    strcpy(newNode->str, buf);
    newNode->next = nullptr;
    if (prev != nullptr)
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
        std::cout << head->str;
        head = head->next;
    }
}

void FreeList(Node* head)
{
    if (head == nullptr)
    {
        std::cout << "Список пуст" << std::endl;
        return;
    }
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
        if (!fgets(buf, M, stdin)) break;
        if (buf[0] == '.') break;
        prev = AddNode(prev, buf);
        if (head == nullptr) head = prev;
    }
    PrintList(head);
    FreeList(head);
    return 0;
}