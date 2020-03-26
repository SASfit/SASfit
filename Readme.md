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

macOS
-----

*(Tested with Catalina 10.15)*

On a fresh system, install the required development tools with [Homebrew](https://brew.sh):

    % brew install git cmake gcc
    % brew cask install xquartz 

To avoid using macOS default gcc (which as clang actually), specify the compiler by the following environment variables:

    % export CC=/usr/local/bin/gcc-9
    % export CXX=/usr/local/bin/g++-9

Get a copy of the SASfit source code:

    % git clone https://github.com/SASfit/SASfit.git sasfit

Create a build directory:

    % cd sasfit
    sasfit % mkdir build
    sasfit % cd build

Configure the source with CMake which builds required packages on the way:

    sasfit/build % cmake ../src

Finally, build SASfit itself which should generate a binary package if it was successful:

    sasfit/build % make


