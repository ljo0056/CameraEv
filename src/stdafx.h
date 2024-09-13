#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // live555 컴파일 에러
#endif

#include <atltypes.h>
#include <atlstr.h>

#ifdef _DEBUG
// Memory leak 을 찾는 라이브러리
// https://kinddragon.github.io/vld/
// 필요할때만 include 하자. 그 이유는 굉장히 느리다.
// #include "vld/vld.h"
#endif

// log
#include <plog/Log.h>
#include <plog/Custom/PrintfLog.h>

// fmt
#define FMT_HEADER_ONLY
#define FMT_SHARED
#include <fmt/format.h>
#include <fmt/printf.h>

#include "GlobalDefine.h"
