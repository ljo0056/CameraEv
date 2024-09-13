// VS2017 colsole 모드에서 제작된 샘플 프로그램 입니다.
// By Jong Oh Lee  2019.04.12


#include <iostream>

// fmt define
// dll 로 build 한 fmt 를 사용하기 위해 include 보다 먼저 선언한다.
#define FMT_HEADER_ONLY
#define FMT_SHARED


// fmt include
#include <fmt/format.h>
#include <fmt/printf.h>


#ifdef _DEBUG
    #pragma comment(lib, "fmt/fmt_dlld.lib")
#else
    #pragma comment(lib, "fmt/fmt_dll.lib")
#endif

int main()
{
    std::string s = fmt::format(("{0}"), 42);
    std::cout << "Hello World!\n" << s; 
}

