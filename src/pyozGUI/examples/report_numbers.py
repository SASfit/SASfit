# -*- coding: utf-8 -*-
"""Regenerate every number quoted in the report, so none is transcribed by hand."""
import json, numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from polydisperse_yukawa_sq import PolydisperseOneYukawaSq
from oneYukawaWrapper import one_yukawa_compute
from PiX_two_routes import route_B
from Jhat_derivation_check import Jhat_derived, Jhat_printed

out = {}

# --- T1: N=1 contact value vs liboneyukawa exact (a+b)-cZe^-Z ---
rows = []
for Z, K, phi in [(6.,1.,.1),(6.,.5,.1),(2.,.3,.1),(2.5,.5,.1),(3.,1.,.3),
                  (8.,2.,.15),(4.,.8,.25),(10.,3.,.35),(.5,.1,.05),
                  (6.,-1.,.1),(3.,-1.,.2),(6.,-2.,.1)]:
    _, ok, (a,b,c,d) = one_yukawa_compute(Z,K,phi,np.array([1.0]))
    if not ok: continue
    ex = (a+b)-c*Z*np.exp(-Z)
    s = PolydisperseOneYukawaMSA(np.array([1.0]),np.array([6*phi/np.pi]),z=Z,K=K)
    g = s.contact_value(s.solve_gamma())[0,0]
    rows.append([Z,K,phi,float(ex),float(g),float(abs(g-ex)/abs(ex))])
out["T1_contact"] = rows

# --- T2: N=1 S(Q) vs liboneyukawa ---
qs = np.array([0.1,0.3,1.0,2.0,3.14159,5.0,7.5,15.,25.,60.,150.,400.])
rows = []
for Z, K, phi in [(6.,1.,.1),(2.,.3,.1),(3.,1.,.3),(10.,3.,.35),(6.,-1.,.1),(3.,-1.,.2)]:
    s = PolydisperseOneYukawaMSA(np.array([1.0]),np.array([6*phi/np.pi]),z=Z,K=K)
    sq = PolydisperseOneYukawaSq(s, s.solve_gamma())
    Sref,ok,_ = one_yukawa_compute(Z,K,phi,qs)
    rows.append([Z,K,phi,float(max(abs(sq.S_number(q)-r)/abs(r) for q,r in zip(qs,Sref)))])
out["T2_sq"] = rows

# --- T3: Pi/X two routes, printed vs derived Jhat, at N>1 ---
rng = np.random.default_rng(1); rows = []
for sig,rho,Z in [(np.array([1.0]),np.array([0.19]),6.0),
                  (np.array([1.0,2.0]),np.array([0.1,0.02]),6.0),
                  (np.array([1.0,1.5,2.5]),np.array([0.08,0.03,0.006]),4.0)]:
    sv = PolydisperseOneYukawaMSA(sig,rho,z=Z,K=1.0)
    Jd,Jp = Jhat_derived(sv), Jhat_printed(sv)
    ed=ep=0.0
    for _ in range(4):
        Bh = rng.normal(size=len(sig))
        _,XB,_,_ = route_B(sv,Bh)
        ed=max(ed,float(np.max(np.abs(sv.gamma_hat+Jd@Bh-XB))))
        ep=max(ep,float(np.max(np.abs(sv.gamma_hat+Jp@Bh-XB))))
    rows.append([len(sig),ed,ep])
out["T3_jhat"] = rows

# --- T4: K->0 limits ---
rows=[]
for phi in [0.05,0.1,0.2,0.3,0.4]:
    s=PolydisperseOneYukawaMSA(np.array([1.0]),np.array([6*phi/np.pi]),z=6.0,K=1e-10)
    g=s.contact_value(s.solve_gamma())[0,0]; ex=(1+phi/2)/(1-phi)**2
    rows.append([phi,float(ex),float(g),float(abs(g-ex)/ex)])
out["T4_hs"]=rows

json.dump(out, open("report_numbers.json","w"), indent=1)
print(json.dumps({k:(len(v) if isinstance(v,list) else v) for k,v in out.items()}))
print("worst contact rel:", max(r[5] for r in out["T1_contact"]))
print("worst S(Q) rel   :", max(r[3] for r in out["T2_sq"]))
print("worst HS rel     :", max(r[3] for r in out["T4_hs"]))
