#include "pch.h"
#include "FPSChecker.h"

FPSChecker::FPSChecker()
{

}

FPSChecker::~FPSChecker()
{
    
}

void FPSChecker::SetCheckFPS(int ms)
{
    m_check_fps = ms;
}

void FPSChecker::SetCallback(std::function<void(double ms, int count)> callback)
{
    m_callback = callback;
}

void FPSChecker::DoCheckFPS()
{
    m_count++;

    chrono_tp end = std::chrono::high_resolution_clock::now();
    chrono_duration_milli elapsed = end - m_fps_check;

    if (elapsed.count() >= m_check_fps)
    {
        if (m_callback)
            m_callback(elapsed.count(), m_count);

        m_fps_check = std::chrono::high_resolution_clock::now();
        m_count = 0;
    }
}