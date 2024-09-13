// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

/*
[Todo : JongOh 2019.04.12]
   - 프로젝트 전처리기에서 "#define FMT_EXPORT" 선언을 해주어야 dll 로 빌드가 된다.

   - 빌드된 dll 을 사용하는 곳에서는 아래와 같이 include 하기 전에 define 설정을 해주어야 한다.

      #define FMT_HEADER_ONLY
      #define FMT_SHARED

      #include <fmt/format.h>
      #include <fmt/printf.h>

*/

// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
