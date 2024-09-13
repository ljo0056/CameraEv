#pragma once
#include <plog/Appenders/IAppender.h>
#include <plog/Util.h>
#include <plog/WinApi.h>
#include <iostream>

#ifndef _WIN32
#error "Not supported for linux..."
#endif

namespace plog
{
#ifdef _WIN32
    BOOL WINAPI ConsoleCtrlHandler(DWORD event)
    {
        BOOL handled = FALSE;

        switch (event)
        {
        case CTRL_C_EVENT:     
            handled = TRUE; 
            break;
        case CTRL_BREAK_EVENT: 
            handled = TRUE; 
            break;
        case CTRL_CLOSE_EVENT: 
            handled = TRUE;
            break;
        case CTRL_SHUTDOWN_EVENT:
            handled = TRUE;
            break;
        default:
            break;
        }

        return handled;
    }

    template<class Formatter>
    class WinConsoleAppender : public IAppender
    {
    public:
		WinConsoleAppender() :
			m_stdoutHandle(NULL),
			m_originalAttr(0)
        {
			AllocConsole();
			m_stdoutHandle = GetStdHandle(stdHandle::kOutput);

			CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
			GetConsoleScreenBufferInfo(m_stdoutHandle, &csbiInfo);

			m_originalAttr = csbiInfo.wAttributes;

            SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
        }

        virtual ~WinConsoleAppender()
        {
            FreeConsole();
        }

        virtual void write(const Record& record)
        {
			util::nstring str = Formatter::format(record);
			util::MutexLock lock(m_mutex);

			setColor(record.getSeverity());
			writestr(str);
			resetColor();
        }

    protected:
        void writestr(const util::nstring& str)
        {
			if (NULL == m_stdoutHandle)
				return;

			WriteConsoleW(m_stdoutHandle, str.c_str(), static_cast<DWORD>(str.size()), NULL, NULL);
        }

	private:
		void setColor(Severity severity)
		{
			if (m_stdoutHandle)
			{
				switch (severity)
				{
				case fatal:
					SetConsoleTextAttribute(this->m_stdoutHandle, foreground::kRed | foreground::kGreen | foreground::kBlue | foreground::kIntensity | background::kRed); // white on red background
					break;

				case error:
					SetConsoleTextAttribute(this->m_stdoutHandle, static_cast<WORD>(foreground::kRed | foreground::kIntensity | (m_originalAttr & 0xf0))); // red
					break;

				case warning:
					SetConsoleTextAttribute(this->m_stdoutHandle, static_cast<WORD>(foreground::kRed | foreground::kGreen | foreground::kIntensity | (m_originalAttr & 0xf0))); // yellow
					break;

				case debug:
					SetConsoleTextAttribute(this->m_stdoutHandle, static_cast<WORD>(foreground::kBlue | foreground::kIntensity | (m_originalAttr & 0xf0))); // blue
					break;

				case verbose:
					SetConsoleTextAttribute(this->m_stdoutHandle, static_cast<WORD>(foreground::kGreen | foreground::kBlue | foreground::kIntensity | (m_originalAttr & 0xf0))); // cyan
					break;

				default:
					break;
				}
			}
		}

		void resetColor()
		{
			if (m_stdoutHandle)
			{
				SetConsoleTextAttribute(m_stdoutHandle, m_originalAttr);
			}
		}

    protected:
        util::Mutex m_mutex;
        HANDLE      m_stdoutHandle;
		WORD		m_originalAttr;
    };
#endif
}


/*  // Sample code...
    static plog::WinConsoleAppender<plog::CTecFormatter> consoleAppender;
    plog::Logger<PLOG_DEFAULT_INSTANCE>& logger = plog::init(plog::verbose, &consoleAppender);

    // Log severity levels are printed in different colors.
    LOG_VERBOSE << "This is a VERBOSE message";
    LOG_DEBUG   << "This is a DEBUG message";
    LOG_INFO    << "This is an INFO message";
    LOG_WARNING << "This is a WARNING message";
    LOG_ERROR   << "This is an ERROR message";
    LOG_FATAL   << "This is a FATAL message";
*/
