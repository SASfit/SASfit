[![Latest Download](doc/images/badge_latest_build.svg)](https://cloudsmith.io/~sasfit/repos/build/packages/?q=version:latest)
[![SASfit on Zenodo](https://zenodo.org/badge/doi/10.5281/zenodo.7530356.svg)](https://doi.org/10.5281/zenodo.7530356)

[![Build status Windows](https://img.shields.io/appveyor/job/build/SASfit/sasfit/Image:%20Visual%20Studio%202019;%20Environment:%20DUMMY=?label=Windows%20build)](https://ci.appveyor.com/project/SASfit/sasfit)
[![Build status macOS](https://img.shields.io/appveyor/job/build/SASfit/sasfit/Image:%20macos-sonoma;%20Environment:%20DUMMY=?label=macOS%20build)](https://ci.appveyor.com/project/SASfit/sasfit)
[![Build status Linux](https://img.shields.io/appveyor/job/build/SASfit/sasfit/Image:%20Ubuntu2204;%20Environment:%20DUMMY=?label=Linux%20build)](https://ci.appveyor.com/project/SASfit/sasfit)

# A Software package for fitting small-angle scattering curves

**SASfit** has been written for analyzing and plotting small angle scattering data.
It can calculate integral structural parameters like radius of gyration, scattering invariant,
Porod constant. Furthermore it can fit size distributions together with several form factors
including different structure factors.

Additionally, an algorithm has been implemented, which allows
to simultaneously fit several scattering curves with a common set of (global) parameters. This last
option is especially important in contrast variation experiments or measurements with polarised
neutrons. The global fit helps to determine fit parameters unambiguously which by analysing a single
curve would be otherwise strongly correlated.

![Global fit screenshot](https://github.com/SASfit/SASfit/raw/master/doc/images/sasfit_screenshot_w640.png
"Global fit screenshot")

The program has been written to fulfill the needs at
[the small angle neutron scattering facility at PSI](https://www.psi.ch/en/sinq/sansi).
The numerical routines have
been written in [*C*](https://en.wikipedia.org/wiki/C_(programming_language))
whereas the menu interface has been written in
[*Tcl*](https://en.wikipedia.org/wiki/Tcl)/[*Tk*](https://en.wikipedia.org/wiki/Tk_(software))
and the plotting routine with the [BLT](http://blt.sourceforge.net) extension.

## Author Information

SASfit was developed and is maintained by:

- Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
- Ingo Bressler (ingo.bressler@bam.de)

```
Laboratory for Neutron Scattering and Imaging
Paul Scherrer Institut
CH-5232 Villigen PSI
Switzerland
Tel: +41 (0)56 310 3165
Fax: +41 (0)56 310 3131
```

- *SASfit* has been developed at the Paul Scherrer Institute (PSI) and remains copyright of the PSI.
- *SASfit* is available for users analysing data taken at PSI.
- *SASfit* is provided for users of the PSI facilities.
- *SASfit* is provided "as is" and with no warranty.
- *SASfit* is licensed under the GPLv3. For more details see [COPYING.txt](https://github.com/SASfit/SASfit/raw/master/COPYING.txt).

## Publications

Academic papers about SASfit have been published in:

-  [J. Appl. Cryst. (2015). 48, 1587-1598](https://dx.doi.org/10.1107/S1600576715016544)
-  [J. Appl. Cryst. (2017). 50, 1395-1403](https://dx.doi.org/10.1107/S1600576717011979)
-  [J. Appl. Cryst. (2022). 55, 1677-1688](https://dx.doi.org/10.1107/S1600576722009037)

If SASfit has been significant in your research, and you would like to acknowledge the project in your academic publication, we suggest citing one of the aforementioned publications.

## [The Manual](https://raw.githubusercontent.com/SASfit/SASfit/master/doc/manual/sasfit.pdf)

There is a comprehensive manual included the software package with a jumble of formulas used by SASfit.
It evolves with the program to cover most of the implemented models and functions.
The latest version can also be [viewed online](https://raw.githubusercontent.com/SASfit/SASfit/master/doc/manual/sasfit.pdf).

## [Videos](https://www.youtube.com/user/SASfitTeam)

For introduction and demonstration of the program and its main features, some tutorial videos can be found [in our YouTube channel](https://www.youtube.com/user/SASfitTeam):

- [Introducing SASfit](https://www.youtube.com/watch?v=4ONwKQB1AC0)
- [SASfit demo fitting dense spherical particles SAS data](https://www.youtube.com/watch?v=LwTmc38X3X0)
- plus somewhat older videos

## Versions and Changes

The latest stable changes and versions of the program can be found in the
[Releases section at GitHub](https://github.com/SASfit/SASfit/releases).

Up to date development snapshots for all supported platforms can be found [on cloudsmith](https://cloudsmith.io/~sasfit/repos/build/groups/) (this link is also placed at the top on this page).

## MacOS Startup issues

As of March 13th '24 [the latest SASfit development binary package](https://cloudsmith.io/~sasfit/repos/build/packages/?q=version%3A240306081722) was tested successfully (single-fit -> simulate single curve -> plots curve) on the following macOS (Intel) versions:

- Sonoma 14.4
- Ventura 13.6
- Monterey 12.7

### *no display name and no $DISPLAY environment variable*

It is very likely that the XQuartz package is missing which is a requirement for SASfit to run on modern macOS. You can download its package from here: [https://www.xquartz.org/releases/index.html](https://www.xquartz.org/releases/index.html)
Descriptions on using *macports* or *homebrew* for installing it can be found there too.

### *SASfit is from an unidentified developer*

![sasfit.command error on macOS](https://raw.githubusercontent.com/SASfit/SASfit/master/doc/images/sasfit.command%20error%20on%20macOS.png)

#### Solution A

In case of this error about an unidentified developer (because we are not registered with Apple yet) it *should* work to **right-click -> Open** on the file `sasfit.command` to get a dialog with an **Open** button:

![Open sasfit.command on macOS](https://raw.githubusercontent.com/SASfit/SASfit/master/doc/images/sasfit.command%20open.png)

Opening a downloaded package for the first time in this way, you might have to confirm this security exception with an admin account (won't pop-up when opening it the 2nd time or later):

![Confirm Security Exception for opening sasfit.command on macOS](https://raw.githubusercontent.com/SASfit/SASfit/master/doc/images/sasfit.command%20open%20secpref.png)

#### Solution B

Another workaround is to remove the *quarantine* label of the downloaded file on macOS. This can be done by the following command in a Terminal window. It requires admin privileges:

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

Open the terminal named **MSYS2 MinGW 64-bit** in the Windows Start Menu (Do not use the plain *MSYS2* terminal, it does not work as expected).

The following commands have to be executed in the previously installed MSYS2 shell.
Ensure software repository mirrors and GIT version control system are up to date:

    pacman -S --needed --noconfirm pacman-mirrors git

Update all packages first. Twice, because the MSYS2 window has to be closed in between and opened again, just as the updater in the shell tells you:

    pacman -Syu --noconfirm
    pacman -Syu --noconfirm

Check out the SASfit source tree with GIT (here to ~/Downloads/sasfit):

    cd $USERPROFILE/Downloads
    git clone --recursive https://github.com/SASfit/SASfit.git sasfit

Next, run the install script for slightly older gcc-13 install:

    mkdir packages && cd packages
    sh ../sasfit/src/appveyor/citools/msys2_get_pckg+deps.sh gcc-13
    sh ../sasfit/src/appveyor/citools/msys2_get_pckg+deps.sh cmake-3.29.2
    sh ../sasfit/src/appveyor/citools/msys2_get_pckg+deps.sh python-pip-24.0
    pacman -U --noconfirm *.zst

- **Note:** If it complains about dependency conflicts and installing the packages in the previous
step failed (`pacman -U`), you need to remove the conflicting packages first.
This may be caused by another GCC version with all dependent packages already present:

        pacman -Rnc mingw-w64-x86_64-libwinpthread

Installing other required packages for building SASfit:

    pacman -S --noconfirm make diffutils coreutils patch mingw-w64-x86_64-fftw mingw-w64-x86_64-gsl

Create a build directory and make it the work dir:

    cd $USERPROFILE/Downloads/sasfit
    mkdir build && cd build

The terminal should have the commands *gcc* and *cmake* in PATH now.
Configure the source with CMake which builds required packages on the way:

    sasfit/build % cmake -G "MSYS Makefiles" ../src

Finally, build SASfit itself which should generate a binary package if it was successful:

    sasfit/build % make

### macOS

*(Tested with Catalina 10.15)*

On a fresh system, install the required development tools with [Homebrew](https://brew.sh):

    % brew install git cmake gcc gsl fftw zlib
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

*(Tested on Ubuntu 22+24)*

Install *GIT* first and get a copy of the latest SASfit source code:

    apt install git
    git clone --depth 1 https://github.com/SASfit/SASfit.git sasfit

#### Optional: Prepare a podman container for building SASfit

**Note:** This works for building except packaging for now. Getting the appimagetool working in a container for packaging is not working yet. (would require `--device /dev/fuse --device /dev/snd` below, at least)

Run container interactively:

    podman run -it -u 0 -v .:/sasfit --rm ubuntu:latest bash

In the container shell:

    apt update && apt install -y lsb-release sudo fuse-overlayfs

#### Install the required packages for building

Run this on your system natively or within the container started as described before.
Use the scripts provided for continous integration (CI) in subfolder `src/appveyor`:

    cd <SASfit-path>
    sh src/appveyor/0_install_linux4building.sh

#### Run the build script

For the compiler, the latest installed gcc is selected and indicated at the beginning of the output. To override this, export the **CC** and **CXX** environment variables with an absolute path to the compiler for *C* and *C++* respectively.

    cd <SASfit-path>
    sh src/appveyor/4_build.sh

The build process might end with an error message of `appimage-builder` not being found which is ok, since it was not installed and an *.AppImage* package is not created. But the *libsasfit.so* library and the *saskit* integrated Tcl/Tk interpreter should exist and it can be used to run SASfit from the source directory directly:

    ./saskit/saskit_linux64 sasfit.vfs/main.tcl

If desired, the system packages and tools for building an *.AppImage* package can be installed by running the following script similar to the one earlier:

    cd <SASfit-path>
    sh src/appveyor/0_install_linux4packaging.sh

This script also installs docker or makes sure it is installed and adjusts the required user & group settings of the current user to use docker. For these changes to take effect, a log out and log in is necessary, including a restart of the graphical desktop session if there is one in use.

**Please note:**
1. Make sure to remove the *tests* section from `src/AppImageBuilder.template.yml`. These tests are for the automatic builds and they will fail on a more recent Linux version, because created AppImage will not be backward compatible with older Linux/Ubuntu versions than the one it was built on due to the glibc version embedded in the AppImage.
2. Building SASfit again via `4_build.sh` should create an *.AppImage* in `<SASfit-path>`.
3. In case of errors during AppImage creation, make sure to delete the folders `AppDir` and `appimage-build` in `<SASfit-path>` between consecutive runs.

### Common helpers

Clean the source tree from build products and other temporary files with GIT by running:

    $ git clean -xdf

This command removes files with are set to be ignored by GIT (*-x*), removes files in sub-directories recursively (*-d*) and does not ask questions (*-f*).
 
## Sponsors

[![Hosted By: Cloudsmith](https://github.com/SASfit/SASfit/raw/master/doc/images/badge_cloudsmith.svg)](https://cloudsmith.com)

Package repository hosting is graciously provided by [Cloudsmith](https://cloudsmith.com).
Cloudsmith is the only fully hosted, cloud-native, universal package management solution, that
enables your organization to create, store and share packages in any format, to any place, with total
confidence.

