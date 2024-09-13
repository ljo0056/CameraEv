@echo off

set PROTOC="../../../bin/x64/protoc.exe"
set TARGAT="./camera_control.proto"
set GRPC_CPP_PLUGIN_PATH="../../../bin/x64/grpc_cpp_plugin.exe"
set DEST_PATH="./"
set PROTO_PATH="./"


@echo on
%PROTOC% -I %PROTO_PATH% --grpc_out=%DEST_PATH% --plugin=protoc-gen-grpc=%GRPC_CPP_PLUGIN_PATH% %TARGAT%
pause
%PROTOC% -I %PROTO_PATH% --cpp_out=%DEST_PATH% %TARGAT%
pause