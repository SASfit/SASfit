/* rad.c
   Interactive program to find integrated black body radiation between
   two given wavelengths.

   Steve Moshier
   August, 1999   */

#include <stdio.h>
#include "mconf.h"

/* Get a number from keyboard.
   Display previous value and keep it if user just hits <CR>.  */
void
  getnum (line, val)
     char *line;
     double *val;
{
  char s[40];

  printf ("%s = %.7E ? ", line, *val);
  gets (s);
  if (s[0] != '\0')
    sscanf (s, "%lf", val);
  else
    printf ("\n");
  printf ("%.7E\n", *val);
}

extern double planck_c1, planck_c2;		/* See planck.c  */
static double T = 5555.;
static double l1 = 4.e-7;
static double l2 = 7.e-7;

int
  main ()
{
  double rtotal, u, rleft, rleftc, rright, rrightc;

  printf ("Planck black body radiation formula\n");
  printf ("Program by Steve Moshier\n");

restart:
  getnum ("Enter temperature (degrees Kelvin)", &T);
  if (T <= 0.0)
    {
      printf (" ? temperature must be > 0.\n");
      if (T < 0.0)
	exit (0);
      goto restart;
    }

  u = T / planck_c2;
  u = u * u;
  rtotal = 6.493939402266829149096 * planck_c1 * u * u;
  printf ("Total radiated power = %.7e watts per square meter\n", rtotal);

  u = planckw (T);
  printf ("Wavelength of maximum radiation = %.7e meter\n", u);
  printf ("Power between two wavelengths:\n");
  getnum ("  Enter lower wavelength (meters)", &l1);
  getnum ("  Enter upper wavelength (meters)", &l2);
  if (l1 < 0.0 || l2 < 0.0)
    {
      printf (" ? wavelength < 0.\n");
      goto restart;
    }

  /* Ensure l1 <= l2.  */
  if (l1 > l2)
    {
      u = l1;
      l2 = l1;
      l1 = u;
    }

  rleft = plancki (l1, T);
  rleftc = planckc (l1, T);
  printf ("  Portion of radiation at wavelengths less than lower = %.7e\n",
	  rleft / rtotal);

  rright = plancki (l2, T);
  rrightc = planckc (l2, T);
  printf ("  Portion of radiation at wavelengths greater than upper = %.7e\n",
	  rrightc / rtotal);
  printf ("  Power between the two wavelengths = %.7e watts / square meter\n",
	  rright - rleft);
  printf ("  Portion of total radiated power = %.7e\n",
	  (rright - rleft) / rtotal);
  goto restart;
  exit (0);
}
