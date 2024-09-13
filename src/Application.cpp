#include "stdafx.h"
#include "Application.h"
#include "Camera.h"

#include <plog/Custom/WinConsoleAppender.h>
#include <plog/Custom/OneDayFileAppender.h>
#include <plog/Appenders/DebugOutputAppender.h>

#include "common/PLogCommon.h"
#include "common/Util.h"
#include "common/TimerLockerManager.h"
#include "common/CmdLineParser.h"
//#include "common/MiniDump.h"


Application::Application()
    : m_cmd_line_parser(std::make_unique<CmdLineParser>())
{
    CTimerLockerManager::GetInstance(); // timer initialize
}

Application::~Application()
{
    //MiniDump::End();
}

int Application::InitializeLog()
{
    tstring process_path;
    GetProcessPath(process_path);
    tstring log_path = fmt::format(_T("{}\\Log\\{}"), process_path, convert_unicode(m_process_name));
    //MiniDump::Begin(log_path.c_str());

    CreateDirectoryD(log_path);
    static plog::OneDayFileAppender<plog::CommonFormatter>  onedayfile_appender(log_path.c_str());
    static plog::WinConsoleAppender<plog::CommonFormatter>  console_appender;
    static plog::DebugOutputAppender<plog::CommonFormatter> debugoutput_appender;

    plog::Logger<PLOG_DEFAULT_INSTANCE>& logger = plog::init(plog::verbose, &console_appender);
    logger.addAppender(&debugoutput_appender);
    logger.addAppender(&onedayfile_appender);

    return 0;
}

int Application::Initialize(int argc, char* argv[])
{
    m_cmd_line_parser->ParseCommandLine(argc, argv);

    std::string process_name = "PROCESS_CAMERA";
    ReadCmdLineValue(CMD_KEY_PROCESS_NAME, process_name);
    m_process_name = process_name;

    InitializeLog();

    LOGI << "<<====================     START     ====================>>";
    LOGI << "Build Date : " << (__DATE__) << " " << (__TIME__);
    LOGI << fmt::format("Version [{}]  Build Date [{} {}]", APP_VERSION, (__DATE__), (__TIME__));
    LOGI << "Process Name : " << process_name;
    LOGI << fmt::format("Process Name[{}] PID[{}]", process_name, GetCurrentProcessId());

    m_singleton_event = CreateEventA(NULL, TRUE, FALSE, process_name.c_str());
    int code = ::GetLastError();
    if (NULL == m_singleton_event)
    {
        LOGE << fmt::format("Create failed... once event object  Code[{}]", code);
        SetExitCode(APP_ONCE_EVENT_FAILED);
        return APP_ONCE_EVENT_FAILED;
    }
    if (ERROR_ALREADY_EXISTS == code)
    {
        SetExitCode(APP_EXIST_PROCESS_NAME);
        return APP_EXIST_PROCESS_NAME;
    }

    m_console_h = GetConsoleWindow();
    m_camera    = std::make_unique<Camera>();

    ::SetWindowTextA(m_console_h, process_name.c_str());

    int ret = m_camera->Initialize();
    if (ret)
    {
        SetExitCode(ret);
        return ret;
    }

    return 0;
}

int Application::Finalize()
{
    if (m_camera)
        m_camera->Finalize();

    if (m_singleton_event)
    {
        CloseHandle(m_singleton_event);
        m_singleton_event = NULL;
    }

    LOGI << "Exit Code : " << GetExitCode();
    LOGI << "<<====================     END     ====================>>";

    return 0;
}

int Application::Activate()
{
    int ret = m_camera->Activate();
    if (ret)
    {
        SetExitCode(ret);
        return ret;
    }

    return 0;
}

int Application::Deactivate()
{
    LOGI << "Deactivate start...";
    if (m_camera)
        m_camera->Deactivate();
    LOGI << "Deactivate end...";
    return 0;
}

bool Application::WaitForExitEvent(DWORD ms)
{
    if (WAIT_OBJECT_0 == WaitForSingleObject(m_singleton_event, ms))
        return true;

    return false;
}

bool Application::IsExitProcess() const
{
    return m_do_exit;
}

int Application::GetExitCode() const
{
    return m_exit_code;
}

void Application::SetExitCode(int code)
{
    m_exit_code = code;
    m_do_exit = true;
    SetEvent(m_singleton_event);
}

bool Application::HasCmdKey(const std::string& key)
{
    return m_cmd_line_parser->HasKey(key);
}

bool Application::ReadCmdLineValue(const std::string& key, int& value)
{
    return m_cmd_line_parser->ReadCmdLineValue(key, value);
}

bool Application::ReadCmdLineValue(const std::string& key, std::string& value)
{
    return m_cmd_line_parser->ReadCmdLineValue(key, value);
}