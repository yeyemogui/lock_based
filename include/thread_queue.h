#include <memory>
#include <mutex>
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

        decltype(auto) get_tail()
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            return tail;
        }

        decltype(auto) pop_head()
        {
            std::unique_ptr<node> res = nullptr;
            std::lock_guard<std::mutex> head_lock(head_mutex);
            if(head.get() != get_tail())
            {
                res = std::move(head);
                head = std::move(res->next);
            }
            return res;
        }
    public:
        thread_queue() : head(new node), tail(head.get()) {}
        
        thread_queue(const thread_queue& other) = delete;
        thread_queue& operator=(const thread_queue& other) = delete;
        decltype(auto) try_pop()
        {
            auto old_head = pop_head();
            return old_head? old_head->data: std::shared_ptr<T>();
        }

        void push(T new_value)
        {
            auto data = std::make_shared<T>(std::move(new_value));
            std::unique_ptr<node> p(new node);
            node* const new_tail = p.get();
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail->data = data;
            tail->next = std::move(p);
            tail = new_tail;
        }   

        void clear()
        {
            while(try_pop()) {};
        } 
};