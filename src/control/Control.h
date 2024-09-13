#pragma once

//////////////////////////////////////////////////////////////////////////
// class Controler
//////////////////////////////////////////////////////////////////////////
/// class Controler description
/// - CCTV 카메라의 제어를 담당 합니다.
/// - 제어는 와이퍼 동작, 리셋, defog, 등등의 제어를 말합니다.

class Controler
{
private:

public:
    Controler();
    ~Controler();

    int Activate();
    int Deactivate();
};