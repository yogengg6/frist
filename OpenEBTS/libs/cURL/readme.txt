
To obtain the 4 binaries, libcurl.lib, libcurld.lib,  x64/libcurl.lib and x64/libcurld.lib,
the following steps were performed:


1) The sources for libcurl 7.55.1 were downloaded from http://curl.haxx.se.

2) After extraction of the compressed package, the solution curl-7.55.1\projects\Windows\VC14\curl-all.sln was opened.
with Visual Studio 2015.

3) For Solution Configurations Lib Debug and Lib Release and Solution Platforms Win32 and x64 or the libcurl project, the C++ Code Generation Runtime Library was set from the DLL version to the non-DLL version.

4) The 4 Configuration/Platform combinations of libcurl were built, resulting in these 4 files, which were collected and checked-in for use by OpenEBTS

curl-7.55.1\build\Win32\VC14\LIB Release\libcurl.lib
curl-7.55.1\build\Win32\VC14\LIB Debug\libcurld.lib
curl-7.55.1\build\Win64\VC14\LIB Release\libcurl.lib
curl-7.55.1\build\Win64\VC14\LIB Debug\libcurld.lib

