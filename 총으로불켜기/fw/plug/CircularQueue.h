#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H
#include <cmath>

class CircularQueue
{
public:
    CircularQueue(int size);
    ~CircularQueue();
    void enqueue(int value);
    void clear();
    void display();
    bool hasIncreaseDecreaseEdge(int K);

private:
    int *queue;
    int front;
    int rear;
    int capacity;
    bool isFull();
    bool isEmpty();
};

#endif
