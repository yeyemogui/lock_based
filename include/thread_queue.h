#include <memory>
#include <mutex>
#include <condition_variable>
template<typename T>
class thread_queue
{
    private:
        struct  node
        {
            std::shared_ptr<T> data;
            std::unique_ptr<node> next;
        };
        
        std::unique_ptr<node> head;
        node* tail;

        std::mutex head_mutex;
        std::mutex tail_mutex;
        std::condition_variable data_cond;

        decltype(auto) get_tail()
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            return tail;
        }

        decltype(auto) pop_head()
        {
            auto old_head = std::move(head);
            head = std::move(old_head->next);
            return old_head;
        }

        std::unique_lock<std::mutex> wait_for_data()
        {
            std::unique_lock<std::mutex> head_lock(head_mutex);
            data_cond.wait(head_lock, [&] {return head.get() != get_tail();});
            return std::move(head_lock);
        }

        decltype(auto) wait_pop_head()
        {
            std::unique_lock<std::mutex> head_lock(wait_for_data());
            return pop_head();
        }
    public:
        thread_queue() : head(new node), tail(head.get()) {}
        
        thread_queue(const thread_queue& other) = delete;
        thread_queue& operator=(const thread_queue& other) = delete;
        decltype(auto) wait_and_pop()
        {
            auto old_head = wait_pop_head();
            return old_head->data;
        }

        void push(T new_value)
        {
            auto data = std::make_shared<T>(std::move(new_value));
            std::unique_ptr<node> p(new node);
            node* const new_tail = p.get();
            {
                std::lock_guard<std::mutex> tail_lock(tail_mutex);
                tail->data = data;
                tail->next = std::move(p);
                tail = new_tail;
            }
            data_cond.notify_one();
        }   

        void clear()
        {
            std::lock(head_mutex, tail_mutex);
            std::lock_guard<std::mutex> head_lock(head_mutex, std::adopt_lock);
            std::lock_guard<std::mutex> tail_lock(tail_mutex, std::adopt_lock);
            while(head.get() != tail)
            {
                pop_head();
            }
        } 
};