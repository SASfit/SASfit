@echo off

cd %APPVEYOR_BUILD_FOLDER%

echo Compiler: %COMPILER%
echo Architecture: %MSYS2_ARCH%
echo Platform: %PLATFORM%
echo MSYS2 directory: %MSYS2_DIR%
echo MSYS2 system: %MSYSTEM%
echo Bits: %BIT%

REM Create a writeable TMPDIR
rem mkdir %APPVEYOR_BUILD_FOLDER%\tmp
rem set TMPDIR=%APPVEYOR_BUILD_FOLDER%\tmp

IF %COMPILER%==msys2 (
  @echo on
  SET "PATH=C:\%MSYS2_DIR%\%MSYSTEM%\bin;C:\%MSYS2_DIR%\usr\bin;%PATH%"
  sh -lc "pacman -S --needed --noconfirm pacman-mirrors"
  sh -lc "pacman -S --needed --noconfirm git"
  REM Update
  sh -lc "pacman -Syu --noconfirm"
)

rem vim: set ts=2 sw=2 sts=2 tw=0:
