// stdafx.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일입니다. 성공하려면 컴파일이 필요합니다.

#include "stdafx.h"

// 일반적으로 이 파일을 무시하지만 미리 컴파일된 헤더를 사용하는 경우 유지합니다.

#ifdef _DEBUG
#pragma comment(lib, "fmt/fmt_dlld.lib")
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "live555/Live555d.lib")
#pragma comment(lib, "grpc/libprotobufd.lib")
#else
#pragma comment(lib, "fmt/fmt_dll.lib")
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "live555/Live555.lib")
#pragma comment(lib, "grpc/libprotobuf.lib")
#endif
