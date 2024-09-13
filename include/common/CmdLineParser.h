#pragma once

///    
///  @file    CmdLineSetting.h
///  @date    2018/03/06
///  @author  Lee Jong Oh
///

#include <string>
#include <map>

///  @class   CmdLineParser
///  @brief   ���α׷� ���� �ÿ� ������ ��� �μ�(�Ķ����) �� �и��ϰ� ���� �Ѵ�.
///           "ex) -LPRMainName:LPREngine-01" �� ���¸� �и� �Ѵ�.
///           ó�� �����ڴ� '-', '/' ��ȣ �� �ÿ� �ν��ؾ� �� ��� �μ��� �޾� ���̸�
///           key �� value ���� �����ڴ� ':" ���� ���� �Ѵ�.

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

