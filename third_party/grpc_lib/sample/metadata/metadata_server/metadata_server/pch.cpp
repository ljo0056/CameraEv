// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일입니다. 성공하려면 컴파일이 필요합니다.

#include "pch.h"

// 일반적으로 이 파일을 무시하지만 미리 컴파일된 헤더를 사용하는 경우 유지합니다.

#ifdef _DEBUG
#pragma comment(lib, "ws2_32.lib")                // dependency grpc.lib
#pragma comment(lib, "grpc/libprotobufd.lib")
#pragma comment(lib, "grpc/zlibstaticd.lib")      // dependency grpc.lib
#pragma comment(lib, "grpc/gpr.lib")              // dependency grpc.lib
#pragma comment(lib, "grpc/address_sorting.lib")  // dependency grpc.lib
#pragma comment(lib, "grpc/cares.lib")            // dependency grpc.lib
#pragma comment(lib, "grpc/grpc.lib")             // dependency grpc++.lib
#pragma comment(lib, "grpc/grpc++.lib")
#else
#pragma comment(lib, "ws2_32.lib")                // dependency grpc.lib
#pragma comment(lib, "grpc/libprotobuf.lib")
#pragma comment(lib, "grpc/zlibstatic.lib")       // dependency grpc.lib
#pragma comment(lib, "grpc/gpr.lib")              // dependency grpc.lib
#pragma comment(lib, "grpc/address_sorting.lib")  // dependency grpc.lib
#pragma comment(lib, "grpc/cares.lib")            // dependency grpc.lib
#pragma comment(lib, "grpc/grpc.lib")             // dependency grpc++.lib
#pragma comment(lib, "grpc/grpc++.lib")
#endif




