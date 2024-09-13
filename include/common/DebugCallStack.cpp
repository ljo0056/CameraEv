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

        // SYMBOL_INFO ����ü�� �ʿ��� ������ setting
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

    // �ּҿ� �����ϴ� �ɺ��� �����̸� + line�� ������ ��ü      
    IMAGEHLP_LINE64 line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    // SymbolBuffer ��ü�� address�� �����ϴ� �ɺ� �̸�(�Լ���) ����
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

    DCallStack stack; //< ��ü�� �����Ǹ鼭 DCallStack�� ����

    for (size_t i = 0; i < stack.GetCount(); ++i) {
        const string& str = lookup.GetSymbolString(stack[i]);
        cout << str << endl;
    }

    return 0;
}
*/