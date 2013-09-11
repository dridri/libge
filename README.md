LibGE
=====

The Gamma Engine library is a fast and easy multiplatform game engine, currently working on Linux,
Windows, PSP (open source SDK) and Android (NDK with GLES 2.0+) platforms



Building
========

    $ mkdir build
    $ cd build
    $ cmake .. [-Dtarget=TARGET] [-Dvideo=VIDEO]
    $ make
    $ make install

The target platform will be automatically detected. If you want to cross-compile to other platform,
run the 'cmake' command with -Dtarget option, where _TARGET_ can be :
 * linux
 * win
 * psp
 * android

On Linux/Windows/Mac platforms, you can also choose the OpenGL support version between 2.1 and 3.0+
with the -Dvideo option, where _VIDEO_ can be :
 * opengl21
 * opengl30
