@ECHO off
ECHO Copying binaries to root folder...
IF EXIST %~dp0\Release\virtuelium.exe COPY %~dp0\Release\virtuelium.exe %~dp0\..
IF EXIST %~dp0\Debug\virtueliumd.exe COPY %~dp0\Debug\virtueliumd.exe %~dp0\..
