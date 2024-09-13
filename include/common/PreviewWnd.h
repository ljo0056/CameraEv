#pragma once

///  @class   PreviewWnd
///  @brief   Console 프로그램에서 영상의 디스플레이를 확인 하는 모듈 입니다.
///  @author  Lee Jong Oh

class PreviewWnd
{
private:
    class PreviewWndImpl;
    std::shared_ptr<PreviewWndImpl>     m_impl;

public:
    PreviewWnd();
    ~PreviewWnd();

    ///  @brief      초기화, 종료 시킨다.
    ///  @return     성공 시에 0, 실패 시에 1이상 값을 리턴
    int Initialize();
    int Finalize();

    ///  @brief      활성화, 비활성화 시킨다. Initialize() 선행 호출후에 사용해야 한다.
    ///  @return     성공 시에 0, 실패 시에 1이상 값을 리턴
    int Activate();
    int Deactivate();

    ///  @brief      영상 데이터를 넣어주면 윈도우에 그려준다.
    ///  @param data[in] : BGR24 타입의 Image 데이터
    ///  @param width[in] : 영상의 넓이
    ///  @param height[in] : 영상의 높이
    ///  @return     성공 시에 0, 실패 시에 1이상 값을 리턴
    int SetImage(byte* data, int width, int height);
    int DrawImage(byte* data, int width, int height);
};