#include "pch.h"
#include "Locker.h"

Locker::~Locker()
{
    m_condition_variable.notify_all();
}

void Locker::wait()
{
    std::unique_lock<std::mutex> locker(m_mutex);
    m_condition_variable.wait(locker, [this]
    {
        bool ret = false;
        if (m_lock_count > 0)
        {
            ret = true;
            m_lock_count--;
        }

        return ret;
    });
}

void Locker::wakeup()
{
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_lock_count = m_lock_max_count;
    }
    m_condition_variable.notify_one();
}

void Locker::set_max_count(int count)
{
    m_lock_max_count = count;
}