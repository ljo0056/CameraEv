#include "stdafx.h"
#include "DebugCallStack.h"


#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

//////////////////////////////////////////////////////////////////////////
// struct SymbolBuffer

struct SymbolBuffer : public SYMBOL_INFO
{
    SymbolBuffer()
    {
        SYMBOL_INFO* info = this;
        memset(info, 0, sizeof(SYMBOL_INFO));

        // SYMBOL_INFO 구조체에 필요한 정보를 setting
        MaxNameLen   = static_cast<ULONG>(kBufferSize);
        SizeOfStruct = sizeof(SYMBOL_INFO);

        memset(buffer_, 0, sizeof(buffer_));
    }

    static const size_t kBufferSize = 256U;
    char buffer_[kBufferSize];
};

//////////////////////////////////////////////////////////////////////////
// class DCallStack

DCallStack::DCallStack()
{
    count_ = CaptureStackBackTrace(0, kMaxStackDepth, addresses_, &hash_);
}

//////////////////////////////////////////////////////////////////////////
// class DSymbolLookup

DSymbolLookup::DSymbolLookup() 
    : handle_(GetCurrentProcess())
{
    SymInitialize(handle_, nullptr, TRUE);
    SymSetOptions(SYMOPT_LOAD_LINES);
}

std::string DSymbolLookup::GetSymbolString(void* address) const
{
    if (address == nullptr) {
        return "";
    }

    DWORD displacement = 0;
    DWORD64 addr = reinterpret_cast<DWORD64>(address);
    SymbolBuffer symbol;
    char buffer[kStrBufferSize] = { 0, };

    // 주소에 대응하는 심볼의 파일이름 + line을 저장할 객체      
    IMAGEHLP_LINE64 line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    // SymbolBuffer 객체에 address에 대응하는 심볼 이름(함수명) 저장
    SymFromAddr(handle_, addr, 0, &symbol);

    BOOL ret = SymGetLineFromAddr64(handle_, addr, &displacement, &line);
    if (ret) {
        sprintf_s(buffer, kStrBufferSize, "%s(%d) : %s",
            line.FileName, line.LineNumber, symbol.Name);
    }
    else {
        sprintf_s(buffer, kStrBufferSize, "No line info : %s", symbol.Name);
    }

    return std::string(buffer);
}


/*
// Sample source code...
#include <iostream>
#include <string>
using namespace std;
 
int main(int argc, char* argv[])
{
    DSymbolLookup lookup;

    DCallStack stack; //< 객체가 생성되면서 DCallStack을 찍음

    for (size_t i = 0; i < stack.GetCount(); ++i) {
        const string& str = lookup.GetSymbolString(stack[i]);
        cout << str << endl;
    }

    return 0;
}
*/