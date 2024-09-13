#include "stdafx.h"
#include "grpc_define.h"

#ifdef _DEBUG
#pragma comment(lib, "ws2_32.lib")                // dependency grpc.lib
#pragma comment(lib, "debug/grpc/libprotobufd.lib")
#pragma comment(lib, "debug/grpc/zlibstaticd.lib")      // dependency grpc.lib
#pragma comment(lib, "debug/grpc/gpr.lib")              // dependency grpc.lib
#pragma comment(lib, "debug/grpc/address_sorting.lib")  // dependency grpc.lib
#pragma comment(lib, "debug/grpc/cares.lib")            // dependency grpc.lib
#pragma comment(lib, "debug/grpc/grpc.lib")             // dependency grpc++.lib
#pragma comment(lib, "debug/grpc/grpc++.lib")
#else
#pragma comment(lib, "ws2_32.lib")                // dependency grpc.lib
#pragma comment(lib, "release/grpc/libprotobuf.lib")
#pragma comment(lib, "release/grpc/zlibstatic.lib")       // dependency grpc.lib
#pragma comment(lib, "release/grpc/gpr.lib")              // dependency grpc.lib
#pragma comment(lib, "release/grpc/address_sorting.lib")  // dependency grpc.lib
#pragma comment(lib, "release/grpc/cares.lib")            // dependency grpc.lib
#pragma comment(lib, "release/grpc/grpc.lib")             // dependency grpc++.lib
#pragma comment(lib, "release/grpc/grpc++.lib")
#endif




