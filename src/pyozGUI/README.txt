Download bundle -- 2026-09-07

WHAT NEEDS COPYING, AND WHAT DOES NOT
-------------------------------------
Most of this session's work was deployed straight to your disk via the
filesystem connector and is ALREADY IN PLACE. Only two categories are here
because they could not be:

  1. BINARIES. The connector writes text only, so PDFs had to come this way.
       docs/manuscript/manuscript.pdf            15 pages, built and checked
       docs/manuscript/figures/fig_survey.pdf    NEW -- heat map from your
                                                 complete 120-point survey

  2. A FILE NEVER DEPLOYED.
       mixture_validation_tab.py  ->  copy to src/pyozgui/
     Already registered in oZgui.py EXTRA_TABS as "Mixture validation", so it
     will appear as soon as the file is in place. It puts the numerical solver
     beside the analytic references (mixscatter's Vrij PY and gazzillo_msa)
     and reports the coarse/fine grid ratio -- the diagnostic that exposed the
     identical-cores defect.

The .tex files and build.sh are included for completeness and are IDENTICAL to
what is already on your disk. You do not need to copy them.

*** DO NOT copy an older file over a newer deployment. *** Earlier in this
work a downloaded bundle overwrote a newer gazzillo_msa.py, silently removing
a function the validation tab imports; the only symptom would have been an
AttributeError in an apparently unrelated place. If in doubt, copy only the
two items in categories 1 and 2 above.

TO REBUILD THE MANUSCRIPT
-------------------------
    cd docs/manuscript && sh build.sh
Last verified: 15 pages, 0 errors, 0 undefined references, 0 overfull boxes.
The two section .tex files are the MASTERS; manuscript.tex \input{}s them.

SECTION 5.3 NOW USES YOUR COMPLETE SURVEY
-----------------------------------------
All 120 points, four systems, every point converged. The completed adhesive
row overturned the conclusion that had been drawn from its first 8 points, so
this is the FIFTH version of that section. The draft note in the .tex records
all five and warns against tightening the language without more data.

Still to do there: a second Yukawa parameter set (the regime-2 claim rests on
one screening length and one strength), and a physicality check on the
s = phi = 0.40 corner before the factor-286 figure is quoted.

NEXT STEPS are in docs/NEXT_SESSION.md, already on your disk. It covers the
solver-cascade gap in section53_survey.py, the timing table to measure, the
heavy-tailed quadrature question, and joining the two report documents.
