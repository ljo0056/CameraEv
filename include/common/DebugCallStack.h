#pragma once

///  @class   DCallStack, DSymbolLookup
///  @brief   디버그 용으로 사용되며 스택에 쌓여있는 함수 호출 리스트를 문자열로 출력한다.
///           출처: https://narss.tistory.com/entry/CallStack을-찍어보자 [날쑤의 연습장]

//////////////////////////////////////////////////////////////////////////
// class DCallStack

class DCallStack final
{
public:
    static const size_t kMaxStackDepth = 16U;

public:
    explicit DCallStack();

    void* operator[] (const size_t index) const { return addresses_[index]; }

    ULONG GetHash() const { return hash_; }
    size_t GetCount() const { return count_; }

private:
    void* addresses_[kMaxStackDepth];
    ULONG hash_;
    size_t count_;
};

//////////////////////////////////////////////////////////////////////////
// class DSymbolLookup

class DSymbolLookup final
{
public:
    DSymbolLookup();

    std::string GetSymbolString(void* address) const;

private:
    const static size_t kStrBufferSize = 1024U;
private:
    HANDLE handle_;
};

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