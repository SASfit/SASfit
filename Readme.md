[![Latest Download](https://img.shields.io/badge/Latest%20Download-210415170012-blue.svg?colorA=12577E&colorB=187EB6&link=https%3A%2F%2Fcloudsmith.io%2F~sasfit%2Frepos%2Fbuild%2Fpackages%2F%3Fq=version%3Alatest&logo=data%3Aimage%2Fpng%3Bbase64%2CiVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAA10lEQVRIS%2BWUwQ3CMBRD%2FXoGCQnEENwYgFVgBTZhBlgCNmEK4MLdqAgkVJK0Dc2JnBP72843KnwojK%2FeBLYnkm6vwS7ALDVkLwLbDoEBUZxBCCRtgEOQvE8GMQU1RkzFUAruwPgnBTnTP5XFLLI9l3SStGyxsQKC4UcJUtNKOgOLrtl9Kci1Ikb4XwRXYNrV%2B%2Fe94C8K5ZCqg6wusj2SdARWnwANcgNVFkHoUdGys72WtC%2FWprn70bnsbO8kbYspqIGLZvCevEnS9n07W9R3wZKLlgsWevcARxNbGYCoyEgAAAAASUVORK5CYII%3D&longCache=false&style=flat)](https://cloudsmith.io/~sasfit/repos/build/packages/?q=version:latest)

[![Build status Windows](https://img.shields.io/appveyor/job/build/SASfit/sasfit/Image:%20Visual%20Studio%202019;%20Environment:%20DUMMY=?label=Windows%20build)](https://ci.appveyor.com/project/SASfit/sasfit)
[![Build status macOS](https://img.shields.io/appveyor/job/build/SASfit/sasfit/Image:%20macos;%20Environment:%20DUMMY=?label=macOS%20build)](https://ci.appveyor.com/project/SASfit/sasfit)
[![Build status Linux](https://img.shields.io/appveyor/job/build/SASfit/sasfit/Image:%20Ubuntu2004;%20Environment:%20DUMMY=?label=Linux%20build)](https://ci.appveyor.com/project/SASfit/sasfit)

- [SASfit for fitting small-angle scattering curves](#sasfit-for-fitting-small-angle-scattering-curves)
   1. [Startup issues on macOS](#startup-issues-on-macos)
   2. [Building from source](#building-from-source)
      - [Windows](#windows)
      - [macOS](#macos)
      - [Linux](#linux)
      - [Common helpers](#common-helpers)
   3. [Sponsors](#sponsors)

# SASfit for fitting small-angle scattering curves

SASfit has been written for analyzing and plotting small angle scattering data. 
It can calculate integral structural parameters like radius of gyration, scattering invariant, 
Porod constant. Furthermore it can fit size distributions together with several form factors 
including different structure factors. Additionally an algorithm has been implemented, which allows
to simultaneously fit several scattering curves with a common set of (global) parameters. This last
option is especially important in contrast variation experiments or measurements with polarised 
neutrons. The global fit helps to determine fit parameters unambiguously which by analysing a single
curve would be otherwise strongly correlated. The program has been written to fulfill the needs at 
the small angle neutron scattering facility at PSI (kur.web.psi.ch). The numerical routines have 
been written in C whereas the menu interface has been written in tcl/tk and the plotting routine 
with the extension blt.

Author Information:
- Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
- Ingo Bressler (ingo.bressler@bam.de)

Laboratory for Neutron Scattering and Imaging  
Paul Scherrer Institut  
CH-5232 Villigen PSI  
Switzerland  
Tel: +41 (0)56 310 3165  
Fax: +41 (0)56 310 3131  

SASfit is available for users analysing data taken at PSI.  
SASfit has been developed at the Paul Scherrer Institute (PSI) and remains copyright of the PSI.  
SASfit is provided for users of the PSI facilities.  
SASfit is provided "as is" and with no warranty.  
SASfit is licensed under the GPLv3. For more details see COPYING.txt.

Academic papers about SASfit have been published in:
-  [J. Appl. Cryst. (2015). 48, 1587-1598](https://dx.doi.org/10.1107/S1600576715016544)
-  [J. Appl. Cryst. (2017). 50, 1395-1403](https://dx.doi.org/10.1107/S1600576717011979)

## Startup issues on macOS

![sasfit.command error on macOS](doc/images/sasfit.command%20error%20on%20macOS.png)

In case of this error about an unidentified developer (because we are not registered with Apple yet) one possible workaround is to remove the *quarantine* label of the downloaded file on macOS. This can be done by the following command in a Terminal window. It requires admin privileges:

    % cd Downloads
    % sudo xattr -c sasfit_*_macos64.tar.bz2

If your user is not allowed to call *sudo*, you have to change to a user which does:

    ~ % cd /
    / % su admin
    Password:...
    / % sudo xattr -c /Users/<username>/Downloads/sasfit_*_macos64.tar.bz2

Once the *quarantine* label was removed, the package can be extracted by double-clicking on it. Starting the program by clicking on the `sasfit.command` file should open it without further errors.

## Building from source

### Windows

*(Tested with Windows 10 x64 1809)*

On a fresh system, download and install the latest [MSYS2](https://www.msys2.org) (x86\_64) software distro and building platform for Windows.

The following commands have to be executed in the previously installed MSYS2 shell.
Ensure software repository mirrors and GIT version control system are up to date:

    pacman -S --needed --noconfirm pacman-mirrors
    pacman -S --needed --noconfirm git

Update all packages first. Twice, because the MSYS2 window has to be closed in between and opened again, just as the updater in the shell tells you:

    pacman -Syu --noconfirm
    pacman -Syu --noconfirm

Installing required packages for building SASfit:

    pacman -S --noconfirm make diffutils patch mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake

Get a copy of the latest SASfit source code:

    git clone https://github.com/SASfit/SASfit.git sasfit

Create a build directory:

    % cd sasfit
    sasfit % mkdir build
    sasfit % cd build

Close the terminal window and open the one named **MSYS2 MinGW 64-bit** in the Windows Start Menu (Do not use the plain *MSYS2* terminal, it does not work as expected).
The terminal should have the commands *gcc* and *cmake* in PATH.

Configure the source with CMake which builds required packages on the way:

    sasfit/build % cmake -G "MSYS Makefiles" ../src

Finally, build SASfit itself which should generate a binary package if it was successful:

    sasfit/build % make

### macOS

*(Tested with Catalina 10.15)*

On a fresh system, install the required development tools with [Homebrew](https://brew.sh):

    % brew install git cmake gcc
    % brew cask install xquartz 

To avoid using macOS default gcc (which as clang actually), specify the compiler by the following environment variables:

    % export CC=/usr/local/bin/gcc-9
    % export CXX=/usr/local/bin/g++-9

Get a copy of the latest SASfit source code:

    % git clone https://github.com/SASfit/SASfit.git sasfit

Create a build directory:

    % cd sasfit
    sasfit % mkdir build
    sasfit % cd build

Configure the source with CMake which builds required packages on the way:

    sasfit/build % cmake ../src

Finally, build SASfit itself which should generate a binary package if it was successful:

    sasfit/build % make -j4

### Linux

*(Tested with Ubuntu 20.04 LTS)*

On a fresh system, install the required development tools:

    $ sudo apt install git build-essential cmake gcc

Additionally install the X11 development packages:

    $ sudo apt install libx11-dev

Get a copy of the latest SASfit source code:

    $ git clone https://github.com/SASfit/SASfit.git sasfit

Create a build directory:

    $ cd sasfit
    sasfit $ mkdir build
    sasfit $ cd build

Configure the source with CMake which builds required packages on the way:

    sasfit/build $ cmake ../src

Finally, build SASfit itself which should generate a binary package if it was successful:

    sasfit/build $ make -j4

### Common helpers

Clean the source tree from build products and other temporary files with GIT by running:

    $ git clean -xdf

This command removes files with are set to be ignored by GIT (*-x*), removes files in sub-directories recursively (*-d*) and does not ask questions (*-f*).
 
## Sponsors

[![Hosted By: Cloudsmith](https://img.shields.io/badge/OSS%20hosting%20by-cloudsmith-blue?logo=cloudsmith&style=flat)](https://cloudsmith.com)

Package repository hosting is graciously provided by [Cloudsmith](https://cloudsmith.com).
Cloudsmith is the only fully hosted, cloud-native, universal package management solution, that
enables your organization to create, store and share packages in any format, to any place, with total
confidence.

