/*
 * src/sasfit_core/sf_phys_porod.c
 *
 * Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

/*
 * porod approximation/function: I(Q) = c0 + c4 * Q^(D-4)
 */

typedef real scalar;

typedef enum
{
	INT, REAL, COMPLEX, NONE
} sf_data_type;

typedef struct
{
	int len;
	const char * str;
} sf_string_t;

typedef struct
{
	const sf_string_t name;
	const sf_string_t displayName;
	const sf_string_t description;
} sf_descr_t;

// parameter meta data
typedef struct
{
	sf_data_type type; // or just byte count instead (sizeof) ?
	                   // complex ?
	int idx; // math. parameter index at the according function
	int offset; // offset within (void*) array (gpointer)
	sf_descr_t descr;
} sf_phys_component_info_t;

// static parameter info, once e.g. in a plugin
typedef struct
{ // missing: default value, upper/lower boundary
	sf_phys_component_info_t comp;
	sf_phys_f * part_deriv; // part. derivation pointer
} sf_phys_param_info_t;

// do we need this ?
// static result info
typedef struct
{
	sf_phys_component_info_t comp;
} sf_phys_result_info_t;

typedef struct
{
	const int len;
	// ordered by sf_phys_param_info_t->index
	// whole param struct is exported to Tcl -> GUI order may be different
	// zeiger auf array dieser structuren (continuous memory)
	// const data defined once in plugin
	const sf_phys_param_info_t * info;
	// access this via G_STRUCT_MEMBER()
	// dynamic data allocated before call, somehow
	gpointer * data; // void *, init NULL, may be > len
} sf_phys_param_t;

typedef struct
{
	const int len;
	const sf_phys_result_info_t * info;
	gpointer * data;
} sf_phys_result_t;

typedef struct
{
	sf_string_t msg;
	int code; // 0: no error, != 0: error
} sf_phys_error_t;

typedef struct
{
	sf_phys_param_t param;
	sf_phys_result_t result;
	sf_phys_error_t error;
} sf_phys_function_arg_t;

// covers all functions which receive the final input argument set
typedef struct
{
// goes here
//	sf_phys_f * f; // function pointer
//	int type; // SD, FF, SQ, PEAK `v
	const int type; // decides the function prototype
	gpointer f; // muss gecastet werden, alle versch
} sf_phys_function_helper_t;

typedef struct
{
	const int size;
	sf_phys_function_helper_t * list;
} sf_phys_function_helpers_t;

// problem: mapping gsl_vector -> (void*)[]
// jeder (void*) muss individuellen ableitbaren paramter darstellen
// beim fitten: int active[] // list der gefitteten parameter
//
// update in jedem fit-schritt:
// gsl_vector * fitdata;
// for(fi=ai=0; fi < fitdata->size; fi++)
// {
// 	// does not work for complex
// 	function_t->param[active[i]] = fitdata[i];
// 	function_t->param->info->comp.(type|offset|len)
// 	// instead, write continously to param[active[i]]
// }
//
// => es gibt spezielle info structuren für die einzelnen operationen:
// fitten: array mit activen variablen
// integrieren: einzelner index der zu integrierenden variable
// ... erlaubt erweiterung auf weitere operationen (2d-plotting zB)
//
// missing: unterscheidung versch. funktionstypen (integr. range zB)
//
// static descriptor for a single model/physical function

typedef struct
{
	sf_descr_t descr;
	sf_phys_function_helpers_t helpers;
	sf_phys_function_arg_t args;
} sf_phys_function_t;

typedef void sf_phys_f (sf_phys_function_arg_t * arg);

#define GET_PARAM(arg, idx, type) \
	*(type*) (((char*)arg->param.data) + arg->param.info[idx].comp.offset)
#define C0 GET_PARAM(arg, 0, double)
// does not work as ##define
switch(arg->param.info[(idx)].comp.type) {
case INT:
	*(int*) (((char*)arg->param.data) + arg->param.info[(idx)].comp.offset)
	break;
case COMPLEX:
		break;
case DOUBLE:
default:
*(double*) (((char*)arg->param.data) + arg->param.info[0].comp.offset)
		break;
}
arg->result.data
#define C0 CAST(arg->param->data[0])
#define RESULT r->value[0]

// - backend-parameter auswahl ?
//   -> performance-bzgl oldschool, 
//      denkbar: 2nd level abstraction via hashes (?)
void sf_phys_porod(f_phys_function_arg_t * arg)
{
	RESULT = C0 + p->q*p->c1 + p->c4*pow(p->q, p->d - 4.0);
}

// jede funktion(ableitung) einzeln ? 
// -> wegen performance: nicht alle parameter werden gefittet: nur die
//    benötigten ableitungen berechnen
// -> oder kombinierte funktion ?
//    analytische abl. oder numerisch (GSL), wenn nötig
//    -> form factoren ? ebenso, bisherige funktion wird gekapselt
//                              -> result-vektor
//
// kombinierte funktion, die auf anfrage berechnet, was gewünscht ist
//
// parameter-typen gemischt ? -> nicht immer ausschließlich float
//  -> parameter struktur ? minus usability, pro namensproblem

typedef struct
{
} fit_function_descr_t;

typedef struct
{
	const gsl_vector * y;
	const gsl_vector * sigma;
} data_t;

// automatisch, generalisiert
// in data muss die funktion enthalten sein
// in data muss zuordnung x->param von f stehen (nicht alle werden gefittet)
// in data müssen default params geliefert werden 
//    (gefittete werden überschrieben)
     int
     expb_f (const gsl_vector * x, void *data, 
             gsl_vector * f)
     {
       size_t n = ((struct data *)data)->n;
       double *y = ((struct data *)data)->y;
       double *sigma = ((struct data *) data)->sigma;
     
       double A = gsl_vector_get (x, 0);
       double lambda = gsl_vector_get (x, 1);
       double b = gsl_vector_get (x, 2);
     
       size_t i;
     
       for (i = 0; i < n; i++)
         {
           /* Model Yi = A * exp(-lambda * i) + b */
           double t = i;
           double Yi = A * exp (-lambda * t) + b;
           gsl_vector_set (f, i, (Yi - y[i])/sigma[i]); // zielfunktion f_i
         }
     
       return GSL_SUCCESS;
     }
// automatisch, generalisiert
// - automatische zuordnung von fit-variablen zu part.ableitungen ?
     int
     expb_df (const gsl_vector * x, void *data, 
              gsl_matrix * J)
     {
       size_t n = ((struct data *)data)->n;
       double *sigma = ((struct data *) data)->sigma;
     
       double A = gsl_vector_get (x, 0);
       double lambda = gsl_vector_get (x, 1);
     
       size_t i;
     
       // hängt nur von \sigma ab, nicht von data/yi
       for (i = 0; i < n; i++)
         {
           /* Jacobian matrix J(i,j) = dfi / dxj, */
           /* where fi = (Yi - yi)/sigma[i],      */
           /*       Yi = A * exp(-lambda * i) + b  */
           /* and the xj are the parameters (A,lambda,b) */
           double t = i;
           double s = sigma[i];
           double e = exp(-lambda * t);
           gsl_matrix_set (J, i, 0, e/s); // part. ableitungen d. zielfunktion
           gsl_matrix_set (J, i, 1, -t * A * e/s);
           gsl_matrix_set (J, i, 2, 1/s);
         }
       return GSL_SUCCESS;
     }

void Porod(Q,a,y,dyda,na,error,errstr)
float Q, a[], *y, dyda[];
int   na;
bool  *error;
char  *errstr;
{
   float c0, c1, c4, D;

   if (na != 4) {
      *error  = TRUE;
      sprintf(errstr,
              "Porod: needs three paramater in vector a[] but got %d",na);
      return;
   }
   c0 = a[0];
   c1 = a[1];
   c4 = a[2];
   D  = a[3];
   
   if (Q <= 0.0) {
      *error = TRUE;
      sprintf(errstr,
              "Porod: Q must be > 0; actual Q-value: %lg",Q);
      return;
   }

   *y = c0 + Q*c1 + c4*pow(Q,D-4.0);
   dyda[0] = 1.0;
   dyda[1] = Q;
   dyda[2] = pow(Q,D-4.0);
   dyda[3] = c4*pow(Q,D-4.0)*log(Q);
}

