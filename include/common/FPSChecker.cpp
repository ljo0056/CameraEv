#include "stdafx.h"
#include "FPSChecker.h"

FPSChecker::FPSChecker()
{
}

FPSChecker::~FPSChecker()
{
    
}

void FPSChecker::SetPeriod(int ms)
{
    m_period = ms;
}

float FPSChecker::GetAnalysisFps() const
{
    return m_analysis_fps;
}

int FPSChecker::GetAnaysisFpsRount() const
{
    return (int)std::round(m_analysis_fps);
}

void FPSChecker::DoCheckFps(const callback_check& callback)
{
    if (0 == m_analysis_fps)    // fps 초기 값은 DoCheckCount() 를 통해서 지정한다.
    {
        DoCheckCount(callback_check());
        return;
    }

    m_count++;

    chrono_tp end = std::chrono::high_resolution_clock::now();
    chrono_duration_milli elapsed = end - m_fps_check;

    int check_count = m_period / (int)std::round(1000 / m_analysis_fps);
    if (0 == check_count)
        return;

    if (0 == (m_count % check_count))
    {
        m_analysis_fps = m_count * 1000 / (float)elapsed.count();
        if (m_analysis_fps > 60)
            m_analysis_fps = 60;

        if (callback)
            callback(elapsed.count(), m_count, m_analysis_fps);

        m_fps_check = std::chrono::high_resolution_clock::now();
        m_count = 0;
    }
}

void FPSChecker::DoCheckCount(const callback_check& callback)
{
    if (chrono_tp() == m_fps_check)
    {
        m_fps_check = std::chrono::high_resolution_clock::now();
        return;
    }

    m_count++;
    
    chrono_tp end = std::chrono::high_resolution_clock::now();
    chrono_duration_milli elapsed = end - m_fps_check;

    if (elapsed.count() >= m_period)
    {
        m_analysis_fps = m_count * m_period / (float)elapsed.count();

        if (callback)
            callback(elapsed.count(), m_count, m_analysis_fps);

        m_fps_check = std::chrono::high_resolution_clock::now();
        m_count = 0;
    }
}