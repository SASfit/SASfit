# -*- coding: utf-8 -*-
r"""
Use any SASfit plugin form factor in the polydisperse model.

The polydisperse tab ships two hand-written form factors, sphere and
core-shell. SASfit has a hundred or more, already compiled. This module makes
them usable directly, through the ctypes layer in SASstudio/lib/classes.py.

Each SASfit form-factor plugin exports three functions:

    sasfit_ff_<name>       (q, param*)         scattering INTENSITY, |F|^2
    sasfit_ff_<name>_f     (q, param*)         scattering AMPLITUDE, F
    sasfit_ff_<name>_v     (q, param*, dist)   INTERACTING VOLUME

and the third is what makes this more than a convenience.

WHY THE INTERACTING VOLUME MATTERS
----------------------------------
PolydisperseSASBase needs two different sizes:

  * a scattering amplitude F(Q), which the form factor supplies, and
  * a HARD-CORE DIAMETER sigma_ij at which to place the excluded volume when
    solving the Ornstein-Zernike equations.

Until now the two were locked together as R = sigma/2 -- the scattering radius
WAS the hard-core radius. That is right for bare silica or a charged colloid,
where the particle is the hard core, but wrong for a sterically stabilised
particle whose contrast-matched brush scatters nothing yet still excludes
volume, and wrong for any anisotropic or multi-shell particle.

The interacting volume breaks that lock. Taking the interacting radius as the
radius of the sphere of equal volume,

    R_int = (3 V_int / 4 pi)^(1/3),

gives an excluded-volume size derived from the particle's own geometry rather
than assumed equal to its scattering size. That is exactly the
equivalent-hard-sphere construction usually made by hand when applying a
hard-sphere structure factor to a non-spherical particle -- here it comes from
the plugin.

The sphere used for the structure factor is therefore the equal-volume sphere,
which is an approximation for anything non-spherical: it captures excluded
volume but not shape anisotropy or orientational correlation. For a rod or a
disc at appreciable volume fraction that is a real limitation, not a detail.

WHAT HAS BEEN VERIFIED
----------------------
Checked directly against libsasfit.so from the Linux AppImage build
260905165034, which exports 141 sasfit_ff_* triads. With p[0] = R and
p[3] = eta, sasfit_ff_sphere_f reproduces the analytic sphere amplitude

    eta 4 pi [sin(qR) - qR cos(qR)] / q^3

to a ratio of 1.0000000000 at q = 1e-5, 1e-3, 0.02, 0.05 and 0.1, and
sasfit_ff_sphere returns exactly its square. No initialisation call is
needed: a bare ctypes.CDLL is enough.

PARAMETER ORDER: JUST USE THE HEADER TABLE
------------------------------------------
The Doxygen \par Required parameters table in each plugin header lists the
parameters IN p[] ORDER, including unused slots, together with a description
and often a default and a domain. For sphere (in sasfit_sphericalshells.h):

    p[0] = R      radius               (default=10)    [0;+inf)
    p[1] = dummy
    p[2] = dummy
    p[3] = eta    scattering_contrast  (default=1e10)  (-inf;+inf)

so position in that table IS the index, and SASstudio's Plugin class already
parses it into parameter_labels / parameter_descriptions /
parameter_default_values / parameter_bounds. Nothing further is needed to map
names to slots.

A NOTE ON HOW THIS WAS GOT WRONG, since the mistake is easy to repeat: the
sphere source only #defines R and ETA, so it is tempting to conclude that the
table has two entries and that documented order and array layout diverge. It
does not -- the header documents the unused slots as "dummy". Read the header
table, not the #defines.

Builds for Windows, Linux and macOS are published at
https://cloudsmith.io/~sasfit/repos/build/packages/?q=version:latest

PLATFORM
--------
The shared-library suffix differs per platform -- .dll on Windows, .so on
Linux, .dylib on macOS -- and so does the sensible default location, so the
path is taken from the SASSTUDIO_PATH environment variable in preference to
any built-in guess. The Windows path below is only a fallback for the machine
this was developed against; on any other system, set SASSTUDIO_PATH or pass
root= explicitly.

USAGE
-----
    from sasfit_formfactors import SASfitFormFactor, availableFormFactors
    print(availableFormFactors())

    ff = SASfitFormFactor("sphere", params=[1.0, 50.0], sizeIndex=1)
    print(ff.describe())          # parameter names, descriptions, defaults

    from generic_polydisperse_sas import GenericPolydisperseSAS
    sas = GenericPolydisperseSAS("HardSphere", (), phi=0.2, srel=0.2,
                                 nbins=3, nFF=60, closure="Percus-Yevick",
                                 formfactor=ff, meanRadius=50.0)
"""
import os
import sys

import numpy as np


#Fallback only: the machine this was developed against. SASSTUDIO_PATH takes
#precedence, which is what any non-Windows install should use.
DEFAULT_SASSTUDIO = r"C:\Users\kohlbrecher\switchdrive\SASstudio"

#Shared-library suffixes to recognise when listing plugins. .dylib is included
#for the macOS builds; omitting it would silently return an empty plugin list
#there rather than an error, which is the more confusing failure.
_LIB_SUFFIXES = (".dll", ".so", ".dylib")


def _importSASstudio(root=None):
    """Put SASstudio on sys.path and return its Plugin classes."""
    root = root or os.environ.get("SASSTUDIO_PATH", DEFAULT_SASSTUDIO)
    if not os.path.isdir(root):
        raise ImportError(
            f"SASstudio not found at {root!r}. Set SASSTUDIO_PATH, or pass "
            "root= explicitly. It provides the ctypes bindings to the "
            "compiled SASfit plugins.")
    if root not in sys.path:
        sys.path.insert(0, root)
    #The DLLs sit in the SASstudio root and are found relative to the working
    #directory, so the loader is run from there.
    cwd = os.getcwd()
    try:
        os.chdir(root)
        from lib.classes import Plugin, Plugin_Parameters
    finally:
        os.chdir(cwd)
    return Plugin, Plugin_Parameters


class SASfitFormFactor:
    """A SASfit plugin form factor, usable by PolydisperseSASBase.

    plugin      : plugin name, e.g. "sphere", "fuzzysphere", "cylinder"
    params      : parameter values IN HEADER-TABLE ORDER, which is p[] order
                  including any slots the table marks "dummy". Use describe()
                  or the plugin's own parameter_labels and
                  parameter_default_values to see the layout and sensible
                  starting values; for sphere that is
                  [R, dummy, dummy, eta]. A value left in the wrong slot
                  fails SILENTLY -- eta at the wrong index simply multiplies
                  the amplitude by zero.
    sizeIndex   : which slot is the polydisperse size. That slot is
                  overwritten per size class; everything else is held fixed.
    root        : SASstudio directory (defaults to SASSTUDIO_PATH)

    Provides the two methods PolydisperseSASBase requires -- F(Q, R) and
    outer_radius(R) -- so it is a drop-in replacement for the built-in Sphere
    and CoreShell.
    """

    def __init__(self, plugin, params, sizeIndex=0, root=None,
                 useInteractingVolume=True):
        Plugin, Plugin_Parameters = _importSASstudio(root)
        self._Params = Plugin_Parameters
        #Plugin does not record its own name, so keep it here for messages.
        self.name = plugin
        self.plugin = Plugin(plugin)
        if self.plugin.type != "ff":
            raise ValueError(
                f"{plugin!r} is a {self.plugin.type!r} plugin, not a form "
                "factor ('ff')")
        self.params = list(params)
        self.sizeIndex = int(sizeIndex)
        self.useInteractingVolume = bool(useInteractingVolume)
        self._amplitude = self.plugin.function_signatures["scattering amplitude"]
        self._volume = self.plugin.function_signatures["volume"]

    # ------------------------------------------------------------------
    def _paramsFor(self, size):
        p = list(self.params)
        p[self.sizeIndex] = float(size)
        return self._Params(p).params_pointer

    def interactingVolume(self, size):
        """V_int for one size, from the plugin's own _v function.

        The third argument is not a distribution index in the usual sense.
        sasfit_ff_sphere_v reads

            if (dist != 0) x = R;
            return 4/3 pi x^3;

        so dist == 0 means "use the x I passed as the radius" and dist != 0
        means "ignore x, use R from the parameters". dist = 1 is passed here
        so the radius comes from the parameters. Verified: 523599.0 =
        4 pi R^3 / 3 for R = 50, both as _v(R, param, 0) and _v(0, param, 1).
        """
        return float(self._volume(0.0, self._paramsFor(size), 1))

    def outer_radius(self, R):
        """Radius used for the HARD CORE in the structure-factor solve.

        With useInteractingVolume (the default) this is the radius of the
        sphere of equal interacting volume, so the excluded-volume size comes
        from the particle's geometry rather than being assumed equal to its
        scattering size. Set useInteractingVolume=False to recover the old
        behaviour, R_hard = R, which is what the built-in Sphere does.
        """
        R = np.atleast_1d(np.asarray(R, float))
        if not self.useInteractingVolume:
            return R
        out = np.empty_like(R)
        for i, r in enumerate(R):
            V = self.interactingVolume(r)
            #A plugin that does not implement _v may return 0; fall back to
            #the scattering radius rather than producing a zero hard core,
            #which would silently remove the excluded volume altogether.
            out[i] = (3.0*V/(4.0*np.pi))**(1.0/3.0) if V > 0 else r
        return out

    def F(self, Q, R):
        """Scattering amplitude, shape (nQ, nR).

        One ctypes call per (Q, size) pair. For 200 Q points and 60 size
        classes that is 12000 calls, which is a fraction of a second -- and
        negligible beside the Ornstein-Zernike solve it accompanies.
        """
        Q = np.atleast_1d(np.asarray(Q, float))
        R = np.atleast_1d(np.asarray(R, float))
        out = np.empty((Q.size, R.size))
        for j, r in enumerate(R):
            pp = self._paramsFor(r)
            for i, q in enumerate(Q):
                out[i, j] = self._amplitude(float(q), pp)
        return out

    # ------------------------------------------------------------------
    def describe(self):
        """Parameter names, descriptions and defaults, as the plugin reports."""
        return list(zip(self.plugin.parameter_labels,
                        self.plugin.parameter_descriptions,
                        self.plugin.parameter_default_values))

    def __repr__(self):
        return (f"SASfitFormFactor({self.name!r}, sizeIndex="
                f"{self.sizeIndex}, useInteractingVolume="
                f"{self.useInteractingVolume})")


def availableFormFactors(root=None):
    """Names of the SASfit plugins that can be loaded.

    Recognises .dll, .so and .dylib as well as the pure-Python SASstudio
    wrappers, so the same call works on all three platforms for which SASfit
    is built.

    NOTE this lists every plugin found, not only form factors: the type ('ff',
    'sq', 'sd') is only known once the plugin is loaded. SASfitFormFactor
    raises if handed something that is not a form factor.
    """
    root = root or os.environ.get("SASSTUDIO_PATH", DEFAULT_SASSTUDIO)
    pluginDir = os.path.join(root, "plugins")
    if not os.path.isdir(pluginDir):
        return []
    names = set()
    for fn in os.listdir(pluginDir):
        base, ext = os.path.splitext(fn)
        ext = ext.lower()
        if ext in _LIB_SUFFIXES and base.startswith("libsasfit_"):
            names.add(base[len("libsasfit_"):])
        elif ext == ".py" and base.startswith("sasstudio_"):
            names.add(base[len("sasstudio_"):])
    return sorted(names)


if __name__ == "__main__":                                 # pragma: no cover
    print("SASfit form-factor plugins found:")
    for n in availableFormFactors():
        print("   ", n)
