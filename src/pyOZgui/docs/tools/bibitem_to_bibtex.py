#!/usr/bin/env python3
"""Transcribe the report's \\bibitem entries into BibTeX.

    cd docs
    python tools/bibitem_to_bibtex.py > report_bibitems.bib

Reads the thebibliography blocks of

    ozGUI_ozLib_documentation.tex
    ozGUI_ozLib_multicomponent_supplement.tex

and writes one @Article per \\bibitem, keeping the existing citation keys so
the \\cite commands in the text keep working unchanged.

WHY THIS EXISTS. The report carries its references as two hand-written
thebibliography blocks, one per fragment, so the built PDF prints TWO
bibliographies. Moving them into the shared docs/ozgui.bib fixes that and
gives the report and the manuscript one source of bibliographic truth. The
metadata is already present in the \\bibitem text, so this is TRANSCRIPTION,
not reconstruction -- which matters, because reconstructing citation metadata
from memory is how a fabricated author reached ozgui.bib once already.

*** THE OUTPUT IS TRANSCRIBED, NOT VERIFIED. ***
No DOI is resolved and no field is checked against the article. Import the
result into JabRef and let it fetch metadata by DOI before relying on it.
Of 35 entries, 33 parse cleanly; two need hand attention:

    likos2002      journal string has an unusual "5, No. 1(29), 173" form
    blumarias2006  the original \\bibitem has no journal at all

CONVERSION, once the entries are in ozgui.bib:
  1. delete both thebibliography blocks from the fragments;
  2. add to ozGUI_ozLib_report.tex, before \\end{document}:
         \\bibliographystyle{plainnat}
         \\bibliography{ozgui}
     (no path prefix -- the master sits beside the .bib);
  3. \\usepackage[authoryear,round]{natbib} in the master preamble;
  4. add a bibtex pass to build_report.sh, as in manuscript/build.sh.
Keys are preserved, so no \\cite in the text needs editing.
"""
import re
import sys

SOURCES = ("ozGUI_ozLib_documentation.tex",
           "ozGUI_ozLib_multicomponent_supplement.tex")


def blocks(path):
    with open(path, encoding="utf-8") as fh:
        s = fh.read()
    m = re.search(r"\\begin\{thebibliography\}.*?\\end\{thebibliography\}",
                  s, re.S)
    if not m:
        return
    for part in re.split(r"\\bibitem\{", m.group(0))[1:]:
        key = part.split("}")[0]
        txt = part[len(key) + 1:].split("\\end{thebibliography}")[0]
        yield key, txt


def _tidy(x):
    #NOTE the second replacement. "\\ " is an escaped inter-word space in the
    #source ("J.\ Chem.\ Phys.\ \textbf{111}"); replacing it with the empty
    #string rather than a space glues the volume onto the journal name, and
    #then only 7 of 35 entries split correctly. It cost a debugging round.
    x = x.replace("~", " ").replace("\\ ", " ").replace("\\&", "and")
    x = re.sub(r"\\textbf\{([^}]*)\}", r"\1", x)
    x = re.sub(r"\\emph\{([^}]*)\}", r"\1", x)
    return " ".join(x.split()).strip(" ,.")


def parse(key, raw):
    tm = re.search(r"\\emph\{(.*?)\}", raw, re.S)
    title = " ".join(tm.group(1).split()) if tm else ""
    author = _tidy(raw[:tm.start()] if tm else raw)
    tail = _tidy(raw[tm.end():] if tm else "")

    eprint = ""
    em = re.search(r"arXiv:([0-9a-zA-Z\-\./]+?)[.;\s]*$", tail)
    if em:
        eprint = em.group(1).rstrip(".")
    tailNoArxiv = re.sub(r";?\s*arXiv:.*$", "", tail).strip(" ,.;")

    m2 = re.match(r"^(.*?)\s+(\d+)\s*,\s*([0-9]+)\s*\((\d{4})\)", tailNoArxiv)
    if m2:
        journal, vol, pages, year = [m2.group(i).strip(" .,")
                                     for i in (1, 2, 3, 4)]
    else:
        ym = re.search(r"\((\d{4})\)", tailNoArxiv)
        journal, vol, pages = tailNoArxiv, "", ""
        year = ym.group(1) if ym else ""
    journal = re.sub(r"\s+", " ", journal).strip()
    return dict(key=key, author=author, title=title, journal=journal,
                volume=vol, pages=pages, year=year, eprint=eprint)


def main():
    seen, entries = set(), []
    for src in SOURCES:
        try:
            for key, raw in blocks(src):
                if key in seen:
                    continue
                seen.add(key)
                entries.append(parse(key, raw))
        except FileNotFoundError:
            print(f"% WARNING: {src} not found -- run this from docs/",
                  file=sys.stderr)

    print("% Transcribed from the \\bibitem entries of the two report")
    print("% fragments by tools/bibitem_to_bibtex.py.")
    print("%")
    print("% TRANSCRIBED, NOT VERIFIED: no DOI resolved, no field checked")
    print("% against the article. Import into JabRef and fetch metadata by")
    print("% DOI before relying on any of it. likos2002 and blumarias2006")
    print("% need hand attention -- see the script docstring.")
    print()
    for e in entries:
        print(f"@Article{{{e['key']},")
        for field, value in (("author", e["author"]), ("title", e["title"]),
                             ("journal", e["journal"]),
                             ("volume", e["volume"]),
                             ("pages", e["pages"]), ("year", e["year"])):
            if value:
                print(f"  {field:8s}= {{{value}}},")
        if e["eprint"]:
            print(f"  eprint  = {{{e['eprint']}}},")
            print("  archivePrefix = {arXiv},")
        print("}\n")
    print(f"% {len(entries)} entries transcribed.", file=sys.stderr)


if __name__ == "__main__":
    main()
