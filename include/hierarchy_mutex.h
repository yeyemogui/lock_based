#include <mutex>
#include <climits>
class hierarchical_mutex
{
    private:
        std::mutex internal_mutex;
        unsigned long const hierarchy_value;
        unsigned long previous_hierarchy_value;
        static thread_local unsigned long this_thread_hierachy_value;
        void checkViolation()
        {
            this_thread_hierachy_value <= hierarchy_value? 
            throw std::logic_error("mutex hierarchy violated"):;
        }
        void updateHierarchyValue()
        {
            previous_hierarchy_value = this_thread_hierachy_value;
            this_thread_hierachy_value = hierarchy_value;
        }
    public:
        explicit hierarchical_mutex(unsigned long value): 
        hierarchy_value(value), previous_hierarchy_value(0) {}
        void lock()
        {
            checkViolation();
            internal_mutex.lock();
            updateHierarchyValue();
        }
        void unlock()
        {
            if (this_thread_hierachy_value != hierarchy_value)
                throw std::logic_error("mutex hierarchy violated");
            this_thread_hierachy_value = previous_hierarchy_value;
            internal_mutex.unlock();
        }
};
thread_local unsigned long hierarchical_mutex::this_thread_hierachy_value(ULONG_MAX);