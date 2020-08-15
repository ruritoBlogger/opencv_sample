#include <iostream>
#include <string>
#include <thread>
#include "queue.cpp"

void add_que(safe_queue<int> &que)
{
    while(true)
    {
	que.push((int)(que.size()));
    }
}

void pop_que(safe_queue<int> &que)
{
    while(true)
    {
	if( !que.empty() ) std::cout << *que.pop().get() << std::endl;
    }
}

int main()
{
    safe_queue<int> que;

    std::thread t1(add_que, std::ref(que));
    std::thread t2(pop_que, std::ref(que));

    t1.join();
    t2.join();
}
