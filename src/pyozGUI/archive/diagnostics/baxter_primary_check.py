import numpy as np, warnings
warnings.filterwarnings("ignore")
from oneYukawaWrapper import one_yukawa_compute

def baxter_1968_ab(eta, mu=0.0):
    """Baxter (1968) J.Chem.Phys.49,2770, eq.17-18, pure hard-sphere (mu=0,
    i.e. tau->infinity, no adhesion)."""
    alpha = (1 + 2*eta - mu)**2 / (1-eta)**4
    beta = (-3*eta*(2+eta)**2 + 2*mu*(1+7*eta+eta**2) - mu**2*(2+eta)) / (2*(1-eta)**4)
    return alpha, beta

print(f"{'eta':>6}{'alpha(Baxter)':>15}{'a_Clib^2':>12}{'beta(Baxter)':>15}{'candidates for beta from a_Clib,b_Clib':>45}")
for eta in [0.1, 0.2, 0.3]:
    phi = eta  # same reduced packing fraction convention
    _, ok, (a, b, c, d) = one_yukawa_compute(6.0, 1e-6, phi, np.array([1.0]))
    alpha_ref, beta_ref = baxter_1968_ab(eta)
    print(f"{eta:6.2f}{alpha_ref:15.6f}{a**2:12.6f}{beta_ref:15.6f}   a={a:.5f} b={b:.5f}  "
          f"a*b={a*b:.5f}  12*eta*a*b={12*eta*a*b:.5f}  -12*eta*a*b={-12*eta*a*b:.5f}  "
          f"a^2/2={a**2/2:.5f}  a*b*(-1)={-a*b:.5f}")
