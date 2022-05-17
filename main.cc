#include "thread_queue.h"
#include <iostream>
#include <thread>
using namespace std;

thread_queue<int> queue_data;
void produce_queue()
{
    const unsigned int numItems = 20;
    for(unsigned int i = 0; i < numItems; i++)
    {
        queue_data.push(i);
        cout << "produce thread " << std::this_thread::get_id() << " - produced data: " << i << endl;
    };
}

void consume_queue()
{
    while(true)
    {
        auto dataPtr = queue_data.try_pop();
        if(dataPtr)
        {
            cout << "consume thread: " << std::this_thread::get_id() << " - consumed data: " << *dataPtr << endl;
        }
        else
        {
            cout << "no data!" << endl;
            return;
        }
    }
}

int main()
{
    queue_data.clear();
    while(true)
    {
        std::thread a(produce_queue);
        std::thread b(consume_queue);
        std::thread c(consume_queue);
    
        a.join();
        b.join();
        c.join();
    }

   
    return 0;
}