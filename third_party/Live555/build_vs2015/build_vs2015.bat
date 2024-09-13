
call "genWindowsMakefiles.cmd"
pause
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat"
pause

cd liveMedia
del *.obj *.lib
nmake /B -f liveMedia.mak
cd ..\groupsock
del *.obj *.lib
nmake /B -f groupsock.mak
cd ..\UsageEnvironment
del *.obj *.lib
nmake /B -f UsageEnvironment.mak
cd ..\BasicUsageEnvironment
del *.obj *.lib
nmake /B -f BasicUsageEnvironment.mak
cd ..\testProgs
del *.obj *.lib
nmake /B -f testProgs.mak
cd ..\mediaServer
del *.obj *.lib
nmake /B -f mediaServer.mak
cd ..
pause

mkdir release\include
mkdir release\lib

xcopy liveMedia\*.lib release\lib\ /Y
xcopy groupsock\*.lib release\lib\ /Y
xcopy UsageEnvironment\*.lib release\lib\ /Y
xcopy BasicUsageEnvironment\*.lib release\lib\ /Y

xcopy liveMedia\include\*.hh release\include\ /Y
xcopy groupsock\include\*.hh release\include\ /Y
xcopy groupsock\include\*.h release\include\ /Y
xcopy UsageEnvironment\include\*.hh release\include\ /Y
xcopy BasicUsageEnvironment\include\*.hh release\include\ /Y
pause