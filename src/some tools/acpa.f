CACPAPFQ.  A NUMERICAL EVALUATOR FOR THE GENERALIZED HYPERGEOMETRIC     ACPA0000
C1  SERIES.  W.F. PERGER, A. BHALLA, M. NARDIN.                         ACPA0000
CREF. IN COMP. PHYS. COMMUN. 77 (1993) 249                              ACPA0000
C     ****************************************************************  ACPA0001
C     *                                                              *  ACPA0002
C     *    SOLUTION TO THE GENERALIZED HYPERGEOMETRIC FUNCTION       *  ACPA0003
C     *                                                              *  ACPA0004
C     *                           by                                 *  ACPA0005
C     *                                                              *  ACPA0006
C     *                      W. F. PERGER,                           *  ACPA0007
C     *                                                              *  ACPA0008
C     *              MARK NARDIN  and ATUL BHALLA                    *  ACPA0009
C     *                                                              *  ACPA0010
C     *                                                              *  ACPA0011
C     *            Electrical Engineering Department                 *  ACPA0012
C     *            Michigan Technological University                 *  ACPA0013
C     *                  1400 Townsend Drive                         *  ACPA0014
C     *                Houghton, MI  49931-1295   USA                *  ACPA0015
C     *                     Copyright 1993                           *  ACPA0016
C     *                                                              *  ACPA0017
C     *               e-mail address: wfp@mtu.edu                    *  ACPA0018
C     *                                                              *  ACPA0019
C     *  Description : A numerical evaluator for the generalized     *  ACPA0020
C     *    hypergeometric function for complex arguments with large  *  ACPA0021
C     *    magnitudes using a direct summation of the Gauss series.  *  ACPA0022
C     *    The method used allows an accuracy of up to thirteen      *  ACPA0023
C     *    decimal places through the use of large integer arrays    *  ACPA0024
C     *    and a single final division.                              *  ACPA0025
C     *    (original subroutines for the confluent hypergeometric    *  ACPA0026
C     *    written by Mark Nardin, 1989; modifications made to cal-  *  ACPA0027
C     *    culate the generalized hypergeometric function were       *  ACPA0028
C     *    written by W.F. Perger and A. Bhalla, June, 1990)         *  ACPA0029
C     *                                                              *  ACPA0030
C     *  The evaluation of the pFq series is accomplished by a func- *  ACPA0031
C     *  ion call to PFQ, which is a double precision complex func-  *  ACPA0032
C     *  tion.  The required input is:                               *  ACPA0033
C     *  1. Double precision complex arrays A and B.  These are the  *  ACPA0034
C     *     arrays containing the parameters in the numerator and de-*  ACPA0035
C     *     nominator, respectively.                                 *  ACPA0036
C     *  2. Integers IP and IQ.  These integers indicate the number  *  ACPA0037
C     *     of numerator and denominator terms, respectively (these  *  ACPA0038
C     *     are p and q in the pFq function).                        *  ACPA0039
C     *  3. Double precision complex argument Z.                     *  ACPA0040
C     *  4. Integer LNPFQ.  This integer should be set to '1' if the *  ACPA0041
C     *     result from PFQ is to be returned as the natural logaritm*  ACPA0042
C     *     of the series, or '0' if not.  The user can generally set*  ACPA0043
C     *     LNPFQ = '0' and change it if required.                   *  ACPA0044
C     *  5. Integer IX.  This integer should be set to '0' if the    *  ACPA0045
C     *     user desires the program PFQ to estimate the number of   *  ACPA0046
C     *     array terms (in A and B) to be used, or an integer       *  ACPA0047
C     *     greater than zero specifying the number of integer pos-  *  ACPA0048
C     *     itions to be used.  This input parameter is escpecially  *  ACPA0049
C     *     useful as a means to check the results of a given run.   *  ACPA0050
C     *     Specificially, if the user obtains a result for a given  *  ACPA0051
C     *     set of parameters, then changes IX and re-runs the eval- *  ACPA0052
C     *     uator, and if the number of array positions was insuffi- *  ACPA0053
C     *     cient, then the two results will likely differ.  The rec-*  ACPA0054
C     *     commended would be to generally set IX = '0' and then set*  ACPA0055
C     *     it to 100 or so for a second run.  Note that the LENGTH  *  ACPA0056
C     *     parameter currently sets the upper limit on IX to 777,   *  ACPA0057
C     *     but that can easily be changed (it is a single PARAMETER *  ACPA0058
C     *     statement) and the program recompiled.                   *  ACPA0059
C     *  6. Integer NSIGFIG.  This integer specifies the requested   *  ACPA0060
C     *     number of significant figures in the final result.  If   *  ACPA0061
C     *     the user attempts to request more than the number of bits*  ACPA0062
C     *     in the mantissa allows, the program will abort with an   *  ACPA0063
C     *     appropriate error message.  The recommended value is 10. *  ACPA0064
C     *                                                              *  ACPA0065
C     *     Note: The variable NOUT is the file to which error mess- *  ACPA0066
C     *           ages are written (default is 6).  This can be      *  ACPA0067
C     *           changed in the FUNCTION PFQ to accomodate re-      *  ACPA0068
C     *           of output to another file                          *  ACPA0069
C     *                                                              *  ACPA0070
C     *  Subprograms called: HYPER.                                  *  ACPA0071
C     *                                                              *  ACPA0072
C     ****************************************************************  ACPA0073
C                                                                       ACPA0074
                                                                        ACPA0075
      FUNCTION PFQ (A,B,IP,IQ,Z,LNPFQ,IX,NSIGFIG,IERR,ERRSTR,STRLEN)    ACPA0076
                                                                        ACPA0077
      INTEGER LNPFQ,IX,IP,IQ,NSIGFIG,IERR,STRLEN                        ACPA0078
      COMPLEX*16 HYPER,A,B,Z,PFQ                                        ACPA0079
      COMPLEX*16 CGAMMA,A1(2),B1(1),GAM1,GAM2,GAM3,GAM4,HYPER1,         ACPA0080
     :     HYPER2,GAM5,GAM6,GAM7,Z1                                     ACPA0081
      DIMENSION A(IP),B(IQ)                                             ACPA0082
      CHARACTER*256 ERRSTR                                              ACPAADDD
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA0083
      COMMON/IO/NOUT                                                    ACPA0084
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA0085
      DOUBLE PRECISION DNUM,PRECIS,ARGI,ARGR,DIFF                       ACPA0086
                                                                        ACPA0087
      IERR = 0                                                          ACPAADDD
      NOUT=6                                                            ACPA0088
      IF ((LNPFQ .NE. 0) .AND. (LNPFQ .NE. 1)) THEN                     ACPA0089
         ERRSTR = ' ERROR IN INPUT ARGUMENTS: LNPFQ /= 0 OR 1'          ACPA0090
         IERR = 1                                                       acoa_add
         RETURN                                                         ACPA0091
      END IF                                                            ACPA0092
      IF ((IP .GT. IQ) .AND. (ABS(Z) .GT. ONE)) THEN                    ACPA0093
         WRITE (ERRSTR,300) IP,IQ,ABS(Z)                                ACPA0094
         IERR = 1                                                       acoa_add
         RETURN                                                         ACPA0095
      END IF                                                            ACPA0096
      IF (IP .EQ. 2 .AND. IQ .EQ. 1 .AND. ABS(Z) .GT. 0.9) THEN         ACPA0097
         IF (LNPFQ .EQ. 1) GO TO 30                                     ACPA0098
*                                                                       ACPA0099
*     Check to see if the Gamma function arguments are o.k.; if not,    ACPA0100
*     then the series will have to be used.                             ACPA0101
*                                                                       ACPA0102
*                                                                       ACPA0103
*     PRECIS - MACHINE PRECISION                                        ACPA0104
*                                                                       ACPA0105
         PRECIS = ONE                                                   ACPA0106
 10      PRECIS = PRECIS/TWO                                            ACPA0107
         DNUM = PRECIS+ONE                                              ACPA0108
         IF (DNUM .GT. ONE) GOTO 10                                     ACPA0109
         PRECIS = TWO*PRECIS                                            ACPA0110
         DO 20 I=1,6                                                    ACPA0111
            IF (I .EQ. 1) THEN                                          ACPA0112
               ARGI=DIMAG(B(1))                                         ACPA0113
               ARGR=DBLE(B(1))                                          ACPA0114
            ELSE IF (I .EQ. 2) THEN                                     ACPA0115
               ARGI=DIMAG(B(1)-A(1)-A(2))                               ACPA0116
               ARGR=DBLE(B(1)-A(1)-A(2))                                ACPA0117
            ELSE IF (I .EQ. 3) THEN                                     ACPA0118
               ARGI=DIMAG(B(1)-A(1))                                    ACPA0119
               ARGR=DBLE(B(1)-A(1))                                     ACPA0120
            ELSE IF (I .EQ. 4) THEN                                     ACPA0121
               ARGI=DIMAG(A(1)+A(2)-B(1))                               ACPA0122
               ARGR=DBLE(A(1)+A(2)-B(1))                                ACPA0123
            ELSE IF (I .EQ. 5) THEN                                     ACPA0124
               ARGI=DIMAG(A(1))                                         ACPA0125
               ARGR=DBLE(A(1))                                          ACPA0126
            ELSE IF (I .EQ. 6) THEN                                     ACPA0127
               ARGI=DIMAG(A(2))                                         ACPA0128
               ARGR=DBLE(A(2))                                          ACPA0129
            END IF                                                      ACPA0130
*                                                                       ACPA0131
*     CASES WHERE THE ARGUMENT IS REAL                                  ACPA0132
*                                                                       ACPA0133
            IF (ARGI .EQ. ZERO) THEN                                    ACPA0134
*                                                                       ACPA0135
*     CASES WHERE THE ARGUMENT IS REAL AND NEGATIVE                     ACPA0136
*                                                                       ACPA0137
               IF (ARGR .LE. ZERO) THEN                                 ACPA0138
*                                                                       ACPA0139
*     USE THE SERIES EXPANSION IF THE ARGUMENT IS TOO NEAR A POLE       ACPA0140
*                                                                       ACPA0141
                  DIFF = ABS (DBLE (NINT (ARGR))-ARGR)                  ACPA0142
                  IF (DIFF .LE. TWO*PRECIS) GO TO 30                    ACPA0143
               END IF                                                   ACPA0144
            END IF                                                      ACPA0145
 20      CONTINUE                                                       ACPA0146
         GAM1=CGAMMA(B(1),LNPFQ)                                        ACPA0147
         GAM2=CGAMMA(B(1)-A(1)-A(2),LNPFQ)                              ACPA0148
         GAM3=CGAMMA(B(1)-A(1),LNPFQ)                                   ACPA0149
         GAM4=CGAMMA(B(1)-A(2),LNPFQ)                                   ACPA0150
         GAM5=CGAMMA(A(1)+A(2)-B(1),LNPFQ)                              ACPA0151
         GAM6=CGAMMA(A(1),LNPFQ)                                        ACPA0152
         GAM7=CGAMMA(A(2),LNPFQ)                                        ACPA0153
         A1(1)=A(1)                                                     ACPA0154
         A1(2)=A(2)                                                     ACPA0155
         B1(1)=A(1)+A(2)-B(1)+ONE                                       ACPA0156
         Z1=ONE-Z                                                       ACPA0157
         HYPER1=HYPER(A1,B1,IP,IQ,Z1,LNPFQ,IX,NSIGFIG,IERR,ERRSTR)      ACPA0158
         IF (IERR .GE. 1) return
         A1(1)=B(1)-A(1)                                                ACPA0159
         A1(2)=B(1)-A(2)                                                ACPA0160
         B1(1)=B(1)-A(1)-A(2)+ONE                                       ACPA0161
         HYPER2=HYPER(A1,B1,IP,IQ,Z1,LNPFQ,IX,NSIGFIG,IERR,ERRSTR)      ACPA0162
         IF (IERR .GE. 1) return
         PFQ=GAM1*GAM2*HYPER1/(GAM3*GAM4)+(ONE-Z)**(B(1)-A(1)-A(2))*    ACPA0163
     :        GAM1*GAM5*HYPER2/(GAM6*GAM7)                              ACPA0164
         RETURN                                                         ACPA0165
      END IF                                                            ACPA0166
 30   CONTINUE                                                          ACPA0167
      PFQ=HYPER(A,B,IP,IQ,Z,LNPFQ,IX,NSIGFIG,IERR,ERRSTR)               ACPA0168
      IF (IERR .GE. 1) return
      RETURN                                                            ACPA0169
 300  FORMAT (/,1X,'IP=',1I2,3X,'IQ=',1I2,3X,'AND ABS(Z)=',             ACPA0170
     :     1E12.5,2X,/,' WHICH IS GREATER THAN ONE--SERIES DOES',       ACPA0171
     :     ' NOT CONVERGE')                                             ACPA0172
      END                                                               ACPA0173
C     ****************************************************************  ACPA0174
C     *                                                              *  ACPA0175
C     *                   FUNCTION BITS                              *  ACPA0176
C     *                                                              *  ACPA0177
C     *                                                              *  ACPA0178
C     *  Description : Determines the number of significant figures  *  ACPA0179
C     *    of machine precision to arrive at the size of the array   *  ACPA0180
C     *    the numbers must be stored in to get the accuracy of the  *  ACPA0181
C     *    solution.                                                 *  ACPA0182
C     *                                                              *  ACPA0183
C     *  Subprograms called: none                                    *  ACPA0184
C     *                                                              *  ACPA0185
C     ****************************************************************  ACPA0186
                                                                        ACPA0187
      DOUBLE PRECISION FUNCTION BITS(FAKE)                              ACPA0188
                                                                        ACPA0189
      DOUBLE PRECISION BIT,BIT2,FAKE                                    ACPA0190
      INTEGER COUNT                                                     ACPA0191
                                                                        ACPA0192
      BIT=1.0                                                           ACPA0193
      COUNT=0                                                           ACPA0194
 10   COUNT=COUNT+1                                                     ACPA0195
      BIT2=BIT*2.0                                                      ACPA0196
      BIT=BIT2+1.0                                                      ACPA0197
      IF ((BIT-BIT2) .NE. 0.0) GOTO 10                                  ACPA0198
      BITS=COUNT-3                                                      ACPA0199
      RETURN                                                            ACPA0200
      END                                                               ACPA0201
C     ****************************************************************  ACPA0202
C     *                                                              *  ACPA0203
C     *                   FUNCTION HYPER                             *  ACPA0204
C     *                                                              *  ACPA0205
C     *                                                              *  ACPA0206
C     *  Description : Function that sums the Gauss series.          *  ACPA0207
C     *                                                              *  ACPA0208
C     *  Subprograms called: ARMULT, ARYDIV, BITS, CMPADD, CMPMUL,   *  ACPA0209
C     *                      IPREMAX.                                *  ACPA0210
C     *                                                              *  ACPA0211
C     ****************************************************************  ACPA0212
                                                                        ACPA0213
      FUNCTION HYPER (A,B,IP,IQ,Z,LNPFQ,IX,NSIGFIG,IERR,ERRSTR)         ACPA0214
                                                                        ACPA0215
      PARAMETER (LENGTH=777)                                            ACPA0216
      INTEGER L,I,IBIT,LNPFQ,IP,IQ,IX,NSIGFIG,NMACH,ICOUNT,IXCNT        ACPA0217
      INTEGER REXP,IR10,II10,LMAX,IPREMAX,IERR                          ACPA0218
      CHARACTER*256 ERRSTR                                              acpa_add
      COMPLEX*16 A,B,Z,FINAL,TEMP,OLDTEMP,HYPER,FACTOR,CDUM1,CDUM2      ACPA0219
      COMPLEX*16 TEMP1                                                  ACPA0220
      DIMENSION SUMR(-1:LENGTH),SUMI(-1:LENGTH),NUMR(-1:LENGTH)         ACPA0221
      DIMENSION NUMI(-1:LENGTH),DENOMR(-1:LENGTH),DENOMI(-1:LENGTH)     ACPA0222
      DIMENSION QR1(-1:LENGTH),QR2(-1:LENGTH),                          ACPA0223
     :     QI1(-1:LENGTH),QI2(-1:LENGTH)                                ACPA0224
      DIMENSION A(IP),B(IQ),AR(10),AI(10),AR2(10),AI2(10),              ACPA0225
     :     CR(10),CI(10),CR2(10),CI2(10)                                ACPA0226
      DIMENSION WK1(-1:LENGTH),WK2(-1:LENGTH),WK3(-1:LENGTH),           ACPA0227
     :     WK4(-1:LENGTH),WK5(-1:LENGTH),WK6(-1:LENGTH)                 ACPA0228
      DOUBLE PRECISION AR,AI,CR,CI,XR,XI,SUMR,SUMI,CNT,SIGFIG           ACPA0229
      DOUBLE PRECISION NUMR,NUMI,DENOMR,DENOMI,RMAX,MX1,MX2,CREAL       ACPA0230
      DOUBLE PRECISION QR1,QR2,QI1,QI2,AR2,AI2,CR2,CI2,XR2,XI2          ACPA0231
      DOUBLE PRECISION ACCY,BITS,EXPON,XL,X                             ACPA0232
      DOUBLE PRECISION DUM1,DUM2,RR10,RI10,LOG2                         ACPA0233
      DOUBLE PRECISION WK1,WK2,WK3,WK4,WK5,WK6                          ACPA0234
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA0235
      COMMON/IO/NOUT                                                    ACPA0236
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA0237
                                                                        ACPA0238
      zero=0.0d0                                                        ACPA0239
      LOG2=LOG10(TWO)                                                   ACPA0240
      IBIT=INT(BITS(1.D1))                                                  ACPA0241
      RMAX=TWO**(IBIT/2)                                                ACPA0242
      SIGFIG=TWO**(IBIT/4)                                              ACPA0243
*                                                                       ACPA0244
      DO 10 I1=1,IP                                                     ACPA0245
         AR2(I1)=DBLE(A(I1))*SIGFIG                                     ACPA0246
         AR(I1)=AINT(AR2(I1))                                           ACPA0247
         AR2(I1)=ANINT((AR2(I1)-AR(I1))*RMAX)                           ACPA0248
         AI2(I1)=DIMAG(A(I1))*SIGFIG                                    ACPA0249
         AI(I1)=AINT(AI2(I1))                                           ACPA0250
         AI2(I1)=ANINT((AI2(I1)-AI(I1))*RMAX)                           ACPA0251
 10   CONTINUE                                                          ACPA0252
      DO 20 I1=1,IQ                                                     ACPA0253
         CR2(I1)=DBLE(B(I1))*SIGFIG                                     ACPA0254
         CR(I1)=AINT(CR2(I1))                                           ACPA0255
         CR2(I1)=ANINT((CR2(I1)-CR(I1))*RMAX)                           ACPA0256
         CI2(I1)=DIMAG(B(I1))*SIGFIG                                    ACPA0257
         CI(I1)=AINT(CI2(I1))                                           ACPA0258
         CI2(I1)=ANINT((CI2(I1)-CI(I1))*RMAX)                           ACPA0259
 20   CONTINUE                                                          ACPA0260
      XR2=DBLE(Z)*SIGFIG                                                ACPA0261
      XR=AINT(XR2)                                                      ACPA0262
      XR2=ANINT((XR2-XR)*RMAX)                                          ACPA0263
      XI2=DIMAG(Z)*SIGFIG                                               ACPA0264
      XI=AINT(XI2)                                                      ACPA0265
      XI2=ANINT((XI2-XI)*RMAX)                                          ACPA0266
*                                                                       ACPA0267
*     WARN THE USER THAT THE INPUT VALUE WAS SO CLOSE TO ZERO THAT IT   ACPA0268
*     WAS SET EQUAL TO ZERO.                                            ACPA0269
*                                                                       ACPA0270
      DO 30 I1=1,IP                                                     ACPA0271
         IF ((DBLE(A(I1)) .NE. ZERO) .AND. (AR(I1) .EQ. ZERO)           ACPA0272
     :        .AND. (AR2(I1) .EQ. ZERO))                                ACPA0273
     :        WRITE (NOUT,300) I1                                       ACPA0274
         IF ((DIMAG(A(I1)) .NE. ZERO) .AND. (AI(I1) .EQ. ZERO)          ACPA0275
     :        .AND. (AI2(I1) .EQ. ZERO))                                ACPA0276
     :        WRITE (NOUT,301) I1                                       ACPA0277
 30   CONTINUE                                                          ACPA0278
      DO 40 I1=1,IQ                                                     ACPA0279
         IF ((DBLE(B(I1)) .NE. ZERO) .AND. (CR(I1) .EQ. ZERO)           ACPA0280
     :        .AND. (CR2(I1) .EQ. ZERO))                                ACPA0281
     :        WRITE (NOUT,302) I1                                       ACPA0282
         IF ((DIMAG(B(I1)) .NE. ZERO) .AND. (CI(I1) .EQ. ZERO)          ACPA0283
     :        .AND. (CI2(I1) .EQ. ZERO))                                ACPA0284
     :        WRITE (NOUT,303) I1                                       ACPA0285
 40   CONTINUE                                                          ACPA0286
      IF ((DBLE(Z).NE.ZERO) .AND. (XR.EQ.ZERO) .AND. (XR2.EQ.ZERO))     ACPA0287
     :     THEN                                                         ACPA0288
         WRITE (NOUT,*) ' WARNING - REAL PART OF Z WAS SET TO ZERO'     ACPA0289
         Z = DCMPLX(ZERO,DIMAG(Z))                                      ACPA0290
      END IF                                                            ACPA0291
      IF ((DIMAG(Z).NE.ZERO) .AND. (XI.EQ.ZERO) .AND. (XI2.EQ.ZERO))    ACPA0292
     :     THEN                                                         ACPA0293
         WRITE (NOUT,*) ' WARNING - IMAG PART OF Z WAS SET TO ZERO'     ACPA0294
         Z = DCMPLX(DBLE(Z),ZERO)                                       ACPA0295
      END IF                                                            ACPA0296
                                                                        ACPA0297
*                                                                       ACPA0298
*     SCREENING OF NUMERATOR ARGUMENTS FOR NEGATIVE INTEGERS OR ZERO.   ACPA0299
*     ICOUNT WILL FORCE THE SERIES TO TERMINATE CORRECTLY.              ACPA0300
*                                                                       ACPA0301
      NMACH=INT(LOG10(TWO**INT(BITS(1.D1))))                                ACPA0302
      ICOUNT=-1                                                         ACPA0303
      DO 50 I1=1,IP                                                     ACPA0304
         IF ((AR2(I1) .EQ. ZERO) .AND. (AR(I1) .EQ. ZERO) .AND.         ACPA0305
     :        (AI2(I1) .EQ. ZERO) .AND. (AI(I1) .EQ. ZERO)) THEN        ACPA0306
            HYPER=DCMPLX(ONE,ZERO)                                      ACPA0307
            RETURN                                                      ACPA0308
         END IF                                                         ACPA0309
         IF ((AI(I1) .EQ. ZERO) .AND. (AI2(I1) .EQ. ZERO) .AND.         ACPA0310
     :        (DBLE(A(I1)) .LT. ZERO)) THEN                             ACPA0311
            IF (ABS(DBLE(A(I1))-DBLE(NINT(DBLE(A(I1)))))                ACPA0312
     :           .LT. TEN**(-NMACH)) THEN                               ACPA0313
               IF (ICOUNT .NE. -1) THEN                                 ACPA0314
                  ICOUNT=MIN(ICOUNT,-NINT(DBLE(A(I1))))                 ACPA0315
               ELSE                                                     ACPA0316
                  ICOUNT=-NINT(DBLE(A(I1)))                             ACPA0317
               END IF                                                   ACPA0318
            END IF                                                      ACPA0319
         END IF                                                         ACPA0320
 50   CONTINUE                                                          ACPA0321
*                                                                       ACPA0322
*    SCREENING OF DENOMINATOR ARGUMENTS FOR ZEROES OR NEGATIVE INTEGERS.ACPA0323
*                                                                       ACPA0324
      DO 60 I1=1,IQ                                                     ACPA0325
         IF ((CR(I1) .EQ. ZERO) .AND. (CR2(I1) .EQ. ZERO) .AND.         ACPA0326
     :        (CI(I1) .EQ. ZERO) .AND. (CI2(I1) .EQ. ZERO)) THEN        ACPA0327
            WRITE (ERRSTR,304) I1                                       ACPA0328
            IERR = 1
            RETURN                                                      ACPA0329
         END IF                                                         ACPA0330
         IF ((CI(I1) .EQ. ZERO) .AND. (CI2(I1) .EQ. ZERO) .AND.         ACPA0331
     :        (DBLE(B(I1)) .LT. ZERO)) THEN                             ACPA0332
            IF ((ABS(DBLE(B(I1))-DBLE(NINT(DBLE(B(I1)))))               ACPA0333
     :           .LT. TEN**(-NMACH)) .AND.                              ACPA0334
     :           (ICOUNT .GE. -NINT(DBLE(B(I1))) .OR.                   ACPA0335
     :           ICOUNT .EQ. -1)) THEN                                  ACPA0336
               WRITE (ERRSTR,305) I1                                    ACPA0337
               IERR = 1
               RETURN                                                   ACPA0338
            END IF                                                      ACPA0339
         END IF                                                         ACPA0340
 60   CONTINUE                                                          ACPA0341
*                                                                       ACPA0342
      NMACH=INT(LOG10(TWO**IBIT))                                       ACPA0343
      NSIGFIG=MIN(NSIGFIG,INT(LOG10(TWO**IBIT)))                        ACPA0344
      ACCY = TEN**(-NSIGFIG)                                            ACPA0345
      L=IPREMAX(A,B,IP,IQ,Z)                                            ACPA0346
      IF (L .EQ. 1) GO TO 110                                           ACPA0347
*                                                                       ACPA0348
*    First, estimate the exponent of the maximum term in the pFq series.ACPA0349
*                                                                       ACPA0350
      EXPON=ZERO                                                        ACPA0351
      XL=DBLE(L)                                                        ACPA0352
      DO 70 I=1,IP                                                      ACPA0353
         EXPON=EXPON+DBLE(FACTOR(A(I)+XL-ONE))-DBLE(FACTOR(A(I)-ONE))   ACPA0354
 70   CONTINUE                                                          ACPA0355
      DO 80 I=1,IQ                                                      ACPA0356
         EXPON=EXPON-DBLE(FACTOR(B(I)+XL-ONE))+DBLE(FACTOR(B(I)-ONE))   ACPA0357
 80   CONTINUE                                                          ACPA0358
      EXPON = EXPON + XL*DBLE(LOG(Z)) - DBLE(FACTOR(DCMPLX(XL,ZERO)))   ACPA0359
      LMAX=INT(LOG10(EXP(ONE))*EXPON)                                   ACPA0360
      L=LMAX                                                            ACPA0361
*                                                                       ACPA0362
*     Now, estimate the exponent of where the pFq series will terminate.ACPA0363
*                                                                       ACPA0364
      TEMP1=CMPLX(ONE,ZERO)                                             ACPA0365
      CREAL=ONE                                                         ACPA0366
      DO 90 I1=1,IP                                                     ACPA0367
         TEMP1=TEMP1*CMPLX(AR(I1),AI(I1))/SIGFIG                        ACPA0368
 90   CONTINUE                                                          ACPA0369
      DO 100 I1=1,IQ                                                    ACPA0370
         TEMP1=TEMP1/(CMPLX(CR(I1),CI(I1))/SIGFIG)                      ACPA0371
         CREAL=CREAL*CR(I1)                                             ACPA0372
 100  CONTINUE                                                          ACPA0373
      TEMP1=TEMP1*CMPLX(XR,XI)                                          ACPA0374
*                                                                       ACPA0375
*     Triple it to make sure.                                           ACPA0376
*                                                                       ACPA0377
      L=3*L                                                             ACPA0378
*                                                                       ACPA0379
*    Divide the number of significant figures necessary by the number ofACPA0380
*    digits available per array position.                               ACPA0381
*                                                                       ACPA0382
      L=INT((2*L+NSIGFIG)/NMACH)+2                                      ACPA0383
*                                                                       ACPA0384
*     Make sure there are at least 5 array positions used.              ACPA0385
*                                                                       ACPA0386
 110  L=MAX(L,5)                                                        ACPA0387
      L=MAX(L,IX)                                                       ACPA0388
*     write (6,*) ' Estimated value of L=',L                            ACPA0389
      IF ((L .LT. 0) .OR. (L .GT. LENGTH)) THEN                         ACPA0390
         WRITE (ERRSTR,306) LENGTH                                      ACPA0391
         IERR = 1                                                       acpa_add
         RETURN                                                         ACPA0392
      END IF                                                            ACPA0393
      IF (NSIGFIG .GT. NMACH) THEN                                      ACPA0394
         WRITE (NOUT,307) NMACH                                         ACPA0395
      END IF                                                            ACPA0396
                                                                        ACPA0397
      SUMR(-1)=ONE                                                      ACPA0398
      SUMI(-1)=ONE                                                      ACPA0399
      NUMR(-1)=ONE                                                      ACPA0400
      NUMI(-1)=ONE                                                      ACPA0401
      DENOMR(-1)=ONE                                                    ACPA0402
      DENOMI(-1)=ONE                                                    ACPA0403
      DO 120 I=0,L+1                                                    ACPA0404
         SUMR(I)=ZERO                                                   ACPA0405
         SUMI(I)=ZERO                                                   ACPA0406
         NUMR(I)=ZERO                                                   ACPA0407
         NUMI(I)=ZERO                                                   ACPA0408
         DENOMR(I)=ZERO                                                 ACPA0409
         DENOMI(I)=ZERO                                                 ACPA0410
 120  CONTINUE                                                          ACPA0411
      SUMR(1)=ONE                                                       ACPA0412
      NUMR(1)=ONE                                                       ACPA0413
      DENOMR(1)=ONE                                                     ACPA0414
      CNT=SIGFIG                                                        ACPA0415
      TEMP=DCMPLX(ZERO,ZERO)                                            ACPA0416
      OLDTEMP=TEMP                                                      ACPA0417
      IXCNT=0                                                           ACPA0418
      REXP=IBIT/2                                                       ACPA0419
      X=REXP*(SUMR(L+1)-2)                                              ACPA0420
      RR10=X*LOG2                                                       ACPA0421
      IR10=INT(RR10)                                                    ACPA0422
      RR10=RR10-IR10                                                    ACPA0423
      X=REXP*(SUMI(L+1)-2)                                              ACPA0424
      RI10=X*LOG2                                                       ACPA0425
      II10=INT(RI10)                                                    ACPA0426
      RI10=RI10-II10                                                    ACPA0427
      DUM1=SIGN(SUMR(1)*RMAX*RMAX+SUMR(2)*RMAX+SUMR(3),SUMR(-1))        ACPA0428
      DUM2=SIGN(SUMI(1)*RMAX*RMAX+SUMI(2)*RMAX+SUMI(3),SUMI(-1))        ACPA0429
      DUM1=DUM1*10**RR10                                                ACPA0430
      DUM2=DUM2*10**RI10                                                ACPA0431
      CDUM1=DCMPLX(DUM1,DUM2)                                           ACPA0432
      X=REXP*(DENOMR(L+1)-2)                                            ACPA0433
      RR10=X*LOG2                                                       ACPA0434
      IR10=INT(RR10)                                                    ACPA0435
      RR10=RR10-IR10                                                    ACPA0436
      X=REXP*(DENOMI(L+1)-2)                                            ACPA0437
      RI10=X*LOG2                                                       ACPA0438
      II10=INT(RI10)                                                    ACPA0439
      RI10=RI10-II10                                                    ACPA0440
      DUM1=SIGN(DENOMR(1)*RMAX*RMAX+DENOMR(2)*RMAX+                     ACPA0441
     :     DENOMR(3),DENOMR(-1))                                        ACPA0442
      DUM2=SIGN(DENOMI(1)*RMAX*RMAX+DENOMI(2)*RMAX+                     ACPA0443
     :     DENOMI(3),DENOMI(-1))                                        ACPA0444
      DUM1=DUM1*10**RR10                                                ACPA0445
      DUM2=DUM2*10**RI10                                                ACPA0446
      CDUM2=DCMPLX(DUM1,DUM2)                                           ACPA0447
      TEMP=CDUM1/CDUM2                                                  ACPA0448
                                                                        ACPA0449
*     130 IF (IP .GT. 0) THEN                                           ACPA0450
 130  if (ip .lt. 0) then                                               ACPA0451
         IF (SUMR(1) .LT. HALF) THEN                                    ACPA0452
            MX1=SUMI(L+1)                                               ACPA0453
         ELSE IF (SUMI(1) .LT. HALF) THEN                               ACPA0454
            MX1=SUMR(L+1)                                               ACPA0455
         ELSE                                                           ACPA0456
            MX1=DMAX1(SUMR(L+1),SUMI(L+1))                              ACPA0457
         ENDIF                                                          ACPA0458
         IF (NUMR(1) .LT. HALF) THEN                                    ACPA0459
            MX2=NUMI(L+1)                                               ACPA0460
         ELSE IF (NUMI(1) .LT. HALF) THEN                               ACPA0461
            MX2=NUMR(L+1)                                               ACPA0462
         ELSE                                                           ACPA0463
            MX2=DMAX1(NUMR(L+1),NUMI(L+1))                              ACPA0464
         ENDIF                                                          ACPA0465
         IF (MX1-MX2 .GT.  2.0) THEN                                    ACPA0466
            IF (CREAL .GE. ZERO) THEN                                   ACPA0467
*     write (6,*) ' cdabs(temp1/cnt)=',cdabs(temp1/cnt)                 ACPA0468
               IF (CDABS(TEMP1/CNT) .LE. ONE) GOTO 240                  ACPA0469
            ENDIF                                                       ACPA0470
         ENDIF                                                          ACPA0471
      ELSE                                                              ACPA0472
         CALL ARYDIV(SUMR,SUMI,DENOMR,DENOMI,TEMP,L,LNPFQ,RMAX,IBIT)    ACPA0473
*                                                                       ACPA0474
*    First, estimate the exponent of the maximum term in the pFq series.ACPA0475
*                                                                       ACPA0476
         EXPON=ZERO                                                     ACPA0477
         XL=DBLE(ixcnt)                                                 ACPA0478
         DO 140 I=1,IP                                                  ACPA0479
            EXPON=EXPON+DBLE(FACTOR(A(I)+XL-ONE))-DBLE(FACTOR(A(I)-ONE))ACPA0480
 140     CONTINUE                                                       ACPA0481
         DO 150 I=1,IQ                                                  ACPA0482
            EXPON=EXPON-DBLE(FACTOR(B(I)+XL-ONE))+DBLE(FACTOR(B(I)-ONE))ACPA0483
 150     CONTINUE                                                       ACPA0484
         EXPON = EXPON + XL*DBLE(LOG(Z)) - DBLE(FACTOR(DCMPLX(XL,ZERO)))ACPA0485
         LMAX=INT(LOG10(EXP(ONE))*EXPON)                                ACPA0486
         IF (ABS(OLDTEMP-TEMP) .LT. ABS(TEMP*ACCY)) GO TO 240           ACPA0487
         OLDTEMP=TEMP                                                   ACPA0488
      END IF                                                            ACPA0489
      IF (IXCNT .EQ. ICOUNT) GO TO 240                                  ACPA0490
      IXCNT=IXCNT+1                                                     ACPA0491
      DO 160 I1=1,IQ                                                    ACPA0492
*                                                                       ACPA0493
*     TAKE THE CURRENT SUM AND MULTIPLY BY THE DENOMINATOR OF THE NEXT  ACPA0494
*     TERM, FOR BOTH THE MOST SIGNIFICANT HALF (CR,CI) AND THE LEAST    ACPA0495
*     SIGNIFICANT HALF (CR2,CI2).                                       ACPA0496
*                                                                       ACPA0497
         CALL CMPMUL(SUMR,SUMI,CR(I1),CI(I1),QR1,QI1,                   ACPA0498
     :        WK1,WK2,WK3,WK4,WK5,WK6,L,RMAX)                           ACPA0499
         CALL CMPMUL(SUMR,SUMI,CR2(I1),CI2(I1),QR2,QI2,                 ACPA0500
     :        WK1,WK2,WK3,WK4,WK5,WK6,L,RMAX)                           ACPA0501
         QR2(L+1)=QR2(L+1)-1                                            ACPA0502
         QI2(L+1)=QI2(L+1)-1                                            ACPA0503
*                                                                       ACPA0504
*     STORE THIS TEMPORARILY IN THE SUM ARRAYS.                         ACPA0505
*                                                                       ACPA0506
         CALL CMPADD(QR1,QI1,QR2,QI2,SUMR,SUMI,WK1,L,RMAX)              ACPA0507
 160  CONTINUE                                                          ACPA0508
                                                                        ACPA0509
*                                                                       ACPA0510
*     MULTIPLY BY THE FACTORIAL TERM.                                   ACPA0511
*                                                                       ACPA0512
      CALL ARMULT(SUMR,CNT,SUMR,WK6,L,RMAX)                             ACPA0513
      CALL ARMULT(SUMI,CNT,SUMI,WK6,L,RMAX)                             ACPA0514
*                                                                       ACPA0515
*     MULTIPLY BY THE SCALING FACTOR, SIGFIG, TO KEEP THE SCALE CORRECT.ACPA0516
*                                                                       ACPA0517
      DO 170 I1=1,IP-IQ                                                 ACPA0518
         CALL ARMULT(SUMR,SIGFIG,SUMR,WK6,L,RMAX)                       ACPA0519
         CALL ARMULT(SUMI,SIGFIG,SUMI,WK6,L,RMAX)                       ACPA0520
 170  CONTINUE                                                          ACPA0521
      DO 180 I1=1,IQ                                                    ACPA0522
*                                                                       ACPA0523
*     UPDATE THE DENOMINATOR.                                           ACPA0524
*                                                                       ACPA0525
         CALL CMPMUL(DENOMR,DENOMI,CR(I1),CI(I1),QR1,QI1,               ACPA0526
     :        WK1,WK2,WK3,WK4,WK5,WK6,L,RMAX)                           ACPA0527
         CALL CMPMUL(DENOMR,DENOMI,CR2(I1),CI2(I1),QR2,QI2,             ACPA0528
     :        WK1,WK2,WK3,WK4,WK5,WK6,L,RMAX)                           ACPA0529
         QR2(L+1)=QR2(L+1)-1                                            ACPA0530
         QI2(L+1)=QI2(L+1)-1                                            ACPA0531
         CALL CMPADD(QR1,QI1,QR2,QI2,DENOMR,DENOMI,WK1,L,RMAX)          ACPA0532
 180  CONTINUE                                                          ACPA0533
                                                                        ACPA0534
*                                                                       ACPA0535
*     MULTIPLY BY THE FACTORIAL TERM.                                   ACPA0536
*                                                                       ACPA0537
      CALL ARMULT(DENOMR,CNT,DENOMR,WK6,L,RMAX)                         ACPA0538
      CALL ARMULT(DENOMI,CNT,DENOMI,WK6,L,RMAX)                         ACPA0539
*                                                                       ACPA0540
*     MULTIPLY BY THE SCALING FACTOR, SIGFIG, TO KEEP THE SCALE CORRECT.ACPA0541
*                                                                       ACPA0542
      DO 190 I1=1,IP-IQ                                                 ACPA0543
         CALL ARMULT(DENOMR,SIGFIG,DENOMR,WK6,L,RMAX)                   ACPA0544
         CALL ARMULT(DENOMI,SIGFIG,DENOMI,WK6,L,RMAX)                   ACPA0545
 190  CONTINUE                                                          ACPA0546
*                                                                       ACPA0547
*     FORM THE NEXT NUMERATOR TERM BY MULTIPLYING THE CURRENT           ACPA0548
*     NUMERATOR TERM (AN ARRAY) WITH THE A ARGUMENT (A SCALAR).         ACPA0549
*                                                                       ACPA0550
      DO 200 I1=1,IP                                                    ACPA0551
         CALL CMPMUL(NUMR,NUMI,AR(I1),AI(I1),QR1,QI1,                   ACPA0552
     :        WK1,WK2,WK3,WK4,WK5,WK6,L,RMAX)                           ACPA0553
         CALL CMPMUL(NUMR,NUMI,AR2(I1),AI2(I1),QR2,QI2,                 ACPA0554
     :        WK1,WK2,WK3,WK4,WK5,WK6,L,RMAX)                           ACPA0555
         QR2(L+1)=QR2(L+1)-1                                            ACPA0556
         QI2(L+1)=QI2(L+1)-1                                            ACPA0557
         CALL CMPADD(QR1,QI1,QR2,QI2,NUMR,NUMI,WK1,L,RMAX)              ACPA0558
 200  CONTINUE                                                          ACPA0559
*                                                                       ACPA0560
*     FINISH THE NEW NUMERATOR TERM BY MULTIPLYING BY THE Z ARGUMENT.   ACPA0561
*                                                                       ACPA0562
      CALL CMPMUL(NUMR,NUMI,XR,XI,QR1,QI1,                              ACPA0563
     :     WK1,WK2,WK3,WK4,WK5,WK6,L,RMAX)                              ACPA0564
      CALL CMPMUL(NUMR,NUMI,XR2,XI2,QR2,QI2,                            ACPA0565
     :     WK1,WK2,WK3,WK4,WK5,WK6,L,RMAX)                              ACPA0566
      QR2(L+1)=QR2(L+1)-1                                               ACPA0567
      QI2(L+1)=QI2(L+1)-1                                               ACPA0568
      CALL CMPADD(QR1,QI1,QR2,QI2,NUMR,NUMI,WK1,L,RMAX)                 ACPA0569
*                                                                       ACPA0570
*     MULTIPLY BY THE SCALING FACTOR, SIGFIG, TO KEEP THE SCALE CORRECT.ACPA0571
*                                                                       ACPA0572
      DO 210 I1=1,IQ-IP                                                 ACPA0573
         CALL ARMULT(NUMR,SIGFIG,NUMR,WK6,L,RMAX)                       ACPA0574
         CALL ARMULT(NUMI,SIGFIG,NUMI,WK6,L,RMAX)                       ACPA0575
 210  CONTINUE                                                          ACPA0576
*                                                                       ACPA0577
*     FINALLY, ADD THE NEW NUMERATOR TERM WITH THE CURRENT RUNNING      ACPA0578
*     SUM OF THE NUMERATOR AND STORE THE NEW RUNNING SUM IN SUMR, SUMI. ACPA0579
*                                                                       ACPA0580
      CALL CMPADD(SUMR,SUMI,NUMR,NUMI,SUMR,SUMI,WK1,L,RMAX)             ACPA0581
*                                                                       ACPA0582
*     BECAUSE SIGFIG REPRESENTS "ONE" ON THE NEW SCALE, ADD SIGFIG      ACPA0583
*     TO THE CURRENT COUNT AND, CONSEQUENTLY, TO THE IP ARGUMENTS       ACPA0584
*     IN THE NUMERATOR AND THE IQ ARGUMENTS IN THE DENOMINATOR.         ACPA0585
*                                                                       ACPA0586
      CNT=CNT+SIGFIG                                                    ACPA0587
      DO 220 I1=1,IP                                                    ACPA0588
         AR(I1)=AR(I1)+SIGFIG                                           ACPA0589
 220  CONTINUE                                                          ACPA0590
      DO 230 I1=1,IQ                                                    ACPA0591
         CR(I1)=CR(I1)+SIGFIG                                           ACPA0592
 230  CONTINUE                                                          ACPA0593
      GOTO 130                                                          ACPA0594
 240  CALL ARYDIV(SUMR,SUMI,DENOMR,DENOMI,FINAL,L,LNPFQ,RMAX,IBIT)      ACPA0595
*     write (6,*) 'Number of terms=',ixcnt                              ACPA0596
      HYPER=FINAL                                                       ACPA0597
      RETURN                                                            ACPA0598
 300  FORMAT (1X,'WARNING - REAL PART OF A(',1I2,') WAS SET TO ZERO')   ACPA0599
 301  FORMAT (1X,'WARNING - IMAG PART OF A(',1I2,') WAS SET TO ZERO')   ACPA0600
 302  FORMAT (1X,'WARNING - REAL PART OF B(',1I2,') WAS SET TO ZERO')   ACPA0601
 303  FORMAT (1X,'WARNING - IMAG PART OF B(',1I2,') WAS SET TO ZERO')   ACPA0602
 304  FORMAT (1X,'ERROR - ARGUMENT B(',1I2,') WAS EQUAL TO ZERO')       ACPA0603
 305  FORMAT (1X,'ERROR - ARGUMENT B(',1I2,') WAS A NEGATIVE',          ACPA0604
     :     ' INTEGER')                                                  ACPA0605
 306  FORMAT (1X,'ERROR IN FN HYPER: L MUST BE < ',1I4)                 ACPA0606
 307  FORMAT (1X,' WARNING--THE NUMBER OF SIGNIFICANT FIGURES REQU',    ACPA0607
     :     'ESTED',/,'IS GREATER THAN THE MACHINE PRECISION--',         ACPA0608
     :     'FINAL ANSWER',/,'WILL BE ACCURATE TO ONLY',I3,' DIGITS')    ACPA0609
      END                                                               ACPA0610
                                                                        ACPA0611
C     ****************************************************************  ACPA0612
C     *                                                              *  ACPA0613
C     *                 SUBROUTINE ARADD                             *  ACPA0614
C     *                                                              *  ACPA0615
C     *                                                              *  ACPA0616
C     *  Description : Accepts two arrays of numbers and returns     *  ACPA0617
C     *    the sum of the array.  Each array is holding the value    *  ACPA0618
C     *    of one number in the series.  The parameter L is the      *  ACPA0619
C     *    size of the array representing the number and RMAX is     *  ACPA0620
C     *    the actual number of digits needed to give the numbers    *  ACPA0621
C     *    the desired accuracy.                                     *  ACPA0622
C     *                                                              *  ACPA0623
C     *  Subprograms called: none                                    *  ACPA0624
C     *                                                              *  ACPA0625
C     ****************************************************************  ACPA0626
                                                                        ACPA0627
      SUBROUTINE ARADD(A,B,C,Z,L,RMAX)                                  ACPA0628
                                                                        ACPA0629
      INTEGER L                                                         ACPA0630
      DOUBLE PRECISION A,B,C,Z,RMAX                                     ACPA0631
      INTEGER EDIFF,I,J                                                 ACPA0632
      DIMENSION A(-1:*),B(-1:*),C(-1:*),Z(-1:*)                         ACPA0633
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA0634
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA0635
                                                                        ACPA0636
      DO 10 I=0,L+1                                                     ACPA0637
         Z(I)=ZERO                                                      ACPA0638
 10   CONTINUE                                                          ACPA0639
      EDIFF=ANINT(A(L+1)-B(L+1))                                        ACPA0640
      IF (ABS(A(1)) .LT. HALF .OR. EDIFF .LE. -L) GOTO 20               ACPA0641
      IF (ABS(B(1)) .LT. HALF .OR. EDIFF .GE. L) GOTO 40                ACPA0642
      GOTO 60                                                           ACPA0643
 20   DO 30 I=-1,L+1                                                    ACPA0644
         C(I)=B(I)                                                      ACPA0645
 30   CONTINUE                                                          ACPA0646
      GOTO 350                                                          ACPA0647
 40   DO 50 I=-1,L+1                                                    ACPA0648
         C(I)=A(I)                                                      ACPA0649
 50   CONTINUE                                                          ACPA0650
      GOTO 350                                                          ACPA0651
 60   Z(-1)=A(-1)                                                       ACPA0652
      IF (ABS(A(-1)-B(-1)) .LT. HALF) GOTO 80                           ACPA0653
      IF (EDIFF .GT. 0) THEN                                            ACPA0654
         Z(L+1)=A(L+1)                                                  ACPA0655
         GOTO 190                                                       ACPA0656
      ENDIF                                                             ACPA0657
      IF (EDIFF .LT. 0) THEN                                            ACPA0658
         Z(L+1)=B(L+1)                                                  ACPA0659
         Z(-1)=B(-1)                                                    ACPA0660
         GOTO 240                                                       ACPA0661
      ENDIF                                                             ACPA0662
      DO 70 I=1,L                                                       ACPA0663
         IF (A(I) .GT. B(I)) THEN                                       ACPA0664
            Z(L+1)=A(L+1)                                               ACPA0665
            GOTO 190                                                    ACPA0666
         ENDIF                                                          ACPA0667
         IF (A(I) .LT. B(I)) THEN                                       ACPA0668
            Z(L+1)=B(L+1)                                               ACPA0669
            Z(-1)=B(-1)                                                 ACPA0670
            GOTO 240                                                    ACPA0671
         ENDIF                                                          ACPA0672
 70   CONTINUE                                                          ACPA0673
      GOTO 300                                                          ACPA0674
                                                                        ACPA0675
 80   IF (EDIFF .GT. 0) GOTO 110                                        ACPA0676
      IF (EDIFF .LT. 0) GOTO 150                                        ACPA0677
      Z(L+1)=A(L+1)                                                     ACPA0678
      DO 90 I=L,1,-1                                                    ACPA0679
         Z(I)=A(I)+B(I)+Z(I)                                            ACPA0680
         IF (Z(I) .GE. RMAX) THEN                                       ACPA0681
            Z(I)=Z(I)-RMAX                                              ACPA0682
            Z(I-1)=ONE                                                  ACPA0683
         ENDIF                                                          ACPA0684
 90   CONTINUE                                                          ACPA0685
      IF (Z(0) .GT. HALF) THEN                                          ACPA0686
         DO 100 I=L,1,-1                                                ACPA0687
            Z(I)=Z(I-1)                                                 ACPA0688
 100     CONTINUE                                                       ACPA0689
         Z(L+1)=Z(L+1)+ONE                                              ACPA0690
         Z(0)=ZERO                                                      ACPA0691
      ENDIF                                                             ACPA0692
      GOTO 300                                                          ACPA0693
 110  Z(L+1)=A(L+1)                                                     ACPA0694
      DO 120 I=L,1+EDIFF,-1                                             ACPA0695
         Z(I)=A(I)+B(I-EDIFF)+Z(I)                                      ACPA0696
         IF (Z(I) .GE. RMAX) THEN                                       ACPA0697
            Z(I)=Z(I)-RMAX                                              ACPA0698
            Z(I-1)=ONE                                                  ACPA0699
         ENDIF                                                          ACPA0700
 120  CONTINUE                                                          ACPA0701
      DO 130 I=EDIFF,1,-1                                               ACPA0702
         Z(I)=A(I)+Z(I)                                                 ACPA0703
         IF (Z(I) .GE. RMAX) THEN                                       ACPA0704
            Z(I)=Z(I)-RMAX                                              ACPA0705
            Z(I-1)=ONE                                                  ACPA0706
         ENDIF                                                          ACPA0707
 130  CONTINUE                                                          ACPA0708
      IF (Z(0) .GT. HALF) THEN                                          ACPA0709
         DO 140 I=L,1,-1                                                ACPA0710
            Z(I)=Z(I-1)                                                 ACPA0711
 140     CONTINUE                                                       ACPA0712
         Z(L+1)=Z(L+1)+1                                                ACPA0713
         Z(0)=ZERO                                                      ACPA0714
      ENDIF                                                             ACPA0715
      GOTO 300                                                          ACPA0716
 150  Z(L+1)=B(L+1)                                                     ACPA0717
      DO 160 I=L,1-EDIFF,-1                                             ACPA0718
         Z(I)=A(I+EDIFF)+B(I)+Z(I)                                      ACPA0719
         IF (Z(I) .GE. RMAX) THEN                                       ACPA0720
            Z(I)=Z(I)-RMAX                                              ACPA0721
            Z(I-1)=ONE                                                  ACPA0722
         ENDIF                                                          ACPA0723
 160  CONTINUE                                                          ACPA0724
      DO 170 I=0-EDIFF,1,-1                                             ACPA0725
         Z(I)=B(I)+Z(I)                                                 ACPA0726
         IF (Z(I) .GE. RMAX) THEN                                       ACPA0727
            Z(I)=Z(I)-RMAX                                              ACPA0728
            Z(I-1)=ONE                                                  ACPA0729
         ENDIF                                                          ACPA0730
 170  CONTINUE                                                          ACPA0731
      IF (Z(0) .GT. HALF) THEN                                          ACPA0732
         DO 180 I=L,1,-1                                                ACPA0733
            Z(I)=Z(I-1)                                                 ACPA0734
 180     CONTINUE                                                       ACPA0735
         Z(L+1)=Z(L+1)+ONE                                              ACPA0736
         Z(0)=ZERO                                                      ACPA0737
      ENDIF                                                             ACPA0738
      GOTO 300                                                          ACPA0739
                                                                        ACPA0740
 190  IF (EDIFF .GT. 0) GOTO 210                                        ACPA0741
      DO 200 I=L,1,-1                                                   ACPA0742
         Z(I)=A(I)-B(I)+Z(I)                                            ACPA0743
         IF (Z(I) .LT. ZERO) THEN                                       ACPA0744
            Z(I)=Z(I)+RMAX                                              ACPA0745
            Z(I-1)=-ONE                                                 ACPA0746
         ENDIF                                                          ACPA0747
 200  CONTINUE                                                          ACPA0748
      GOTO 290                                                          ACPA0749
 210  DO 220 I=L,1+EDIFF,-1                                             ACPA0750
         Z(I)=A(I)-B(I-EDIFF)+Z(I)                                      ACPA0751
         IF (Z(I) .LT. ZERO) THEN                                       ACPA0752
            Z(I)=Z(I)+RMAX                                              ACPA0753
            Z(I-1)=-ONE                                                 ACPA0754
         ENDIF                                                          ACPA0755
 220  CONTINUE                                                          ACPA0756
      DO 230 I=EDIFF,1,-1                                               ACPA0757
         Z(I)=A(I)+Z(I)                                                 ACPA0758
         IF (Z(I) .LT. ZERO) THEN                                       ACPA0759
            Z(I)=Z(I)+RMAX                                              ACPA0760
            Z(I-1)=-ONE                                                 ACPA0761
         ENDIF                                                          ACPA0762
 230  CONTINUE                                                          ACPA0763
      GOTO 290                                                          ACPA0764
                                                                        ACPA0765
 240  IF (EDIFF .LT. 0) GOTO 260                                        ACPA0766
      DO 250 I=L,1,-1                                                   ACPA0767
         Z(I)=B(I)-A(I)+Z(I)                                            ACPA0768
         IF (Z(I) .LT. ZERO) THEN                                       ACPA0769
            Z(I)=Z(I)+RMAX                                              ACPA0770
            Z(I-1)=-ONE                                                 ACPA0771
         ENDIF                                                          ACPA0772
 250  CONTINUE                                                          ACPA0773
      GOTO 290                                                          ACPA0774
 260  DO 270 I=L,1-EDIFF,-1                                             ACPA0775
         Z(I)=B(I)-A(I+EDIFF)+Z(I)                                      ACPA0776
         IF (Z(I) .LT. ZERO) THEN                                       ACPA0777
            Z(I)=Z(I)+RMAX                                              ACPA0778
            Z(I-1)=-ONE                                                 ACPA0779
         ENDIF                                                          ACPA0780
 270  CONTINUE                                                          ACPA0781
      DO 280 I=0-EDIFF,1,-1                                             ACPA0782
         Z(I)=B(I)+Z(I)                                                 ACPA0783
         IF (Z(I) .LT. ZERO) THEN                                       ACPA0784
            Z(I)=Z(I)+RMAX                                              ACPA0785
            Z(I-1)=-ONE                                                 ACPA0786
         ENDIF                                                          ACPA0787
 280  CONTINUE                                                          ACPA0788
                                                                        ACPA0789
 290  IF (Z(1) .GT. HALF) GOTO 330                                      ACPA0790
      I=1                                                               ACPA0791
 300  I=I+1                                                             ACPA0792
      IF (Z(I) .LT. HALF .AND. I .LT. L+1) GOTO 300                     ACPA0793
      IF (I .EQ. L+1) THEN                                              ACPA0794
         Z(-1)=ONE                                                      ACPA0795
         Z(L+1)=ZERO                                                    ACPA0796
         GOTO 330                                                       ACPA0797
      ENDIF                                                             ACPA0798
      DO 310 J=1,L+1-I                                                  ACPA0799
         Z(J)=Z(J+I-1)                                                  ACPA0800
 310  CONTINUE                                                          ACPA0801
      DO 320 J=L+2-I,L                                                  ACPA0802
         Z(J)=ZERO                                                      ACPA0803
 320  CONTINUE                                                          ACPA0804
      Z(L+1)=Z(L+1)-I+1                                                 ACPA0805
 330  DO 340 I=-1,L+1                                                   ACPA0806
         C(I)=Z(I)                                                      ACPA0807
 340  CONTINUE                                                          ACPA0808
 350  IF (C(1) .LT. HALF) THEN                                          ACPA0809
         C(-1)=ONE                                                      ACPA0810
         C(L+1)=ZERO                                                    ACPA0811
      ENDIF                                                             ACPA0812
      RETURN                                                            ACPA0813
      END                                                               ACPA0814
                                                                        ACPA0815
                                                                        ACPA0816
C     ****************************************************************  ACPA0817
C     *                                                              *  ACPA0818
C     *                 SUBROUTINE ARSUB                             *  ACPA0819
C     *                                                              *  ACPA0820
C     *                                                              *  ACPA0821
C     *  Description : Accepts two arrays and subtracts each element *  ACPA0822
C     *    in the second array from the element in the first array   *  ACPA0823
C     *    and returns the solution.  The parameters L and RMAX are  *  ACPA0824
C     *    the size of the array and the number of digits needed for *  ACPA0825
C     *    the accuracy, respectively.                               *  ACPA0826
C     *                                                              *  ACPA0827
C     *  Subprograms called: ARADD                                   *  ACPA0828
C     *                                                              *  ACPA0829
C     ****************************************************************  ACPA0830
                                                                        ACPA0831
      SUBROUTINE ARSUB(A,B,C,WK1,WK2,L,RMAX)                            ACPA0832
                                                                        ACPA0833
      INTEGER L,I                                                       ACPA0834
      DOUBLE PRECISION A,B,C,WK1,WK2,RMAX                               ACPA0835
      DIMENSION A(-1:*),B(-1:*),C(-1:*),WK1(-1:*),WK2(-1:*)             ACPA0836
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA0837
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA0838
                                                                        ACPA0839
      DO 10 I=-1,L+1                                                    ACPA0840
         WK2(I)=B(I)                                                    ACPA0841
 10   CONTINUE                                                          ACPA0842
      WK2(-1)=(-ONE)*WK2(-1)                                            ACPA0843
      CALL ARADD(A,WK2,C,WK1,L,RMAX)                                    ACPA0844
      RETURN                                                            ACPA0845
      END                                                               ACPA0846
                                                                        ACPA0847
                                                                        ACPA0848
C     ****************************************************************  ACPA0849
C     *                                                              *  ACPA0850
C     *                 SUBROUTINE ARMULT                            *  ACPA0851
C     *                                                              *  ACPA0852
C     *                                                              *  ACPA0853
C     *  Description : Accepts two arrays and returns the product.   *  ACPA0854
C     *    L and RMAX are the size of the arrays and the number of   *  ACPA0855
C     *    digits needed to represent the numbers with the required  *  ACPA0856
C     *    accuracy.                                                 *  ACPA0857
C     *                                                              *  ACPA0858
C     *  Subprograms called: none                                    *  ACPA0859
C     *                                                              *  ACPA0860
C     ****************************************************************  ACPA0861
                                                                        ACPA0862
      SUBROUTINE ARMULT(A,B,C,Z,L,RMAX)                                 ACPA0863
                                                                        ACPA0864
      INTEGER L                                                         ACPA0865
      DOUBLE PRECISION A,B,C,Z,B2,CARRY,RMAX                            ACPA0866
      DIMENSION A(-1:*),C(-1:*),Z(-1:*)                                 ACPA0867
      INTEGER I                                                         ACPA0868
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA0869
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA0870
                                                                        ACPA0871
      Z(-1)=SIGN(ONE,B)*A(-1)                                           ACPA0872
      B2=ABS(B)                                                         ACPA0873
      Z(L+1)=A(L+1)                                                     ACPA0874
      DO 10 I=0,L                                                       ACPA0875
         Z(I)=ZERO                                                      ACPA0876
 10   CONTINUE                                                          ACPA0877
      IF (B2 .LE. EPS .OR. A(1) .LE. EPS) THEN                          ACPA0878
         Z(-1)=ONE                                                      ACPA0879
         Z(L+1)=ZERO                                                    ACPA0880
         GOTO 60                                                        ACPA0881
      ENDIF                                                             ACPA0882
      DO 20 I=L,1,-1                                                    ACPA0883
         Z(I)=A(I)*B2+Z(I)                                              ACPA0884
         IF (Z(I) .GE. RMAX) THEN                                       ACPA0885
            CARRY=AINT(Z(I)/RMAX)                                       ACPA0886
            Z(I)=Z(I)-CARRY*RMAX                                        ACPA0887
            Z(I-1)=CARRY                                                ACPA0888
         ENDIF                                                          ACPA0889
 20   CONTINUE                                                          ACPA0890
      IF (Z(0) .LT. HALF) GOTO 50                                       ACPA0891
      DO 30 I=L,1,-1                                                    ACPA0892
         Z(I)=Z(I-1)                                                    ACPA0893
 30   CONTINUE                                                          ACPA0894
      Z(L+1)=Z(L+1)+ONE                                                 ACPA0895
      IF (Z(1) .GE. RMAX) THEN                                          ACPA0896
         DO 40 I=L,1,-1                                                 ACPA0897
            Z(I)=Z(I-1)                                                 ACPA0898
 40      CONTINUE                                                       ACPA0899
         CARRY=AINT(Z(1)/RMAX)                                          ACPA0900
         Z(2)=Z(2)-CARRY*RMAX                                           ACPA0901
         Z(1)=CARRY                                                     ACPA0902
         Z(L+1)=Z(L+1)+ONE                                              ACPA0903
      END IF                                                            ACPA0904
      Z(0)=ZERO                                                         ACPA0905
 50   CONTINUE                                                          ACPA0906
 60   DO 70 I=-1,L+1                                                    ACPA0907
         C(I)=Z(I)                                                      ACPA0908
 70   CONTINUE                                                          ACPA0909
      IF (C(1) .LT. HALF) THEN                                          ACPA0910
         C(-1)=ONE                                                      ACPA0911
         C(L+1)=ZERO                                                    ACPA0912
      ENDIF                                                             ACPA0913
      RETURN                                                            ACPA0914
      END                                                               ACPA0915
                                                                        ACPA0916
C     ****************************************************************  ACPA0917
C     *                                                              *  ACPA0918
C     *                 SUBROUTINE CMPADD                            *  ACPA0919
C     *                                                              *  ACPA0920
C     *                                                              *  ACPA0921
C     *  Description : Takes two arrays representing one real and    *  ACPA0922
C     *    one imaginary part, and adds two arrays representing      *  ACPA0923
C     *    another complex number and returns two array holding the  *  ACPA0924
C     *    complex sum.                                              *  ACPA0925
C     *              (CR,CI) = (AR+BR, AI+BI)                        *  ACPA0926
C     *                                                              *  ACPA0927
C     *  Subprograms called: ARADD                                   *  ACPA0928
C     *                                                              *  ACPA0929
C     ****************************************************************  ACPA0930
                                                                        ACPA0931
      SUBROUTINE CMPADD(AR,AI,BR,BI,CR,CI,WK1,L,RMAX)                   ACPA0932
                                                                        ACPA0933
      INTEGER L                                                         ACPA0934
      DOUBLE PRECISION AR,AI,BR,BI,CR,CI,RMAX,WK1                       ACPA0935
      DIMENSION AR(-1:*),AI(-1:*),BR(-1:*),BI(-1:*)                     ACPA0936
      DIMENSION CR(-1:*),CI(-1:*),WK1(-1:*)                             ACPA0937
                                                                        ACPA0938
      CALL ARADD(AR,BR,CR,WK1,L,RMAX)                                   ACPA0939
      CALL ARADD(AI,BI,CI,WK1,L,RMAX)                                   ACPA0940
      RETURN                                                            ACPA0941
      END                                                               ACPA0942
                                                                        ACPA0943
                                                                        ACPA0944
C     ****************************************************************  ACPA0945
C     *                                                              *  ACPA0946
C     *                 SUBROUTINE CMPSUB                            *  ACPA0947
C     *                                                              *  ACPA0948
C     *                                                              *  ACPA0949
C     *  Description : Takes two arrays representing one real and    *  ACPA0950
C     *    one imaginary part, and subtracts two arrays representing *  ACPA0951
C     *    another complex number and returns two array holding the  *  ACPA0952
C     *    complex sum.                                              *  ACPA0953
C     *              (CR,CI) = (AR+BR, AI+BI)                        *  ACPA0954
C     *                                                              *  ACPA0955
C     *  Subprograms called: ARADD                                   *  ACPA0956
C     *                                                              *  ACPA0957
C     ****************************************************************  ACPA0958
                                                                        ACPA0959
      SUBROUTINE CMPSUB(AR,AI,BR,BI,CR,CI,WK1,WK2,L,RMAX)               ACPA0960
                                                                        ACPA0961
      INTEGER L                                                         ACPA0962
      DOUBLE PRECISION AR,AI,BR,BI,CR,CI,RMAX,WK1,WK2                   ACPA0963
      DIMENSION AR(-1:*),AI(-1:*),BR(-1:*),BI(-1:*)                     ACPA0964
      DIMENSION CR(-1:*),CI(-1:*),WK1(-1:*),WK2(-1:*)                   ACPA0965
                                                                        ACPA0966
      CALL ARSUB(AR,BR,CR,WK1,WK2,L,RMAX)                               ACPA0967
      CALL ARSUB(AI,BI,CI,WK1,WK2,L,RMAX)                               ACPA0968
      RETURN                                                            ACPA0969
      END                                                               ACPA0970
                                                                        ACPA0971
                                                                        ACPA0972
C     ****************************************************************  ACPA0973
C     *                                                              *  ACPA0974
C     *                 SUBROUTINE CMPMUL                            *  ACPA0975
C     *                                                              *  ACPA0976
C     *                                                              *  ACPA0977
C     *  Description : Takes two arrays representing one real and    *  ACPA0978
C     *    one imaginary part, and multiplies it with two arrays     *  ACPA0979
C     *    representing another complex number and returns the       *  ACPA0980
C     *    complex product.                                          *  ACPA0981
C     *                                                              *  ACPA0982
C     *  Subprograms called: ARMULT, ARSUB, ARADD                    *  ACPA0983
C     *                                                              *  ACPA0984
C     ****************************************************************  ACPA0985
                                                                        ACPA0986
      SUBROUTINE CMPMUL(AR,AI,BR,BI,CR,CI,WK1,WK2,CR2,D1,D2,WK6,L,RMAX) ACPA0987
                                                                        ACPA0988
      INTEGER L,I                                                       ACPA0989
      DOUBLE PRECISION AR,AI,BR,BI,CR,CI,D1,D2,CR2,RMAX,WK1,WK2,WK6     ACPA0990
      DIMENSION AR(-1:*),AI(-1:*),CR(-1:*),CI(-1:*),WK1(-1:*),WK2(-1:*) ACPA0991
      DIMENSION CR2(-1:*),D1(-1:*),D2(-1:*)                             ACPA0992
                                                                        ACPA0993
      CALL ARMULT(AR,BR,D1,WK6,L,RMAX)                                  ACPA0994
      CALL ARMULT(AI,BI,D2,WK6,L,RMAX)                                  ACPA0995
      CALL ARSUB(D1,D2,CR2,WK1,WK2,L,RMAX)                              ACPA0996
      CALL ARMULT(AR,BI,D1,WK6,L,RMAX)                                  ACPA0997
      CALL ARMULT(AI,BR,D2,WK6,L,RMAX)                                  ACPA0998
      CALL ARADD(D1,D2,CI,WK1,L,RMAX)                                   ACPA0999
      DO 10 I=-1,L+1                                                    ACPA1000
         CR(I)=CR2(I)                                                   ACPA1001
 10   CONTINUE                                                          ACPA1002
      RETURN                                                            ACPA1003
      END                                                               ACPA1004
                                                                        ACPA1005
                                                                        ACPA1006
C     ****************************************************************  ACPA1007
C     *                                                              *  ACPA1008
C     *                 SUBROUTINE ARYDIV                            *  ACPA1009
C     *                                                              *  ACPA1010
C     *                                                              *  ACPA1011
C     *  Description : Returns the double precision complex number   *  ACPA1012
C     *    resulting from the division of four arrays, representing  *  ACPA1013
C     *    two complex numbers.  The number returned will be in one  *  ACPA1014
C     *    of two different forms:  either standard scientific or as *  ACPA1015
C     *    the log (base 10) of the number.                          *  ACPA1016
C     *                                                              *  ACPA1017
C     *  Subprograms called: CONV21, CONV12, EADD, ECPDIV, EMULT.    *  ACPA1018
C     *                                                              *  ACPA1019
C     ****************************************************************  ACPA1020
                                                                        ACPA1021
      SUBROUTINE ARYDIV(AR,AI,BR,BI,C,L,LNPFQ,RMAX,IBIT)                ACPA1022
                                                                        ACPA1023
      INTEGER L,IBIT,REXP,IR10,II10,LNPFQ,ITNMAX                        ACPA1024
      COMPLEX*16 C,CDUM                                                 ACPA1025
      DOUBLE PRECISION AR,AI,BR,BI,PHI,N1,N2,N3,E1,E2,E3,RR10,RI10,X    ACPA1026
      DOUBLE PRECISION AE,BE,X1,X2,DUM1,DUM2,CE,RMAX,TENMAX             ACPA1027
      DIMENSION AR(-1:*),AI(-1:*),BR(-1:*),BI(-1:*)                     ACPA1028
      DIMENSION AE(2,2),BE(2,2),CE(2,2)                                 ACPA1029
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1030
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1031
                                                                        ACPA1032
      REXP=IBIT/2                                                       ACPA1033
      X=REXP*(AR(L+1)-2)                                                ACPA1034
      RR10=X*LOG10(TWO)/LOG10(TEN)                                      ACPA1035
      IR10=INT(RR10)                                                    ACPA1036
      RR10=RR10-IR10                                                    ACPA1037
      X=REXP*(AI(L+1)-2)                                                ACPA1038
      RI10=X*LOG10(TWO)/LOG10(TEN)                                      ACPA1039
      II10=INT(RI10)                                                    ACPA1040
      RI10=RI10-II10                                                    ACPA1041
      DUM1=SIGN(AR(1)*RMAX*RMAX+AR(2)*RMAX+AR(3),AR(-1))                ACPA1042
      DUM2=SIGN(AI(1)*RMAX*RMAX+AI(2)*RMAX+AI(3),AI(-1))                ACPA1043
      DUM1=DUM1*10**RR10                                                ACPA1044
      DUM2=DUM2*10**RI10                                                ACPA1045
      CDUM=DCMPLX(DUM1,DUM2)                                            ACPA1046
      CALL CONV12(CDUM,AE)                                              ACPA1047
      AE(1,2)=AE(1,2)+IR10                                              ACPA1048
      AE(2,2)=AE(2,2)+II10                                              ACPA1049
      X=REXP*(BR(L+1)-2)                                                ACPA1050
      RR10=X*LOG10(TWO)/LOG10(TEN)                                      ACPA1051
      IR10=INT(RR10)                                                    ACPA1052
      RR10=RR10-IR10                                                    ACPA1053
      X=REXP*(BI(L+1)-2)                                                ACPA1054
      RI10=X*LOG10(TWO)/LOG10(TEN)                                      ACPA1055
      II10=INT(RI10)                                                    ACPA1056
      RI10=RI10-II10                                                    ACPA1057
      DUM1=SIGN(BR(1)*RMAX*RMAX+BR(2)*RMAX+BR(3),BR(-1))                ACPA1058
      DUM2=SIGN(BI(1)*RMAX*RMAX+BI(2)*RMAX+BI(3),BI(-1))                ACPA1059
      DUM1=DUM1*10**RR10                                                ACPA1060
      DUM2=DUM2*10**RI10                                                ACPA1061
      CDUM=DCMPLX(DUM1,DUM2)                                            ACPA1062
      CALL CONV12(CDUM,BE)                                              ACPA1063
      BE(1,2)=BE(1,2)+IR10                                              ACPA1064
      BE(2,2)=BE(2,2)+II10                                              ACPA1065
      CALL ECPDIV(AE,BE,CE)                                             ACPA1066
      IF (LNPFQ .EQ. 0) THEN                                            ACPA1067
         CALL CONV21(CE,C)                                              ACPA1068
      ELSE                                                              ACPA1069
         CALL EMULT(CE(1,1),CE(1,2),CE(1,1),CE(1,2),N1,E1)              ACPA1070
         CALL EMULT(CE(2,1),CE(2,2),CE(2,1),CE(2,2),N2,E2)              ACPA1071
         CALL EADD(N1,E1,N2,E2,N3,E3)                                   ACPA1072
         N1=CE(1,1)                                                     ACPA1073
         E1=CE(1,2)-CE(2,2)                                             ACPA1074
         X2=CE(2,1)                                                     ACPA1075
*                                                                       ACPA1076
*     TENMAX - MAXIMUM SIZE OF EXPONENT OF 10                           ACPA1077
*     THE FOLLOWING CODE CAN BE USED TO DETERMINE TENMAX, BUT IT        ACPA1078
*     WILL LIKELY GENERATE AN IEEE FLOATING POINT UNDERFLOW ERROR       ACPA1079
*     ON A SUN WORKSTATION.  REPLACE TENMAX WITH THE VALUE APPROPRIATE  ACPA1080
*     FOR YOUR MACHINE.                                                 ACPA1081
*                                                                       ACPA1082
         TENMAX = 320                                                   ACPA1083
         ITNMAX = 1                                                     ACPA1084
         DNUM   = 0.1D0                                                 ACPA1085
 10      ITNMAX = ITNMAX+1                                              ACPA1086
         DNUM = DNUM*0.1D0                                              ACPA1087
         IF (DNUM .GT. ZERO) GOTO 10                                    ACPA1088
         ITNMAX = ITNMAX-1                                              ACPA1089
         TENMAX = DBLE(ITNMAX)                                          ACPA1090
*                                                                       ACPA1091
         IF (E1 .GT. TENMAX) THEN                                       ACPA1092
            X1=TENMAX                                                   ACPA1093
         ELSEIF (E1 .LT. -TENMAX) THEN                                  ACPA1094
            X1=ZERO                                                     ACPA1095
         ELSE                                                           ACPA1096
            X1=N1*(TEN**E1)                                             ACPA1097
         END IF                                                         ACPA1098
         IF (X2 .NE. ZERO) THEN                                         ACPA1099
            PHI=ATAN2(X2,X1)                                            ACPA1100
         ELSE                                                           ACPA1101
            PHI = ZERO                                                  ACPA1102
         END IF                                                         ACPA1103
         C=DCMPLX(HALF*(LOG(N3)+E3*LOG(TEN)),PHI)                       ACPA1104
      ENDIF                                                             ACPA1105
      RETURN                                                            ACPA1106
      END                                                               ACPA1107
                                                                        ACPA1108
C     ****************************************************************  ACPA1109
C     *                                                              *  ACPA1110
C     *                 SUBROUTINE EMULT                             *  ACPA1111
C     *                                                              *  ACPA1112
C     *                                                              *  ACPA1113
C     *  Description : Takes one base and exponent and multiplies it *  ACPA1114
C     *    by another numbers base and exponent to give the product  *  ACPA1115
C     *    in the form of base and exponent.                         *  ACPA1116
C     *                                                              *  ACPA1117
C     *  Subprograms called: none                                    *  ACPA1118
C     *                                                              *  ACPA1119
C     ****************************************************************  ACPA1120
                                                                        ACPA1121
      SUBROUTINE EMULT(N1,E1,N2,E2,NF,EF)                               ACPA1122
                                                                        ACPA1123
      DOUBLE PRECISION N1,E1,N2,E2,NF,EF                                ACPA1124
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1125
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1126
                                                                        ACPA1127
      NF=N1*N2                                                          ACPA1128
      EF=E1+E2                                                          ACPA1129
      IF (ABS(NF) .GE. TEN) THEN                                        ACPA1130
         NF=NF/TEN                                                      ACPA1131
         EF=EF+ONE                                                      ACPA1132
      ENDIF                                                             ACPA1133
      RETURN                                                            ACPA1134
      END                                                               ACPA1135
                                                                        ACPA1136
                                                                        ACPA1137
C     ****************************************************************  ACPA1138
C     *                                                              *  ACPA1139
C     *                 SUBROUTINE EDIV                              *  ACPA1140
C     *                                                              *  ACPA1141
C     *                                                              *  ACPA1142
C     *  Description : returns the solution in the form of base and  *  ACPA1143
C     *    exponent of the division of two exponential numbers.      *  ACPA1144
C     *                                                              *  ACPA1145
C     *  Subprograms called: none                                    *  ACPA1146
C     *                                                              *  ACPA1147
C     ****************************************************************  ACPA1148
                                                                        ACPA1149
      SUBROUTINE EDIV(N1,E1,N2,E2,NF,EF)                                ACPA1150
                                                                        ACPA1151
      DOUBLE PRECISION N1,E1,N2,E2,NF,EF                                ACPA1152
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1153
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1154
                                                                        ACPA1155
      NF=N1/N2                                                          ACPA1156
      EF=E1-E2                                                          ACPA1157
      IF ((ABS(NF) .LT. ONE) .AND. (NF .NE. ZERO)) THEN                 ACPA1158
         NF=NF*TEN                                                      ACPA1159
         EF=EF-ONE                                                      ACPA1160
      ENDIF                                                             ACPA1161
      RETURN                                                            ACPA1162
      END                                                               ACPA1163
                                                                        ACPA1164
                                                                        ACPA1165
C     ****************************************************************  ACPA1166
C     *                                                              *  ACPA1167
C     *                 SUBROUTINE EADD                              *  ACPA1168
C     *                                                              *  ACPA1169
C     *                                                              *  ACPA1170
C     *  Description : Returns the sum of two numbers in the form    *  ACPA1171
C     *    of a base and an exponent.                                *  ACPA1172
C     *                                                              *  ACPA1173
C     *  Subprograms called: none                                    *  ACPA1174
C     *                                                              *  ACPA1175
C     ****************************************************************  ACPA1176
                                                                        ACPA1177
      SUBROUTINE EADD(N1,E1,N2,E2,NF,EF)                                ACPA1178
                                                                        ACPA1179
      DOUBLE PRECISION N1,E1,N2,E2,NF,EF,EDIFF                          ACPA1180
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1181
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1182
                                                                        ACPA1183
      EDIFF=E1-E2                                                       ACPA1184
      IF (EDIFF .GT. 36.0D0) THEN                                       ACPA1185
         NF=N1                                                          ACPA1186
         EF=E1                                                          ACPA1187
      ELSE IF (EDIFF .LT. -36.0D0) THEN                                 ACPA1188
         NF=N2                                                          ACPA1189
         EF=E2                                                          ACPA1190
      ELSE                                                              ACPA1191
         NF=N1*(TEN**EDIFF)+N2                                          ACPA1192
         EF=E2                                                          ACPA1193
 10      IF (ABS(NF) .LT. TEN) GOTO 20                                  ACPA1194
         NF=NF/TEN                                                      ACPA1195
         EF=EF+ONE                                                      ACPA1196
         GOTO 10                                                        ACPA1197
 20      IF ((ABS(NF) .GE. ONE) .OR. (NF .EQ. ZERO)) GOTO 30            ACPA1198
         NF=NF*TEN                                                      ACPA1199
         EF=EF-ONE                                                      ACPA1200
         GOTO 20                                                        ACPA1201
      ENDIF                                                             ACPA1202
 30   RETURN                                                            ACPA1203
      END                                                               ACPA1204
                                                                        ACPA1205
C     ****************************************************************  ACPA1206
C     *                                                              *  ACPA1207
C     *                 SUBROUTINE ESUB                              *  ACPA1208
C     *                                                              *  ACPA1209
C     *                                                              *  ACPA1210
C     *  Description : Returns the solution to the subtraction of    *  ACPA1211
C     *    two numbers in the form of base and exponent.             *  ACPA1212
C     *                                                              *  ACPA1213
C     *  Subprograms called: EADD                                    *  ACPA1214
C     *                                                              *  ACPA1215
C     ****************************************************************  ACPA1216
                                                                        ACPA1217
      SUBROUTINE ESUB(N1,E1,N2,E2,NF,EF)                                ACPA1218
                                                                        ACPA1219
      DOUBLE PRECISION N1,E1,N2,E2,NF,EF                                ACPA1220
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1221
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1222
                                                                        ACPA1223
      CALL EADD(N1,E1,N2*(-ONE),E2,NF,EF)                               ACPA1224
      RETURN                                                            ACPA1225
      END                                                               ACPA1226
                                                                        ACPA1227
                                                                        ACPA1228
C     ****************************************************************  ACPA1229
C     *                                                              *  ACPA1230
C     *                 SUBROUTINE CONV12                            *  ACPA1231
C     *                                                              *  ACPA1232
C     *                                                              *  ACPA1233
C     *  Description : Converts a number from complex notation to a  *  ACPA1234
C     *    form of a 2x2 real array.                                 *  ACPA1235
C     *                                                              *  ACPA1236
C     *  Subprograms called: none                                    *  ACPA1237
C     *                                                              *  ACPA1238
C     ****************************************************************  ACPA1239
                                                                        ACPA1240
      SUBROUTINE CONV12(CN,CAE)                                         ACPA1241
                                                                        ACPA1242
      COMPLEX*16 CN                                                     ACPA1243
      DOUBLE PRECISION CAE                                              ACPA1244
      DIMENSION CAE(2,2)                                                ACPA1245
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1246
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1247
                                                                        ACPA1248
      CAE(1,1)=DBLE(CN)                                                 ACPA1249
      CAE(1,2)=ZERO                                                     ACPA1250
 10   IF (ABS(CAE(1,1)) .LT. TEN) GOTO 20                               ACPA1251
      CAE(1,1)=CAE(1,1)/TEN                                             ACPA1252
      CAE(1,2)=CAE(1,2)+ONE                                             ACPA1253
      GOTO 10                                                           ACPA1254
 20   IF ((ABS(CAE(1,1)) .GE. ONE) .OR. (CAE(1,1) .EQ. ZERO))           ACPA1255
     :     GOTO 30                                                      ACPA1256
      CAE(1,1)=CAE(1,1)*TEN                                             ACPA1257
      CAE(1,2)=CAE(1,2)-ONE                                             ACPA1258
      GOTO 20                                                           ACPA1259
 30   CAE(2,1)=DIMAG(CN)                                                ACPA1260
      CAE(2,2)=ZERO                                                     ACPA1261
 40   IF (ABS(CAE(2,1)) .LT. TEN) GOTO 50                               ACPA1262
      CAE(2,1)=CAE(2,1)/TEN                                             ACPA1263
      CAE(2,2)=CAE(2,2)+ONE                                             ACPA1264
      GOTO 40                                                           ACPA1265
 50   IF ((ABS(CAE(2,1)) .GE. ONE) .OR. (CAE(2,1) .EQ. ZERO))           ACPA1266
     :     GOTO 60                                                      ACPA1267
      CAE(2,1)=CAE(2,1)*TEN                                             ACPA1268
      CAE(2,2)=CAE(2,2)-ONE                                             ACPA1269
      GOTO 50                                                           ACPA1270
 60   RETURN                                                            ACPA1271
      END                                                               ACPA1272
                                                                        ACPA1273
C     ****************************************************************  ACPA1274
C     *                                                              *  ACPA1275
C     *                 SUBROUTINE CONV21                            *  ACPA1276
C     *                                                              *  ACPA1277
C     *                                                              *  ACPA1278
C     *  Description : Converts a number represented in a 2x2 real   *  ACPA1279
C     *    array to the form of a complex number.                    *  ACPA1280
C     *                                                              *  ACPA1281
C     *  Subprograms called: none                                    *  ACPA1282
C     *                                                              *  ACPA1283
C     ****************************************************************  ACPA1284
                                                                        ACPA1285
      SUBROUTINE CONV21(CAE,CN)                                         ACPA1286
                                                                        ACPA1287
      DOUBLE PRECISION CAE,DNUM,TENMAX                                  ACPA1288
      INTEGER ITNMAX                                                    ACPA1289
      COMPLEX*16 CN                                                     ACPA1290
      DIMENSION CAE(2,2)                                                ACPA1291
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1292
      COMMON/IO/NOUT                                                    ACPA1293
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1294
*                                                                       ACPA1295
*     TENMAX - MAXIMUM SIZE OF EXPONENT OF 10                           ACPA1296
*                                                                       ACPA1297
      ITNMAX = 1                                                        ACPA1298
      DNUM   = 0.1D0                                                    ACPA1299
 1    ITNMAX = ITNMAX+1                                                 ACPA1300
      DNUM = DNUM*0.1D0                                                 ACPA1301
      IF (DNUM .GT. ZERO) GOTO 1                                        ACPA1302
      ITNMAX = ITNMAX-2                                                 ACPA1303
      TENMAX = DBLE(ITNMAX)                                             ACPA1304
*                                                                       ACPA1305
      IF (CAE(1,2) .GT. TENMAX .OR. CAE(2,2) .GT. TENMAX) THEN          ACPA1306
*     CN=DCMPLX(TENMAX,TENMAX)                                          ACPA1307
         WRITE (NOUT,300) ITNMAX                                        ACPA1308
         STOP                                                           ACPA1309
      ELSE IF (CAE(2,2) .LT. -TENMAX) THEN                              ACPA1310
         CN=DCMPLX(CAE(1,1)*(10**CAE(1,2)),ZERO)                        ACPA1311
      ELSE                                                              ACPA1312
         CN=DCMPLX(CAE(1,1)*(10**CAE(1,2)),CAE(2,1)*(10**CAE(2,2)))     ACPA1313
      ENDIF                                                             ACPA1314
      RETURN                                                            ACPA1315
 300  FORMAT (' ERROR - VALUE OF EXPONENT REQUIRED FOR SUMMATION',      ACPA1316
     :     ' WAS LARGER',/,' THAN THE MAXIMUM MACHINE EXPONENT ',       ACPA1317
     :     1I3,/,                                                       ACPA1318
     :     ' SUGGESTIONS:',/,' 1) RE-RUN USING LNPFQ=1.',/,             ACPA1319
     :     ' 2) IF YOU ARE USING A VAX, TRY USING THE',                 ACPA1320
     :     ' FORTRAN/G_FLOATING OPTION')                                ACPA1321
      END                                                               ACPA1322
                                                                        ACPA1323
                                                                        ACPA1324
C     ****************************************************************  ACPA1325
C     *                                                              *  ACPA1326
C     *                 SUBROUTINE ECPMUL                            *  ACPA1327
C     *                                                              *  ACPA1328
C     *                                                              *  ACPA1329
C     *  Description : Multiplies two numbers which are each         *  ACPA1330
C     *    represented in the form of a two by two array and returns *  ACPA1331
C     *    the solution in the same form.                            *  ACPA1332
C     *                                                              *  ACPA1333
C     *  Subprograms called: EMULT, ESUB, EADD                       *  ACPA1334
C     *                                                              *  ACPA1335
C     ****************************************************************  ACPA1336
                                                                        ACPA1337
      SUBROUTINE ECPMUL(A,B,C)                                          ACPA1338
                                                                        ACPA1339
      DOUBLE PRECISION A,B,C,N1,E1,N2,E2,C2                             ACPA1340
      DIMENSION A(2,2),B(2,2),C(2,2),C2(2,2)                            ACPA1341
                                                                        ACPA1342
      CALL EMULT(A(1,1),A(1,2),B(1,1),B(1,2),N1,E1)                     ACPA1343
      CALL EMULT(A(2,1),A(2,2),B(2,1),B(2,2),N2,E2)                     ACPA1344
      CALL ESUB(N1,E1,N2,E2,C2(1,1),C2(1,2))                            ACPA1345
      CALL EMULT(A(1,1),A(1,2),B(2,1),B(2,2),N1,E1)                     ACPA1346
      CALL EMULT(A(2,1),A(2,2),B(1,1),B(1,2),N2,E2)                     ACPA1347
      CALL EADD(N1,E1,N2,E2,C(2,1),C(2,2))                              ACPA1348
      C(1,1)=C2(1,1)                                                    ACPA1349
      C(1,2)=C2(1,2)                                                    ACPA1350
      RETURN                                                            ACPA1351
      END                                                               ACPA1352
                                                                        ACPA1353
                                                                        ACPA1354
C     ****************************************************************  ACPA1355
C     *                                                              *  ACPA1356
C     *                 SUBROUTINE ECPDIV                            *  ACPA1357
C     *                                                              *  ACPA1358
C     *                                                              *  ACPA1359
C     *  Description : Divides two numbers and returns the solution. *  ACPA1360
C     *    All numbers are represented by a 2x2 array.               *  ACPA1361
C     *                                                              *  ACPA1362
C     *  Subprograms called: EADD, ECPMUL, EDIV, EMULT               *  ACPA1363
C     *                                                              *  ACPA1364
C     ****************************************************************  ACPA1365
                                                                        ACPA1366
      SUBROUTINE ECPDIV(A,B,C)                                          ACPA1367
                                                                        ACPA1368
      DOUBLE PRECISION A,B,C,N1,E1,N2,E2,B2,N3,E3,C2                    ACPA1369
      DIMENSION A(2,2),B(2,2),C(2,2),B2(2,2),C2(2,2)                    ACPA1370
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1371
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1372
                                                                        ACPA1373
      B2(1,1)=B(1,1)                                                    ACPA1374
      B2(1,2)=B(1,2)                                                    ACPA1375
      B2(2,1)=-ONE*B(2,1)                                               ACPA1376
      B2(2,2)=B(2,2)                                                    ACPA1377
      CALL ECPMUL(A,B2,C2)                                              ACPA1378
      CALL EMULT(B(1,1),B(1,2),B(1,1),B(1,2),N1,E1)                     ACPA1379
      CALL EMULT(B(2,1),B(2,2),B(2,1),B(2,2),N2,E2)                     ACPA1380
      CALL EADD(N1,E1,N2,E2,N3,E3)                                      ACPA1381
      CALL EDIV(C2(1,1),C2(1,2),N3,E3,C(1,1),C(1,2))                    ACPA1382
      CALL EDIV(C2(2,1),C2(2,2),N3,E3,C(2,1),C(2,2))                    ACPA1383
      RETURN                                                            ACPA1384
      END                                                               ACPA1385
C     ****************************************************************  ACPA1386
C     *                                                              *  ACPA1387
C     *                   FUNCTION IPREMAX                           *  ACPA1388
C     *                                                              *  ACPA1389
C     *                                                              *  ACPA1390
C     *  Description : Predicts the maximum term in the pFq series   *  ACPA1391
C     *    via a simple scanning of arguments.                       *  ACPA1392
C     *                                                              *  ACPA1393
C     *  Subprograms called: none.                                   *  ACPA1394
C     *                                                              *  ACPA1395
C     ****************************************************************  ACPA1396
                                                                        ACPA1397
      FUNCTION IPREMAX(A,B,IP,IQ,Z)                                     ACPA1398
      COMPLEX*16 A,B,Z,FACTOR                                           ACPA1399
      INTEGER IP,IQ,J,IPREMAX                                           ACPA1400
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1401
      COMMON/IO/NOUT                                                    ACPA1402
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1403
      DOUBLE PRECISION EXPON,XL,XTERM,XMAX                              ACPA1404
      DIMENSION A(IP),B(IQ)                                             ACPA1405
*                                                                       ACPA1406
      XTERM=0                                                           ACPA1407
      DO 10 J=1,100000                                                  ACPA1408
*                                                                       ACPA1409
*     Estimate the exponent of the maximum term in the pFq series.      ACPA1410
*                                                                       ACPA1411
         EXPON=ZERO                                                     ACPA1412
         XL=DBLE(J)                                                     ACPA1413
         DO 20 I=1,IP                                                   ACPA1414
            EXPON=EXPON+DBLE(FACTOR(A(I)+XL-ONE))-DBLE(FACTOR(A(I)-ONE))ACPA1415
 20      CONTINUE                                                       ACPA1416
         DO 30 I=1,IQ                                                   ACPA1417
            EXPON=EXPON-DBLE(FACTOR(B(I)+XL-ONE))+DBLE(FACTOR(B(I)-ONE))ACPA1418
 30      CONTINUE                                                       ACPA1419
         EXPON = EXPON + XL*DBLE(LOG(Z)) - DBLE(FACTOR(DCMPLX(XL,ZERO)))ACPA1420
         XMAX=LOG10(EXP(ONE))*EXPON                                     ACPA1421
         IF ((XMAX .LT. XTERM) .AND. (J .GT. 2)) THEN                   ACPA1422
            IPREMAX=J                                                   ACPA1423
            RETURN                                                      ACPA1424
         END IF                                                         ACPA1425
         XTERM=MAX(XMAX,XTERM)                                          ACPA1426
 10   CONTINUE                                                          ACPA1427
      WRITE (NOUT,*) ' ERROR IN IPREMAX--DID NOT FIND MAXIMUM EXPONENT' ACPA1428
      STOP                                                              ACPA1429
      END                                                               ACPA1430
C     ****************************************************************  ACPA1431
C     *                                                              *  ACPA1432
C     *                   FUNCTION FACTOR                            *  ACPA1433
C     *                                                              *  ACPA1434
C     *                                                              *  ACPA1435
C     *  Description : This function is the log of the factorial.    *  ACPA1436
C     *                                                              *  ACPA1437
C     *  Subprograms called: none.                                   *  ACPA1438
C     *                                                              *  ACPA1439
C     ****************************************************************  ACPA1440
                                                                        ACPA1441
      FUNCTION FACTOR(Z)                                                ACPA1442
      COMPLEX*16 Z,FACTOR                                               ACPA1443
      DOUBLE PRECISION PI                                               ACPA1444
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1445
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1446
C                                                                       ACPA1447
      IF (((DBLE(Z) .EQ. ONE) .AND. (DIMAG(Z) .EQ. ZERO))               ACPA1448
     :     .OR. (ABS(Z) .EQ. ZERO)) THEN                                ACPA1449
         FACTOR=DCMPLX(ZERO,ZERO)                                       ACPA1450
         RETURN                                                         ACPA1451
      END IF                                                            ACPA1452
      PI=TWO*TWO*ATAN(ONE)                                              ACPA1453
      FACTOR=HALF*LOG(TWO*PI)+(Z+HALF)*LOG(Z)-Z+(ONE/(12.0D0*Z))*       ACPA1454
     :     (ONE-(ONE/(30.D0*Z*Z))*(ONE-(TWO/(7.0D0*Z*Z))))              ACPA1455
      RETURN                                                            ACPA1456
      END                                                               ACPA1457
C     ****************************************************************  ACPA1458
C     *                                                              *  ACPA1459
C     *                   FUNCTION CGAMMA                            *  ACPA1460
C     *                                                              *  ACPA1461
C     *                                                              *  ACPA1462
C     *  Description : Calculates the complex gamma function.  Based *  ACPA1463
C     *     on a program written by F.A. Parpia published in Computer*  ACPA1464
C     *     Physics Communications as the `GRASP2' program (public   *  ACPA1465
C     *     domain).                                                 *  ACPA1466
C     *                                                              *  ACPA1467
C     *                                                              *  ACPA1468
C     *  Subprograms called: none.                                   *  ACPA1469
C     *                                                              *  ACPA1470
C     ****************************************************************  ACPA1471
      FUNCTION CGAMMA (ARG,LNPFQ)                                       ACPA1472
*                                                                       ACPA1473
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)                               ACPA1474
      LOGICAL FIRST,NEGARG                                              ACPA1475
      INTEGER LNPFQ                                                     ACPA1476
      COMPLEX*16 CGAMMA,ARG                                             ACPA1477
*                                                                       ACPA1478
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1479
      DOUBLE PRECISION TENTH                                            ACPA1480
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1481
      COMMON/IO/NOUT                                                    ACPA1482
      DIMENSION FN(7),FD(7)                                             ACPA1483
*                                                                       ACPA1484
*----------------------------------------------------------------------*ACPA1485
*                                                                      *ACPA1486
*     THESE ARE THE BERNOULLI NUMBERS B02, B04, ..., B14, EXPRESSED AS *ACPA1487
*     RATIONAL NUMBERS. FROM ABRAMOWITZ AND STEGUN, P. 810.            *ACPA1488
*                                                                      *ACPA1489
      DATA FN/  1.0D 00,   -1.0D 00,    1.0D 00,                        ACPA1490
     :     -1.0D 00,    5.0D 00, -691.0D 00,                            ACPA1491
     :     7.0D 00/                                                     ACPA1492
      DATA FD/  6.0D 00,   30.0D 00,   42.0D 00,                        ACPA1493
     :     30.0D 00,   66.0D 00, 2730.0D 00,                            ACPA1494
     :     6.0D 00/                                                     ACPA1495
*                                                                       ACPA1496
*----------------------------------------------------------------------*ACPA1497
*                                                                       ACPA1498
      DATA HLNTPI/1.0D 00/                                              ACPA1499
*                                                                       ACPA1500
      DATA FIRST/.TRUE./                                                ACPA1501
*                                                                       ACPA1502
      DATA TENTH/0.1D 00/                                               ACPA1503
*                                                                       ACPA1504
      ARGR = DBLE(ARG)                                                  ACPA1505
      ARGI = DIMAG(ARG)                                                 ACPA1506
*                                                                       ACPA1507
*     ON THE FIRST ENTRY TO THIS ROUTINE, SET UP THE CONSTANTS REQUIRED ACPA1508
*     FOR THE REFLECTION FORMULA (CF. ABRAMOWITZ AND STEGUN 6.1.17) AND ACPA1509
*     STIRLING'S APPROXIMATION (CF. ABRAMOWITZ AND STEGUN 6.1.40).      ACPA1510
*                                                                       ACPA1511
      IF (FIRST) THEN                                                   ACPA1512
         PI= 4.0D0*ATAN(ONE)                                            ACPA1513
*                                                                       ACPA1514
*     SET THE MACHINE-DEPENDENT PARAMETERS:                             ACPA1515
*                                                                       ACPA1516
*     TENMAX - MAXIMUM SIZE OF EXPONENT OF 10                           ACPA1517
*                                                                       ACPA1518
         ITNMAX = 1                                                     ACPA1519
         DNUM   = TENTH                                                 ACPA1520
 10      ITNMAX = ITNMAX+1                                              ACPA1521
         DNUM = DNUM*TENTH                                              ACPA1522
         IF (DNUM .GT. ZERO) GOTO 10                                    ACPA1523
         ITNMAX = ITNMAX-1                                              ACPA1524
         TENMAX = DBLE (ITNMAX)                                         ACPA1525
*                                                                       ACPA1526
*     EXPMAX - MAXIMUM SIZE OF EXPONENT OF E                            ACPA1527
*                                                                       ACPA1528
         DNUM = TENTH**ITNMAX                                           ACPA1529
         EXPMAX = -LOG (DNUM)                                           ACPA1530
*                                                                       ACPA1531
*     PRECIS - MACHINE PRECISION                                        ACPA1532
*                                                                       ACPA1533
         PRECIS = ONE                                                   ACPA1534
 20      PRECIS = PRECIS/TWO                                            ACPA1535
         DNUM = PRECIS+ONE                                              ACPA1536
         IF (DNUM .GT. ONE) GOTO 20                                     ACPA1537
         PRECIS = TWO*PRECIS                                            ACPA1538
*                                                                       ACPA1539
         HLNTPI = HALF*LOG (TWO*PI)                                     ACPA1540
*                                                                       ACPA1541
         DO 30 I = 1,7                                                  ACPA1542
            FN(I) = FN(I)/FD(I)                                         ACPA1543
            TWOI = TWO*DBLE (I)                                         ACPA1544
            FN(I) = FN(I)/(TWOI*(TWOI-ONE))                             ACPA1545
 30      CONTINUE                                                       ACPA1546
*                                                                       ACPA1547
         FIRST = .FALSE.                                                ACPA1548
*                                                                       ACPA1549
      ENDIF                                                             ACPA1550
*                                                                       ACPA1551
*     CASES WHERE THE ARGUMENT IS REAL                                  ACPA1552
*                                                                       ACPA1553
      IF (ARGI .EQ. ZERO) THEN                                          ACPA1554
*                                                                       ACPA1555
*     CASES WHERE THE ARGUMENT IS REAL AND NEGATIVE                     ACPA1556
*                                                                       ACPA1557
         IF (ARGR .LE. ZERO) THEN                                       ACPA1558
*                                                                       ACPA1559
*     STOP WITH AN ERROR MESSAGE IF THE ARGUMENT IS TOO NEAR A POLE     ACPA1560
*                                                                       ACPA1561
            DIFF = ABS (DBLE (NINT (ARGR))-ARGR)                        ACPA1562
            IF (DIFF .LE. TWO*PRECIS) THEN                              ACPA1563
               WRITE (NOUT,300)                                         ACPA1564
               WRITE (NOUT,301) ARGR,ARGI                               ACPA1565
               STOP '010801'                                            ACPA1566
            ELSE                                                        ACPA1567
*                                                                       ACPA1568
*    OTHERWISE USE THE REFLECTION FORMULA (ABRAMOWITZ AND STEGUN 6.1.17)ACPA1569
*    TO ENSURE THAT THE ARGUMENT IS SUITABLE FOR STIRLING'S             ACPA1570
*    FORMULA                                                            ACPA1571
*                                                                       ACPA1572
               ARGUM = PI/(-ARGR*SIN (PI*ARGR))                         ACPA1573
               IF (ARGUM .LT. ZERO) THEN                                ACPA1574
                  ARGUM = -ARGUM                                        ACPA1575
                  CLNGI = PI                                            ACPA1576
               ELSE                                                     ACPA1577
                  CLNGI = ZERO                                          ACPA1578
               ENDIF                                                    ACPA1579
               FACNEG = LOG (ARGUM)                                     ACPA1580
               ARGUR = -ARGR                                            ACPA1581
               NEGARG = .TRUE.                                          ACPA1582
*                                                                       ACPA1583
            ENDIF                                                       ACPA1584
*                                                                       ACPA1585
*     CASES WHERE THE ARGUMENT IS REAL AND POSITIVE                     ACPA1586
*                                                                       ACPA1587
         ELSE                                                           ACPA1588
*                                                                       ACPA1589
            CLNGI = ZERO                                                ACPA1590
            ARGUR = ARGR                                                ACPA1591
            NEGARG = .FALSE.                                            ACPA1592
*                                                                       ACPA1593
         ENDIF                                                          ACPA1594
*                                                                       ACPA1595
*     USE ABRAMOWITZ AND STEGUN FORMULA 6.1.15 TO ENSURE THAT           ACPA1596
*     THE ARGUMENT IN STIRLING'S FORMULA IS GREATER THAN 10             ACPA1597
*                                                                       ACPA1598
         OVLFAC = ONE                                                   ACPA1599
 40      IF (ARGUR .LT. TEN) THEN                                       ACPA1600
            OVLFAC = OVLFAC*ARGUR                                       ACPA1601
            ARGUR = ARGUR+ONE                                           ACPA1602
            GOTO 40                                                     ACPA1603
         ENDIF                                                          ACPA1604
*                                                                       ACPA1605
*     NOW USE STIRLING'S FORMULA TO COMPUTE LOG (GAMMA (ARGUM))         ACPA1606
*                                                                       ACPA1607
         CLNGR = (ARGUR-HALF)*LOG (ARGUR)-ARGUR+HLNTPI                  ACPA1608
         FAC = ARGUR                                                    ACPA1609
         OBASQ = ONE/(ARGUR*ARGUR)                                      ACPA1610
         DO 50 I = 1,7                                                  ACPA1611
            FAC = FAC*OBASQ                                             ACPA1612
            CLNGR = CLNGR+FN(I)*FAC                                     ACPA1613
 50      CONTINUE                                                       ACPA1614
*                                                                       ACPA1615
*     INCLUDE THE CONTRIBUTIONS FROM THE RECURRENCE AND REFLECTION      ACPA1616
*     FORMULAE                                                          ACPA1617
*                                                                       ACPA1618
         CLNGR = CLNGR-LOG (OVLFAC)                                     ACPA1619
         IF (NEGARG) CLNGR = FACNEG-CLNGR                               ACPA1620
*                                                                       ACPA1621
      ELSE                                                              ACPA1622
*                                                                       ACPA1623
*     CASES WHERE THE ARGUMENT IS COMPLEX                               ACPA1624
*                                                                       ACPA1625
         ARGUR = ARGR                                                   ACPA1626
         ARGUI = ARGI                                                   ACPA1627
         ARGUI2 = ARGUI*ARGUI                                           ACPA1628
*                                                                       ACPA1629
*     USE THE RECURRENCE FORMULA (ABRAMOWITZ AND STEGUN 6.1.15)         ACPA1630
*     TO ENSURE THAT THE MAGNITUDE OF THE ARGUMENT IN STIRLING'S        ACPA1631
*     FORMULA IS GREATER THAN 10                                        ACPA1632
*                                                                       ACPA1633
         OVLFR = ONE                                                    ACPA1634
         OVLFI = ZERO                                                   ACPA1635
 60      ARGUM = SQRT (ARGUR*ARGUR+ARGUI2)                              ACPA1636
         IF (ARGUM .LT. TEN) THEN                                       ACPA1637
            TERMR = OVLFR*ARGUR-OVLFI*ARGUI                             ACPA1638
            TERMI = OVLFR*ARGUI+OVLFI*ARGUR                             ACPA1639
            OVLFR = TERMR                                               ACPA1640
            OVLFI = TERMI                                               ACPA1641
            ARGUR = ARGUR+ONE                                           ACPA1642
            GOTO 60                                                     ACPA1643
         ENDIF                                                          ACPA1644
*                                                                       ACPA1645
*     NOW USE STIRLING'S FORMULA TO COMPUTE LOG (GAMMA (ARGUM))         ACPA1646
*                                                                       ACPA1647
         ARGUR2 = ARGUR*ARGUR                                           ACPA1648
         TERMR = HALF*LOG (ARGUR2+ARGUI2)                               ACPA1649
         TERMI = ATAN2 (ARGUI,ARGUR)                                    ACPA1650
         CLNGR = (ARGUR-HALF)*TERMR-ARGUI*TERMI-ARGUR+HLNTPI            ACPA1651
         CLNGI = (ARGUR-HALF)*TERMI+ARGUI*TERMR-ARGUI                   ACPA1652
         FAC = (ARGUR2+ARGUI2)**(-2)                                    ACPA1653
         OBASQR = (ARGUR2-ARGUI2)*FAC                                   ACPA1654
         OBASQI = -TWO*ARGUR*ARGUI*FAC                                  ACPA1655
         ZFACR = ARGUR                                                  ACPA1656
         ZFACI = ARGUI                                                  ACPA1657
         DO 70 I = 1,7                                                  ACPA1658
            TERMR = ZFACR*OBASQR-ZFACI*OBASQI                           ACPA1659
            TERMI = ZFACR*OBASQI+ZFACI*OBASQR                           ACPA1660
            FAC = FN(I)                                                 ACPA1661
            CLNGR = CLNGR+TERMR*FAC                                     ACPA1662
            CLNGI = CLNGI+TERMI*FAC                                     ACPA1663
            ZFACR = TERMR                                               ACPA1664
            ZFACI = TERMI                                               ACPA1665
 70      CONTINUE                                                       ACPA1666
*                                                                       ACPA1667
*     ADD IN THE RELEVANT PIECES FROM THE RECURRENCE FORMULA            ACPA1668
*                                                                       ACPA1669
         CLNGR = CLNGR-HALF*LOG (OVLFR*OVLFR+OVLFI*OVLFI)               ACPA1670
         CLNGI = CLNGI-ATAN2 (OVLFI,OVLFR)                              ACPA1671
*                                                                       ACPA1672
      ENDIF                                                             ACPA1673
      IF (LNPFQ .EQ. 1) THEN                                            ACPA1674
         CGAMMA = DCMPLX(CLNGR,CLNGI)                                   ACPA1675
         RETURN                                                         ACPA1676
      END IF                                                            ACPA1677
*                                                                       ACPA1678
*     NOW EXPONENTIATE THE COMPLEX LOG GAMMA FUNCTION TO GET            ACPA1679
*     THE COMPLEX GAMMA FUNCTION                                        ACPA1680
*                                                                       ACPA1681
      IF ( (CLNGR .LE.  EXPMAX) .AND.                                   ACPA1682
     :     (CLNGR .GE. -EXPMAX) ) THEN                                  ACPA1683
         FAC = EXP (CLNGR)                                              ACPA1684
      ELSE                                                              ACPA1685
         WRITE (NOUT,300)                                               ACPA1686
         WRITE (NOUT,302) CLNGR                                         ACPA1687
         STOP '010802'                                                  ACPA1688
      ENDIF                                                             ACPA1689
      RESR = FAC*COS (CLNGI)                                            ACPA1690
      RESI = FAC*SIN (CLNGI)                                            ACPA1691
      CGAMMA = DCMPLX(RESR,RESI)                                        ACPA1692
*                                                                       ACPA1693
      RETURN                                                            ACPA1694
*                                                                       ACPA1695
 300  FORMAT (///' ***** ERROR IN SUBROUTINE CGAMMA *****')             ACPA1696
 301  FORMAT (' ARGUMENT (',1P,1D14.7,',',1D14.7,') TOO CLOSE TO A',    ACPA1697
     :     ' POLE.')                                                    ACPA1698
 302  FORMAT (' ARGUMENT TO EXPONENTIAL FUNCTION (',1P,1D14.7,          ACPA1699
     :     ') OUT OF RANGE.')                                           ACPA1700
*                                                                       ACPA1701
      END                                                               ACPA1702
                                                                        ACPA1703  
C     ****************************************************************  ACPA1704
C     *                                                              *  ACPA1705
C     *                 BLOCK DATA BLDAT1                            *  ACPA1706
C     *                                                              *  ACPA1707
C     *                                                              *  ACPA1708
C     *  Description : Sets of frequently used numbers in a common   *  ACPA1709
C     *    block.  This makes it easier to convert the code to a     *  ACPA1710
C     *    single precision version.                                 *  ACPA1711
C     *                                                              *  ACPA1712
C     ****************************************************************  ACPA1713
                                                                        ACPA1714
      BLOCK DATA BLDAT1                                                 ACPA1715
C                                                                       ACPA1716
      COMMON/CONSTS/ZERO,HALF,ONE,TWO,TEN,EPS                           ACPA1717
      DOUBLE PRECISION ZERO,HALF,ONE,TWO,TEN,EPS                        ACPA1718
      DATA ZERO,HALF,ONE,TWO,TEN,EPS/0.0D0,0.5D0,1.0D0,2.0D0,           ACPA1719
     :     10.0D0,1.0D-10/                                              ACPA1720
      END                                                               ACPA1721
                                                                        ACPA****