#include <iostream>

struct list
{
int ring;
char from;
char to;

list* next;
list* prev; 
};

list* head = nullptr;
list* tail = nullptr;

void AddNode(int ring, char from, char to)
{
    list* node = new list;
    node->ring = ring;
    node->from = from;
    node->to = to;   // budem chitat'


    if (tail == nullptr) 
        head = node;
    else
        tail->next = node; 

    tail = node;

}


void perestavka(int n, char from, char to, char via) // n колец с from на to через via 
{
    if(n==1)
    {
        AddNode(n, from, to);
        return;
    }
    perestavka(n-1, from, via, to); 
    AddNode(n, from, to);
    perestavka(n-1,via, to, from);
}




int main(int argc, char* argv[])
{
perestavka(std::atoi(argv[1]),argv[2][0], argv[3][0], argv[4][0]);


list* cur = head;
    while (cur != nullptr) {
        std::cout << "ring " << cur->ring << ": " << cur->from << " -> " << cur->to << "\n";
        cur = cur->next;
    }


// чистим память


cur = head;
while (cur != nullptr) {
    list* next = cur->next;
    delete cur;
    cur = next;
}


}