import sys, numpy as np, warnings
sys.path.insert(0, '/mnt/user-data/uploads/pySASfit/pyozGUI')
sys.path.insert(0, '/home/claude/work')          # has liboneyukawa.so built next to oneYukawaWrapper.py -- must win the module search
warnings.filterwarnings("ignore")
import ozLib
from oneYukawaWrapper import one_yukawa_compute
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA

def tail_contact(Z, K, phi, qlo=300.0, qhi=995.0, n=6000):
    q = np.linspace(qlo, qhi, n)
    Sq, ok, _ = one_yukawa_compute(Z, K, phi, q)
    assert ok
    y = (Sq - 1.0) * q**2 / np.cos(q)
    mask = np.abs(np.cos(q)) > 0.3
    return np.median(y[mask]) / (24.0*phi)

def ozlib_contact(Z, K, phi, hsPts=4000, solver="scipy Anderson"):
    res = ozLib.solve(potential='Yukawa', phi=phi, potentialArgs=(1.0/Z, -K, True),
                       closure='MSA', solver=solver, maxIterations=3000,
                       numberOfRadialSamplingPoints=hsPts*15, hardSphereDiameterInPoints=hsPts)
    r, g = res.r, res.gr
    idx = np.arange(hsPts, hsPts+4)
    # linear extrapolation of g(r) back to r=1.0 exactly, from the first few
    # points strictly outside the hard core
    p = np.polyfit(r[idx], g[idx], 1)
    return np.polyval(p, 1.0)

def mine(Z, K, phi):
    sigma = np.array([1.0]); rho = np.array([6*phi/np.pi])
    solver = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=K)
    sol = solver.solve()
    return solver.contact_value(sol)[0,0]

cases = [(6.0, 1e-6, 0.2), (6.0, 0.5, 0.1), (6.0, 1.0, 0.1), (6.0, 2.0, 0.2), (3.0, 0.5, 0.15)]
print(f"{'Z':>5}{'K':>7}{'phi':>6}{'liboneyukawa (tail)':>20}{'ozLib (real-space MSA)':>24}{'ours':>10}")
for Z,K,phi in cases:
    gt = tail_contact(Z,K,phi)
    go = ozlib_contact(Z,K,phi)
    gm = mine(Z,K,phi)
    print(f"{Z:5.1f}{K:7.4f}{phi:6.2f}{gt:20.5f}{go:24.5f}{gm:10.5f}")
