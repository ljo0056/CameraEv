#pragma once

//////////////////////////////////////////////////////////////////////////
// class PTZ
//////////////////////////////////////////////////////////////////////////
/// class PTZ description
/// - CCTV 카메라의 PTZ 제어를 담당 합니다.
/// - 제어는 pan tilt zoom, preset 이동, 연속 이동, 절대 좌표 이동등 CCTV 카메라의 움직임과 관련이 있습니다.

class PTZ
{
private:


public:
    PTZ();
    ~PTZ();

    int Activate();
    int Deactivate();
};