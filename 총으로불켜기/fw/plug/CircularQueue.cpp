#include "CircularQueue.h"
#include <iostream>

CircularQueue::CircularQueue(int size)
{
    capacity = size;
    queue = new int[capacity];
    front = 0;
    rear = 0;
}

CircularQueue::~CircularQueue()
{
    delete[] queue;
}

bool CircularQueue::isFull()
{
    return (rear + 1) % capacity == front;
}

bool CircularQueue::isEmpty()
{
    return front == rear;
}

void CircularQueue::enqueue(int value)
{
    if (isFull())
    {
        front = (front + 1) % capacity; // 오래된 항목 제거
    }
    queue[rear] = value;
    rear = (rear + 1) % capacity;
}

void CircularQueue::clear()
{
    front = rear = 0;
}

void CircularQueue::display()
{
    int i = rear - 1;
    if (i < 0)
        i += capacity;
    while (i != front)
    {
        std::cout << queue[i] << " ";
        if (i == 0)
        {
            i = capacity - 1;
        }
        else
        {
            --i;
        }
        if (i == rear)
            break; // 전체 순환 방지
    }
    std::cout << std::endl;
}
bool CircularQueue::hasIncreaseDecreaseEdge(int K)
{
    if (!isFull())
    { // 큐가 가득 찼을 때만 패턴 검사 진행
        return false;
    }

    int minVal = queue[front];
    int maxVal = minVal;
    bool increaseFound = false;

    for (int i = (front + 1) % capacity; i != rear; i = (i + 1) % capacity)
    {
        int curr = queue[i];

        if (!increaseFound)
        {
            if (curr > maxVal)
            {
                maxVal = curr; // 새로운 최대값 갱신
                if (maxVal - minVal >= K)
                {
                    increaseFound = true; // K 이상 증가 패턴 찾음
                }
            }
            else if (curr < minVal)
            {
                minVal = curr; // 아직 증가 패턴을 찾지 못했다면 최소값 갱신
            }
        }
        else
        { // 증가 패턴을 찾은 후 감소 패턴 찾기
            if (maxVal - curr >= K)
            {
                return true; // K 이상 감소 패턴 찾음
            }
        }
    }

    return false; // 전체적인 증가 후 감소 패턴 없음
}
