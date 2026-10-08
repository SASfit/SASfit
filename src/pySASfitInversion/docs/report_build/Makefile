# Minimal makefile for the standalone sasfit-inversion PDF report
# (narrative pages only -- installation, usage, theory).
#
# Usage:
#   make latexpdf      -- build sasfit-inversion.pdf in _build/latex/
#   make html          -- build an HTML preview in _build/html/
#   make clean         -- remove _build/

SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = .
BUILDDIR      = _build

help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
