#include <memory>
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
    public:
    thread_queue() : head(new node), tail(head.get()) {}
    
    thread_queue(const thread_queue& other) = delete;
    thread_queue& operator=(const thread_queue& other) = delete;
    std::shared_ptr<T> try_pop()
    {
        if(head.get() == tail)
        {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(head->data);
        std::unique_ptr<node> const old_head = std::move(head);
        head = std::move(old_head->next);
        return res;
    }

    void push(T new_value)
    {
        auto data = std::make_shared<T>(std::move(new_value));
        std::unique_ptr<node> p(new node);
        tail->data = data;
        node* const new_tail = p.get();
        tail->next = std::move(p);
        tail = new_tail;
    }   

    void clear()
    {
        while(try_pop()) {};
    } 
};