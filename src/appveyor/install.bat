@echo off

cd %APPVEYOR_BUILD_FOLDER%

echo Compiler: %COMPILER%
echo Architecture: %MSYS2_ARCH%
echo Platform: %PLATFORM%
echo MSYS2 directory: %MSYS2_DIR%
echo MSYS2 system: %MSYSTEM%
echo Bits: %BIT%

IF NOT %COMPILER%==msys2 exit

@echo on
set "PATH=C:\%MSYS2_DIR%\%MSYSTEM%\bin;C:\%MSYS2_DIR%\usr\bin;%PATH%"

echo "Ensure pacman mirrors and GIT are up to date:"
sh -lc "pacman -S --needed --noconfirm pacman-mirrors"
sh -lc "pacman -S --needed --noconfirm git"

echo "Updating packages first:"
sh -lc "pacman -Syu --noconfirm"

echo "Installing required packages:"
sh -lc "pacman -S --noconfirm cmake make diffutils patch"

@echo off
rem vim: set ts=2 sw=2 sts=2 tw=0:
