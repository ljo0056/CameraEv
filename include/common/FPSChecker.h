#pragma once

///  @author  Lee Jong Oh
///  @class   FPSChecker
///  @brief   영상의 FPS(Frame Per Second) 를 계산 한다.

#include <chrono>
#include <functional>

class FPSChecker
{
private:
    using chrono_tp             = std::chrono::high_resolution_clock::time_point;
    using chrono_duration_milli = std::chrono::duration<double, std::milli>;
    using callback_check        = std::function<void(double ms, int count, float fps)>;

    int         m_count = 0;
    int         m_period = 1000;       // ms 단위
    float       m_analysis_fps = 0;    // 실제 측정한 fps
    chrono_tp   m_fps_check;

public:
    FPSChecker();
    ~FPSChecker();

    // ms 단위로 fps 체크를 설정 한다. default 는 1000ms(1초)
    void SetPeriod(int ms);

    // 측정한 FPS 값을 return 한다.
    float GetAnalysisFps() const;
    // 측정한 FPS 값을 반올림하여 return 한다.
    int   GetAnaysisFpsRount() const;

    // fps 만큼 Frame 카운트가 되었을 시에 시간을 확인
    // callback_check : 체크 할 fps 마다 호출되는 콜백 함수 -> m_period 값이 1000ms 설정되어 있다면 1000ms 마다 콜백 함수 호출
    void DoCheckFps(const callback_check& callback);

    // fps 시간에 실제로 Count 가 몇번 되었는지 확인
    // callback_check : 체크 할 fps 마다 호출되는 콜백 함수 -> m_period 값이 1000ms 설정되어 있다면 1000ms 마다 콜백 함수 호출
    void DoCheckCount(const callback_check& callback);
};

// Sample code
#if 0

FPSChecker fps_checker;

// FPS 체크를 위해 호출
fps_checker.DoCheckCount([this](double ms, int count, float fps) {
    // 1000ms 마다 호출됨
    LOGI << fmt::format("fps   Time[{1:.1f}] count[{2}] FPS[{3:.1f}]", ms, count, fps);
});

#endif