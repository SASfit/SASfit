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
@echo ## Installing required packages:
:: sh -lc "pacman -S --noconfirm make diffutils patch mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-python-pip mingw-w64-x86_64-fftw mingw-w64-x86_64-gsl mingw-w64-x86_64-zlib mingw-w64-x86_64-boost"
@echo ## Remove existing package with wrong versions first
sh -lc "pacman -Rns --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-libwinpthread"
sh -lc "pacman -S --noconfirm curl gawk"

@echo ## Install specific versions of some build packages
mkdir packages
cd packages
sh -x "../src/appveyor/citools/msys2_get_pckg+deps.sh" gcc-13
sh "../src/appveyor/citools/msys2_get_pckg+deps.sh" cmake-3.29.2
sh "../src/appveyor/citools/msys2_get_pckg+deps.sh" python-pip-24.0
sh -lc "pacman -U --noconfirm *.zst"

@echo ## Install more general build tools
sh -lc "pacman -S --noconfirm make diffutils coreutils patch mingw-w64-x86_64-fftw mingw-w64-x86_64-gsl"

@echo.
@echo off
