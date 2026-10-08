# Standalone PDF report build

This folder builds a single standalone PDF report (`sasfit-inversion.pdf`)
from the project's real documentation sources -- `installation.rst`,
`usage.rst`, and `theory.rst` one directory up -- via Sphinx's LaTeX
builder, rather than a hand-maintained separate `.tex` file. Each `.rst`
file here (`installation.rst`, `usage.rst`, `theory.rst`) is just a
one-line `.. include:: ../<name>.rst`, so this report always reflects
whatever is currently in `docs/`; there is nothing to keep in sync by
hand.

The API reference pages (`docs/api/*.rst`) are intentionally **not**
included here, since they use Sphinx autodoc and require the real
package to be installed/importable; they're part of the full HTML docs
build (`docs/`) instead. This report is the narrative/investigation
write-up only.

## Build

```bash
pip install -e "..[docs]"   # from the project root, if not already done
cd docs/report_build
make latexpdf        # Linux/macOS -- requires a LaTeX distribution
                      #   (TeX Live or MiKTeX) with latexmk on PATH
make.bat latexpdf     # Windows
```

The PDF is written to `_build/latex/sasfit-inversion.pdf`.

A quick HTML preview of the same three pages (no LaTeX required) is also
available via `make html` / `make.bat html`.

## Notes

- `conf.py` sets `fontpkg = ""` to fall back to the default Computer
  Modern fonts, so this builds without needing the `texlive-fonts-extra`
  (tex-gyre) package installed.
- Harmless `unknown document`/`unknown reference` warnings may appear
  during the build for `:doc:`/`:ref:` links that point into the
  (excluded-here) `api/` tree -- these are suppressed via
  `suppress_warnings` in `conf.py` and resolve fine in the full
  `docs/` build.
