#pragma once

///  @class   DCallStack, DSymbolLookup
///  @brief   ����� ������ ���Ǹ� ���ÿ� �׿��ִ� �Լ� ȣ�� ����Ʈ�� ���ڿ��� ����Ѵ�.
///           ��ó: https://narss.tistory.com/entry/CallStack��-���� [������ ������]

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

    DCallStack stack; //< ��ü�� �����Ǹ鼭 DCallStack�� ����

    for (size_t i = 0; i < stack.GetCount(); ++i) {
        const string& str = lookup.GetSymbolString(stack[i]);
        cout << str << endl;
    }

    return 0;
}
*/