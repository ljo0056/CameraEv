#pragma once

#include <memory>
#include <string>

class Camera;
class CmdLineParser;

class Application
{
private:
    std::string m_process_name;

    int     m_exit_code     = 0;
    bool    m_do_exit = false;

    HANDLE  m_singleton_event = NULL;
    HWND    m_console_h = NULL;

    std::unique_ptr<Camera>         m_camera;
    std::unique_ptr<CmdLineParser>  m_cmd_line_parser;

private:
    Application();
    ~Application();

public:
    static Application* Instance()
    {
        static Application app;
        return &app;
    }

    int  InitializeLog();
    int  Initialize(int argc, char* argv[]);
    int  Finalize();

    int  Activate();
    int  Deactivate();

    bool WaitForExitEvent(DWORD ms = INFINITE);
    bool IsExitProcess() const;
    int  GetExitCode() const;
    void SetExitCode(int code);

    bool HasCmdKey(const std::string& key);
    bool ReadCmdLineValue(const std::string& key, int& value);
    bool ReadCmdLineValue(const std::string& key , std::string& value);
};