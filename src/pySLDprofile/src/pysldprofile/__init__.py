"""pysldprofile -- radial scattering-length-density profiles from small-angle scattering.

Python implementation of

    S. Hansen, "Estimation of the density distribution from small-angle
    scattering data", J. Appl. Cryst. 49 (2016) 856-865,

i.e. the direct (one-step) Bayesian estimation of the density distribution
rho(r) of spherically, cylindrically or lamellarly symmetric scatterers with a
smoothness or maximum-entropy constraint, and Bayesian estimation of the
noise level (alpha) and the maximum radius r_max.
"""

from . import geometry, models, resolution
from .density import DensityProfileFit, ProfileResult, ProfileSolution
from .geometry import amplitude_matrix, pddf, radius_of_gyration
from .io_utils import load_data
from .regularization import MaxEnt, Smoothness, smoothness_operator
from .resolution import GaussianResolution, MatrixResolution, NoResolution, Resolution, RicianResolution, SlitResolution
from .background import Background, Constant, FunctionTerm, OrnsteinZernike, PowerLaw

__version__ = "0.1.0"

__all__ = [
    "DensityProfileFit",
    "Background",
    "Constant",
    "PowerLaw",
    "OrnsteinZernike",
    "FunctionTerm",
    "ProfileResult",
    "ProfileSolution",
    "amplitude_matrix",
    "pddf",
    "radius_of_gyration",
    "load_data",
    "Smoothness",
    "MaxEnt",
    "smoothness_operator",
    "NoResolution",
    "Resolution",
    "RicianResolution",
    "GaussianResolution",
    "SlitResolution",
    "MatrixResolution",
    "geometry",
    "models",
    "resolution",
]
