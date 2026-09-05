# -*- coding: utf-8 -*-
"""
Independent NUMERICAL multicomponent OZ solver with the Yukawa MSA closure.

Purpose: an externally independent check on the analytic N>1 solution
(polydisperse_yukawa_msa.py + polydisperse_yukawa_sq.py). Every other N>1
check available to us is internal to [BH02]'s equation set, or ties S_ij(Q)
back to our own contact values -- and four misprints have now been found in
that paper (spec 7h/7i/7k). This solver shares NO equation with the analytic
route: it just iterates the OZ relation on a radial grid under the same
physical closure.

Matrix OZ, Ashcroft-Landreth symmetrised (D = diag(sqrt(rho))):
    Chat = D Ctilde D,  Hhat = (I - Chat)^-1 Chat,  S = I + Hhat = (I - Chat)^-1
MSA closure (identical physics to [BH02] eq.5/6, nothing else borrowed):
    h_ij(r) = -1                              r < sigma_ij
    c_ij(r) = K_ij e^{-z (r - sigma_ij)}/r    r > sigma_ij
Picard iteration on gamma = h - c.

3D radial transforms via DST-I:
    ftilde(k) = (2 pi dr/k) * DST1[r f(r)]
    f(r)      = (dk/(4 pi^2 r)) * DST1[k ftilde(k)]
"""
import numpy as np
from scipy.fft import dst, idst


class MixtureOZ:
    def __init__(self, sigma, rho, z, K, delta=None, npts=8192, dr=0.0025):
        self.sig = np.asarray(sigma, float)
        self.rho = np.asarray(rho, float)
        self.z, self.K = float(z), float(K)
        self.N = len(self.sig)
        self.delta = np.ones(self.N) if delta is None else np.asarray(delta, float)
        self.M, self.dr = npts, dr
        self.r = (np.arange(npts) + 1)*dr
        self.dk = np.pi/((npts + 1)*dr)
        self.k = (np.arange(npts) + 1)*self.dk
        self.sij = 0.5*(self.sig[:, None] + self.sig[None, :])
        # K_ij = K delta_i delta_j  ([BA] eq.4 factored form)
        self.Kij = self.K*np.outer(self.delta, self.delta)
        self.D = np.sqrt(self.rho)

    def _fwd(self, f):
        return (2*np.pi*self.dr/self.k)*dst(self.r*f, type=1)

    def _inv(self, ft):
        return (self.dk/(4*np.pi**2*self.r))*dst(self.k*ft, type=1)

    def solve(self, alpha=0.4, tol=1e-12, maxiter=6000):
        N, M, r = self.N, self.M, self.r
        gam = np.zeros((N, N, M))
        c = np.zeros((N, N, M))
        for it in range(maxiter):
            # closure -> c(r)
            for i in range(N):
                for j in range(N):
                    s = self.sij[i, j]
                    inside = r < s
                    c[i, j] = np.where(
                        inside, -1.0 - gam[i, j],
                        self.Kij[i, j]*np.exp(-self.z*(r - s))/np.maximum(r, 1e-300))
            # OZ in k-space
            ct = np.empty_like(c)
            for i in range(N):
                for j in range(i, N):
                    ct[i, j] = self._fwd(c[i, j])
                    if i != j:
                        ct[j, i] = ct[i, j]
            Chat = self.D[:, None, None]*ct*self.D[None, :, None]
            # batched over the k grid: (M,N,N)
            Cm = np.moveaxis(Chat, 2, 0)
            Hm = np.linalg.solve(np.eye(N) - Cm, Cm)
            Gm = (Hm - Cm)/np.outer(self.D, self.D)[None, :, :]
            gam_new = np.moveaxis(Gm, 0, 2).copy()
            for i in range(N):
                for j in range(N):
                    gam_new[i, j] = self._inv(gam_new[i, j])
            err = np.max(np.abs(gam_new - gam))
            gam = (1 - alpha)*gam + alpha*gam_new
            if err < tol:
                break
        self.gam, self.c, self.iters, self.err = gam, c.copy(), it + 1, err
        return self

    def S_AL_grid(self):
        """S_ij^AL on the internal k grid."""
        N, M = self.N, self.M
        ct = np.empty((N, N, M))
        for i in range(N):
            for j in range(i, N):
                ct[i, j] = self._fwd(self.c[i, j])
                if i != j:
                    ct[j, i] = ct[i, j]
        Chat = self.D[:, None, None]*ct*self.D[None, :, None]
        Cm = np.moveaxis(Chat, 2, 0)
        S = np.moveaxis(np.linalg.inv(np.eye(N) - Cm), 0, 2).copy()
        return self.k, S

    def contact(self):
        """g_ij(sigma_ij+) by linear extrapolation of h from just outside."""
        N = self.N
        g = np.zeros((N, N))
        for i in range(N):
            for j in range(N):
                s = self.sij[i, j]
                h = self.gam[i, j] + self.c[i, j]
                m = (self.r > s) & (self.r < s + 0.05)
                idx = np.where(m)[0][:20]
                p = np.polyfit(self.r[idx], h[idx], 2)
                g[i, j] = 1.0 + np.polyval(p, s)
        return g
