@echo off

cd %APPVEYOR_BUILD_FOLDER%

echo Compiler: %COMPILER%
echo Architecture: %MSYS2_ARCH%
echo Platform: %PLATFORM%
echo MSYS2 directory: %MSYS2_DIR%
echo MSYS2 system: %MSYSTEM%
echo Bits: %BIT%

rem Create a writeable TMPDIR
rem mkdir %APPVEYOR_BUILD_FOLDER%\tmp
rem set TMPDIR=%APPVEYOR_BUILD_FOLDER%\tmp

IF %COMPILER%==msys2 (
  @echo on
  set "PATH=C:\%MSYS2_DIR%\%MSYSTEM%\bin;C:\%MSYS2_DIR%\usr\bin;%PATH%"
  sh -lc "pacman -S --needed --noconfirm pacman-mirrors"
  sh -lc "pacman -S --needed --noconfirm git"
  rem Update
  sh -lc "pacman -Syu --noconfirm"
  rem Install required packages
  rem sh -lc "pacman -S --noconfirm mingw-w64-x86_64-gcc cmake make diffutils patch"
  sh -lc "pacman -S --noconfirm cmake make diffutils patch"
)

@echo off
rem vim: set ts=2 sw=2 sts=2 tw=0:
