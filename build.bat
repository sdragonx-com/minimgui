@echo off

set path=E:\Qt\Qt5.14.2\Tools\mingw730_32\bin;%path%

mkdir build
cd build
cmake -G "MinGW Makefiles" ../

cmake --build ./ --config debug
rem cmake --build ./ --config release

rem pause
