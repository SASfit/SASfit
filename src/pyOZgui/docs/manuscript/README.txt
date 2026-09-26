Manuscript bundle -- polydisperse structure factors (JAC draft)

CONTENTS
  manuscript.pdf            14 pages, built and checked (0 errors, 0 undefined refs)
  manuscript.tex            main file; \input{}s the two section files below
  validation_section.tex    MASTER copy of the Validation section
  section_5_3_schemes.tex   MASTER copy of section 5.3
  build.sh                  two-pass pdflatex build
  figures/                  all figures, including the new fig_survey.pdf
  tools/section53_survey.py the survey script
  tools/survey.jsonl        the 60 raw survey results

THE SECTION FILES ARE THE MASTERS. Edit validation_section.tex or
section_5_3_schemes.tex, then run build.sh. Do not edit a merged copy.

TO REBUILD
    sh build.sh

TO REGENERATE THE SURVEY AND ITS FIGURE
    python tools/section53_survey.py --out survey.jsonl
    python tools/section53_survey.py --plot survey.jsonl
    python tools/section53_survey.py --summarise survey.jsonl
Both the hard-sphere and square-well rows in survey.jsonl are complete
(30 points each, all converged). The adhesive-hard-sphere and Yukawa rows
have NOT been run; --resume will add them without recomputing the rest.

STATE OF THE DRAFT -- read before circulating
  * Section 5.3 rests on 60 converged state points. Two earlier versions of
    this section supported DIFFERENT conclusions: a four-point version said
    the scheme ordering reverses between potentials (an artefact of a defect,
    see the Validation section), and a nine-point version said the local
    monodisperse approximation is uniformly best. Neither survives.
  * fig2_scheme_error.pdf PREDATES those corrections. Its caption says so.
    It should be regenerated or dropped before submission.
  * The factor-286 error at s = phi = 0.40 has not been checked for
    physicality (min S(q) >= 0, largest size class not jammed). Do that
    before quoting it.
  * No fit to real measured data yet; all results use synthetic data
    generated from the model being fitted.
  * Timings are quoted informally and were never measured on a stated machine.
  * The introduction's literature framing is not a survey.
  * AI-assistance disclosure: substantial parts of the implementation were
    developed with AI assistance. IUCr policy should be checked.
