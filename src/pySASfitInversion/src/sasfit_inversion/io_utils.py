"""
Flexible loader for 2- or 3-column ASCII scattering data files (q, I[, dI]).

Real-world SAS data files come in wildly inconsistent formats (comment
lines starting with #, %, or !, header text, tab/space/comma delimiters,
occasional blank lines). Rather than assume one specific format, this
tries every line and keeps the ones that parse as 2 or 3 floats,
discarding the rest -- simple, and hard to break on formats not
anticipated in advance.
"""
from __future__ import annotations

from dataclasses import dataclass

import numpy as np


@dataclass
class SASData:
    q: np.ndarray
    I: np.ndarray
    dI: np.ndarray | None
    source_path: str
    n_lines_skipped: int


def load_sas_data(path: str) -> SASData:
    """
    Load a q, I[, dI] ASCII file. Raises ValueError if fewer than 3 valid
    data rows are found (too little to do anything with).
    """
    rows: list[tuple[float, ...]] = []
    n_skipped = 0

    with open(path, "r", errors="replace") as f:
        for line in f:
            stripped = line.strip()
            if not stripped or stripped[0] in "#%!;":
                n_skipped += 1
                continue
            parts = stripped.replace(",", " ").split()
            if len(parts) not in (2, 3):
                n_skipped += 1
                continue
            try:
                values = tuple(float(p) for p in parts)
            except ValueError:
                n_skipped += 1
                continue
            rows.append(values)

    if len(rows) < 3:
        raise ValueError(
            f"Could not find at least 3 valid data rows (q, I[, dI]) in {path!r} "
            f"-- found {len(rows)}, skipped {n_skipped} lines."
        )

    n_cols = len(rows[0])
    if any(len(r) != n_cols for r in rows):
        # Mixed 2- and 3-column rows: keep only rows matching the majority column count.
        from collections import Counter
        counts = Counter(len(r) for r in rows)
        n_cols = counts.most_common(1)[0][0]
        before = len(rows)
        rows = [r for r in rows if len(r) == n_cols]
        n_skipped += before - len(rows)

    arr = np.array(rows, dtype=float)
    q, I = arr[:, 0], arr[:, 1]
    dI = arr[:, 2] if n_cols == 3 else None

    order = np.argsort(q)
    q, I = q[order], I[order]
    dI = dI[order] if dI is not None else None

    return SASData(q=q, I=I, dI=dI, source_path=path, n_lines_skipped=n_skipped)
