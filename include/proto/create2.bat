@echo off

set PROTOC="../../third_party/grpc_lib/bin/x64/protoc.exe"
set DST_DIR="./"
set SRC_DIR="./"

echo Delete pb.cc, pb,h
del CameraInfoFile.pb.cc
del CameraInfoFile.pb.h
pause
echo Create protobuf
%PROTOC% -I=%SRC_DIR% --cpp_out=%DST_DIR% %SRC_DIR%/"CameraInfoFile.proto"
pause