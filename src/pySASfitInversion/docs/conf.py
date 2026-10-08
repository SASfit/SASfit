"""Sphinx configuration for sasfit-inversion."""
import os
import sys

# Make the package importable without an editable install (docs can be
# built straight from a checkout).
sys.path.insert(0, os.path.abspath("../src"))

project = "sasfit-inversion"
author = "Joachim Kohlbrecher"
copyright = "2026, Paul Scherrer Institut"

try:
    from sasfit_inversion import __version__ as release
except ImportError:
    release = "0.0.1"
version = release

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.autosummary",
    "sphinx.ext.napoleon",       # NumPy/Google-style docstrings
    "sphinx.ext.viewcode",
    "sphinx.ext.mathjax",        # for the theory page's equations
    "sphinx.ext.intersphinx",
    "sphinx_autodoc_typehints",
]

autosummary_generate = True
autodoc_default_options = {
    "members": True,
    "undoc-members": False,
    "show-inheritance": True,
}
autodoc_typehints = "description"
napoleon_numpy_docstring = True
napoleon_google_docstring = True

intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
    "numpy": ("https://numpy.org/doc/stable/", None),
    "scipy": ("https://docs.scipy.org/doc/scipy/", None),
}

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# PySide6/matplotlib aren't needed to build the docs and may not be
# installed in a docs-only environment -- mock them out rather than
# requiring the full [gui] extra just to run autodoc over gui.app.
autodoc_mock_imports = ["PySide6", "matplotlib"]

html_theme = "furo"
html_static_path = ["_static"]
