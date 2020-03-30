SASfit for fitting small-angle scattering curves
================================================

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

Building from source
====================

Windows
-------

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

Close the terminal window and open the one named *MSYS2 MinGW 64-bit* in the Windows Start Menu. It should have the commands *gcc* and *cmake* in PATH.

Configure the source with CMake which builds required packages on the way:

    sasfit/build % cmake -G "MSYS Makefiles" ../src

Finally, build SASfit itself which should generate a binary package if it was successful:

    sasfit/build % make

macOS
-----

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

    sasfit/build % make

Common helpers
--------------

Clean the source tree from build products and other temporary files with GIT by running:

    $ git clean -xdf

This command removes files with are set to be ignored by GIT (*-x*), removes files in sub-directories recursively (*-d*) and does not ask questions (*-f*).
 
