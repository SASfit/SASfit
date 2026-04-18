#ifndef PROTOS_H
#define PROTOS_H
#ifdef __STDC__
double plancki (double, double);
double planckc (double, double);
double planckw (double);
double polylog (int, double);
double spence (double);
double zetac (double);
double exp (double);
double log (double);
double cos (double);
double sin (double);
double fabs (double);
double gamma (double);
double pow (double, double);
double powi (double, int);
double polevl (double, void *, int);
double p1evl (double, void *, int);
double floor (double);
double frexp (double, int *);
double ldexp (double, int);
int isnan (double);
int isfinite (double);
int signbit (double);
int mtherr (char *, int);
#else
double plancki ();
double planckc ();
double planckw ();
double polylog ();
double spence ();
double zetac ();
double exp ();
double log ();
double cos ();
double sin ();
double fabs ();
double gamma ();
double pow ();
double powi ();
double polevl ();
double p1evl ();
double floor ();
double frexp ();
double ldexp ();
int isnan ();
int isfinite ();
int signbit ();
int mtherr ();
#endif /* __STDC__ */
#endif /* PROTOS_H */
