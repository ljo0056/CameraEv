#pragma once
#include <plog/Record.h>
#include <plog/Util.h>
#include <iomanip>

namespace plog
{
	class CCommonFormatterImpl
	{
    private:
        static util::nstring severityToStringShort(Severity severity);
        
	public:
        static util::nstring header();		
        static util::nstring format(const Record& record);
	};

	class CCommonFormatter : public CCommonFormatterImpl {};

    Record& operator<<(Record& record, const bool& value);
}
