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
<<<<<<< HEAD
sh -lc "pacman -S --noconfirm make diffutils patch mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-python-pip mingw-w64-x86_64-zlib"
=======
sh -lc "pacman -S --noconfirm make diffutils patch mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-python-pip mingw-w64-x86_64-fftw mingw-w64-x86_64-gsl"
>>>>>>> cb515dedcc7bb78fd4dab1fb5ca9d2895390588c

@echo.
@echo off
