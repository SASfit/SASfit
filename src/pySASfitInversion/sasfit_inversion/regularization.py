"""
Regularization operators L for the penalty term ||L x||^2 in

    chi^2 = ||b - A x||_w^2 + lambda^2 ||L x||^2

These are standard finite-difference operators (Donatelli & Reichel, 2014,
as cited in the SASfit JAC 2022 paper). The C code (sasfit_extrapol.c)
implements 9 boundary-condition variants; this starts with the three most
common ones (identity, 1st derivative, 2nd derivative with free/natural
boundaries). The remaining Dirichlet/Neumann boundary combinations from the
C code can be added once we've confirmed exactly which ones the existing
SASfit fits actually rely on -- no point guessing boundary conventions that
won't be exercised.
"""
from __future__ import annotations

import numpy as np
import scipy.sparse as sp


def identity_operator(n: int) -> sp.spmatrix:
    """L = I. Penalizes the magnitude of the solution itself."""
    return sp.identity(n, format="csr")


def first_derivative_operator(n: int) -> sp.spmatrix:
    """
    Discrete first-derivative operator, (n-1) x n, free boundary
    (no assumption about the derivative at the endpoints).

        (Lx)_i = x_{i+1} - x_i
    """
    L = sp.diags([-np.ones(n - 1), np.ones(n - 1)], offsets=[0, 1], shape=(n - 1, n))
    return L.tocsr()


def second_derivative_operator(n: int) -> sp.spmatrix:
    """
    Discrete second-derivative operator, (n-2) x n, free boundary.

        (Lx)_i = x_{i+1} - 2 x_i + x_{i-1}
    """
    L = sp.diags(
        [np.ones(n - 2), -2 * np.ones(n - 2), np.ones(n - 2)],
        offsets=[0, 1, 2],
        shape=(n - 2, n),
    )
    return L.tocsr()


OPERATORS = {
    "identity": identity_operator,
    "first_derivative": first_derivative_operator,
    "second_derivative": second_derivative_operator,
}
