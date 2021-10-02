call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64_x86
cd /d "%~dp0"
::rd /s /q build
mkdir build
::  "-DCMAKE_MSVC_RUNTIME_LIBRARY:STRING=MultiThreaded$<$<CONFIG:Debug>:Debug>"
:: The dds API can not allocate date in one dll and free in other dll 
set "PATH=%PATH%;C:\Program Files\LLVM\bin"
cmake -Bbuild -Hiceoryx_meta -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -GNinja ^
	"-DCMAKE_C_FLAGS_INIT=--target=i686-pc-windows-msvc" ^
	"-DCMAKE_CXX_FLAGS_INIT=--target=i686-pc-windows-msvc -Zc:threadSafeInit- " ^
	-DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl
mkdir build
cmake --build build

cd build
cmake .
:: ninja install
:: pause
