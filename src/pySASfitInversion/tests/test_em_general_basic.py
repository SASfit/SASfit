import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from sasfit_inversion.solvers import em_general


def gaussian(x, mu=0.0, sig=1.0, scale=1.0):
    return scale / (np.sqrt(2 * np.pi) * sig) * np.exp(-0.5 * ((x - mu) / sig) ** 2)


def main():
    rng = np.random.default_rng(0)

    x = np.linspace(-1.5, 1.5, 120)
    r = np.linspace(0, 1, 60)
    sigma = 0.05

    # signed kernel, same style as the notebook's test_neg_pr_general_kernel.py
    K = np.array([(1.5 - r_) * gaussian(x, mu=r_, sig=2 * sigma) for r_ in r]).T

    # a signed target p(r): positive lobe minus a smaller negative lobe
    p_true = gaussian(r, mu=0.3, sig=0.05, scale=1.0) - 0.4 * gaussian(r, mu=0.6, sig=0.08, scale=1.0)

    b_clean = K @ p_true
    db = 0.02 * np.abs(b_clean).max() * np.ones_like(b_clean)
    b = b_clean + rng.normal(scale=db)

    result = em_general.solve(K, b, db, max_iterations=500)

    corr = np.corrcoef(result.x, p_true)[0, 1]
    print(f"correlation with true signed p(r) = {corr:.3f}")
    print(f"recovered p(r) has both signs: "
          f"min={result.x.min():.4f}, max={result.x.max():.4f}")
    print(f"final chi2_r = {result.chi2_r_history[-1]:.3f}")

    assert result.x.min() < 0, "should recover a genuinely negative region, not just clip to zero"
    assert corr > 0.5, "should correlate at least loosely with the true signed distribution"
    print("OK (qualitative sanity check only -- see em_general.py docstring caveats)")


if __name__ == "__main__":
    main()
