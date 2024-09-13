#include "stdafx.h"
#include <conio.h>
#include "Application.h"
#include "common/Util.h"
#include "proto/CameraInfoFile.pb.h"

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Application* app = Application::Instance();
    if (0 == app->Initialize(argc, argv))
        app->Activate();

    while (true)
    {
        if (_kbhit())
        {
            char ch = tolower(getchar());
            if ('q' == ch)
            {
                LOGI << "[Main] Quit key press...";
                break;
            }
        }

        if (app->WaitForExitEvent())
        {
            LOGI << "[Main] Recv app exit event...";
            break;
        }
        //if (app->IsExitProcess())
        //{
        //    LOGI << "[Main] Set app exit code...";
        //    break;
        //}
        //std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    app->Deactivate();
    app->Finalize();

    google::protobuf::ShutdownProtobufLibrary();

    return app->GetExitCode();
}
