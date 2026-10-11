References
==========

Literature relevant for the methods implemented in *sasfit-inversion*,
grouped by topic. Each entry notes where it is used in the package or in
this documentation. DOIs are given where they were checked.

Small-angle scattering: indirect Fourier transformation and size distributions
-------------------------------------------------------------------------------

* O. Glatter (1977). A new method for the evaluation of small-angle
  scattering data. *J. Appl. Cryst.* **10**, 415–421.
  -- Indirect Fourier transformation with a cubic B-spline basis; basis of
  the B-spline option (:doc:`theory_discretization`).
* O. Glatter (1980). Determination of particle-size distribution functions
  from small-angle scattering data by means of the indirect transformation
  method. *J. Appl. Cryst.* **13**, 7–11.
  https://doi.org/10.1107/S0021889880011429
  -- Size distributions with the IFT approach (sphere kernel).
* P. B. Moore (1980). Small-angle scattering. Information content and
  error analysis. *J. Appl. Cryst.* **13**, 168–175.
  -- Shannon sampling of :math:`I(q)`, number of independent parameters,
  sine-series expansion of :math:`p(r)` (:doc:`theory_forward_model`,
  :doc:`theory_discretization`).
* D. I. Svergun, A. V. Semenyuk & L. A. Feigin (1988). Small-angle-
  scattering-data treatment by the regularization method. *Acta Cryst.*
  **A44**, 244–250.
  -- Tikhonov regularization for SAS data.
* D. I. Svergun (1992). Determination of the regularization parameter in
  indirect-transform methods using perceptual criteria. *J. Appl. Cryst.*
  **25**, 495–503.
  -- GNOM; choice of the regularization parameter.
* S. Hansen & J. S. Pedersen (1991). A comparison of three different
  methods for analysing small-angle scattering data. *J. Appl. Cryst.*
  **24**, 541–548.
  -- Comparison of IFT, maximum entropy and regularization for SAS.
* S. Steenstrup & S. Hansen (1994). The maximum-entropy method without the
  positivity constraint -- applications to the determination of the
  distance-distribution function in small-angle scattering. *J. Appl.
  Cryst.* **27**, 574–580.
  -- MaxEnt for signed :math:`p(r)`; background for the positive/negative
  entropy of the signed EM (:doc:`theory_em_maxent`).
* S. Hansen (2000). Bayesian estimation of hyperparameters for indirect
  Fourier transformation in small-angle scattering. *J. Appl. Cryst.*
  **33**, 1415–1421.
  -- Smoothness penalty with :math:`p(0)=p(D_{max})=0` (eq. 19 and the
  matrix stated in the text), Bayesian choice of :math:`\lambda`
  (``bayesian_evidence``, :doc:`theory_regularization`).
* B. Vestergaard & S. Hansen (2006). Application of Bayesian analysis to
  indirect Fourier transformation in small-angle scattering. *J. Appl.
  Cryst.* **39**, 797–804.
  -- Evidence formula and number of good parameters :math:`N_g`
  (:mod:`sasfit_inversion.solvers.bayesian_evidence`).
* S. Hansen (2012). BayesApp: a web site for indirect transformation of
  small-angle scattering data. *J. Appl. Cryst.* **45**, 566–567.
  -- Reference implementation of Bayesian IFT.
* A. H. Larsen & M. C. Pedersen (2021). Experimental noise in small-angle
  scattering can be assessed using the Bayesian indirect Fourier
  transformation. *J. Appl. Cryst.* **54**, 1281–1289.
  https://doi.org/10.1107/S1600576721006877
  -- Estimating and rescaling the error level with Bayesian IFT; related
  to the evidence-based error scale (``noise_scale="estimate"``).
* J. A. Potton, G. J. Daniell & B. D. Rainford (1988). A new method for the
  determination of particle size distributions from small angle neutron
  scattering measurements. *J. Appl. Cryst.* **21**, 891ff.
  https://doi.org/10.1107/S0021889888004595
  -- Maximum entropy for SANS size distributions.
* P. R. Jemian, J. R. Weertman, G. G. Long & R. D. Spal (1991).
  Characterization of 9Cr-1MoVNb steel by anomalous small-angle X-ray
  scattering. *Acta Metall. Mater.* **39**, 2477–2487.
  -- Maximum-entropy size distributions (the MaxEnt method of Irena).
* J. S. Pedersen (1994). Determination of size distributions from
  small-angle scattering data for systems with effective hard-sphere
  interactions. *J. Appl. Cryst.* **27**, 595–608.
  -- Size distributions by regularized least squares.
* B. R. Pauw, J. S. Pedersen, S. Tardif, M. Takata & B. B. Iversen (2013).
  Improvements and considerations for size distribution retrieval from
  small-angle scattering data by Monte Carlo methods. *J. Appl. Cryst.*
  **46**, 365–371.
  -- Monte Carlo alternative to regularized inversion (McSAS).
* I. Bressler, B. R. Pauw & A. F. Thünemann (2015). McSAS: software for the
  retrieval of model parameter distributions from scattering patterns.
  *J. Appl. Cryst.* **48**, 962–969.
  https://doi.org/10.1107/S1600576715007347
* I. Breßler, J. Kohlbrecher & A. F. Thünemann (2015). SASfit: a tool for
  small-angle scattering data analysis using a library of analytical
  expressions. *J. Appl. Cryst.* **48**, 1587–1598.
* J. Kohlbrecher & I. Breßler (2022). Updates in SASfit for fitting
  analytical expressions and numerical models to small-angle scattering
  patterns. *J. Appl. Cryst.* **55**, 1677–1688.
  https://doi.org/10.1107/S1600576722009037
  -- The "JAC 2022 paper" of the code comments: EM, EM + smoothing,
  EM + MaxEnt with constant and adaptive prior (eq. 36–55), size weighting
  :math:`x=N\,R^\alpha`, G-test, the SASfit C solvers this package ports
  (``em_discrepancy``, ``maxent_em``).
* D. Franke, C. M. Jeffries & D. I. Svergun (2015). Correlation Map, a
  goodness-of-fit test for one-dimensional X-ray scattering spectra.
  *Nature Methods* **12**, 419–422. https://doi.org/10.1038/nmeth.3358
  -- Goodness of fit without error bars (run-length test); an alternative
  to the statistics in :mod:`sasfit_inversion.goodness_of_fit`.

Books on small-angle scattering
-------------------------------

* O. Glatter & O. Kratky (eds.) (1982). *Small Angle X-ray Scattering*.
  Academic Press, London.
* L. A. Feigin & D. I. Svergun (1987). *Structure Analysis by Small-Angle
  X-ray and Neutron Scattering*. Plenum Press, New York.
* A. Guinier & G. Fournet (1955). *Small-Angle Scattering of X-rays*.
  Wiley, New York.
* P. Debye (1915). Zerstreuung von Röntgenstrahlen. *Ann. Phys.* **351**
  (4. Folge **46**), 809–823. -- Orientational average, :math:`j_0(qr)`
  kernel.

Ill-posed problems and regularization
-------------------------------------

* D. L. Phillips (1962). A technique for the numerical solution of certain
  integral equations of the first kind. *J. ACM* **9**, 84–97.
* S. Twomey (1963). On the numerical solution of Fredholm integral
  equations of the first kind by the inversion of the linear system
  produced by quadrature. *J. ACM* **10**, 97–101.
* A. N. Tikhonov (1963). Solution of incorrectly formulated problems and
  the regularization method. *Soviet Math. Dokl.* **4**, 1035–1038.
* A. N. Tikhonov & V. Y. Arsenin (1977). *Solutions of Ill-Posed
  Problems*. Winston, Washington.
* V. A. Morozov (1966). On the solution of functional equations by the
  method of regularization. *Soviet Math. Dokl.* **7**, 414–417.
  -- Discrepancy principle (:doc:`theory_regularization`).
* G. H. Golub, M. Heath & G. Wahba (1979). Generalized cross-validation as
  a method for choosing a good ridge parameter. *Technometrics* **21**,
  215–223. -- GCV (``tikhonov_standard_gcv``, ``tsvd_gcv``,
  ``general_tikhonov_gcv``).
* P. C. Hansen (1992). Analysis of discrete ill-posed problems by means of
  the L-curve. *SIAM Review* **34**, 561–580.
* P. C. Hansen & D. P. O'Leary (1993). The use of the L-curve in the
  regularization of discrete ill-posed problems. *SIAM J. Sci. Comput.*
  **14**, 1487–1503.
* A. Cultrera & L. Callegaro (2020). A simple algorithm to find the
  L-curve corner in the regularisation of ill-posed inverse problems.
  *IOP SciNotes* **1**, 025004. https://doi.org/10.1088/2633-1357/abad0d
  -- Menger-curvature corner criterion (:mod:`sasfit_inversion.lambda_selection`).
* P. C. Hansen (1994). Regularization Tools: a Matlab package for analysis
  and solution of discrete ill-posed problems. *Numer. Algorithms* **6**,
  1–35; version 4.0: *Numer. Algorithms* **46** (2007), 189–194.
  -- ``std_form``, filter factors, ``maxent.m`` (transcribed in
  :mod:`~sasfit_inversion.solvers.general_tikhonov`,
  :mod:`~sasfit_inversion.solvers.svd_methods`,
  :mod:`~sasfit_inversion.solvers.hansen_maxent`).
* P. C. Hansen (1998). *Rank-Deficient and Discrete Ill-Posed Problems:
  Numerical Aspects of Linear Inversion*. SIAM, Philadelphia.
* P. C. Hansen (2010). *Discrete Inverse Problems: Insight and
  Algorithms*. SIAM, Philadelphia.
* H. W. Engl, M. Hanke & A. Neubauer (1996). *Regularization of Inverse
  Problems*. Kluwer, Dordrecht.
* M. Donatelli & L. Reichel (2014). Square smoothing regularization
  matrices with accurate boundary conditions. *J. Comput. Appl. Math.*
  **272**, 334–349. -- Boundary conditions of the difference operators
  (:mod:`sasfit_inversion.regularization`).
* R. E. Jones (2023). ARLS -- Automatically Regularized Least Squares.
  MATLAB Central File Exchange no. 130259; described in C. Moler's blog
  *Cleve's Corner*, 16 June 2023. -- ``arlsnn`` (discrete Picard
  condition, no parameter to choose).

Bayesian inference and maximum entropy
--------------------------------------

* D. J. C. MacKay (1992). Bayesian interpolation. *Neural Computation*
  **4**, 415–447. -- Evidence framework; joint estimation of the
  regularization and noise hyperparameters (error scale :math:`s`).
* J. Skilling & R. K. Bryan (1984). Maximum entropy image reconstruction:
  general algorithm. *Mon. Not. R. Astron. Soc.* **211**, 111–124.
* S. F. Gull (1989). Developments in maximum entropy data analysis. In
  J. Skilling (ed.), *Maximum Entropy and Bayesian Methods*, pp. 53–71.
  Kluwer, Dordrecht.
* S. F. Gull & J. Skilling (1990). *The MEMSYS5 Users' Manual*. Maximum
  Entropy Data Consultants Ltd, Royston. -- Positive/negative entropy.
* M. P. Hobson & A. N. Lasenby (1998). The entropic prior for
  distributions with positive and negative values. *Mon. Not. R. Astron.
  Soc.* **298**, 905–908. -- Derivation of
  :math:`S_\pm=\sum[\psi-2m-p\,\mathrm{asinh}(p/2m)]` used by the signed
  EM + MaxEnt.
* K. Horne (1985). Images of accretion discs. I. The eclipse mapping
  method. *Mon. Not. R. Astron. Soc.* **213**, 129–141. -- Default model
  from a smoothed version of the image (adaptive prior of ``maxent_em``).

EM algorithm and related iterative methods
------------------------------------------

* W. H. Richardson (1972). Bayesian-based iterative method of image
  restoration. *J. Opt. Soc. Am.* **62**, 55–59.
* L. B. Lucy (1974). An iterative technique for the rectification of
  observed distributions. *Astron. J.* **79**, 745–754.
* A. P. Dempster, N. M. Laird & D. B. Rubin (1977). Maximum likelihood from
  incomplete data via the EM algorithm. *J. R. Stat. Soc. B* **39**, 1–38.
* L. A. Shepp & Y. Vardi (1982). Maximum likelihood reconstruction for
  emission tomography. *IEEE Trans. Med. Imaging* **1**, 113–122.
* Y. Vardi & D. Lee (1993). From image deblurring to optimal investments:
  maximum likelihood solutions for positive linear inverse problems.
  *J. R. Stat. Soc. B* **55**, 569–598. -- EM for Fredholm equations with
  non-negative kernels.
* B. W. Silverman, M. C. Jones, J. D. Wilson & D. W. Nychka (1990). A
  smoothed EM approach to indirect estimation problems, with particular
  reference to stereology and emission tomography. *J. R. Stat. Soc. B*
  **52**, 271–324. -- EMS: EM with a smoothing step after every
  iteration (``em_discrepancy``).
* D. W. Nychka (1990). Some properties of adding a smoothing step to the
  EM algorithm. *Stat. Probab. Lett.* **9**, 187–193.
* P. J. Green (1990). Bayesian reconstructions from emission tomography
  data using a modified EM algorithm. *IEEE Trans. Med. Imaging* **9**,
  84–93. -- One-step-late EM for penalized likelihood.
* P. P. B. Eggermont & V. N. LaRiccia (1995). Maximum smoothed likelihood
  density estimation for inverse problems. *Ann. Statist.* **23**,
  199–220.
* P. P. B. Eggermont (1999). Nonlinear smoothing and the EM algorithm for
  positive integral equations of the first kind. *Appl. Math. Optim.*
  **39**, 75–91.
* C. Byrne & P. P. B. Eggermont (2011). EM algorithms. In O. Scherzer
  (ed.), *Handbook of Mathematical Methods in Imaging*, pp. 271–344.
  Springer, New York.
* M. Chae, R. Martin & S. G. Walker (2019). On an algorithm for solving
  Fredholm integrals of the first kind. *Stat. Comput.* **29**, 645–654.
  -- Shift and kernel split for signed solutions and kernels
  (``em_general_signed_kernel``, :doc:`theory_em_maxent`).
* D. S. C. Biggs & M. Andrews (1995). Conjugate gradient acceleration of
  maximum-likelihood image restoration. *Electron. Lett.* **31**,
  1985–1986.
* D. S. C. Biggs & M. Andrews (1997). Acceleration of iterative image
  restoration algorithms. *Appl. Opt.* **36**, 1766–1775. -- Vector
  extrapolation used for all EM-type iterations
  (:mod:`sasfit_inversion.solvers.acceleration`).
* D. S. C. Biggs (1998). *Accelerated Iterative Blind Deconvolution*. PhD
  thesis, University of Auckland. http://hdl.handle.net/2292/1760

Splines, interpolation and quadrature
-------------------------------------

* C. de Boor (1972). On calculating with B-splines. *J. Approx. Theory*
  **6**, 50–62.
* M. G. Cox (1972). The numerical evaluation of B-splines. *IMA J. Appl.
  Math.* **10**, 134–149.
* C. de Boor (1978). *A Practical Guide to Splines*. Springer, New York.
  -- Cox–de Boor recursion, not-a-knot end conditions
  (:func:`~sasfit_inversion.kernels.cubic_basis`,
  :mod:`sasfit_inversion.spline_basis`).
* F. N. Fritsch & R. E. Carlson (1980). Monotone piecewise cubic
  interpolation. *SIAM J. Numer. Anal.* **17**, 238–246. -- PCHIP;
  discussed and not used (non-linear in the node values).
* C. E. Shannon (1949). Communication in the presence of noise. *Proc.
  IRE* **37**, 10–21. -- Sampling theorem, Shannon number.

Error estimation and goodness of fit
------------------------------------

* A. Savitzky & M. J. E. Golay (1964). Smoothing and differentiation of
  data by simplified least squares procedures. *Anal. Chem.* **36**,
  1627–1639. -- Local polynomial least-squares fits; basis of the
  5-point cubic error estimate (``estimate_errors(model="poly")``).
* J. Rice (1984). Bandwidth choice for nonparametric regression. *Ann.
  Statist.* **12**, 1215–1230. -- Difference-based estimate of the noise
  variance.
* T. Gasser, L. Sroka & C. Jennen-Steinmetz (1986). Residual variance and
  residual pattern in nonlinear regression. *Biometrika* **73**, 625–633.
  -- Pseudo-residuals from the interpolation between neighbours, the
  3-point estimate (``estimate_errors(model="scatter")``).
* P. Hall, J. W. Kay & D. M. Titterington (1990). Asymptotically optimal
  difference-based estimation of variance in nonparametric regression.
  *Biometrika* **77**, 521–528. https://doi.org/10.1093/biomet/77.3.521
* P. J. Rousseeuw & C. Croux (1993). Alternatives to the median absolute
  deviation. *J. Am. Stat. Assoc.* **88**, 1273–1283. -- Median absolute
  deviation, factor 1.4826 for Gaussian noise.
* K. Pearson (1900). On the criterion that a given system of deviations
  from the probable in the case of a correlated system of variables is
  such that it can be reasonably supposed to have arisen from random
  sampling. *Phil. Mag.* (5) **50**, 157–175. -- Pearson :math:`\chi^2`.
* S. S. Wilks (1938). The large-sample distribution of the likelihood ratio
  for testing composite hypotheses. *Ann. Math. Stat.* **9**, 60–62.
  -- Likelihood-ratio (G) test.
* S. Kullback & R. A. Leibler (1951). On information and sufficiency.
  *Ann. Math. Stat.* **22**, 79–86. -- KL divergence.
* B. Efron (1979). Bootstrap methods: another look at the jackknife. *Ann.
  Statist.* **7**, 1–26. -- Bootstrap (:doc:`theory_uncertainty`).

Numerical optimization
----------------------

* R. P. Brent (1973). *Algorithms for Minimization without Derivatives*.
  Prentice-Hall, Englewood Cliffs. -- Root bracketing and Brent's method
  in the discrepancy-principle searches.
* R. H. Byrd, P. Lu, J. Nocedal & C. Zhu (1995). A limited memory algorithm
  for bound constrained optimization. *SIAM J. Sci. Comput.* **16**,
  1190–1208. -- L-BFGS-B (``hansen_maxent``).
* J. Nocedal & S. J. Wright (2006). *Numerical Optimization*, 2nd ed.
  Springer, New York. -- Newton polish, Armijo backtracking.
