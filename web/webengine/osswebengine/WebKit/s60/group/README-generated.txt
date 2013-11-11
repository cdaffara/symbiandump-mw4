set SOURCE_ROOT env to your webcore dir

Derived files for Symbian are build using scripts from Windows port which relies on Cygin environment.
To regenerate the derived files for Symbian you need to install Cygwin from

http://www.cygwin.com/

The installer is available from

http://www.cygwin.com/setup.exe

Besides basic packages, you need to select and install the following additional packages:

    bison
    flex
    gcc
    gperf
    make
    perl
    perl-libwin32

Cygwin environment should be installed to default location under c:\cygwin.

To regenerate derived files use

    SymbianBuild\build-generated.bat

Derived files for WebCore go to directory

    ..\DerivedSources\WebCore


ok

