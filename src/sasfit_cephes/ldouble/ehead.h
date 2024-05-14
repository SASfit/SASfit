
/* Include file for extended precision arithmetic programs.
 */

/* Number of 16 bit words in external x type format */
#define NE 6
/* #define NE 10 */

/* Number of 16 bit words in internal format */
#define NI (NE+3)

/* Array offset to exponent */
#define E 1

/* Array offset to high guard word */
#define M 2

/* Number of bits of precision */
#define NBITS ((NI-4)*16)

/* Maximum number of decimal digits in ASCII conversion
 * = NBITS*log10(2)
 */
#define NDEC (NBITS*8/27)

/* The exponent of 1.0 */
#define EXONE (0x3fff)


void eadd(), esub(), emul(), ediv();
int ecmp(), enormlz(), eshift();
void eshup1(), eshup8(), eshup6(), eshdn1(), eshdn8(), eshdn6();
void eabs(), eneg(), emov(), eclear(), einfin(), efloor();
void eldexp(), efrexp(), eifrac(), ltoe();
void esqrt(), elog(), eexp(), etanh(), epow();
void asctoe(), asctoe24(), asctoe53(), asctoe64();
void etoasc(), e24toasc(), e53toasc(), e64toasc();
void etoe64(), etoe53(), etoe24(), e64toe(), e53toe(), e24toe();
int mtherr();

extern unsigned short ezero[], ehalf[], eone[], etwo[];
extern unsigned short elog2[], esqrt2[];


/* by Stephen L. Moshier. */
