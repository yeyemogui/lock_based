#include <memory>
template<typename T>
class thread_queue
{
    private:
    struct  node
    {
        T data;
        std::unique_ptr<node> next;
        node (T data_) : data(std::move(data_)) {}
    };
    
    std::unique_ptr<node> head;
    node* tail;
    public:
    thread_queue() : tail(nullptr) {}
    thread_queue(const thread_queue& other) = delete;
    thread_queue& operator=(const thread_queue& other) = delete;
    std::shared_ptr<T> try_pop()
    {
        if(!head)
        {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(std::make_shared<T>(std::move(head->data)));
        std::unique_ptr<node> const old_head = std::move(head);
        head = std::move(old_head->next);
        if(!head)
        {
            tail = nullptr;
        }
        return res;

    }

    void push(T new_value)
    {
        std::unique_ptr<node> p(new node(std::move(new_value)));
        node* const new_tail = p.get();
        if(tail)
        {
            tail->next = std::move(p);
        }
        else
        {
            head = std::move(p);
        }
        tail = new_tail;
    }   

    void clear()
    {
        while(try_pop()) {};
    } 
};