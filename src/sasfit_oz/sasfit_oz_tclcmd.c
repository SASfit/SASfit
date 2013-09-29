/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   modified by Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   27.9.2013
 *   Tcl Wrapper by Ingo Bressler (ingo.bressler@bam.de)
 *   29.09.2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sasfit_oz.h>

#define OZMAXPAR 16

typedef char * string;

//typedef double  OZ_pot (double, double *);

int assign_closure(char *token, sasfit_oz_data *OZD) {
    string ClosureNames[12];
    int i,eq;
    ClosureNames[0] = "Percus-Yevick";
    ClosureNames[1] = "PY";
    ClosureNames[2] = "hypernetted-chain";
    ClosureNames[3] = "HNC";
    ClosureNames[4] = "Rogers-Young";
    ClosureNames[5] = "RY";
    ClosureNames[6] = "Verlet";
    ClosureNames[7] = "V";
    ClosureNames[8] = "Martynov-Sarkisov";
    ClosureNames[9] = "MS";
    ClosureNames[10] = "Ballone-Pastore-Galli-Gazzillo";
    ClosureNames[11] = "BPGG";

    i=0;
    eq=-1;
    while (i<12 && eq != 0) {
        eq = strcmp(token,ClosureNames[i]);
        i++;
    }
    printf("%s %d\n",token,i-1);
//    PY, HNC, RY, Verlet, MS, BPGG
    switch (i-1) {
        case 0 :
            OZD->cl=PY;
            break;
        case 1 :
            OZD->cl=PY;
            break;
        case 2 :
            OZD->cl=HNC;
            break;
        case 3 :
            OZD->cl=HNC;
            break;
        case 4 :
            OZD->cl=RY;
            break;
        case 5 :
            OZD->cl=RY;
            break;
        case 6 :
            OZD->cl=Verlet;
            break;
        case 7 :
            OZD->cl=Verlet;
            break;
        case 8 :
            OZD->cl=MS;
            break;
        case 9 :
            OZD->cl=MS;
            break;
        case 10 :
            OZD->cl=BPGG;
            break;
        case 11 :
            OZD->cl=BPGG;
            break;
        default :
            OZD->cl=PY;
            return 0;
    }
    return 1;
}

int assign_pot(char *token, sasfit_oz_data *OZD) {
    string PotentialNames[12];
    int i,eq;
    PotentialNames[0] = "HardSphere";
    PotentialNames[1] = "HS";
    PotentialNames[2] = "StickyHardSphere";
    PotentialNames[3] = "SHS";
    PotentialNames[4] = "SoftSphere";
    PotentialNames[5] = "SS";
    PotentialNames[6] = "LennardJones";
    PotentialNames[7] = "LJ";
    PotentialNames[8] = "IonicMicrogel";
    PotentialNames[9] = "IM";
    PotentialNames[10] = "Depletion";
    PotentialNames[11] = "D";
    PotentialNames[12] = "LennardJones";
    PotentialNames[13] = "LJ";
    PotentialNames[13] = "DLVO";
    PotentialNames[14] = "PSM";
    PotentialNames[15] = "GGCM-n";

    i=0;
    eq=-1;
    while (i<12 && eq != 0) {
        eq = strcmp(token,PotentialNames[i]);
        i++;
    }
    printf("%s %d\n",token,i-1);
    switch (i-1) {
        case 0 :
            OZD->potential=&U_Hard_Sphere;
            break;
        case 1 :
            OZD->potential=&U_Hard_Sphere;
            break;
        case 2 :
            OZD->potential=&U_Sticky_Hard_Sphere;
            break;
        case 3 :
            OZD->potential=&U_Sticky_Hard_Sphere;
            break;
        case 4 :
            OZD->potential=&U_Soft_Sphere;
            break;
        case 5 :
            OZD->potential=&U_Soft_Sphere;
            break;
        case 6 :
            OZD->potential=&U_Lennard_Jones;
            break;
        case 7 :
            OZD->potential=&U_Lennard_Jones;
            break;
        case 8 :
            OZD->potential=&U_Ionic_Microgel;
            break;
        case 9 :
            OZD->potential=&U_Ionic_Microgel;
            break;
        case 10 :
            OZD->potential=&U_Depletion;
            break;
        case 11 :
            OZD->potential=&U_Depletion;
            break;
        case 12 :
            OZD->potential=&U_Lennard_Jones;
            break;
        case 13 :
            OZD->potential=&U_Lennard_Jones;
            break;
        case 14 :
            OZD->potential=&U_DLVO;
            break;
        case 15 :
            OZD->potential=&U_PSM;
            break;
        case 16 :
            OZD->potential=&U_GGCM_n;
            break;
        default :
            OZD->potential=&U_Hard_Sphere;
            return 0;
    }
    return 1;
}

int read_input(char *FN, sasfit_oz_data *OZD) {
   FILE *Fid;
   int i,sstatus;
   double dtmp;
   char sBuffer[1024], *token;
   Fid = fopen(FN,"r");
   if (Fid == NULL) {
      fprintf(stdout,"Parameter input file \"%s\" does not exists.\n",FN);
      return 1;
   }
   if (!feof(Fid)) {
        fgets(sBuffer,1024,Fid);
        if (sscanf(sBuffer,"%d",&OZD->Npoints)==0) {
            OZD->Npoints=4096;
            fprintf(stderr,"Grid length has been set to 4096.\n");
        } else {
            fprintf(stderr,"Grid length: %d.\n",OZD->Npoints);
        }
   } else {
        OZD->Npoints=4096;
        fprintf(stderr,"Grid length has been set to 4096.\n");
   }

   if (!feof(Fid)) {
        fgets(sBuffer,1024,Fid);
        if (sscanf(sBuffer,"%d",&OZD->maxsteps)==0) {
            OZD->maxsteps=1000;
            fprintf(stderr,"Maximum number of iterations has been set to 1000.\n");
        } else {
            fprintf(stderr,"Maximum number of iterations: %d.\n",OZD->maxsteps);
        }
   } else {
        OZD->maxsteps=1000;
        fprintf(stderr,"Maximum number of iterations has been set to 1000.\n");
   }

   if (!feof(Fid)) {
        fgets(sBuffer,1024,Fid);
        if (sscanf(sBuffer,"%lf",&OZD->relerror)==0) {
            OZD->relerror=1e-15;
            fprintf(stderr,"Relative iteration precision has been set to %g.\n",OZD->relerror);
        } else {
            fprintf(stderr,"Relative iteration precision: %g.\n",OZD->relerror);
        }
   } else {
        OZD->relerror=1e-15;
        fprintf(stderr,"Relative iteration precision has been set to %g.\n",OZD->relerror);
   }

   if (!feof(Fid)) {
        fgets(sBuffer,1024,Fid);
        if (sscanf(sBuffer,"%lf",&OZD->dr_dsigma)==0) {
            OZD->dr_dsigma=0.005;
            fprintf(stderr,"relative grid width will be set to %g.\n",OZD->dr_dsigma);
        } else {
            fprintf(stderr,"relative grid width: %g.\n",OZD->dr_dsigma);
        }
   } else {
        OZD->dr_dsigma=0.005;
        fprintf(stderr,"relative grid width will be set to %g.\n",OZD->dr_dsigma);
   }

   if (!feof(Fid)) {
        fgets(sBuffer,1024,Fid);
        if (sscanf(sBuffer,"%lf",&OZD->mixcoeff)==0) {
            OZD->mixcoeff=1;
            fprintf(stderr,"the mixing coefficient alpha for wighting new and old c(r) will be set to 1. c_next(r)=alpha*c_new(r)+(1-alpha)c_old(r)\n");
        } else {
            fprintf(stderr,"mixing coefficient alpha: %g.\n",OZD->mixcoeff);
        }
   } else {
        OZD->mixcoeff=1;
        fprintf(stderr,"the mixing coefficient alpha for wighting new and old c(r) will be set to 1. c_next(r)=alpha*c_new(r)+(1-alpha)c_old(r)\n");
   }

   if (!feof(Fid)) {
        fgets(sBuffer,1024,Fid);
        token = strtok(sBuffer," \t;:-");
        if (token==NULL) {
            sstatus=assign_closure("PY",OZD);
            fprintf(stderr,"set closure to=Percus-Yevick\n");
        } else {
            sstatus=assign_closure(token,OZD);
            if (sstatus == 1) {
                fprintf(stderr,"set closure to=%s\n",token);
            } else {
                fprintf(stderr,"set closure to=Percus-Yevick\n");
            }
        }
   } else {
        sstatus=assign_closure("PY",OZD);
        fprintf(stderr,"set closure to=Percus-Yevick\n");
   }

  if (!feof(Fid)) {
        fgets(sBuffer,1024,Fid);
        token = strtok(sBuffer," \t;:-");
        if (token==NULL) {
            sstatus=assign_pot("HS",OZD);
            fprintf(stderr,"set potential to=HardSphere\n");
        } else {
            sstatus=assign_pot(token,OZD);
            if (sstatus == 1) {
                fprintf(stderr,"set potential to=%s\n",token);
            } else {
                fprintf(stderr,"set potential to=HardSphere\n");
            }
        }
   } else {
        sstatus=assign_pot("HS",OZD);
        fprintf(stderr,"set potential to=HardSphere\n");
   }

   if (!feof(Fid)) {
        fgets(sBuffer,1024,Fid);
        token = strtok(sBuffer," \t;:-");
        if (token==NULL) {
            OZD->pPot[0]=1;
            fprintf(stderr,"set Ppot[0]=1\n");
        } else {
            i = 0;
            sstatus =1;
            while (token !=NULL && i < OZMAXPAR && sstatus > 0) {
                sstatus = sscanf(token,"%lf",&dtmp);
                fprintf(stderr,"pPot[%d]: %g.\n",i,dtmp);fflush(stdout);fflush(stderr);
                if (sstatus > 0) {
                    (OZD->pPot)[i]=dtmp;
                }
                i++;
                token = strtok(NULL," \t;:-");
            }
        }
   } else {
        OZD->pPot[0]=1.0;
        for (i=1;i<OZMAXPAR;i++) OZD->pPot[i]=0.0;
        fprintf(stderr,"set Ppot[0]=1\n");
   }
   OZD->dr=OZD->dr_dsigma*OZD->pPot[0];

   if (!feof(Fid)) {
        fgets(sBuffer,1024,Fid);
        if (sscanf(sBuffer,"%lf",&OZD->T)==0) {
            OZD->T=300.0;
            fprintf(stderr,"Temperature is set to 300 K.\n");
        } else {
            fprintf(stderr,"Temperature is %g K.\n",OZD->T);
        }
   } else {
        OZD->T=300;
        fprintf(stderr,"Temperature is set to 300 K.\n");
   }

   if (!feof(Fid)) {
        fgets(sBuffer,1024,Fid);
        if (sscanf(sBuffer,"%lf",&OZD->phi)==0) {
            OZD->phi=.3;
            fprintf(stderr,"Volume fraction is set to 0.3.\n");
        } else {
            fprintf(stderr,"Volume fraction is %g.\n",OZD->phi);
        }
   } else {
        OZD->phi=0.3;
        fprintf(stderr,"Volume fraction is set to 0.3.\n");
   }
   fclose(Fid);
   return 0;
}

void print_about(int argc, char *argv[]){
    int i;
    fprintf(stderr,"\nnumber of arguments is %d\n",argc);fflush(stderr);
    for (i=0;i<argc;i++) {
        fprintf(stderr,"argv[%d]: >%s<\n",i,argv[i]);fflush(stderr);
    }
    fprintf(stderr,"\nusage: sasfit_oz <input file> <output directory>\n");fflush(stderr);
    fprintf(stderr," \n");fflush(stderr);
    fprintf(stderr,"input file example:\n");fflush(stderr);
    fprintf(stderr,"===================\n");fflush(stderr);
    fprintf(stderr,"4096     // grid length\n");fflush(stderr);
    fprintf(stderr,"500      // maximum number of iterations\n");fflush(stderr);
    fprintf(stderr,"1e-12 	 // relative iteration precision\n");fflush(stderr);
    fprintf(stderr,"0.005 	 // relative grid width in fraction of the diameter pPot[0]\n");fflush(stderr);
    fprintf(stderr,"1        // mixing coefficient\n");fflush(stderr);
    fprintf(stderr,"PY       // PY, HNC, RY, Verlet, Martynov-Sarkisov, BPGG\n");fflush(stderr);
    fprintf(stderr,"HS       // Potential : HS, SHS, LJ, IM\n");fflush(stderr);
    fprintf(stderr,"1.0 1.1 1.2 1.3 1.4 1.5  // input parameter for potential pPot[]\n");fflush(stderr);
    fprintf(stderr,"300 	 // temperature in K\n");fflush(stderr);
    fprintf(stderr,"0.3 	 // volume fraction phi from which the particle number density rho is calculated as rho=6*phi/(pi*pPot[0]^3)\n\n");fflush(stderr);
}
int main(int argc, char *argv[])
{
   sasfit_oz_data OZD;
   double pPot[OZMAXPAR];
   char sBuffer[1024];
   int i,status;
   FILE *SQid, *CRid, *GRid, *URid;

   OZD.pPot=pPot;
   if (argc != 3) {
        print_about(argc,argv);
        return 0;
   }
   status = read_input(argv[1],&OZD);
   if (status == 1) {
        fprintf(stderr,"input file does not exists\n");
        print_about(argc,argv);
   }
   OZ_init(&OZD);
   OZ_calculation(&OZD);

   strcpy(sBuffer,"");
   strcat(sBuffer,argv[2]);
   strcat(sBuffer,"/S(Q).dat");
   SQid=fopen(sBuffer,"w");
   if (SQid==NULL) {
      fprintf(stderr,"Can not write data to %s\n",sBuffer);
      exit(EXIT_FAILURE);
   }

   strcpy(sBuffer,"");
   strcat(sBuffer,argv[2]);
   strcat(sBuffer,"/g(r).dat");
   GRid=fopen(sBuffer,"w");
   if (GRid==NULL) {
      fprintf(stderr,"Can not write data to %s\n",sBuffer);
      exit(EXIT_FAILURE);
   }

   strcpy(sBuffer,"");
   strcat(sBuffer,argv[2]);
   strcat(sBuffer,"/c(r).dat");
   CRid=fopen(sBuffer,"w");
   if (CRid==NULL) {
      fprintf(stderr,"Can not write data to %s\n",sBuffer);
      exit(EXIT_FAILURE);
   }

   strcpy(sBuffer,"");
   strcat(sBuffer,argv[2]);
   strcat(sBuffer,"/betaU(r).dat");
   URid=fopen(sBuffer,"w");
   if (URid==NULL) {
      fprintf(stderr,"Can not write data to %s\n",sBuffer);
      exit(EXIT_FAILURE);
   }

   fprintf(SQid,"0.0 %g\n", OZD.Sq0);
   fprintf(GRid,"0.0 %g\n", OZD.gr0);
   fprintf(CRid,"0.0 %g\n", OZD.cr0);

   i=0;
   for (i=0;i<OZD.Npoints;i++) {
        fprintf(SQid,"%g %g\n",OZD.k[i], OZD.S[i]);fflush(SQid);fflush(SQid);
        fprintf(GRid,"%g %g\n",OZD.r[i], OZD.g[i]);fflush(GRid);fflush(GRid);
        fprintf(CRid,"%g %g\n",OZD.r[i], OZD.c[i]);fflush(CRid);fflush(CRid);
        fprintf(URid,"%g %g\n",OZD.r[i], OZD.ubeta[i]);fflush(URid);fflush(URid);
   }
   fclose(SQid);
   fclose(CRid);
   fclose(GRid);
   fclose(URid);

   OZ_free(&OZD);
   return 0;
}

int sasfit_oz_calc_cmd(ClientData clientData,
                Tcl_Interp *interp,
                int objc,
                Tcl_Obj *CONST objv[])
{
        Tcl_SetObjResult(interp, Tcl_NewIntObj(sasfit_get_maxpar()));
        return TCL_OK;
}

