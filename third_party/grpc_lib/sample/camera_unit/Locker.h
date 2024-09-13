#ifndef LOCKER_H
#define LOCKER_H

#include <mutex>
#include <condition_variable>

/**
 * @brief The UniqueLock class
 * wrapper for mutex and unique_lock.
 */
class Locker {
public:
    ~Locker();
    void wait();
    void wakeup();
    void set_max_count(int count);

private:
    int m_lock_count = 0;
    int m_lock_max_count = 1;
    std::mutex m_mutex;
    std::condition_variable m_condition_variable;
};

#endif // LOCKER_H
