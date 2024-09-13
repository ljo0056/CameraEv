#ifndef PCH_H
#define PCH_H

// grpc 컴파일 에러 수정
// "Please compile grpc with _WIN32_WINNT of at least 0x600 (aka Windows Vista)"
#include <SDKDDKVer.h>
// end ... 컴파일 에러 수정

#include <plog/Log.h>
#include <plog/Custom/PrintfLog.h>

#endif //PCH_H
