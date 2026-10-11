Using the GUI
=============

Start
-----

.. code-block:: bash

   python src/gui/app.py
   # or, after an editable install:
   sasfit-inversion-gui

Layout
------

The window has a control column on the left and two plots on the right:
:math:`I(q)` with error bars, background and fit (log–log) at the top, the
recovered distribution with its uncertainty band and a dotted zero line at
the bottom.

The control column contains three groups (*Data*, *Background*,
*Inversion*) in a scroll area and, below it, the **info panel**. The
border between them is a splitter: drag it to change the height of the
info panel, like the border between the two plots.

Info panel
    All explanations and all detailed results are shown in this single
    panel instead of in tooltips or long text blocks. It always shows the
    text of the control under the mouse pointer (the control or its label);
    with an open drop-down list it explains the highlighted entry (e.g. each
    solver or kernel). Moving away shows a short default text. Until the
    splitter is dragged, the panel is kept just large enough for the
    longest text at the current width, so the layout does not jump while
    the mouse moves; it never grows beyond that height, and if it is made
    smaller its text scrolls. Dragging it back to full height re-enables
    the automatic height. The
    static texts live in ``src/gui/help_texts.py`` (Qt rich text, one entry
    per control, solver, kernel and solver option).

    The control column itself only shows one-line summaries; the details
    appear in the info panel right after the action and again whenever the
    mouse is over the summary line:

    * **Data** (after loading): file and folder, number of points and
      skipped lines, :math:`q` range and spacing (points per decade for
      log-spaced data), :math:`I`, :math:`\Delta I` and
      :math:`\Delta I/I` ranges, :math:`D_{max}=\pi/q_{min}`, the
      resolution :math:`\pi/q_{max}`, the Shannon number and the number of
      data points per Shannon channel overall and in the upper third of the
      :math:`q` range (with a hint to use B-splines if it is below one).
    * **Background** (after a fit or with a fixed value): model, fit range
      and number of points, all parameters with errors, :math:`\chi^2_r` of
      the fit, the subtracted constant and its fraction of
      :math:`I(q_{max})`, and notes on extrapolation and degenerate
      parameters.
    * **Result** (after a solve): :math:`\chi^2_r` and its mean per third
      of the :math:`q` range, Pearson :math:`\chi^2/M`, Poisson deviance
      (G-test)/M and the KL divergence of the normalized curves, the
      estimated error scale :math:`s` (Bayesian evidence, option *Error
      bars: overall scale estimated*), solver and options, how the regularization
      strength was chosen (:math:`\lambda`, :math:`N_g`, :math:`\ln Z`,
      ...), the representation (grid, interpolation, B-splines, quadrature
      points), kernel and :math:`\alpha`, iterations, computing time,
      derived quantities (PDDF: :math:`I(0)=4\pi\int p\,dr`,
      :math:`R_g^2=\int p\,r^2dr/(2\int p\,dr)` and the position of the
      maximum; size distributions: maximum of :math:`x(r)`, number-weighted
      mean radius and maximum of the volume distribution), the type of
      error band, bootstrap statistics and warnings.

    The current suggestions for :math:`r_{max}` and the number of grid
    points are appended to the corresponding control texts.

Workflow
--------

1. **Load data file** -- three columns :math:`q`, :math:`I`,
   :math:`\Delta I`. :math:`r_{max}` and the number of grid points are
   suggested (:math:`D_{max}=\pi/q_{min}`, Shannon number), and are
   re-suggested when the kernel changes.
   **Error bars** -- from the file's :math:`\Delta I` column, or (always
   for files without one) an error model: estimated from local cubic fits
   through 5 points (default) or from neighbour interpolation, combined over
   a window of points, Poisson (:math:`\sigma^2=I`, counts only) or a relative error
   in percent (see :doc:`theory_forward_model`). The data info shows the
   ratio of the cubic-fit estimate to the file errors as a check.
2. **Background** -- fit :math:`I_{bg}+c\,q^{-\alpha}` in a :math:`q` range
   (only :math:`I_{bg}` is subtracted) or enter a fixed value.
3. **Kernel** -- :math:`4\pi j_0(qr)` for the PDDF (no :math:`\alpha`),
   or the sphere kernel for a radius distribution (:math:`\alpha=6`
   default).
4. **Grid** -- :math:`r_{max}`, number of points, spacing (uniform,
   quadratic, logarithmic) and interpolation between the points (linear or
   cubic). Spacing and interpolation are hidden in B-spline mode;
   interpolation is fixed to linear for the solvers that need
   :math:`x\ge0`.
5. **Solver** and its options (appear only for the solver that has them),
   optionally "Force :math:`\chi^2_r=1`" (Bayesian evidence).
6. **B-splines** -- "Represent p(r) by B-splines (any solver)", number of
   coefficients and knot spacing.
7. **Monte Carlo uncertainty** -- parametric bootstrap with :math:`N`
   replicates.
8. **Solve**.

Result line
-----------

The one-line summary below *Solve* shows :math:`\chi^2_r`, the number of
bootstrap replicates if used, and a warning sign if something needs
attention; everything else is in the info panel (see above). Warnings
include a solver that needs a non-negative kernel used with a signed
kernel, and a :math:`\chi^2_r=1` target that was not reachable.

Plots
-----

The distribution plot shows the MAP solution with the analytic
:math:`\pm1\sigma` band for the Bayesian-evidence solver, or the bootstrap
mean with its :math:`\pm1\sigma` band and the single solve (dashed) when
the Monte Carlo option is used. The :math:`y` axis is :math:`p(r)` for the
PDDF kernel and :math:`x(r)=N(r)\,r^{\alpha}` for size distributions. The
zero line style is set by ``ZERO_LINE_STYLE`` in ``app.py``.
