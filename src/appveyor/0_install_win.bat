@echo off
@echo.

echo Compiler: %COMPILER%
echo Architecture: %MSYS2_ARCH%
echo Platform: %PLATFORM%
echo MSYS2 directory: %MSYS2_DIR%
echo MSYS2 system: %MSYSTEM%

@echo.
@echo ## Updating PATH for MSYS2/MinGW64:
@echo on
set "PATH=C:\%MSYS2_DIR%\%MSYSTEM%\bin;C:\%MSYS2_DIR%\usr\bin;%PATH%"

@echo.
@echo ## Ensure pacman mirrors and GIT are up to date:
sh -lc "pacman -S --needed --noconfirm pacman-mirrors"
sh -lc "pacman -S --needed --noconfirm git"

@echo.
@echo ## Updating packages first:
sh -lc "pacman -Syu --noconfirm"
sh -lc "pacman -Syu --noconfirm"

@echo.
@echo ## Installing required packages:
sh -lc "pacman -S --noconfirm make diffutils patch mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-python-pip"

@echo.
@echo off
rem vim: set ts=4 sw=4 sts=4 tw=0 et:
