#pragma once

#include <chrono>
#include <functional>

class FPSChecker 
{
private:
    typedef std::chrono::high_resolution_clock::time_point   chrono_tp;
    typedef std::chrono::duration<double, std::milli>        chrono_duration_milli;

    int                             m_count = 0;
    int                             m_check_fps = 1000;
    chrono_tp                       m_fps_check;

    std::function<void(double ms, int count)>  m_callback;

public:
    FPSChecker();
    ~FPSChecker();

    void SetCheckFPS(int ms);
    void SetCallback(std::function<void(double ms, int count)> callback);

    void DoCheckFPS();
};

