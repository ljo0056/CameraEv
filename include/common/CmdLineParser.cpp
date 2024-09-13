#include "stdafx.h"
#include "CmdLineParser.h"
#include "Util.h"


CmdLineParser::CmdLineParser()
{

}

CmdLineParser::~CmdLineParser()
{

}

bool CmdLineParser::FindItem(const string_p& key, string_p& value)
{
    MapCmdInfoIter iter = m_mapItem.find(key);
    if (iter == m_mapItem.end())
        return false;

    value = iter->second;

    return true;
}

bool CmdLineParser::ParamDivision(const string_p& param, string_p& key, string_p& value)
{
    string_p::size_type pos = param.find(':');
    if (string_p::npos == pos)
    {
        key = param;
        return true;
    }

    key   = param.substr(0, pos);
    value = param.substr(pos + 1, param.length() - (pos + 1));

    return true;
}

void CmdLineParser::ParseCommandLine(int argc, char_p* argv[])
{
    if (nullptr == argv)
        return;

    // Ex) -LPRMainName:LPREngine-01
    for (int i = 1; i < argc; i++)
    {
        char_p* pszParam = argv[i];
        if (pszParam[0] == '-' || pszParam[0] == '/')
        {
            // remove flag specifier
            ++pszParam;
        }

        string_p key, data;
        if (ParamDivision(pszParam, key, data))
        {
            std::transform(key.begin(), key.end(), key.begin(), ::toupper); // 대문자로 관리함
            m_mapItem[key] = data;
        }
    }
}

bool CmdLineParser::HasKey(const string_p& key)
{
    string_p str_value;
    if (false == FindItem(key, str_value))
        return false;

    return true;
}

bool CmdLineParser::ReadCmdLineValue(const string_p& key, int& value)
{
    string_p str_value;
    if (false == FindItem(key, str_value))
        return false;
    value = std::stoi(str_value);

    return true;
}

bool CmdLineParser::ReadCmdLineValue(const string_p& key, double& value)
{
    string_p str_value;
    if (false == FindItem(key, str_value))
        return false;
    value = std::stod(str_value);

    return true;
}

bool CmdLineParser::ReadCmdLineValue(const string_p& key, float& value)
{
    string_p str_value;
    if (false == FindItem(key, str_value))
        return false;
    value = std::stof(str_value);

    return true;
}

bool CmdLineParser::ReadCmdLineValue(const string_p& key, string_p& value)
{
    if (false == FindItem(key, value))
        return false;

    return true;
}


