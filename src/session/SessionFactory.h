#pragma once

// std c++
#include <functional>
#include <map>

class Session;

//////////////////////////////////////////////////////////////////////////
// class SessionFactory
//////////////////////////////////////////////////////////////////////////

class SessionFactory
{
private:
    typedef std::function<std::shared_ptr<Session>(void)> CreateCallback;
    static std::map<int, CreateCallback>   m_mapSessions;

public:

    static void Initialize();

    static void Register(int type, CreateCallback callback);
    static void Unregister(int type);

    static std::shared_ptr<Session> CreateFactory(int type);
};

