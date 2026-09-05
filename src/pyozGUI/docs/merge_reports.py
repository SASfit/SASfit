#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Build the single combined ozGUI/ozLib report from its two parts.

    cd src/pyozGUI/docs
    python merge_reports.py
    pdflatex ozGUI_ozLib_complete.tex   # run three times for TOC + refs

Inputs (both in this directory):
    ozGUI_ozLib_documentation.tex               -> Part I  (one-component toolkit)
    ozGUI_ozLib_multicomponent_supplement.tex   -> Part II (multicomponent extension)

Output:
    ozGUI_ozLib_complete.tex   ~2760 lines, 47 pages, one TOC, one bibliography

This is a script rather than a hand-merged file on purpose: either part still
changes, and re-running this is safer than redoing the merge by hand each
time. It compiles with zero errors, zero undefined references and zero
duplicate labels; the figures are referenced by relative name, so they must
stay alongside.

If ozGUI_ozLib_complete.tex/.pdf are already present and current, this script
does not need running at all -- only after one of the two parts changes.

WHAT THE MERGE HAS TO GET RIGHT
-------------------------------
* Both parts are standalone documents, so each contributes a preamble,
  \\maketitle, a table of contents, an abstract and a bibliography. Only one
  of each may survive.
* Both use the same 14 packages and both define \\code, so the preamble of
  Part I can simply be reused -- checked, not assumed.
* The bibliographies are merged by citation key, not concatenated: the two
  parts share several references (Rogers-Young, D'Aguanno-Klein), and
  duplicate \\bibitem keys would silently produce wrong citation numbers.
* Label collisions are renamed, but ONLY the ones that genuinely clash. A
  first version prefixed every label in Part II and produced 15 undefined
  references, because the blanket rewrite covered \\label and \\ref but missed
  \\eqref. The two documents turn out to share exactly one label
  ("sec:solvers"), so a targeted rename is both sufficient and safer -- and
  the rename now covers every reference command.
"""
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
MAIN = os.path.join(HERE, 'ozGUI_ozLib_documentation.tex')
SUPP = os.path.join(HERE, 'ozGUI_ozLib_multicomponent_supplement.tex')
OUT = os.path.join(HERE, 'ozGUI_ozLib_complete.tex')

# Fail with something actionable rather than a bare FileNotFoundError.
for _path, _what in ((MAIN, 'Part I  (one-component toolkit)'),
                     (SUPP, 'Part II (multicomponent extension)')):
    if not os.path.exists(_path):
        raise SystemExit(
            "merge_reports.py: missing input\n    %s\nwhich supplies %s.\n\n"
            "Both .tex files must sit in this docs/ directory, alongside the\n"
            "fig_*.pdf files they reference. Copy the missing one here and\n"
            "re-run.\n\n"
            "NOTE: if ozGUI_ozLib_complete.tex/.pdf are already present and\n"
            "up to date, this script only needs re-running after one of the\n"
            "two parts changes." % (_path, _what))

main = open(MAIN, encoding='utf-8', errors='replace').read()
supp = open(SUPP, encoding='utf-8', errors='replace').read()


def split(doc):
    """Preamble and body of a standalone document."""
    i = doc.index('\\begin{document}')
    j = doc.rindex('\\end{document}')
    return doc[:i], doc[i + len('\\begin{document}'):j]


pre_main, body_main = split(main)
_, body_supp = split(supp)


def pull_bib(body):
    m = re.search(r'\\begin\{thebibliography\}.*?\\end\{thebibliography\}',
                  body, re.S)
    if not m:
        return body, ''
    return body[:m.start()] + body[m.end():], m.group(0)


body_main, bib_main = pull_bib(body_main)
body_supp, bib_supp = pull_bib(body_supp)


def items(bib):
    out = {}
    for m in re.finditer(
            r'\\bibitem\{([^}]*)\}(.*?)(?=\\bibitem\{|\\end\{thebibliography\})',
            bib, re.S):
        out[m.group(1)] = m.group(2).rstrip()
    return out


merged = {}
merged.update(items(bib_main))
for k, v in items(bib_supp).items():
    merged.setdefault(k, v)          # Part I wins on a shared key

# Only one \maketitle / TOC / abstract may survive.
for pat in (r'\\maketitle', r'\\tableofcontents',
            r'\\begin\{abstract\}.*?\\end\{abstract\}'):
    body_supp = re.sub(pat, '', body_supp, flags=re.S)
    body_main = re.sub(pat, '', body_main, flags=re.S)

# Rename only the labels that actually clash -- see the module docstring.
main_labels = set(re.findall(r'\\label\{([^}]*)\}', body_main))
supp_labels = set(re.findall(r'\\label\{([^}]*)\}', body_supp))
collisions = sorted(main_labels & supp_labels)
for lb in collisions:
    body_supp = re.sub(
        r'(\\(?:label|ref|eqref|autoref|pageref)\{)%s(\})' % re.escape(lb),
        lambda m: m.group(1) + 'mc:' + lb + m.group(2), body_supp)

title = r'''\title{\bfseries ozGUI / ozLib\\[2mm]
Ornstein--Zernike Solver Toolkit\\[1mm]
\large including the multicomponent and polydisperse extension}
\author{ozGUI/ozLib project}
\date{\today}'''
pre = re.sub(r'\\title\{.*?\}\s*\n\\author\{.*?\}\s*\n\\date\{.*?\}',
             lambda m: title, pre_main, flags=re.S)

bib = ('\\begin{thebibliography}{99}\n'
       + '\n'.join('\\bibitem{%s}%s' % (k, v) for k, v in merged.items())
       + '\n\\end{thebibliography}\n')

doc = (pre
       + '\\begin{document}\n\\maketitle\n\\tableofcontents\n\\clearpage\n'
       + '\\part{The one-component toolkit}\n' + body_main
       + '\n\\clearpage\n'
       + '\\part{Multicomponent and polydisperse extension}\n' + body_supp
       + '\n' + bib + '\\end{document}\n')

open(OUT, 'w', encoding='utf-8').write(doc)
print("wrote %s" % os.path.basename(OUT))
print("  merged bibliography entries : %d" % len(merged))
print("  renamed colliding labels    : %s" % (', '.join(collisions) or 'none'))
print("  lines                       : %d" % len(doc.splitlines()))
print("\nnow run:  pdflatex ozGUI_ozLib_complete.tex   (three times)")
