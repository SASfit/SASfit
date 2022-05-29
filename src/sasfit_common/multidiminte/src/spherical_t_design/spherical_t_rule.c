# include <stdlib.h>
# include <stdio.h>
# include <math.h>

# include "spherical_t_rule.h"

# define NMAX 137

/******************************************************************************/

int sasfit_available_sph_t_table ( int rule )

/******************************************************************************/
/*
  Purpose:

    AVAILABLE_TABLE returns the availability of a spherical-t design rule.

  Modified:

    22 May 2022

  Author:

    Joachim Kohlbrecher

  Reference:

    https://www-user.tu-chemnitz.de/~potts/workgroup/graef/quadrature/index.php.en

  Parameters:

    Input, int RULE, the index of the rule, between 1 and 137.

    Output, int AVAILABLE_TABLE, the availability of the rule.
    * -1, there is no such rule;
    *  0, there is such a rule, but it is not available in this library.
    *  1, the rule is available in this library.
*/
{
  int rule_max = 137;
  int table[137] = {
    1,    1,    1,    0,    1,    0,    1,    1,    1,    1,  //#10
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,  //#20
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,  //#30
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,  //#40
    1,    1,    0,    1,    0,    1,    0,    1,    0,    1,  //#50
    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,  //#60
    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,  //#70
    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,  //#80
    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,  //#90
    0,    0,    0,    1,    0,    0,    0,    1,    0,    1,  //#100
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  //#110
    0,    0,    0,    1,    0,    0,    0,    0,    0,    0,  //#120
    0,    0,    0,    1,    0,    0,    0,    0,    0,    0,  //#130
    0,    0,    1,    0,    1,    1,    0 };
  int value;

  if ( rule < 1 )
  {
    value = - 1;
  }
  else if ( rule_max < rule )
  {
    value = - 1;
  }
  else
  {
    value = table[rule-1];
  }

  return value;
}
/******************************************************************************/
// #include "Design_xxx_xxxx.h" // this needs too much memory
#include "Nxxx_Mxxxx.h"
void Design_130000_500_random_xyz ( double *x, double *y, double *z ) {
  #include "Design_130000_500_random_xyz.h"
  return;
}
void Design_21000_200_random_xyz ( double *x, double *y, double *z ) {
  #include "Design_21000_200_random_xyz.h"
  return;
}
void Design_520000_1000_random_xyz ( double *x, double *y, double *z ) {
  #include "Design_520000_1000_random_xyz.h"
  return;
}
void sasfit_sph_t_by_order ( int order, double *x, double *y, double *z )

/******************************************************************************/
/*
  Purpose:

    SPHT_BY_ORDER returns a spherical-t design angular grid given its order.

  Discussion:

    Only a certain set of such rules are available through this function.

  Modified:

    26 MAy 2022

  Author:

    Joachim Kohlbrecher

  Reference:

    https://www-user.tu-chemnitz.de/~potts/workgroup/graef/quadrature/index.php.en


  Parameters:

    Input, int ORDER, the order of the rule.

    Output, double X[ORDER], Y[ORDER], Z[ORDER], W[ORDER], the coordinates
    and weights of the points.
*/
{
  if ( order == 2 )
  {
    N001_M2_Inv ( x, y, z );
  }
  else if ( order == 4 )
  {
    N002_M4_Tetra ( x, y, z );
  }
  else if ( order == 6 )
  {
    N003_M6_Octa ( x, y, z );
  }
  else if ( order == 12 )
  {
    N005_M12_Ico ( x, y, z );
  }
  else if ( order == 24 )
  {
    N007_M24_Octa ( x, y, z );
  }
  else if ( order == 36 )
  {
    N008_M36_Tetra ( x, y, z );
  }
  else if ( order == 48 )
  {
    N009_M48_Octa ( x, y, z );
  }
  else if ( order == 60 )
  {
    N010_M60_Tetra ( x, y, z );
  }
  else if ( order == 70 )
  {
    N011_M70_C5 ( x, y, z );
  }
  else if ( order == 84 )
  {
    N012_M84_Tetra ( x, y, z );
  }
  else if ( order == 94 )
  {
    N013_M94_Inv ( x, y, z );
  }
  else if ( order == 108 )
  {
    N014_M108_Tetra ( x, y, z );
  }
  else if ( order == 120 )
  {
    N015_M120_Octa ( x, y, z );
  }
  else if ( order == 144 )
  {
    N016_M144_Tetra ( x, y, z );
  }
  else if ( order == 156 )
  {
    N017_M156_Tetra ( x, y, z );
  }
  else if ( order == 180 )
  {
    N018_M180_Tetra ( x, y, z );
  }
  else if ( order == 192 )
  {
    N019_M192_C3 ( x, y, z );
  }
  else if ( order == 216 )
  {
    N020_M216_Tetra ( x, y, z );
  }
  else if ( order == 234 )
  {
     N021_M234_C3 ( x, y, z );
  }
  else if ( order == 264 )
  {
    N022_M264_Octa ( x, y, z );
  }
  else if ( order == 278 )
  {
    N023_M278_C3 ( x, y, z );
  }
  else if ( order == 312 )
  {
    N024_M312_Octa ( x, y, z );
  }
  else if ( order == 328 )
  {
    N025_M328_Inv ( x, y, z );
  }
  else if ( order == 360 )
  {
    N026_M360_Octa ( x, y, z );
  }
  else if ( order == 380 )
  {
    N027_M380_C3 ( x, y, z );
  }
  else if ( order == 420 )
  {
    N028_M420_Tetra ( x, y, z );
  }
  else if ( order == 432 )
  {
    N029_M432_Ico ( x, y, z );
  }
  else if ( order == 480 )
  {
    N030_M480_Octa ( x, y, z );
  }
  else if ( order == 498 )
  {
    N031_M498_C3 ( x, y, z );
  }
  else if ( order == 540 )
  {
    N032_M540_Tetra ( x, y, z );
  }
  else if ( order == 612 )
  {
    N034_M612_Ico ( x, y, z );
  }
  else if ( order == 632 )
  {
    N035_M632_C3 ( x, y, z );
  }
  else if ( order == 684 )
  {
    N036_M684_Tetra ( x, y, z );
  }
  else if ( order == 706 )
  {
    N037_M706_Inv ( x, y, z );
  }
  else if ( order == 756 )
  {
    N038_M756_Tetra ( x, y, z );
  }
  else if ( order == 782 )
  {
    N039_M782_C3 ( x, y, z );
  }
  else if ( order == 840 )
  {
    N040_M840_Octa ( x, y, z );
  }
  else if ( order == 864 )
  {
    N041_M864_C3 ( x, y, z );
  }
  else if ( order == 924 )
  {
    N042_M924_Tetra ( x, y, z );
  }
  else if ( order == 1008 )
  {
    N044_M1008_Octa ( x, y, z );
  }
  else if ( order == 1104 )
  {
    N046_M1104_Octa ( x, y, z );
  }
  else if ( order == 1200 )
  {
    N048_M1200_Octa ( x, y, z );
  }
  else if ( order == 1296 )
  {
    N050_M1296_Octa ( x, y, z );
  }
  else if ( order == 1404 )
  {
    N052_M1404_Tetra ( x, y, z );
  }
  else if ( order == 1512 )
  {
    N054_M1512_Ico ( x, y, z );
  }
  else if ( order == 1620 )
  {
    N056_M1620_Tetra ( x, y, z );
  }
  else if ( order == 1740 )
  {
    N058_M1740_Tetra ( x, y, z );
  }
  else if ( order == 1860 )
  {
    N060_M1860_Tetra ( x, y, z );
  }
  else if ( order == 1980 )
  {
    N062_M1980_Tetra ( x, y, z );
  }
  else if ( order == 2112 )
  {
    N064_M2112_Ico ( x, y, z );
  }
  else if ( order == 2244 )
  {
    N066_M2244_Tetra ( x, y, z );
  }
  else if ( order == 2376 )
  {
    N068_M2376_Octa ( x, y, z );
  }
  else if ( order == 2520 )
  {
    N070_M2520_Octa ( x, y, z );
  }
  else if ( order == 2664 )
  {
    N072_M2664_Octa ( x, y, z );
  }
  else if ( order == 2808 )
  {
    N074_M2808_Octa ( x, y, z );
  }
  else if ( order == 2964 )
  {
    N076_M2964_Tetra ( x, y, z );
  }
  else if ( order == 3120 )
  {
    N078_M3120_Octa ( x, y, z );
  }
  else if ( order == 3276 )
  {
    N080_M3276_Tetra ( x, y, z );
  }
  else if ( order == 3444 )
  {
    N082_M3444_Tetra ( x, y, z );
  }
  else if ( order == 3612 )
  {
    N084_M3612_Ico ( x, y, z );
  }
  else if ( order == 3780 )
  {
    N086_M3780_Tetra ( x, y, z );
  }
  else if ( order == 3960 )
  {
    N088_M3960_Octa ( x, y, z );
  }
  else if ( order == 4140 )
  {
    N090_M4140_Tetra ( x, y, z );
  }
  else if ( order == 4512 )
  {
    N094_M4512_Ico ( x, y, z );
  }
  else if ( order == 4896 )
  {
    N098_M4896_Octa ( x, y, z );
  }
  else if ( order == 5100 )
  {
    N100_M5100_Tetra ( x, y, z );
  }
  else if ( order == 6612 )
  {
    N114_M6612_Ico ( x, y, z );
  }
  else if ( order == 7812 )
  {
    N124_M7812_Ico ( x, y, z );
  }
/*  else if ( order == 5200 )
  {
    Design_5200_100_random_xyz ( x, y, z );
  }
  else if ( order == 5200 )
  {
    Design_5200_100_spiral_xyz ( x, y, z );
  } */
  else if ( order == 21000 )
  {
    Design_21000_200_random_xyz ( x, y, z );
  }
/*  else if ( order == 21000 )
  {
    Design_21000_200_spiral_xyz ( x, y, z );
  } */
  else if ( order == 130000 )
  {
    Design_130000_500_random_xyz ( x, y, z );
  }
  else if ( order == 520000 )
  {
    Design_520000_1000_random_xyz ( x, y, z );
  }
/*  else if ( order == 1002000 )
  {
    Design_1002000_1000_random_xyz ( x, y, z );
  }
  */
  else
  {
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "SPHT_BY_ORDER - Fatal error!\n" );
    fprintf ( stderr, "  Unexpected value of ORDER.\n" );
    exit ( 1 );
  }

  return;
}
/******************************************************************************/


int sasfit_sph_t_order_table ( int rule )

/******************************************************************************/
/*
  Purpose:

    ORDER_TABLE returns the order of a spherical-t design rule.

  Modified:

    25 May 2022

  Author:

    Joachim Kohlbrecher

  Reference:

    https://www-user.tu-chemnitz.de/~potts/workgroup/graef/quadrature/index.php.en

    M. Gräf.
    Efficient Algorithms for the computation of optimal quadrature points on Riemannian manifolds. (PDF)
    PhD Thesis, Chemnitz University of Technology, Universitätsverlag Chemnitz, ISBN 978-3-941003-89-7, 2013.

    M. Gräf and D. Potts.
    On the computation of spherical designs by a new optimization approach based on fast spherical Fourier transforms. (PDF)
    Numer. Math. 119, 699 - 724, 2011.

  Parameters:

    Input, int RULE, the index of the rule, between 1 and 137.

    Output, int ORDER_TABLE, the order of the rule.
*/
{
  int rule_max = 137;
  int table[137]= {
     2,    4,    6,    0,   12,    0,   24,   36,   48,   60,
    70,   84,   94,  108,  120,  144,  156,  180,  192,  216,
   234,  264,  278,  312,  328,  360,  380,  420,  432,  480,
   498,  540,  564,  612,  632,  684,  706,  756,  782,  840,
   864,  924,    0, 1008,    0, 1104,    0, 1200,    0, 1296,
     0, 1404,    0, 1512,    0, 1620,    0, 1740,    0, 1860,
     0, 1980,    0, 2112,    0, 2244,    0, 2376,    0, 2520,
     0, 2664,    0, 2808,    0, 2964,    0, 3120,    0, 3276,
     0, 3444,    0, 3612,    0, 3780,    0, 3960,    0, 4140,
     0,    0,    0, 4512,    0,    0,    0, 4896,    0, 5100,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0, 6612,    0,    0,    0,    0,    0,    0,
     0,    0,    0, 7812,    0,    0,    0,    0,    0,    0,
  5200, 5200, 21000, 21000, 130000, 520000,  1002000  };
  int value;

  if ( rule < 1 )
  {
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "ORDER_TABLE - Fatal error!\n" );
    fprintf ( stderr, "  RULE < 1.\n" );
    exit ( 1 );
  }
  else if ( rule_max < rule )
  {
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "ORDER_TABLE - Fatal error!\n" );
    fprintf ( stderr, "  RULE_MAX < RULE.\n" );
    exit ( 1 );
  }

  value = table[rule-1];

  return value;
}
/******************************************************************************/

int precision_sph_t_table ( int rule )

/******************************************************************************/
/*
  Purpose:

    PRECISION_TABLE returns the precision of a spherical-t design rule.

  Modified:

    25 May 2022

  Author:

    Joachim Kohlbrecher

  Reference:


    https://www-user.tu-chemnitz.de/~potts/workgroup/graef/quadrature/index.php.en

    M. Gräf.
    Efficient Algorithms for the computation of optimal quadrature points on Riemannian manifolds. (PDF)
    PhD Thesis, Chemnitz University of Technology, Universitätsverlag Chemnitz, ISBN 978-3-941003-89-7, 2013.

    M. Gräf and D. Potts.
    On the computation of spherical designs by a new optimization approach based on fast spherical Fourier transforms. (PDF)
    Numer. Math.

  Parameters:

    Input, int RULE, the index of the rule, between 1 and 65.

    Output, int PRECISION_TABLE, the precision of the rule.
*/
{
  int rule_max = 137;
  int table[137]= {
     2,    4,    6,    0,   12,    0,   24,   36,   48,   60,
    70,   84,   94,  108,  120,  144,  156,  180,  192,  216,
   234,  264,  278,  312,  328,  360,  380,  420,  432,  480,
   498,  540,  564,  612,  632,  684,  706,  756,  782,  840,
   864,  924,    0, 1008,    0, 1104,    0, 1200,    0, 1296,
     0, 1404,    0, 1512,    0, 1620,    0, 1740,    0, 1860,
     0, 1980,    0, 2112,    0, 2244,    0, 2376,    0, 2520,
     0, 2664,    0, 2808,    0, 2964,    0, 3120,    0, 3276,
     0, 3444,    0, 3612,    0, 3780,    0, 3960,    0, 4140,
     0,    0,    0, 4512,    0,    0,    0, 4896,    0, 5100,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0, 6612,    0,    0,    0,    0,    0,    0,
     0,    0,    0, 7812,    0,    0,    0,    0,    0,    0,
  5200, 5200, 21000, 21000, 130000, 520000,  1002000  };
  int value;

  if ( rule < 1 )
  {
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "PRECISION_TABLE - Fatal error!\n" );
    fprintf ( stderr, "  RULE < 1.\n" );
    exit ( 1 );
  }
  else if ( rule_max < rule )
  {
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "PRECISION_TABLE - Fatal error!\n" );
    fprintf ( stderr, "  RULE_MAX < RULE.\n" );
    exit ( 1 );
  }

  value = table[rule-1];

  return value;
}
/******************************************************************************/

