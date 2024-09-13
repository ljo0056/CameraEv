#pragma once
#include <plog/Appenders/IAppender.h>
#include <plog/Converters/UTF8Converter.h>
#include <plog/Util.h>
#include <algorithm>

namespace plog
{
	template<class Formatter, class Converter = UTF8Converter>
	class OneDayFileAppender : public IAppender
	{
	public:
		OneDayFileAppender(const util::nchar* rootPath)
			: m_rootPath(rootPath)
			, m_fileExt(PLOG_NSTR("log"))
		{
            checkRootPath();
		}

#ifdef _WIN32
		OneDayFileAppender(const char* rootPath)
			: m_rootPath(util::toWide(rootPath))
			, m_fileExt(PLOG_NSTR("log"))
		{
            checkRootPath();
		}
#endif

		virtual void write(const Record& record)
		{
			util::MutexLock lock(m_mutex);

			openLogFile();

			m_file.write(UTF8Converter::convert(Formatter::format(record)));
		}

	private:
		void checkRootPath()
		{
            #ifdef _WIN32
			if (m_rootPath.empty())
			{
				util::nchar szPath[256] = { 0, };
				GetModuleFileNameW(NULL, szPath, _countof(szPath));

				util::nchar* pPos = _tcsrchr(szPath, PLOG_NSTR('\\'));
				*pPos = NULL;

                m_rootPath = szPath;
                m_rootPath += PLOG_NSTR("\\LOG");
			}

            CreateDirectoryW(m_rootPath.c_str(), NULL);
            #else
            std::array<char, 516> pwd;
            getcwd(pwd.data(), pwd.size());
            strcat(pwd.data(), "/");

            m_rootPath = szPath;
            m_rootPath += PLOG_NSTR("/LOG");

            mkdir(m_rootPath.c_str(), 0755);
            #endif
		}

		void openLogFile()
		{
			util::nstring fileName = buildFileName();

            if (m_fileName != fileName)
            {
                m_file.close();

                off_t fileSize = m_file.open(fileName.c_str());
                m_fileName = fileName;

                if (0 == fileSize)
                {
                    m_file.write(Converter::header(Formatter::header()));
                }
            }			
		}

		util::nstring buildFileName()
		{
			util::nostringstream ss;
			ss << m_rootPath;
            #ifdef _WIN32
			if (m_rootPath.back() != PLOG_NSTR('\\'))
			{
				ss << PLOG_NSTR('\\');
			}
            #else
            if (m_rootPath.back() != PLOG_NSTR('/'))
            {
                ss << PLOG_NSTR('/');
            }
            #endif

			tm t;
            util::Time utime;
            util::ftime(&utime);

			util::localtime_s(&t, &utime.time);
			// yyyy-mm-dd
			ss << t.tm_year + 1900 << "-"
			   << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-")
			   << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");

			if (!m_fileExt.empty())
			{
				ss << '.' << m_fileExt;
			}

			return ss.str();
		}

	private:
		util::Mutex     m_mutex;
        util::File      m_file;
		util::nstring   m_fileExt;
        util::nstring   m_fileName;
		util::nstring   m_rootPath;
	};
}

/*  // Sample code...
    static plog::OneDayFileAppender<plog::CTecFormatter> fileAppender("");
    plog::Logger<PLOG_DEFAULT_INSTANCE>& logger = plog::init(plog::verbose, &fileAppender);

    // Log severity levels are printed in different colors.
    LOG_VERBOSE << "This is a VERBOSE message";
    LOG_DEBUG   << "This is a DEBUG message";
    LOG_INFO    << "This is an INFO message";
    LOG_WARNING << "This is a WARNING message";
    LOG_ERROR   << "This is an ERROR message";
    LOG_FATAL   << "This is a FATAL message";
*/
