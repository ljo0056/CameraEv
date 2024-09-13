#pragma once

///    
///  @file    CmdLineSetting.h
///  @date    2018/03/06
///  @author  Lee Jong Oh
///

#include <string>
#include <map>

///  @class   CmdLineParser
///  @brief   프로그램 시작 시에 들어오는 명령 인수(파라미터) 를 분리하고 관리 한다.
///           "ex) -LPRMainName:LPREngine-01" 의 형태를 분리 한다.
///           처음 구분자는 '-', '/' 기호 일 시에 인식해야 할 명령 인수로 받아 들이며
///           key 와 value 사이 구분자는 ':" 으로 구분 한다.

//#define __SUPPORT_CMD_LINE_UNICODE

class CmdLineParser
{
private:
#ifdef __SUPPORT_CMD_LINE_UNICODE
    using string_p = std::wstring;
    using char_p   = wchar_t;
#else
    using string_p = std::string;
    using char_p   = char;
#endif

    using MapCmdInfo          = std::map<string_p, string_p>;
    using MapCmdInfoIter      = MapCmdInfo::iterator;
    using MapCmdInfoConstIter = MapCmdInfo::const_iterator;

    MapCmdInfo    m_mapItem;

private:
    bool FindItem(const string_p& key, string_p& value);
    bool ParamDivision(const string_p& param, string_p& key, string_p& value);

public:
    CmdLineParser();
    ~CmdLineParser();

    void ParseCommandLine(int argc, char_p* argv[]);

    bool HasKey(const string_p& key);
    bool ReadCmdLineValue(const string_p& key, int& value);
    bool ReadCmdLineValue(const string_p& key, double& value);
    bool ReadCmdLineValue(const string_p& key, float& value);
    bool ReadCmdLineValue(const string_p& key, string_p& value);
};

