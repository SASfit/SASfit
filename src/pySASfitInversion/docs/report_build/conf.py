"""Sphinx configuration for sasfit-inversion (LaTeX/PDF report build).

This is a trimmed, standalone Sphinx project that builds ONLY the
narrative pages (installation, usage, theory) into a single PDF report
via Sphinx's LaTeX builder -- the API reference pages live in the main
docs/ tree (docs/conf.py) and require the real package to be importable
via autodoc; they are intentionally left out of this report-only build
so it can be built from docs content alone, no package install needed.

Usage (from this directory):

    sphinx-build -b latex . _build/latex
    cd _build/latex
    make        # runs latexmk; produces sasfit-inversion.pdf

(or `latexmk -pdf sasfit-inversion.tex` directly, or plain `pdflatex`
run twice if latexmk isn't installed).
"""
project = "sasfit-inversion"
author = "Joachim Kohlbrecher"
copyright = "2026, Paul Scherrer Institut"

release = "0.0.1"
version = release

extensions = [
    "sphinx.ext.mathjax",
]

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

html_theme = "alabaster"
html_static_path = ["_static"]

# LaTeX (PDF) output settings
latex_engine = "pdflatex"
latex_elements = {
    "papersize": "a4paper",
    "pointsize": "11pt",
    # Use default Computer Modern fonts -- avoids a hard dependency on
    # the tex-gyre font packages (texlive-fonts-extra), which may not be
    # installed everywhere this report gets rebuilt.
    "fontpkg": "",
}
latex_documents = [
    ("index", "sasfit-inversion.tex", "sasfit-inversion Documentation",
     "Joachim Kohlbrecher", "manual"),
]

# These narrative pages cross-reference the api/ tree (excluded here),
# e.g. usage.rst's :doc:`api/sasfit_inversion.kernel_registry` -- those
# resolve fine in the full docs build; here they'd just be a harmless
# "unknown document" warning, so silence it for this report-only build.
suppress_warnings = ["ref.ref", "ref.doc", "toc.not_readable"]
