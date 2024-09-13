#pragma once

///  @class   CommonFormatterImpl
///  @brief   plog 라이브러리의 로그 format 구조를 변경
///  @author  Lee Jong Oh

#include <plog/Record.h>
#include <plog/Util.h>
#include <iomanip>

namespace plog
{
	class CommonFormatterImpl
	{
    private:
        static util::nstring severityToStringShort(Severity severity);
        
	public:
        static util::nstring header();
        static util::nstring format(const Record& record);
	};

	class CommonFormatter : public CommonFormatterImpl {};

    Record& operator<<(Record& record, const bool& value);
    Record& operator<<(Record& record, const CPoint& value);
    Record& operator<<(Record& record, const CRect& value);
}

// Sample code
#if 0
#include <plog/Custom/WinConsoleAppender.h>
#include <plog/Custom/OneDayFileAppender.h>
#include <plog/Appenders/DebugOutputAppender.h>
#include "common/PLogCommon.h"

void InitLog()
{
    tstring process_path;
    GetProcessPath(process_path);
    tstring log_path = process_path + _T("\\LOG");

    static plog::OneDayFileAppender<plog::CommonFormatter>  onedayfile_appender(log_path.c_str());
    static plog::WinConsoleAppender<plog::CommonFormatter>  console_appender;
    static plog::DebugOutputAppender<plog::CommonFormatter> debugoutput_appender;

    plog::Logger<PLOG_DEFAULT_INSTANCE>& logger = plog::init(plog::verbose, &console_appender);

    logger.addAppender(&debugoutput_appender);
    logger.addAppender(&onedayfile_appender);
}

void main()
{
    InitLog();

    LOGI << "Hello Log!!!";

    getchar();
}
#endif

//////////////////////////////////////////////////////////////////////////

#define GET_LOG(instance, severity)    IF_LOG_(instance, severity) (*plog::get<instance>())
#define GET_LOGI                       GET_LOG(PLOG_DEFAULT_INSTANCE, plog::info)

class LogTraceI
{
private:
    std::string    m_log;
    plog::Record   m_recode;

public:
    LogTraceI(const char* func, size_t line, const char* file, const void* object, const std::string& log)
        : m_recode(plog::info, func, line, file, object)
        , m_log(log)
    {
        GET_LOGI += m_recode.ref() << "Start " << m_log;
    }
    ~LogTraceI()
    {
        GET_LOGI += plog::Record(plog::info, m_recode.getFunc(), m_recode.getLine(), m_recode.getFile(), m_recode.getObject()).ref() << "End " << m_log;
    }

    plog::Record& ref()
    {
        return m_recode;
    }
};

#define LOG_TRACE_I(log) LogTraceI log_object_i(PLOG_GET_FUNC(), __LINE__, PLOG_GET_FILE(), PLOG_GET_THIS(), log)
