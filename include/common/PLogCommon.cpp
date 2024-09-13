#include "stdafx.h"
#include "PLogCommon.h"

namespace plog
{
    util::nstring CommonFormatterImpl::severityToStringShort(Severity severity)
    {
        switch (severity)
        {
        case fatal:
            return util::nstring(PLOG_NSTR("F"));
        case error:
            return util::nstring(PLOG_NSTR("E"));
        case warning:
            return util::nstring(PLOG_NSTR("W"));
        case info:
            return util::nstring(PLOG_NSTR("I"));
        case debug:
            return util::nstring(PLOG_NSTR("D"));
        case verbose:
            return util::nstring(PLOG_NSTR("V"));
        default:
            return util::nstring(PLOG_NSTR("N"));
        }

        return util::nstring();
    }

    util::nstring CommonFormatterImpl::header()
    {
        return util::nstring(PLOG_NSTR("[CTec Log] Version 1.0\n"));
    }

    util::nstring CommonFormatterImpl::format(const Record& record)
    {
        tm t;
        util::nostringstream ss;

        util::localtime_s(&t, &record.getTime().time);

        // yyyy-mm-dd
        //ss <<
        //    t.tm_year + 1900 << "-" <<
        //    std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-") <<
        //    std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");

        std::string func = record.getFunc();
        size_t pos = func.find("<lambda_");
        if (std::string::npos != pos)   // lambda 함수 이름을 축약함.
        {
            size_t pos2 = func.find("()", pos);
            func.replace(func.begin() + pos, func.begin() + pos2 + 2, "<lambda>");
        }

        // hh:mm:ss.mmm
        ss << 
            std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << 
            std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") <<
            std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".") <<
            std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm << PLOG_NSTR(" ");

        // log level
        ss << std::setfill(PLOG_NSTR(' ')) << std::setw(1) << std::left << severityToStringShort(record.getSeverity()) << PLOG_NSTR(" ");
        // [thread id]
        ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
        // [func_name@source_line]
        ss << PLOG_NSTR("[") << func.c_str() << PLOG_NSTR("@") << record.getLine() << PLOG_NSTR("] ");
        // message
        ss << record.getMessage() << PLOG_NSTR("\n");

        return ss.str();
    }

    Record& operator<<(Record& record, const bool& value)
    {
        if (true == value)
            return record << "true";
        
        return record << "false";
    }

    Record& operator<<(Record& record, const CPoint& value)
    {
        return record << "(" << value.x << "," << value.y << ")";
    }

    Record& operator<<(Record& record, const CRect& value)
    {
        return record << "(" << value.left << "," << value.top << " " << value.Width() << "x" << value.Height() << ")";
    }
}

