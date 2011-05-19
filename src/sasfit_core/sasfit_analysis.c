#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include "include/sasfit_core.h"

#define SASFIT_ERROR 1
#define DBG 0

/** 
 * Creates a vector with according quadrature weights of linear spaced 
 * sampling points.
 * \param v Pointer to a gsl_vector which contains the sampling points.
 * \param w Pointer to a gsl_vector which contains the quadrature weights 
 *          for numerical integration over the created sampling points later.
 * \param start First value of the created range.
 * \param end   Last value of the created range.
 * \param num   Number of sampling points.
 */

int create_sampling_range_lin(gsl_vector ** v, gsl_vector ** w, 
                              double start, double end, size_t num)
{
	size_t i;
	double step, value;

	SASFIT_ASSERT_VAL(v && w && (num > 0), TCL_ERROR);

	*v = gsl_vector_alloc(num);
	*w = gsl_vector_alloc(num);
	SASFIT_ASSERT_VAL(*v && *w, TCL_ERROR);

	if (num == 1) {
		gsl_vector_set(*v, 0, start);
		gsl_vector_set(*w, 0, 1.0);
		return TCL_OK;
	}
	step = (end-start) / (double)(num-1);
	value = start;
	for(i=0; i < num; i++)
	{
		gsl_vector_set(*v, i, value);
		gsl_vector_set(*w, i, step);
		value += step;
	}
	return TCL_OK;
}

int create_range_log(gsl_vector ** v, 
                     double start, double end, size_t num)
{
	size_t i;
	double step, value;

	SASFIT_ASSERT_VAL(v && (start > 0.0) && (end > 0.0) && 
	                  (num > 0), TCL_ERROR);

	*v = gsl_vector_alloc(num);
	SASFIT_ASSERT_VAL(*v, TCL_ERROR);

	if (num == 1) {
		gsl_vector_set(*v, 0, start);
		return TCL_OK;
	}
	step = (log10(end)-log10(start)) / (double)(num-1);
	value = log10(start);
	for(i=0; i < num; i++)
	{
		gsl_vector_set(*v, i, pow(10, value));
		value += step;
	}
	return TCL_OK;
}

/**
 * Creates a sampling points vector with (gaussian) quadrature weights based
 * on the legendre polynomial.
 * \param v Pointer to a gsl_vector which contains the sampling points.
 * \param w Pointer to a gsl_vector which contains the quadrature weights
 *          for numerical integration over the created sampling points later.
 * \param start First value of the created range.
 * \param end   Last value of the created range.
 * \param num   Number of sampling points.
 */
/*
int create_sampling_range_legendre(gsl_vector ** v, gsl_vector ** w,
                                   double start, double end, size_t num)
{
	size_t i;
	size_t len = (num+1) >> 1;
	double scale, shift;
	double points[len];
	double weights[len];
//	double eps = 1e-10;

	SASFIT_ASSERT_VAL(v && w && (num > 0), TCL_ERROR);

	*v = gsl_vector_alloc(num);
	*w = gsl_vector_alloc(num);
	SASFIT_ASSERT_VAL(*v && *w, TCL_ERROR);

	if (num == 1) {
		gsl_vector_set(*v, 0, start);
		gsl_vector_set(*w, 0, 1.0);
		return TCL_OK;
	}

//	sasfit_int_gauss_legendre_tbl((int)num, points, weights, eps);
	printf("gauss legendre res:\n");
	for(i=0; i < len; i++)
	{
		printf("x[%d]: %g       \tw[%d]: %g\n", (int)i, points[i], (int)i, weights[i]);
	}

	scale = (end - start) * 0.5;
	shift = (end + start) * 0.5;
// for debugging
//	scale = 1.0;
//	shift = 0.0;
	for(i=0; i < len; i++)
	{
		gsl_vector_set(*v,       i, scale*points[len-1-i]*(-1.0) + shift);
		gsl_vector_set(*v, num-1-i, scale*points[len-1-i]        + shift);
		gsl_vector_set(*w,       i, scale*weights[len-1-i]);
		gsl_vector_set(*w, num-1-i, scale*weights[len-1-i]);
	}
	printf("gauss legendre res:\n");
	for(i=0; i < num; i++)
	{
		printf("x[%d]: %g       \tw[%d]: %g\n", (int)i, gsl_vector_get(*v,i), (int)i, gsl_vector_get(*w,i));
	}
	return TCL_OK;
}
*/

int vector_norm_l2(gsl_vector * v, double * val)
{
	size_t i;
	if (!v || !val) return TCL_ERROR;
	*val = 0.0;
	for(i=0; i < v->size; i++)
	{
		*val += gsl_vector_get(v, i) * gsl_vector_get(v, i);
	}
	*val = sqrt(*val);
	return TCL_OK;
}

int vector_integrate(const gsl_vector * x, const gsl_vector * w, 
                     const gsl_vector * y, double * val)
{
	size_t i;
	SASFIT_ASSERT_VAL(x && y && w && val && 
	                  (x->size == y->size), TCL_ERROR);
	*val = 0.0;
	// using trapeziod rule, x has to be ascending
	for(i=0; i < x->size-1 && i < w->size-1 && i < y->size-1; i++)
	{
//		width = gsl_vector_get(x, i+1) - gsl_vector_get(x, i);
//		area = width * gsl_vector_get(y, i); // rectangular
		// difference triangle
//		area += (gsl_vector_get(y, i+1) - gsl_vector_get(y, i)) * 
//		        0.5 * width;
//		*val += area;
		*val += gsl_vector_get(w, i) * gsl_vector_get(y, i);
	}
	return TCL_OK;
}

void get_rel_error(gsl_vector * a, const gsl_vector * b)
{
	int i;
	if (a->size != b->size) return;
	gsl_vector_sub(a, b);
	for(i=0; i < a->size; i++)
	{
		if (gsl_vector_get(b, i) != 0.0) {
			gsl_vector_set(a, i, gsl_vector_get(a, i) / gsl_vector_get(b, i));
		} else {
			gsl_vector_set(a, i, 0.0);
		}
	}
}

void print_matrix(const gsl_matrix * m, const char * prefix, int flag)
{
	size_t row, col;
	SASFIT_ASSERT_VOID(m);
	printf("%s\n",prefix);
	for(row=0; row < m->size1; row++) {
		if (flag != 2) { printf("%d: ",(int)row); }
		for(col=0; col < m->size2; col++)
		{
			if (flag == 1) {
				if (row == col) {
					printf("%lg ",gsl_matrix_get(m,row,col));
				}
			} else {
				printf("%lg ",gsl_matrix_get(m,row,col));
			}
		}
		if (flag == 2) printf(";");
		else printf("\n");
	}
}

void print_vector(const gsl_vector * v, const char * prefix)
{
	size_t i;
	SASFIT_ASSERT_VOID(v);
	printf("%s",prefix);
	for(i=0; i < v->size; i++) {
		printf("%lg ",gsl_vector_get(v,i));
	}
	printf("\n");
}

void print_vector_int(const gsl_vector_int * v, const char * prefix)
{
	size_t i;
	SASFIT_ASSERT_VOID(v);
	printf("%s",prefix);
	for(i=0; i < v->size; i++) {
		printf("%d ",gsl_vector_int_get(v,i));
	}
	printf("\n");
}

int matrix_properties(const gsl_matrix * a_m, const char * prefix)
{
	int err = TCL_OK;
	double min, max;
	gsl_eigen_symm_workspace * w = NULL;
	gsl_matrix * copy_m = NULL;
	gsl_vector * eigenval_v = NULL;

	//print_matrix(a_m, "system matrix: ", 0);
	gsl_matrix_minmax(a_m, &min, &max);
//	printf("%s min: %lg, max: %lg\n", prefix, min, max);

	if (a_m->size1 != a_m->size2) {
//		printf("matrix not square !\n");
		return 0;
	}

	// eigenvalue calculation
	if (!err) {
		w = gsl_eigen_symm_alloc(a_m->size1);
		copy_m = gsl_matrix_alloc(a_m->size1, a_m->size2);
		err = gsl_matrix_memcpy(copy_m, a_m);
		eigenval_v = gsl_vector_alloc(a_m->size1);
		if (!w || !copy_m || !eigenval_v) err = TCL_ERROR;
	}
	if (!err) err = gsl_eigen_symm(copy_m, eigenval_v, w);
	if (!err) {
		gsl_vector_minmax(eigenval_v, &min, &max);
		printf("%s Eigenwerte, min: %lg, max: %lg\n", prefix, min, max);
	}
	if (!err) err = sasfit_tcl_set_double(sasfit_env_get_interp(),
		                            "inv_data", "sys_eig_min", min);
	if (!err) err = sasfit_tcl_set_double(sasfit_env_get_interp(),
		                            "inv_data", "sys_eig_max", max);

	if (w) gsl_eigen_symm_free(w);
	if (copy_m) gsl_matrix_free(copy_m);
	if (eigenval_v) gsl_vector_free(eigenval_v);
	return 0;
}

void cleanup(sasfit_analytpar ** ap, scalar ** q, scalar ** y, scalar ** e, 
             scalar ** r, gsl_matrix ** m, gsl_vector ** v1, gsl_vector ** v2,
	     gsl_vector ** v3)
{
	if (ap) { sasfit_free_AP(ap); if (*ap) { *ap = NULL; } }

	if (q) { if (*q) { free_dvector(*q,0,0); *q = NULL; } }
	if (y) { if (*y) { free_dvector(*y,0,0); *y = NULL; } }
	if (e) { if (*e) { free_dvector(*e,0,0); *e = NULL; } }
	if (r) { if (*r) { free_dvector(*r,0,0); *r = NULL; } }
	
	if (m)  { if (*m)  { gsl_matrix_free(*m);  *m  = NULL; } }
	if (v1) { if (*v1) { gsl_vector_free(*v1); *v1 = NULL; } }
	if (v2) { if (*v2) { gsl_vector_free(*v2); *v2 = NULL; } }
	if (v3) { if (*v3) { gsl_vector_free(*v3); *v3 = NULL; } }
}

int tcl_result_add_type(Tcl_Interp * interp, Tcl_Obj ** res, int t)
{
	int err = TCL_OK;
	SASFIT_ASSERT_VAL(interp && res && (t >= 0), TCL_ERROR);
	if (!(*res)) {
		*res = Tcl_NewListObj(0, NULL);
	}
	SASFIT_ASSERT_VAL(*res, TCL_ERROR);

	err = Tcl_ListObjAppendElement(interp, *res, Tcl_NewIntObj(t));

	return err;
}

int tcl_result_add_list(Tcl_Interp * interp, Tcl_Obj ** res, gsl_vector * v)
{
	size_t i;
	int err = TCL_OK;
	double tempval;
	Tcl_Obj * new_list = NULL;
	SASFIT_ASSERT_VAL(interp && res && v, TCL_ERROR);
	if (!(*res)) {
		*res = Tcl_NewListObj(0, NULL);
	}
	SASFIT_ASSERT_VAL(*res, TCL_ERROR);

	new_list = Tcl_NewListObj(0, NULL);
	if (!new_list) return TCL_ERROR;
	for(i=0; !err && i < v->size; i++)
	{
		tempval = gsl_vector_get(v,i);
		err = Tcl_ListObjAppendElement(interp, new_list, Tcl_NewDoubleObj(tempval));
	}
	if (!err) err = Tcl_ListObjAppendElement(interp, *res, new_list);

	return err;
}

void get_test_distrib(Tcl_Interp * interp, gsl_vector ** d_v)
{
	Tcl_Obj * list_name = NULL;
	Tcl_Obj * list_obj = NULL;
	Tcl_Obj ** list = NULL;
	int obj_count, result;
	size_t i;
	double val;

	SASFIT_ASSERT_VOID(interp);
	SASFIT_ASSERT_VOID(d_v);
	
	list_name = Tcl_NewStringObj("::ana_test_distrib", -1);
	if (!list_name) return;
	list_obj  = Tcl_ObjGetVar2(interp, list_name, NULL, 0);
	if (!list_obj) return;
	result = Tcl_ListObjGetElements(interp, list_obj, &obj_count, &list);
	printf("distrib elem count: %d\n",obj_count);
	if (result != TCL_OK || obj_count <= 0) return;

	if (*d_v) gsl_vector_free(*d_v);
	*d_v = gsl_vector_alloc(obj_count);

	result = 0;
	for(i=0; i < (size_t)obj_count; i++)
	{
		result |= Tcl_GetDoubleFromObj(interp, list[i], &val);
		//sasfit_out("elem %d: %lg\n",i,val);
		gsl_vector_set(*d_v, i, val);
	}
	if (result != TCL_OK) {
		gsl_vector_free(*d_v);
		*d_v = NULL;
	}
}

void set_scattering_vector_range(scalar qmin, scalar qmax, 
                                 scalar ** q, scalar ** y, scalar ** e, scalar ** res, 
				 int * ndata)
{
	int i;
	SASFIT_ASSERT_VOID(q && *q && 
	                   y && *y && 
	                   e && *e && 
	                   res && *res && ndata);

	if ((*q)[0] >= qmin && (*q)[(*ndata)-1] <= qmax) return; // nothing to do

	// get the pointer of the lower boundary
	for(i=0; (*q)[i] < qmin && i < *ndata; i++);
	*q = &(*q)[i];
	*y = &(*y)[i];
	*e = &(*e)[i];
	*res = &(*res)[i];
	*ndata = *ndata - i;
	// get the upper boundary -> length of vectors
	for(i=0; (*q)[i] <= qmax && i < *ndata; i++);
	*ndata = i;
}

int get_distrib_idx(sasfit_analytpar * ap, int num_ap, gsl_vector_int * idx_v)
{
	size_t i, k;
	SASFIT_ASSERT_VAL(ap && idx_v, TCL_ERROR);
	// for each scattering contribution
	for(i=0; i < (size_t)num_ap; i++)
	{
		gsl_vector_int_set(idx_v, i, -1);
		// for all parameters
		for(k=0; k < MAXPAR; k++)
		{
			// check if parameter is marked for distribution
			if (ap[i].FF_distr[k] > 0)
			{
				// check for two distribution parameters in
				// this contribution
				if (gsl_vector_int_get(idx_v, i) > -1)
				{
					sasfit_err("Contribution %u has another"
					  " distribution parameter (%u)!\n",i,k);
					return TCL_ERROR;
				} else {
					gsl_vector_int_set(idx_v, i, k);
				}
			}
		}
	}
	return TCL_OK;
}

int reg_scale_input(gsl_matrix * a_m, 
                    gsl_vector * y_v)
{
	double min, max;
	double minexp, maxexp, avgexp;
	double factor;
	int exp, err = TCL_OK;

	SASFIT_ASSERT_VAL(a_m, TCL_ERROR);

	gsl_matrix_minmax(a_m, &min, &max);
	minexp = log10(fabs(min));
	maxexp = log10(fabs(max));
	avgexp = fabs(maxexp-minexp)/2.0;

	exp = (int)(floor((-1.0*minexp)-avgexp+0.5));
	factor = gsl_pow_int(10.0, exp);

	if (!err) err = gsl_matrix_scale(a_m, factor);
//	print_matrix(a_m, "matrix after scale: ", 0);
//	printf("input scale exponent: %d\n", exp);
//	printf("[before] min: %lg, max: %lg\n",min,max);
	gsl_matrix_minmax(a_m, &min, &max);
//	printf("[after] min: %lg, max: %lg\n",min,max);

	if (!err && y_v) err = gsl_vector_scale(y_v, factor);
	return err;
}

int reg_prepare_givens(const gsl_matrix * a_m, // design matrix
                       const gsl_vector * y_v, // measured data
                       const gsl_vector * e_v, // error vector
		             gsl_vector * b_v) // resulting combined vector
{
	int err = 0;
	size_t i;
	gsl_vector * ye_v = NULL;
	double val;

	SASFIT_ASSERT_VAL(a_m && y_v && e_v && b_v, TCL_ERROR);
	SASFIT_ASSERT_VAL(y_v->size == a_m->size1, TCL_ERROR);
	SASFIT_ASSERT_VAL(b_v->size == a_m->size2, TCL_ERROR);
	SASFIT_ASSERT_VAL(y_v->size == e_v->size, TCL_ERROR);

	ye_v = gsl_vector_alloc(y_v->size);
	if (!ye_v) err = 1;

	if (!err) {
//		printf("before y[0]:\t %lg, y[end]: %lg\n",gsl_vector_get(y_v,0),gsl_vector_get(y_v,y_v->size-1));
//		printf("before e[0]:\t %lg, e[end]: %lg\n",gsl_vector_get(e_v,0),gsl_vector_get(e_v,e_v->size-1));
		for(i=0; i < y_v->size; i++)
		{
			val = gsl_vector_get(y_v, i) / 
				(gsl_vector_get(e_v, i) * gsl_vector_get(e_v, i));
			gsl_vector_set(ye_v, i, val);
		}
//		printf("after ye[0]:\t %lg, ye[end]: %lg\n",gsl_vector_get(ye_v,0),gsl_vector_get(ye_v,ye_v->size-1));

		err = gsl_blas_dgemv(CblasTrans, 1.0, a_m, ye_v, 0.0, b_v); // A^T y = b
	}
	if (ye_v) gsl_vector_free(ye_v);
	return err;
}

// size is distribution vector size (number of columns)
// 1 -2  1  0  0  0
// 0  1 -2  1  0  0
// 0  0  1 -2  1  0
// 0  0  0  1 -2  1
int matrix_set_2nd_derivative(gsl_matrix ** l_m, int cols)
{
	size_t row;
	SASFIT_ASSERT_VAL(l_m && cols > 2, TCL_ERROR);
	if (*l_m) gsl_matrix_free(*l_m);
	*l_m = gsl_matrix_calloc((size_t)cols-2, (size_t)cols);
	SASFIT_ASSERT_VAL(*l_m, TCL_ERROR);
	for(row=0; row < (*l_m)->size1 ;row++)
	{
		gsl_matrix_set(*l_m, row, row,    1.0);
		gsl_matrix_set(*l_m, row, row+1, -2.0);
		gsl_matrix_set(*l_m, row, row+2,  1.0);
	}
	return 0;
}

// size is distribution vector size (number of columns)
//  1 -1  0  0  0
//  0  1 -1  0  0
//  0  0  1 -1  0
//  0  0  0  1 -1
int matrix_set_1st_derivative(gsl_matrix ** l_m, int cols)
{
	size_t row;
	SASFIT_ASSERT_VAL(l_m && cols > 1, TCL_ERROR);
	if (*l_m) gsl_matrix_free(*l_m);
	*l_m = gsl_matrix_calloc((size_t)cols-1, (size_t)cols);
	SASFIT_ASSERT_VAL(*l_m, TCL_ERROR);
	for(row=0; row < (*l_m)->size1 ;row++)
	{
		gsl_matrix_set(*l_m, row, row,    1.0);
		gsl_matrix_set(*l_m, row, row+1, -1.0);
	}
	return 0;
}

// size is distribution vector size (number of columns)
//  1  0  0  0
//  0  1  0  0
//  0  0  1  0
//  0  0  0  1
int matrix_set_identity(gsl_matrix ** l_m, int cols)
{
	SASFIT_ASSERT_VAL(l_m && cols > 1, TCL_ERROR);
	if (*l_m) gsl_matrix_free(*l_m);
	*l_m = gsl_matrix_alloc((size_t)cols-1, (size_t)cols);
	SASFIT_ASSERT_VAL(*l_m, TCL_ERROR);
	gsl_matrix_set_identity(*l_m);
	return 0;
}

int reg_prepare_constraint_matrix(gsl_matrix * l_m)
{
	int err = 0;
	gsl_matrix * smooth_m = NULL;

	SASFIT_ASSERT_VAL(l_m, TCL_ERROR);
	SASFIT_ASSERT_VAL(l_m->size1 == l_m->size2, TCL_ERROR);

	err = matrix_set_2nd_derivative(&smooth_m, l_m->size2);
//	err = matrix_set_1st_derivative(&smooth_m, l_m->size2);
//	err = matrix_set_identity(&smooth_m, l_m->size2);
	if (!err) {
		// multiply the smoothing functional matrix by itself: L^T L
		err = gsl_blas_dgemm(CblasTrans, CblasNoTrans, 
		                     1.0, smooth_m, smooth_m, 0.0, l_m);
	}
	gsl_matrix_free(smooth_m);
	return err;
}

int matrix_product(const gsl_matrix * in_m, gsl_matrix * out_m)
{
	int err = GSL_SUCCESS;
	size_t col_fixed, col_cur;
	double sum;
	
	if ( !in_m || !out_m)
		GSL_ERROR("Invalid data pointers!", GSL_EFAULT);
	if ( out_m->size1 != out_m->size2 )
		GSL_ERROR("Output matrix not square!", GSL_ENOTSQR);
	if ( in_m->size2 != out_m->size1 )
		GSL_ERROR("Input/output matrix dimensions do not match!", GSL_EBADLEN);

	// process the lower triangular matrix only
	for(col_fixed=0; col_fixed < in_m->size2; col_fixed++) 
	{
		gsl_vector_const_view cv_fixed = gsl_matrix_const_column(in_m, col_fixed);
		for(col_cur=0; col_cur < col_fixed; col_cur++)
		{
			// copy identical elements
			gsl_matrix_set(out_m, col_cur, col_fixed, 
				gsl_matrix_get(out_m, col_fixed, col_cur));
		}
		for(col_cur=col_fixed; col_cur < in_m->size2; col_cur++)
		{
			gsl_vector_const_view cv_cur = gsl_matrix_const_column(in_m, col_cur);
			gsl_blas_ddot(&cv_fixed.vector, &cv_cur.vector, &sum);
			gsl_matrix_set(out_m, col_cur, col_fixed, sum);
		}
	}
	return err;
}

int reg_prepare_design_matrix(const gsl_matrix * in_m, 
                              const gsl_vector * e_v, 
                                    gsl_matrix * out_m)
{
	int err = GSL_SUCCESS;
	size_t col;
	gsl_matrix * incopy_m = NULL;
	gsl_vector * ecopy_v = NULL;

	if ( !in_m || !e_v || !out_m)
		GSL_ERROR("Invalid data pointers!", GSL_EFAULT);
	if ( in_m->size2 != out_m->size1 )
		GSL_ERROR("Input/output matrix dimensions do not match!", GSL_EBADLEN);
	if ( out_m->size1 != out_m->size2 )
		GSL_ERROR("Output matrix not square!", GSL_ENOTSQR);
	if ( in_m->size1 != e_v->size )
		GSL_ERROR("Matrix size and vector length do not match!", GSL_EBADLEN);

	incopy_m = gsl_matrix_alloc(in_m->size1, in_m->size2);
	if (!incopy_m) GSL_ERROR("Got no space for temp matrix!", GSL_ENOMEM);
	err = gsl_matrix_memcpy(incopy_m, in_m);
	if (err) GSL_ERROR("Could not copy matrix!", GSL_EFAULT);

	// copy and square the error vector
	ecopy_v = gsl_vector_alloc(e_v->size);
	if (!ecopy_v) GSL_ERROR("Got no space for temp vector!", GSL_ENOMEM);
	err = gsl_vector_memcpy(ecopy_v, e_v);
	if (err) GSL_ERROR("Could not copy vector!", GSL_EFAULT);
	
	err = gsl_vector_mul(ecopy_v, e_v); // square the error vector
	if (err) GSL_ERROR("Could not square error vector!", GSL_EFAULT);

	// divide each column of the design matrix by the (square) elements 
	// of the error vector
	for(col=0; !err && col < incopy_m->size2; col++)
	{
		gsl_vector_view column_view = gsl_matrix_column(incopy_m, col);
		err = gsl_vector_div(&column_view.vector, ecopy_v);
	}
	if (err) GSL_ERROR("Error while matrix division by error value!", GSL_EFAULT);

	// one of the matrices has the error applied to
	if (!err) { // A^T A
		err = gsl_blas_dgemm(CblasTrans, CblasNoTrans, 
		                     1.0, incopy_m, incopy_m, 0.0, out_m);
	}

	gsl_matrix_free(incopy_m);
	gsl_vector_free(ecopy_v);
	return err;
}

// compute Ax - y / e
int reg_error(const gsl_matrix * a_m, const gsl_vector * y_v, 
              const gsl_vector * e_v, const gsl_vector * x_v,
	            gsl_vector * ax_v)
{
	int err = 0;
	size_t i;
	double val;

	SASFIT_ASSERT_VAL(a_m && y_v && e_v && x_v && ax_v, TCL_ERROR);
	SASFIT_ASSERT_VAL(y_v->size == a_m->size1, TCL_ERROR);
	SASFIT_ASSERT_VAL(x_v->size == a_m->size2, TCL_ERROR);
	SASFIT_ASSERT_VAL(y_v->size == e_v->size, TCL_ERROR);
	SASFIT_ASSERT_VAL(y_v->size == ax_v->size, TCL_ERROR);

	if (!err) err = gsl_vector_memcpy(ax_v, y_v);
	if (!err) {
		err = gsl_blas_dgemv(CblasNoTrans, 1.0, a_m, x_v, -1.0, ax_v); // A x = ax
	}

	if (!err) {
		for(i=0; i < ax_v->size; i++)
		{
			val = gsl_vector_get(ax_v, i) / gsl_vector_get(e_v, i);
			gsl_vector_set(ax_v, i, val);
		}
	}
	return err;
}

int compatible(const gsl_matrix * a_m,
               const gsl_vector * y_v,
                     gsl_vector * x_v)
{
	SASFIT_ASSERT_VAL(a_m && y_v && x_v, TCL_ERROR);
	SASFIT_ASSERT_VAL(a_m->size1 == y_v->size, TCL_ERROR);
	SASFIT_ASSERT_VAL(a_m->size2 == x_v->size, TCL_ERROR);
	return TCL_OK;
}

int solve_sys_chol(const gsl_matrix * a_m, 
                   const gsl_vector * y_v, 
                         gsl_vector * x_v)
{
	int err = TCL_OK;
	gsl_matrix * chol_m = NULL;

	SASFIT_ASSERT_VAL(!compatible(a_m, y_v, x_v), TCL_ERROR);

	if (!err) {
		chol_m  = gsl_matrix_alloc(a_m->size1, a_m->size2);
		err   = gsl_matrix_memcpy(chol_m, a_m);
		if (!chol_m) err = TCL_ERROR;
	}

	// solve the equation system by Cholesky decomposition
	if (!err) err = gsl_linalg_cholesky_decomp(chol_m);
	if (!err) err = gsl_linalg_cholesky_solve(chol_m, y_v, x_v);
	if (err) {
		print_matrix(a_m, "Cholesky input matrix: ", 2);
		matrix_properties(a_m, "Cholesky input matrix: ");
	}

	if (chol_m)  gsl_matrix_free(chol_m);
	return err;
}

int solve_sys_lu(const gsl_matrix * a_m, 
                 const gsl_vector * y_v, 
                       gsl_vector * x_v)
{
	int err = TCL_OK;
	int signum;
	gsl_vector * res_v = NULL; // residuals
	gsl_matrix * lu_m = NULL;
	gsl_permutation * p = NULL;

	SASFIT_ASSERT_VAL(!compatible(a_m, y_v, x_v), TCL_ERROR);

	if (!err) {
		res_v = gsl_vector_alloc(y_v->size);
		lu_m  = gsl_matrix_alloc(a_m->size1, a_m->size2);
		err   = gsl_matrix_memcpy(lu_m, a_m);
		p     = gsl_permutation_alloc(a_m->size1);
		if (!res_v || !lu_m || !p) err = TCL_ERROR;
	}

	// solve the equation system by LU decomposition
	if (!err) err = gsl_linalg_LU_decomp(lu_m, p, &signum);
	if (!err) err = gsl_linalg_LU_solve(lu_m, p, y_v, x_v);
	// refinement
	if (!err) {
		err = gsl_linalg_LU_refine(a_m, lu_m, p, y_v, x_v, res_v);
//		print_vector(res_v, "residuals: ");
//		printf("residuals norm: %lg\n", gsl_blas_dnrm2(res_v));
	}

	if (res_v) gsl_vector_free(res_v);
	if (lu_m)  gsl_matrix_free(lu_m);
	if (p)     gsl_permutation_free(p);
	return err;
}

int solve_sys_qr(const gsl_matrix * a_m, 
                 const gsl_vector * y_v, 
                       gsl_vector * x_v)
{
	int err = TCL_OK;
	int signum;
	gsl_vector * tau_v = NULL; // residuals
	gsl_vector * w_v = NULL;
	gsl_matrix * qr_m = NULL;
	gsl_permutation * p = NULL;
	gsl_matrix * test_m = NULL;

	SASFIT_ASSERT_VAL(!compatible(a_m, y_v, x_v), TCL_ERROR);

	if (!err) {
		tau_v = gsl_vector_alloc( min(a_m->size1, a_m->size2) );
		w_v   = gsl_vector_alloc(a_m->size2);
		qr_m  = gsl_matrix_alloc(a_m->size1, a_m->size2);
		test_m = gsl_matrix_alloc(a_m->size1, a_m->size2);
		gsl_matrix_set_identity(test_m);
		p     = gsl_permutation_alloc(a_m->size2);
		err   = gsl_matrix_memcpy(qr_m, a_m);
		if (!tau_v || !qr_m || !p) err = TCL_ERROR;
	}

	// solve the equation system by QR decomposition
	if (!err) err = gsl_linalg_QRPT_decomp(qr_m, tau_v, p, &signum, w_v);
	if (!err) err = gsl_linalg_QRPT_solve(qr_m, tau_v, p, y_v, x_v);

	if (tau_v) gsl_vector_free(tau_v);
	if (w_v)   gsl_vector_free(w_v);
	if (qr_m)  gsl_matrix_free(qr_m);
	if (p)     gsl_permutation_free(p);
	return err;
}

int solve_sys_hh(const gsl_matrix * a_m, 
                 const gsl_vector * y_v, 
                       gsl_vector * x_v)
{
	int err = TCL_OK;
	gsl_matrix * tmp_m = NULL;

	SASFIT_ASSERT_VAL(!compatible(a_m, y_v, x_v), TCL_ERROR);

	if (!err) {
		tmp_m = gsl_matrix_alloc(a_m->size1, a_m->size2);
		err   = gsl_matrix_memcpy(tmp_m, a_m);
		if (!tmp_m) err = TCL_ERROR;
	}

	// solve the equation system by HouseHolder transformations
	if (!err) err = gsl_linalg_HH_solve(tmp_m, y_v, x_v);

	if (tmp_m)  gsl_matrix_free(tmp_m);
	return err;
}

int solve_sys_svd(const gsl_matrix * a_m,
                  const gsl_vector * y_v,
                        gsl_vector * x_v)
{
	int err = TCL_OK;
	size_t i;
	gsl_vector * s_v = NULL;
	gsl_vector * work_v = NULL;
	gsl_matrix * u_m = NULL;
	gsl_matrix * v_m = NULL;

	SASFIT_ASSERT_VAL(!compatible(a_m, y_v, x_v), TCL_ERROR);

	if (!err) {
		s_v = gsl_vector_alloc(a_m->size2);
		work_v = gsl_vector_alloc(a_m->size2);
		u_m = gsl_matrix_alloc(a_m->size1, a_m->size2);
		v_m = gsl_matrix_alloc(a_m->size2, a_m->size2);
		if (!s_v || !work_v || !u_m || !v_m) err = TCL_ERROR;
	}
	if (!err) err = gsl_matrix_memcpy(u_m, a_m);

//	if (!err) err = gsl_linalg_SV_decomp_jacobi(u_m, v_m, s_v);
	if (!err) err = gsl_linalg_SV_decomp(u_m, v_m, s_v, work_v);
//	print_vector(s_v, "singular values: ");
	for(i=0; i < s_v->size; i++) {
		if (gsl_vector_get(s_v, i) < DBL_EPSILON)
			gsl_vector_set(s_v, i, 0.0);
	}
	if (!err) err = gsl_linalg_SV_solve(u_m, v_m, s_v, y_v, x_v);

	if (s_v) gsl_vector_free(s_v);
	if (work_v) gsl_vector_free(work_v);
	if (u_m)  gsl_matrix_free(u_m);
	if (v_m)  gsl_matrix_free(v_m);
	return err;
}

#define RET_ERROR 1
#define RET_OK    0
#define TRUE      1
#define FALSE     0
#define ZERO_TOL  DBL_EPSILON

/**
 * \def COLUMN_INDEX
 * Retrieves a column number from the index set.
 * \param[in] s Index set.
 * \param[in] idx Index in the index list where the column number is stored.
 */
#define COLUMN_INDEX(s, idx) (gsl_vector_int_get((s)->arr, (idx)))

/**
 * \def INDEX_SET_VALID
 * Checks validity of an index set.
 * \param[in] s Index set to test.
 * \returns Non-zero for a valid index set, zero otherwise.
 */
#define INDEX_SET_VALID(s) ((s) && (s)->arr && (s)->size_p >= 0 && (s)->size_p <= (s)->arr->size)

/**
 * \def INDEX_SET_Z_EMPTY
 * Checks an index set for an empty set \b Z.
 * \param[in] s index set containing set \b Z and \b P.
 * \returns Zero if there are still columns in Z for which the solution is 
 *          not positive. Non-zero otherwise.
 */
#define INDEX_SET_Z_EMPTY(s) ((s)->arr->size == (s)->size_p)

/**
 * An index set for use in the nnls algorithm.
 * The lower part is set P with order preserved. 
 * The upper part is set Z which is unordered.
 */
typedef struct {
	int              size_p; //!< Number of indices in set P.
	gsl_vector_int * arr;    //!< Pointer to index list.
} index_set_t;


/**
 * Prints the given index set to stdout.
 * \param[in] s index set containing set \b Z and \b P.
 */
int index_set_print(const index_set_t * s)
{
	int i;
	if ( !INDEX_SET_VALID(s) ) GSL_ERROR("Invalid index set!", GSL_EFAULT);
	printf("index set mod: ");
	for(i=0; i < s->arr->size; i++)
	{
		if (i == s->size_p) printf("|");
		printf("%d ", COLUMN_INDEX(s, i));
	}
	printf("\n");
	return GSL_SUCCESS;
}

int index_set_move_z2p (index_set_t * s, const int set_idx);
int index_set_transf_z (const index_set_t * s,
                        const double        tau,
                        const gsl_vector  * hh_v,
                              gsl_matrix  * a_m);
int mod_data (      gsl_matrix * a_m, 
              const int          col, 
              const int          row_diag, 
              const gsl_vector * hh_v,
                    gsl_vector * b_v, 
              const gsl_vector * z_v);

/**
 * Initializes an index_set_t structure. The given solution vector may contain
 * a partial negative solution. Indices of
 * elements smaller than or equal to zero are put into set \b Z otherwise
 * they will be put into set \b P. 
 * \param[in,out] s index_set_t structure to initialize.
 * \param[in,out] x_v Solution vector which may contain an interim solution.
 * \returns Zero if no error occured, non-zero otherwise.
 */
int index_set_init(index_set_t * s, gsl_vector * x_v, gsl_matrix * a_m, gsl_vector * b_v)
{
	int i, err = GSL_SUCCESS;
	gsl_vector * tmp_v = NULL;
	gsl_vector * z_v = NULL;
	if (!s || !x_v || !a_m || !b_v) GSL_ERROR("Invalid data pointers!", GSL_EFAULT);

	s->arr = NULL;
	s->arr = gsl_vector_int_alloc(x_v->size);
	if (!s->arr) GSL_ERROR("Got no space for index list!", GSL_ENOMEM);
	tmp_v = gsl_vector_alloc(a_m->size1);
	z_v = gsl_vector_alloc(b_v->size);
	if (!tmp_v || !z_v) GSL_ERROR("Got no workspace!", GSL_ENOMEM);

	s->size_p = 0;
	// all columns in Z first
	for(i=0; !err && i < x_v->size; i++) gsl_vector_int_set(s->arr, i, x_v->size-i-1);
//	idx_z = x_v->size-1; // start init at last elem
	for(i=0; !err && i < s->arr->size; i++)
	{
		int k = COLUMN_INDEX(s, i);
		if (gsl_vector_get(x_v, k) < ZERO_TOL) // in Z
		{
//			gsl_vector_int_set(s->arr, idx_z, i);
//			idx_z--;
			gsl_vector_set(x_v, k, 0.0);
		} else { // in P
//			gsl_vector_int_set(s->arr, s->size_p, i);
//			s->size_p++;
			
//			printf("transforming column at init\n");
			// TODO: is this useful ??
			// add this matrix column to set P (transf to triag)
			int cur_row = s->size_p;
			double tau;
			gsl_vector_view b_view = gsl_vector_subvector(b_v, cur_row, a_m->size1 - cur_row);
			gsl_vector_view z_view = gsl_vector_subvector(z_v, cur_row, a_m->size1 - cur_row);
			gsl_vector_view hh_view = gsl_vector_subvector(tmp_v, cur_row, a_m->size1 - cur_row);
			gsl_vector_const_view tmp_view = gsl_matrix_const_column(a_m, k);
			gsl_vector_const_view v_view = gsl_vector_const_subvector(&tmp_view.vector, cur_row, a_m->size1 - cur_row);
			// gives error: invalid initializer
			// @Windows, MinGW, GCC 3.4.5, GSL 1.8 for Windows
			// gsl_vector_const_view v_view = gsl_matrix_const_subcolumn(a_m, k, cur_row, a_m->size1 - cur_row);

			if (!err) err = gsl_vector_memcpy(&hh_view.vector, &v_view.vector);
			if (!err) {
				tau = gsl_linalg_householder_transform(&hh_view.vector);
				err = gsl_vector_memcpy(z_v, b_v);
			}
			if (!err) err = gsl_linalg_householder_hv(tau, &hh_view.vector, &z_view.vector);
			if (!err) err = index_set_move_z2p(s, i);
			if (!err) err = mod_data(a_m, k, cur_row, &hh_view.vector, 
				                 &b_view.vector, &z_view.vector);
			if (!err) err = index_set_transf_z(s, tau, &hh_view.vector, a_m);

		}
	}
	gsl_vector_free(tmp_v);
	gsl_vector_free(z_v);
	return err;
}

/**
 * Moves a column to set \b P. Given the index to move within the set, it is
 * appended to the list of indices in \b P.
 * \param[in,out] s index set containing set \b Z and \b P.
 * \param[in] set_idx Index of the column to move in the set.
 * \returns Zero on success, non-zero otherwise.
 */
int index_set_move_z2p (index_set_t * s, const int set_idx)
{
	int err = GSL_SUCCESS;
	if ( !INDEX_SET_VALID(s) || set_idx < 0)
		GSL_ERROR("Invalid index set or index!", GSL_EFAULT);

	err = gsl_vector_int_swap_elements(s->arr, set_idx, s->size_p);
	s->size_p++;
	if (DBG) index_set_print(s);
	return err;
}

/**
 * Transforms all columns in set \b Z by given householder transformation. It 
 * is supposed to be the result of gsl_linalg_householder_transform().
 * \param[in] s index set containing set \b Z and \b P.
 * \param[in] tau Result of <tt>gsl_linalg_householder_transform()</tt>.
 * \param[in] h_v Result of <tt>gsl_linalg_householder_transform()</tt>.
 * \param[in,out] a_m Matrix whose columns are to be transformed.
 * \returns Zero on success, non-zero otherwise.
 */
int index_set_transf_z (const index_set_t * s,
                        const double        tau,
                        const gsl_vector  * hh_v,
                              gsl_matrix  * a_m)
{
	int i, diag_row, err = GSL_SUCCESS;

	if ( !INDEX_SET_VALID(s) || !hh_v || !a_m)
		GSL_ERROR("Invalid index set or input data!", GSL_EFAULT);
	
	// transform all columns in Z by prev. created householder transf.
	diag_row = a_m->size1 - hh_v->size;
	for(i=s->size_p; !err && i < s->arr->size; i++)
	{
		int k = COLUMN_INDEX(s, i);
		gsl_vector_view tmp_view = gsl_matrix_column(a_m, k);
		gsl_vector_view v_view = gsl_vector_subvector(&tmp_view.vector, diag_row, hh_v->size);
		// gives error: invalid initializer
		// @Windows, MinGW, GCC 3.4.5, GSL 1.8 for Windows
		// gsl_vector_view v_view = gsl_matrix_subcolumn(a_m, k, diag_row, hh_v->size);
		err = gsl_linalg_householder_hv(tau, hh_v, &v_view.vector);
	}
	return err;
}

/**
 * Calculates the dual vector \b w. In contrast to the desired solution vector
 * \b x its elements are required to be less than (indices in \b Z) or equal 
 * to zero (indices in \b P).
 * \param[in] s index set containing set \b Z and \b P.
 * \param[in] a_m Matrix of the linear equation system to solve.
 * \param[in] b_v Vector of input data.
 * \param[out] w_v Dual vector \b w to calulate.
 * \returns Zero on success, non-zero otherwise.
 */
int calc_dual_vector(const index_set_t * s, 
                     const gsl_matrix  * a_m, 
                     const gsl_vector  * b_v, 
                           gsl_vector  * w_v)
{
	int i, err = GSL_SUCCESS;
	if ( !INDEX_SET_VALID(s) || !a_m || !b_v || a_m->size1 != b_v->size || !w_v)
		GSL_ERROR("Invalid index set or input data!", GSL_EFAULT);

	for(i=0; !err && i < s->arr->size; i++)
	{
		int k = COLUMN_INDEX(s, i);
		if (i < s->size_p) { // in P
			gsl_vector_set(w_v, k, 0.0);
		} else { // in Z
			double val;
			gsl_vector_const_view tmp_view = gsl_matrix_const_column(a_m, k);
			gsl_vector_const_view a_vv = gsl_vector_const_subvector(&tmp_view.vector, s->size_p, a_m->size1-s->size_p);
			// gives error: invalid initializer
			// @Windows, MinGW, GCC 3.4.5, GSL 1.8 for Windows
			// gsl_vector_const_view a_vv = gsl_matrix_const_subcolumn(a_m, k, s->size_p, a_m->size1-s->size_p);
			gsl_vector_const_view b_vv = gsl_vector_const_subvector(b_v, s->size_p, a_m->size1-s->size_p);
			err = gsl_blas_ddot(&a_vv.vector, &b_vv.vector, &val);
			gsl_vector_set(w_v, k, val);
		}
	}
	return err;
}

/**
 * Finds the largest positive element of the dual vector \b w.
 * Returns the element index and set index of this value.
 * \param[in] s index set containing set \b Z and \b P.
 * \param[in] w_v Dual vector \b w.
 * \param[out] set_idx Index in index_set_t structure, -1 if not found.
 * \param[out] col_idx Element index in dual vector \b w.
 * \returns Zero on success, non-zero otherwise.
 */
int get_max_index2(const index_set_t * s,
                   const gsl_vector  * w_v,
                         int         * set_idx,
                         int         * col_idx)
{
	int i, col_max = -1;

	if ( !INDEX_SET_VALID(s) || !w_v || !set_idx || !col_idx)
		GSL_ERROR("Invalid index set or input data!", GSL_EFAULT);

	for (i=s->size_p; i < s->arr->size; i++) // indices in Z
	{
		int k = COLUMN_INDEX(s, i);
		if ( col_max < 0 || 
		     gsl_vector_get(w_v, k) > gsl_vector_get(w_v, col_max) )
		{
				*set_idx = i;
				*col_idx = k;
				col_max = k;
		}
	}
	if (DBG) printf("col_idx: %d, set_idx: %d, val: %g, ZERO_TOL: %g, cond: %d\n", 
	                *col_idx, *set_idx, gsl_vector_get(w_v, *col_idx), ZERO_TOL, 
	                 gsl_vector_get(w_v, *col_idx) < ZERO_TOL);

	if ( gsl_vector_get(w_v, *col_idx) <= 0.0 ) {

		*col_idx = -1;
		*set_idx = -1;
	}
	return GSL_SUCCESS;
}

/**
 * Checks linear dependency of the new diagonal element regarding the norm of
 * its upper diagonal column. 
 * \param[in] unorm Previously calculated norm of upper triangular column.
 * \param[in] hh_v Result of <tt>gsl_linalg_householder_transform()</tt>.
 * \param[in] tau Result of <tt>gsl_linalg_householder_transform()</tt>.
 * \param[in] b_v Data vector.
 * \param[out] z_v Data vector transformed by supplied householder transformation.
 * \param[out] ztest Proposed new value of global solution vector \b x.
 * \returns Zero on success, non-zero otherwise.
 */
int check_lin_dep(const double        unorm,
		  const gsl_vector  * hh_v,
		  const double        tau,
		  const gsl_vector  * b_v,
		        gsl_vector  * z_v,
                        double      * ztest)
{
	int err = GSL_SUCCESS;
	volatile double beta_test;

	if ( !b_v || !z_v || !hh_v || !ztest)
		GSL_ERROR("Invalid data pointers!", GSL_EFAULT);

	*ztest = -1.0;
	beta_test = gsl_vector_get(hh_v, 0);
	beta_test = unorm + fabs(beta_test)*0.01;
	if ( beta_test-unorm > 0.0) // errors with IEEE arithmetic
	                            // > DBL_EPSILON ? TODO
	{
		// column t passed independence test
		// transform input data b and see if the proposed new
		// value of the solution vector (ztest) is positive
		gsl_vector_memcpy(z_v, b_v);
		if (DBG) print_vector(z_v, "applying HH to zz: ");
		err = gsl_linalg_householder_hv(tau, hh_v, z_v);
		if (DBG) print_vector(z_v, "done:              ");
		*ztest = gsl_vector_get(z_v, 0) / gsl_vector_get(hh_v, 0); // z[0] / beta
		if (DBG) printf("ztest: %g\n", *ztest);
	}
	return err;
}

/**
 * Calculates norm of upper subcolumn.
 * \param[in] a_m Matrix containing the column to calculate the norm of.
 * \param[in] col Column index.
 * \param[in] diag_row Index of the last row to include.
 * \returns Norm of upper subcolumn.
 */
double get_upper_norm(const gsl_matrix   * a_m,
                      const int            col,
                      const int            diag_row)
{
	double unorm = 0.0;

	if (!a_m)
		GSL_ERROR_VAL("Invalid data pointer!", GSL_EFAULT, unorm);

	if (diag_row+1 > 0) 
	{
		// euclidean norm of upper triangluar column segment
		gsl_vector_const_view tmp_view = gsl_matrix_const_column(a_m, col);
		gsl_vector_const_view v_view = gsl_vector_const_subvector(&tmp_view.vector, 0, diag_row+1);
		// gives error: invalid initializer
		// @Windows, MinGW, GCC 3.4.5, GSL 1.8 for Windows
		// gsl_vector_const_view v_view = gsl_matrix_const_subcolumn(a_m, col, 0, diag_row+1);
		unorm = gsl_blas_dnrm2(&v_view.vector);
	}
	return unorm;
}

/**
 * Determine a column to move from set \b Z to \b P based on the current dual
 * vector \b w and index set.
 * \param[in] s index set containing set \b Z and \b P.
 * \param[in] a_m Matrix of the linear equation system to solve.
 * \param[in] b_v Input data vector.
 * \param[in,out] w_v Dual vector \b w.
 * \param[out] hh_v Result of <tt>gsl_linalg_householder_transform()</tt>.
 * \param[out] tau Result of <tt>gsl_linalg_householder_transform()</tt>.
 * \param[out] z_v Workspace containing modified \e b_v.
 * \param[out] col_idx Index of selected maximum positive value in dual vector \b w.
 * \returns Zero on success, non-zero otherwise.
 */
int get_new_p_column(      index_set_t * s,
                     const gsl_matrix  * a_m,
                     const gsl_vector  * b_v,
                           gsl_vector  * w_v,
                           gsl_vector  * hh_v,
                           double      * tau,
                           gsl_vector  * z_v,
			   int         * col_idx)
{
	double ztest = -1.0, unorm, beta;
	int set_idx, err = GSL_SUCCESS;

	if ( !INDEX_SET_VALID(s) || !b_v || !hh_v || !tau || !z_v || !w_v || !col_idx) 
		GSL_ERROR("Invalid index set or input data!", GSL_EFAULT);

	do {
		// get the largest positive w(t)
		err = get_max_index2(s, w_v, &set_idx, col_idx);
		if (DBG) printf("col_idx: %d, set_idx: %d, val: %g, ZERO_TOL: %g, "
		                "cond: %d\n", *col_idx, set_idx, 
				(*col_idx >= 0 ? gsl_vector_get(w_v, *col_idx) : 0.0), 
				ZERO_TOL, 0.0 < ZERO_TOL);
		if ( !err && *col_idx >= 0 )
		{
			// get the current lower triangular subcolumn and apply
			// householder transformation
			gsl_vector_const_view tmp_view = gsl_matrix_const_column(a_m, *col_idx);
			gsl_vector_const_view v_view = gsl_vector_const_subvector(&tmp_view.vector, s->size_p, a_m->size1 - s->size_p);
			// gives error: invalid initializer
			// @Windows, MinGW, GCC 3.4.5, GSL 1.8 for Windows
			// gsl_vector_const_view v_view = gsl_matrix_const_subcolumn(a_m, *col_idx, s->size_p, a_m->size1 - s->size_p);
			gsl_vector_memcpy(hh_v, &v_view.vector);
	//		print_vector(tmp_v, "hh before: ");
			*tau = gsl_linalg_householder_transform(hh_v);
			// gives (vec u)/u_lp, u[0] == s, u[0]*(-tau) == u_lp (see ref NNLS)
	//		print_vector(tmp_v, "hh after: ");
			beta = gsl_vector_get(hh_v, 0); // also called \beta

			unorm = get_upper_norm(a_m, *col_idx, s->size_p-1);
			if (DBG) printf("tau: %g, beta: %g, unorm: %g\n", *tau, beta, unorm);
			err = check_lin_dep(unorm, hh_v, *tau, b_v, z_v, &ztest);
			if (DBG) printf("t: %d, ztest: %g\n", *col_idx, ztest);
			// new diagonal element is not sufficiently linear independent
			// AND the proposed new value for x is negative:
			// w(t) is supposed to be zero => get another one
			if (ztest < 0.0) {
				gsl_vector_set(w_v, *col_idx, 0.0);
			}
		}
	} while (!err && ztest < 0.0 && *col_idx >= 0);

	if (!err && ztest >= 0.0) {
		err = index_set_move_z2p(s, set_idx);
	}

	return err;
}

/**
 * Modifies system matrix and data vector according to previously calculated
 * householder transformation.
 * \param[in,out] a_m Matrix of the linear equation system to solve.
 * \param[in] col Current column to modify.
 * \param[in] row_diag Row of current diagonal element.
 * \param[in] hh_v Result of <tt>gsl_linalg_householder_transform()</tt>.
 * \param[out] b_v Input data vector.
 * \param[in] z_v Already modified vector \e b_v from previous calculations.
 * \returns Zero on success, non-zero otherwise.
 */
int mod_data (      gsl_matrix * a_m, 
              const int          col, 
              const int          row_diag, 
              const gsl_vector * hh_v,
                    gsl_vector * b_v, 
              const gsl_vector * z_v)
{
	int err = GSL_SUCCESS;
	gsl_vector_view a_vv;
	gsl_vector_view tmp_view; // see below

	if (!a_m || !hh_v || !b_v || !z_v)
		GSL_ERROR("Invalid data pointers!", GSL_EFAULT);

	// modify system matrix
	gsl_matrix_set(a_m, row_diag, col, gsl_vector_get(hh_v, 0));
	if (row_diag+1 < a_m->size1)
	{
		// set elements below diagonal of new column to zero
		tmp_view = gsl_matrix_column(a_m, col);
		a_vv = gsl_vector_subvector(&tmp_view.vector, row_diag+1, a_m->size1 - row_diag-1);
		// gives error: invalid initializer
		// @Windows, MinGW, GCC 3.4.5, GSL 1.8 for Windows
		// a_vv = gsl_matrix_subcolumn(a_m, col, row_diag+1, a_m->size1 - row_diag-1);
		gsl_vector_set_zero(&a_vv.vector);
	}

	// modify data vector
	err = gsl_vector_memcpy(b_v, z_v);
	return err;
}

/**
 * Solves a triagonal equation system made up of all columns in set \b P (in
 * this order).
 * \param[in] s Index set containing set \b Z and \b P.
 * \param[in] a_m Matrix containing all columns in set \b P.
 * \param[in,out] z_v Data vector containing the right side of the equation
 * system for input. On output it contains the solution.
 * \returns Zero on success, non-zero otherwise.
 */
int solve_triag (const index_set_t * s, 
                 const gsl_matrix  * a_m, 
                       gsl_vector  * z_v)
{
	int i, err = GSL_SUCCESS;
	gsl_matrix * triag_m = NULL;
	gsl_vector_view z_view;

	if ( !INDEX_SET_VALID(s) || !a_m || !z_v)
		GSL_ERROR("Invalid index set or input data!", GSL_EFAULT);

	triag_m = gsl_matrix_alloc(s->size_p, s->size_p);
	if (!triag_m) GSL_ERROR("No data for workspace matrix!", GSL_ENOMEM);

	z_view = gsl_vector_subvector(z_v, 0, s->size_p);
	if (DBG) {
		print_vector(&z_view.vector, "z_view: ");
		index_set_print(s);
	}
	// copy P-columns into triangular matrix, update triag matrix view
	for(i=0; i < s->size_p; i++)
	{
		int k = COLUMN_INDEX(s, i);
		if (DBG) printf("loophead, k: %d\n", (int)k);
		gsl_vector_const_view tmp_view = gsl_matrix_const_column(a_m, k);
		gsl_vector_const_view col_view = gsl_vector_const_subvector(&tmp_view.vector, 0, s->size_p);
		// gives error: invalid initializer
		// @Windows, MinGW, GCC 3.4.5, GSL 1.8 for Windows
		// gsl_vector_const_view col_view = gsl_matrix_const_subcolumn(a_m, k, 0, s->size_p);
		// k = COLUMN_INDEX(s, i);
		gsl_matrix_set_col(triag_m, i, &col_view.vector);
	}
	if (DBG) print_matrix(triag_m, "Triag-Matrix: ", 0);

	// solve triangular system
	err = gsl_blas_dtrsv(CblasUpper, CblasNoTrans, CblasNonUnit, triag_m, &z_view.vector);
	if (DBG) print_vector(&z_view.vector, "solution z_view: ");

	gsl_matrix_free(triag_m);
	return err;
}

/**
 * Determines an alpha for the NNLS algorithm.
 * \param[in] s Index set containing set \b Z and \b P.
 * \param[in] x_v Interim solution vector.
 * \param[in] z_v New interim solution vector.
 * \param[out] alpha Determined alpha value.
 * \param[out] alpha_index Index within \e s used to calculate \e alpha.
 * \returns Zero if no \e alpha was found, non-zero if a valid alpha was
 *          determined.
 */
int get_alpha2(const index_set_t    * s,
               const gsl_vector     * x_v,
               const gsl_vector     * z_v,
                     double         * alpha,
                     int            * alpha_index)
{
	int i, k;
	double temp;

	if ( !INDEX_SET_VALID(s) || !x_v || !z_v || !alpha || !alpha_index)
		GSL_ERROR_VAL("Invalid index set or input data!", GSL_EFAULT, 0); // no alpha found

	*alpha = 2.0;
	*alpha_index = -1;
	for(i=0; i < s->size_p; i++)
	{
		k = COLUMN_INDEX(s, i);
		// i \in P and z_i <= 0
		if (gsl_vector_get(z_v, i) <= 0.0)
		{
			// find min x_i/(x_i - z_i)
			temp = - gsl_vector_get(x_v, k) / 
				( gsl_vector_get(z_v, i)-gsl_vector_get(x_v, k) );

			if (*alpha > temp) {
				*alpha = temp;
				*alpha_index = i;
			}
		}
	}
	return (*alpha_index >= 0 && *alpha_index < s->size_p); // valid alpha found
}

/**
 * Interpolates between old interim solution \b x and new interim solution 
 * \b z.
 * \param[in] s Index set containing set \b Z and \b P.
 * \param[in] alpha Weight for the interpolation of new solution vector.
 * \param[in] z_v Contains new interim solution.
 * \param[out] x_v Resulting interpolated solution vector.
 * \returns Zero on success, non-zero otherwise.
 * \sa get_alpha2()
 */
int interpolate_newx(const index_set_t * s,
                     const double        alpha,
                     const gsl_vector  * z_v,
                           gsl_vector  * x_v)
{
	int i, k;
	double temp;

	if ( !INDEX_SET_VALID(s) || !z_v || !x_v)
		GSL_ERROR("Invalid index set or input data!", GSL_EFAULT);

	// interpolate between old x and new z for all columns in P
	for(i=0; i < s->size_p; i++) {
		k = COLUMN_INDEX(s, i);
		temp = gsl_vector_get(x_v, k);
		temp += alpha * (gsl_vector_get(z_v, i) - temp);
		if (DBG) printf("new x[%d]: %g\n", (int)k, temp);
		gsl_vector_set(x_v, k, temp);
	}
	return GSL_SUCCESS;
}

/**
 * Checks if all values in set \b P of the interim solution \b x are positive.
 * \param[in] s Index set containing set \b Z and \b P.
 * \param[in] x_v Interim solution to test.
 * \param[out] idx Index in set \b P of the first value which is not positive.
 * \returns Zero on success, non-zero otherwise.
 */
int is_feasible(const index_set_t * s, const gsl_vector * x_v, int * idx)
{
	int col, feasible = 1; // true

	if ( !INDEX_SET_VALID(s) || !x_v || !idx)
		GSL_ERROR_VAL("Invalid index set or input data!", GSL_EFAULT, 0); // not feasible

	// all values of x in P have to be greater zero
	for(*idx=0; feasible && *idx < s->size_p;) 
	{
		col = COLUMN_INDEX(s, *idx);
		if (gsl_vector_get(x_v, col) <= 0.0) 
		{
			feasible = 0;
		} else 
			(*idx)++;
	}
	return feasible;
}

/**
 * Removes a matrix column from index set \b P. Several Givens Rotations are
 * used to maintain the QR decomposition of the matrix.
 * \param[in,out] s Index set containing set \b Z and \b P.
 * \param[in] p_idx Set index of the column to remove.
 * \param[in,out] a_m Matrix containing the column to transform.
 * \param[in,out] b_v Data vector associated to matrix \e a_m. Is also
 *                    transformed to by Givens rotations.
 * \returns Zero on success, non-zero otherwise.
 */
int index_set_remove_column(index_set_t * s, 
                            const int     p_idx, 
                            gsl_matrix  * a_m, 
                            gsl_vector  * b_v)
{
	int col, i, n, col_removed, err = GSL_SUCCESS;
	double cc, ss, val;

	if ( !INDEX_SET_VALID(s) ) GSL_ERROR("Invalid index set!", GSL_ESANITY);
	if ( p_idx < 0 )           GSL_ERROR("Invalid column index! (<0)", GSL_ESANITY);
	if ( p_idx >= s->size_p )  GSL_ERROR("Invalid column index! (>=end)", GSL_ESANITY);
	if ( !a_m || !b_v)         GSL_ERROR("Invalid input data!", GSL_EFAULT);

	// save the column number for later
	col_removed = COLUMN_INDEX(s, p_idx);

	if (p_idx < s->size_p-1) // no transf. for removing the last column
	{
		// for all columns in P after the one being removed
		for(i = p_idx+1; !err && i < s->size_p; i++)
		{
			col = COLUMN_INDEX(s, i);
			gsl_vector_int_set(s->arr, i-1, col); // keep col indices ordered
			if (DBG) print_matrix(a_m, "before givens-rot, mat A: ", 2);
			// get givens rotation for column to remove
			err = gsl_blas_drotg(gsl_matrix_ptr(a_m, i-1, col), 
					     gsl_matrix_ptr(a_m, i, col), &cc, &ss);
			if (DBG) printf("cc: %g, ss: %g, err: %d\n", cc, ss, err);
			// check if this works with neg also TODO
			cc = -1.0*cc; ss = -1.0*ss;
			gsl_matrix_set(a_m, i-1, col, -1.0*gsl_matrix_get(a_m, i-1, col));
			gsl_matrix_set(a_m, i,   col,  0.0);
			// apply givens rotation to all matrix columns
			for(n=0; n < a_m->size2; n++) 
			{
				if (n == col) continue;
				// (use of gsl_blas_drot() is overkill)
				val = gsl_matrix_get(a_m, i-1, n);
				gsl_matrix_set(a_m, i-1, n, cc*val + ss*gsl_matrix_get(a_m, i, n));
				gsl_matrix_set(a_m, i,   n,-ss*val + cc*gsl_matrix_get(a_m, i, n));
			}
			// apply givens rotation to data vector
			// (use of gsl_blas_drot() is overkill)
			val = gsl_vector_get(b_v, i-1);
			gsl_vector_set(b_v, i-1, cc*val + ss*gsl_vector_get(b_v, i));
			gsl_vector_set(b_v, i,  -ss*val + cc*gsl_vector_get(b_v, i));
		}
	}
	// move index from P to Z
	s->size_p--;
	gsl_vector_int_set(s->arr, s->size_p, col_removed);
	if (DBG) index_set_print(s);
	return err;
}

/**
 * Copies values of temporary vector \b z to interim solution vector \b x.
 * \param[in] s Index set containing set \b Z and \b P.
 * \param[in] z_v Temporary interim solution.
 * \param[in,out] x_v New interim solution vector.
 * \returns Zero on success, non-zero otherwise.
 */
int index_set_interim_solution(const index_set_t * s, 
                               const gsl_vector  * z_v, 
                                     gsl_vector  * x_v)
{
	int i, col;
	if ( !INDEX_SET_VALID(s) || !z_v || !x_v)
		GSL_ERROR("Invalid index set or input data!", GSL_EFAULT);

	// set x = z for all columns in P
	for(i=0; i < s->size_p; i++)
	{
		col = COLUMN_INDEX(s, i);
		gsl_vector_set(x_v, col, gsl_vector_get(z_v, i));
		if (DBG) printf("[2], new x[%d]: %g\n", col, gsl_vector_get(x_v, col));
	}
	return GSL_SUCCESS;
}

/**
 * Solves a linear equation system with positivity constraint.
 * \param[in] in_a_m System matrix of the equation system to solve.
 * \param[in] in_b_v Data vector associated to matrix \e in_a_m.
 * \param[in,out] x_v Solution vector which may contain initial interim
 *                    solution which does not satisfy the positivity constraint.
 * \returns Zero on success, non-zero otherwise.
 */
int solve_sys_nnls(const gsl_matrix * in_a_m,
                    const gsl_vector * in_b_v,
                          gsl_vector * x_v)
{
	int err = GSL_SUCCESS;
	gsl_matrix * a_m = NULL;
	gsl_vector * b_v = NULL;
	gsl_vector * w_v = NULL;
	gsl_vector * z_v = NULL;
	gsl_vector_view b_view, z_view, hh_view;
	gsl_vector * tmp_v = NULL;
	double tau, alpha;
	int t, k, alpha_index, cur_row;
	index_set_t index_set;

	SASFIT_ASSERT_VAL(!compatible(in_a_m, in_b_v, x_v), GSL_EBADLEN);

	if (!err) { // allocate memory
		a_m = gsl_matrix_alloc(in_a_m->size1, in_a_m->size2);
		b_v = gsl_vector_alloc(in_b_v->size);
		w_v = gsl_vector_alloc(in_a_m->size2);
		z_v = gsl_vector_alloc(in_a_m->size1);
		tmp_v = gsl_vector_alloc(in_a_m->size1);
		if ( !a_m || !b_v || !w_v || !z_v || !tmp_v )
			err = GSL_ENOMEM;
	}
	if (DBG) printf("N: %d, M: %d\n", (int)in_a_m->size1, (int)in_a_m->size2);
	if (DBG) print_matrix(in_a_m, "input matrix: ", 2);
	if (DBG) print_vector(in_b_v, "input data: ");

	// initialize memory
	if (!err) gsl_matrix_memcpy(a_m, in_a_m);
	if (!err) gsl_vector_memcpy(b_v, in_b_v);
	if (DBG) print_vector(x_v, "input solution: ");
	if (!err) index_set_init(&index_set, x_v, a_m, b_v);
	if (DBG) index_set_print(&index_set);

	while ( !err && !INDEX_SET_Z_EMPTY(&index_set) )
	{
		// w = A^T b for columns in Z, but 0 otherwise
		if (DBG) index_set_print(&index_set);
		err = calc_dual_vector(&index_set, a_m, b_v, w_v);
		if (DBG) print_vector(w_v, "[2] vector w: ");
		if (err) break;
		
		// find the next column to move from Z -> P
		if (DBG) printf("[2] nsetp: %d, max: %d\n", (int)index_set.size_p, (int)a_m->size1);
		cur_row = index_set.size_p;
		b_view = gsl_vector_subvector(b_v, cur_row, a_m->size1 - cur_row);
		z_view = gsl_vector_subvector(z_v, cur_row, a_m->size1 - cur_row);
		hh_view = gsl_vector_subvector(tmp_v, cur_row, a_m->size1 - cur_row);

		// get householder transformation for new p column to add
		err = get_new_p_column(&index_set, a_m, &b_view.vector, w_v, 
		                       &hh_view.vector, &tau, &z_view.vector, &t);
		if (t < 0) break;

		err = mod_data(a_m, t, cur_row, &hh_view.vector, 
		               &b_view.vector, &z_view.vector);
		if (DBG) print_vector(b_v, "modded b_v: ");

		err = index_set_transf_z(&index_set, tau, &hh_view.vector, a_m);
		gsl_vector_set(w_v, t, 0.0);
		if (DBG) { 
			printf("## nsetp: %d\n", (int)index_set.size_p);
			print_matrix(a_m, "Matrix A with mod Z cols: ", 2);
		}

		gsl_vector_memcpy(z_v, b_v);
		err = solve_triag(&index_set, a_m, z_v);

		while (!err && get_alpha2(&index_set, x_v, z_v, &alpha, &alpha_index))
		{
			if (DBG) printf("[2], alpha: %g\n", alpha);
			err = interpolate_newx(&index_set, alpha, z_v, x_v);

//			printf("### preloop\n");
			do {
//				printf("alpha_index: %d\n",alpha_index);
				k = COLUMN_INDEX(&index_set, alpha_index);
				gsl_vector_set(x_v, k, 0.0);
				if (DBG) printf("entering sec. loop for k: %d\n", k);
				err = index_set_remove_column(&index_set, alpha_index, a_m, b_v);
				if (DBG) index_set_print(&index_set);
			// see if the intermediate solution is positive
			// (x values for indices in P greater zero)
			// otherwise, this column will also be moved from P to Z
			} while (!is_feasible(&index_set, x_v, &alpha_index));

			if (DBG) {
				print_matrix(a_m, "after givens-rot, mat A: ", 2);
				print_vector(b_v, "vec b: ");
			}
			// prepare data vector
			gsl_vector_memcpy(z_v, b_v);
			if (DBG) print_vector(z_v, "z_v: ");
			// solve triangular system
			err = solve_triag(&index_set, a_m, z_v);
			if (DBG) print_vector(z_v, "solution2 z_v: ");
		}
		err = index_set_interim_solution(&index_set, z_v, x_v);
	}// while Z is not empty

	if (DBG) print_vector(x_v, "[2] END x: ");
	if (DBG) print_vector(w_v, "[2] END w: ");

	if (a_m) gsl_matrix_free(a_m);
	if (b_v) gsl_vector_free(b_v);
	if (w_v) gsl_vector_free(w_v);
	if (z_v) gsl_vector_free(z_v);
	if (tmp_v) gsl_vector_free(tmp_v);

	return err;
}

int reg_step(const gsl_matrix * aa_m,  // squared design matrix
             const gsl_matrix * ll_m,  // squared smoothing functional
             const gsl_vector * b_v,   // input data: b = A^T y / e^2
                   gsl_vector * x_v,   // result distribution
                   gsl_vector * dx_v,  // d x_v / d alpha
                   double       alpha) // regularization parameter
{
	int err = TCL_OK;
	gsl_matrix * r_m = NULL;
	gsl_vector * llx_v = NULL;
	gsl_vector * y_v = NULL;  // temporary input data == b_v
//	double h = 1.0 + 1e-3;
	gsl_vector * xx_v = NULL;

	SASFIT_ASSERT_VAL(aa_m && ll_m && b_v && x_v && dx_v, TCL_ERROR);
	SASFIT_ASSERT_VAL(aa_m->size1 == ll_m->size1, TCL_ERROR);
	SASFIT_ASSERT_VAL(aa_m->size2 == ll_m->size2, TCL_ERROR);
	SASFIT_ASSERT_VAL(aa_m->size1 == b_v->size, TCL_ERROR);
	SASFIT_ASSERT_VAL(aa_m->size2 == x_v->size, TCL_ERROR);
	SASFIT_ASSERT_VAL(dx_v->size  == x_v->size, TCL_ERROR);

	// allocate memory
	if (!err) {
		r_m = gsl_matrix_alloc(aa_m->size1, aa_m->size2);
		llx_v = gsl_vector_alloc(b_v->size);
		y_v = gsl_vector_alloc(b_v->size);
		xx_v = gsl_vector_alloc(x_v->size);
		if (!r_m || !llx_v || !y_v) err = TCL_ERROR;
	}
	// A^T A + \alpha L^T L
	if (!err) err = gsl_matrix_memcpy(r_m, ll_m);
	if (!err) err = gsl_matrix_scale(r_m, alpha);
	if (!err) err = gsl_matrix_add(r_m, aa_m);
	
//	print_matrix(r_m, "system matrix: ", 0);
//	if (1) matrix_properties(r_m, "System Matrix: ");
//	printf(", alpha: %lg\n", alpha);

	if (!err) err = gsl_vector_memcpy(y_v, b_v);
	if (!err) err = reg_scale_input(r_m, y_v);
//	if (1) matrix_properties(r_m, "scaled System Matrix: ");
	// solve the equation system
//	if (!err) err = solve_sys_qr(r_m, y_v, x_v);
//	if (!err) err = gsl_vector_memcpy(xx_v, x_v);
//	gsl_vector_set_zero(x_v);
	if (!err) err = solve_sys_nnls(r_m, y_v, x_v);
//	get_rel_error(xx_v, x_v);
//	print_vector(xx_v, "NNLS result vector diff (rel): ");
//	gsl_vector_memcpy(x_v, xx_v);

	// calc dx analytical (Hofmann1999): L^T L x
	if (!err) err = gsl_blas_dgemv(CblasNoTrans, 
	                               -1.0, ll_m, x_v, 0.0, llx_v);

	// calculate deviation dx
	// solve (A^T A + \alpha L^T L) dx/dalpha = L^T L x
	if (!err) err = solve_sys_qr(r_m, llx_v, dx_v);
//	print_vector(dx_v, "analytical dx: ");
//	printf("analytical dx length: %lg\n", gsl_blas_dnrm2(dx_v));

//	gsl_vector * tmp_v = gsl_vector_alloc(dx_v->size);
//	gsl_vector_memcpy(tmp_v, dx_v);
	// calc dx numerical
/**
	if (!err) err = gsl_matrix_memcpy(r_m, ll_m);
	if (!err) err = gsl_matrix_scale(r_m, alpha + (alpha*h));
	if (!err) err = gsl_matrix_add(r_m, aa_m);
	if (!err) err = solve_sys_qr(r_m, y_v, dx_v);

	if (!err) err = gsl_matrix_memcpy(r_m, ll_m);
	if (!err) err = gsl_matrix_scale(r_m, alpha - (alpha*h));
	if (!err) err = gsl_matrix_add(r_m, aa_m);
	if (!err) err = solve_sys_qr(r_m, y_v, llx_v);

	if (!err) err = gsl_vector_sub(dx_v, llx_v);
//	printf("numerical dx length: %lg, alpha diff: %lg\n", gsl_blas_dnrm2(dx_v), (2.0*alpha*h));
	if (!err) err = gsl_vector_scale(dx_v, 1.0/(2.0*alpha*h));
//	print_vector(dx_v, "numerical dx: ");
//	printf("numerical dx length: %lg\n", gsl_blas_dnrm2(dx_v));
//	if (!err) err = gsl_blas_ddot(tmp_v, dx_v, &h);
//	printf("dot product of both: %lg\n", h);
*/

	if (r_m)   gsl_matrix_free(r_m);
	if (llx_v) gsl_vector_free(llx_v);
	if (y_v) gsl_vector_free(y_v);
	return err;
}

int regularize(Tcl_Interp * interp, 
               Tcl_Obj ** res,         // tcl result object for interpreter
               const gsl_matrix * a_m, // design matrix
               const gsl_vector * y_v, // input data vector
               const gsl_vector * e_v, // error vector
               const gsl_vector * p_v, // sampling point of distribution
               const gsl_vector * w_v, // quadrature weights of distribution
               int mode)               // type of return curve
{
	int err = TCL_OK;
	size_t i;
	gsl_vector * b_v = NULL;
	gsl_vector * x_v = NULL;
	gsl_vector * dx_v = NULL;
	gsl_vector * reg_err_v = NULL;
	gsl_matrix * aa_m = NULL;
	gsl_matrix * l_m = NULL;
	gsl_matrix * ll_m = NULL;
	gsl_vector * quot_v = NULL;
	gsl_vector * alpha_range_v = NULL;
	gsl_vector * crit_deriv_v = NULL;
	gsl_vector * crit_quot_v = NULL;
	gsl_vector * crit_lx_v = NULL;
	gsl_vector * crit_ly_v = NULL;
	gsl_vector * crit_l_v = NULL;
	double alpha = 50.0;
	double crit_deriv;
	double crit_quot;
	double alpha_min, alpha_max;
	int alpha_num;
	sasfit_timer * tm;

	SASFIT_ASSERT_VAL(interp && res && a_m && y_v && e_v && 
	                            p_v && w_v, TCL_ERROR);

	if (!err) err = sasfit_tcl_get_double(interp, 
	                &alpha, "inv_data", "alpha");
	if (!err) err = sasfit_tcl_get_double(interp, 
	                &alpha_min, "inv_data", "alpha_min");
	if (!err) err = sasfit_tcl_get_double(interp, 
	                &alpha_max, "inv_data", "alpha_max");
	if (!err) err = sasfit_tcl_get_int(interp, 
	                &alpha_num, "inv_data", "alpha_num");

	printf("alpha, min: %lg, max: %lg, num: %d\n", alpha_min, alpha_max, alpha_num);

	// allocating memory
	if (!err) {
		b_v = gsl_vector_alloc(a_m->size2);
		x_v = gsl_vector_alloc(a_m->size2);
		dx_v = gsl_vector_alloc(a_m->size2);
		reg_err_v = gsl_vector_alloc(y_v->size);
		quot_v = gsl_vector_alloc(y_v->size);
		ll_m = gsl_matrix_alloc(a_m->size2, a_m->size2);
		aa_m = gsl_matrix_alloc(a_m->size2, a_m->size2);
		err = matrix_set_2nd_derivative(&l_m, a_m->size2);
//		err = matrix_set_1st_derivative(&l_m, a_m->size2);
//		err = matrix_set_identity(&l_m, a_m->size2);
//		print_matrix(l_m, "contraint matrix: ", 0);
		if (!b_v || !x_v || !dx_v || !reg_err_v || !quot_v || 
		    !l_m || !ll_m || !aa_m) 
		{
			err = TCL_ERROR;
		}
	}
	// b = A^T y / e^2
	if (!err) err = reg_prepare_givens(a_m, y_v, e_v, b_v);
	// A^T A / e^2
	if (!err) {
		err = reg_prepare_design_matrix(a_m, e_v, aa_m);
	}
	// prepare contraint matrix: L^T L
	if (!err) err = gsl_blas_dgemm(CblasTrans, CblasNoTrans, 
		                       1.0, l_m, l_m, 0.0, ll_m);

//	printf("--------------------------\n");
//	print_matrix(a_m, "matrix A: ", 2);
//	print_matrix(aa_m, "matrix A^T A: ", 2);
//	matrix_properties(a_m, "A Design Matrix");
//	matrix_properties(aa_m, "A^T A Matrix");

	if (!err) err = create_range_log(&alpha_range_v, alpha_min, alpha_max, alpha_num);
//	print_vector(alpha_range_v, "alpha vector: ");
	if (!err) {
		crit_deriv_v = gsl_vector_alloc(alpha_range_v->size);
		crit_quot_v = gsl_vector_alloc(alpha_range_v->size);
		crit_lx_v = gsl_vector_alloc(alpha_range_v->size);
		crit_ly_v = gsl_vector_alloc(alpha_range_v->size);
		crit_l_v = gsl_vector_alloc(l_m->size1);
		if (!crit_deriv_v || !crit_quot_v || 
		    !crit_lx_v || !crit_ly_v || !crit_l_v) err = TCL_ERROR;
	}
	if (mode == 0) gsl_vector_set(alpha_range_v, 0, alpha);

	tm = sasfit_timer_create();
	sasfit_timer_start(tm);
	for(i=0; !err && i < alpha_range_v->size;i++)
	{
		alpha = gsl_vector_get(alpha_range_v, i);
		// printf("calculating for alpha: %lg\n",alpha);
		// solve (A^T A + \alpha L^T L) x = A^T y
		if (!err) err = reg_step(aa_m, ll_m, b_v, x_v, dx_v, alpha);
	//	print_vector(dx_v, "dx/dalpha: ");
		if (!err) err = gsl_vector_scale(dx_v, alpha);

		// derivation criterion
		if (!err) { 
			crit_deriv = gsl_blas_dnrm2(dx_v);
//			printf("derivation crit: %lg\n", crit_deriv);
			gsl_vector_set(crit_deriv_v, i, crit_deriv);
		}

		// quotient criterion
		if (!err) err = reg_error(a_m, y_v, e_v, x_v, reg_err_v);
		if (!err) err = gsl_blas_dgemv(CblasNoTrans, 
					       1.0, a_m, dx_v, 0.0, quot_v);
		if (!err) err = gsl_vector_sub(quot_v, reg_err_v);
		if (!err) {
			crit_quot = gsl_blas_dnrm2(quot_v) / 
				    gsl_blas_dnrm2(reg_err_v);
//			printf("quotient crit: %lg\n", crit_quot);
			gsl_vector_set(crit_quot_v, i, crit_quot);
		}
		/*
		if (crit_quot == 1.0) {
			print_vector(dx_v, "dx/dalpha: ");
			printf("quot: %lg, deriv: %lg\n",crit_quot,crit_deriv);
		}
		*/
		// l-kurve criterion
		if (!err) err = gsl_blas_dgemv(CblasNoTrans,
		                               1.0, l_m, x_v, 0.0, crit_l_v);
		if (!err) {
			gsl_vector_set(crit_lx_v, i, 
				gsl_blas_dnrm2(reg_err_v));
			gsl_vector_set(crit_ly_v, i, 
				gsl_blas_dnrm2(crit_l_v));
		}
		if (mode == 0) break;
	}
	printf("regularization %d steps, avg time: %lg s\n", (int)i, sasfit_timer_measure(tm)/i);

	if (!err) {
//		print_vector(reg_err_v, "error vector: ");
//		printf("chi: %lg\n", gsl_blas_dnrm2(reg_err_v));
	}

	if (mode == 0) {
//		vector_integrate(p_v, w_v, x_v, &tempval);
//		printf("distrib integrate over param: %e\n", tempval);
//		gsl_vector_scale(x_v, 1.0/tempval);
		// fill the tcl result object
		if (!err) err = tcl_result_add_list(interp, res, x_v);
//		if (!err) err = tcl_result_add_list(interp, res, reg_err_v);
	} else if (mode == 23) {
//		print_vector(crit_deriv_v, "derivative crit vec: ");
//		print_vector(crit_quot_v, "quotient crit vec: ");
		if (!err) err = tcl_result_add_type(interp, res, 2);
		if (!err) err = tcl_result_add_list(interp, res, alpha_range_v);
		if (!err) err = tcl_result_add_list(interp, res, crit_deriv_v);
		if (!err) err = tcl_result_add_type(interp, res, 3);
		if (!err) err = tcl_result_add_list(interp, res, alpha_range_v);
		if (!err) err = tcl_result_add_list(interp, res, crit_quot_v);
	} else if (mode == 4) {
		if (!err) err = tcl_result_add_type(interp, res, 4);
		if (!err) err = tcl_result_add_list(interp, res, crit_lx_v);
		if (!err) err = tcl_result_add_list(interp, res, crit_ly_v);
	}
	sasfit_tcl_set_double(interp, "inv_data", "alpha", alpha);

	if (b_v) gsl_vector_free(b_v);
	if (x_v) gsl_vector_free(x_v);
	if (dx_v) gsl_vector_free(dx_v);
	if (reg_err_v) gsl_vector_free(reg_err_v);
	if (quot_v) gsl_vector_free(quot_v);
	if (alpha_range_v) gsl_vector_free(alpha_range_v);
	if (crit_deriv_v) gsl_vector_free(crit_deriv_v);
	if (crit_quot_v) gsl_vector_free(crit_quot_v);
	if (crit_lx_v) gsl_vector_free(crit_lx_v);
	if (crit_ly_v) gsl_vector_free(crit_ly_v);
	if (crit_l_v) gsl_vector_free(crit_l_v);
	if (aa_m) gsl_matrix_free(aa_m);
	if (l_m) gsl_matrix_free(l_m);
	if (ll_m) gsl_matrix_free(ll_m);
	return err;
}

int test_design_matrix(Tcl_Interp * interp, Tcl_Obj ** res, 
                       const gsl_matrix * a_m, const gsl_vector * x_v,
		       const gsl_vector * intensity_v)
{
	gsl_vector * y_v = NULL;
	int err = TCL_OK;

	SASFIT_ASSERT_VAL(interp && res && a_m && x_v && intensity_v, 
	                  TCL_ERROR);

	y_v = gsl_vector_alloc(a_m->size1);
	SASFIT_ASSERT_VAL(y_v, TCL_ERROR);

	if (!err) err = gsl_blas_dgemv(CblasNoTrans, 1.0, a_m, x_v, 0.0, y_v);
//	if (!err) err = gsl_vector_add_constant(y_v, 1e9);

	if (!err) err = tcl_result_add_list(interp, res, y_v);
	if (!err) {
		err = gsl_vector_sub(y_v, intensity_v);
		err = gsl_vector_div(y_v, intensity_v);
		sasfit_out("test_design_matrix(), avg. rel error: %g\n",
		           gsl_blas_dasum(y_v)/y_v->size);
	}
	gsl_vector_free(y_v);
	return err;
}

int calc_shape(double q, double param_val, sasfit_analytpar * ap, int num_ap, 
               gsl_vector_int * distrib_param_idx_v, double * res)
{
	size_t j;
	int idx, err;
	double val;

	SASFIT_ASSERT_VAL(ap && distrib_param_idx_v && res && 
	                  (distrib_param_idx_v->size == num_ap), 1);

	*res = 0.0;
	for (j=0; j < (size_t)num_ap; j++)
	{
		err = 0;
		idx = gsl_vector_int_get(distrib_param_idx_v, j);
		val = 1.0;
		// for invalid index, apply form factor with given parameters
		if (idx > -1 && idx < MAXPAR)
		{
			if (0) // if there is a size distrib
			{
				// a distribution was defined, apply it
				// to the specified form factor parameter 
				// (its value)
				val = sasfit_sd(ap[j].FF_l[idx], 
				                ap[j].SD_a,   // distribution parameter structure
				                &(ap[j].SD),  // distribution function structure
				                NULL, &err);
			} else {
				// set selected form factor parameter to 
				// given distribution value from
				// parameter_vector
				ap[j].FF_l[idx] = param_val;
			}
		}
		if (err) return 1;
		val *= sasfit_ff(q, ap[j].FF_l, &(ap[j].FF), NULL, &err);
//		printf("[q: %lg, x: %lg] ap: %d, idx: %d, val: %lg\n", q, param_val, j, idx, val);
		*res += val;
		if (err) return 1;
	}
//	*res /= (double) num_ap;
	return 0;
}

int set_design_matrix(gsl_matrix       * design_m, 
                      gsl_vector       * scattering_v, 
		      gsl_vector       * param_v,
		      gsl_vector       * weights_v,
		      sasfit_analytpar * ap,
		      int                num_ap)
{
	size_t row, col;
	int err_status = 0;
	double val;
	gsl_vector_int * idx_v = 0; // contains index of distribution parameter
	                            // for each contribution

	SASFIT_ASSERT_VAL(design_m && scattering_v && param_v && weights_v &&
	                  ap && (num_ap > 0), TCL_ERROR);
	
	sasfit_out("setting up design matrix, num APs: %d\n",num_ap);

	// get the indices of parameters whose distribution we want to know
	idx_v = gsl_vector_int_alloc(num_ap);
	err_status = get_distrib_idx(ap, num_ap, idx_v);

//	sasfit_out("index vector: \n");
//	gsl_vector_int_fprintf(stderr, idx_v, "%d");

	if (!err_status) 
	{
		// calculate all the matrix values
		for(row=0; row < scattering_v->size; row++) {
			for(col=0; col < param_v->size; col++)
			{
				// calculate shape (formfactor, known contributions)
				err_status |= calc_shape(gsl_vector_get(scattering_v, row),
							 gsl_vector_get(param_v, col),
							 ap, num_ap, idx_v, &val);
				// apply quadrature weights
				val = val * gsl_vector_get(weights_v, col);
				// set matrix entry
				gsl_matrix_set(design_m, row, col, val);
			}
		}
	}

	if (idx_v)     gsl_vector_int_free(idx_v);

	if (err_status)  return TCL_ERROR;
	return TCL_OK;
}

sasfit_core_DLLEXP int sasfit_sd_analysis_cmd(ClientData     clientData, 
                                              Tcl_Interp   * interp, 
                                              int            argc, 
                                              const char   * argv[])
                                            //  int            objc, 
                                            //  Tcl_Obj *CONST objv[])
{
	sasfit_analytpar * ap;
	int                num_ap;
	scalar             alambda;
	int                error_type;
	// scattering vector, intensities, error of intensities, residuals
	scalar             * q, * y, * e, * res;
	int                ndata; // number of vector elements
	int                err = TCL_OK; // err of GSL operations

	// temporary variables
	int i, mode;
	double tempval;

	// input data vector representation
	gsl_vector * scattering_v = 0;
	gsl_vector * intensity_v = 0;
	gsl_vector * error_v = 0;
	gsl_vector_view s_view;
	gsl_vector_view i_view;
	gsl_vector_view e_view;
	gsl_vector * param_v = 0; // vector of possible occurrences of 
	                         // a single model parameter (e.g. radius)
				 // i.e. sampling points
	gsl_vector * weights_v = 0; // quadrature weights of according 
	                            // sampling points
	gsl_matrix * design_m = 0; // desgin matrix of known information
	                           // about particle shape and background
	gsl_vector * distrib_v = 0; // parameter distribution solution vector

	// GUI variables
	int param_num = 112;
	double param_min = 0.0;
	double param_max = 20.0;

	// err object of this command
	Tcl_Obj * tcl_res = 0;

	sasfit_out("sasfit_sd_analysis_cmd()\n");

	if (argc < 3)
	{
		sasfit_err("Wrong number of arguments !\n"
		    "Should be: sasfit_sd_analysis ?analyt_par? ?xye_data?\n");
		return TCL_ERROR;
	}
	err = get_AP(interp, argv, &ap, &num_ap, &alambda, 
		                &error_type, &q, &y, &e,
				&res, &ndata);

	if (!err) err = sasfit_tcl_get_double(interp, &param_min, "inv_data", "param_min");
	if (!err) err = sasfit_tcl_get_double(interp, &param_max, "inv_data", "param_max");
	if (!err) err = sasfit_tcl_get_int(interp, &param_num, "inv_data", "param_num");
	sasfit_out("param min: %lg, param max: %lg, param num: %d\n",param_min, param_max, param_num);

	if (!err)
	{
		// get gsl-compatible vectors from supplied data arrays without copying
		s_view = gsl_vector_view_array(q, ndata);
		i_view = gsl_vector_view_array(y, ndata);
		e_view = gsl_vector_view_array(e, ndata);
		scattering_v = &s_view.vector;
		intensity_v = &i_view.vector;
		error_v = &e_view.vector;

		// output current vectors for debugging purposes
		// print_vector(scattering_v, "scattering vector: ");
		// print_vector(error_v, "error vector: ");
		// print_vector(intensity_v, "intensity vector: ");

		// get test distribution vector eventually
		get_test_distrib(interp, &distrib_v);
	//	sasfit_out("test distr vec: %lx\n", distrib_v);
		if (distrib_v) {
			//print_vector(distrib_v, "test distrib: ");
			param_num = (int)distrib_v->size;
		}
	}

	if (!err) {
		// create user-defined linear grid of parameter range
		err = create_sampling_range_lin(&param_v, &weights_v, param_min,
		                                   param_max, (size_t)param_num);
//		err = create_sampling_range_legendre(&param_v, &weights_v,
//		                                        param_min, param_max, 
//		                                        param_num);
	}

	if (param_v && weights_v && !err) {
		//gsl_vector_fprintf(stderr, param_v, "%lg");
	} else {
		err = TCL_ERROR;
	}

	if (!err)
	{
		design_m = gsl_matrix_alloc(ndata, (size_t)param_num);

		err = set_design_matrix(design_m, scattering_v, param_v, weights_v, ap, num_ap);
	//	print_matrix(design_m);
	}

	if (!err) err = sasfit_tcl_get_int(interp, &mode, "inv_data", "mode");
	if (!err && distrib_v && mode == 1) // test the design matrix
	{
		err = tcl_result_add_type(interp, &tcl_res, 1);
		if (!err) err = tcl_result_add_list(interp, &tcl_res, scattering_v);
		if (!err) {
			// test loaded distribution
			vector_integrate(param_v, weights_v, distrib_v, &tempval);
			printf("distrib integrate over param: %e\n", tempval);
			gsl_vector_scale(distrib_v, 1.0/tempval);
			err = test_design_matrix(interp, &tcl_res, design_m, 
			                            distrib_v, intensity_v);
		}
	}
	else if (!err) // do regularization
	{
		if (mode == 0) {
			err = tcl_result_add_type(interp, &tcl_res, 0);
			if (!err) err = tcl_result_add_list(interp, &tcl_res, param_v);
		}
		// scale input to improve values
		if (DBG) print_matrix(design_m, "Designmatrix before scaling: ", 2);
		if (!err) err = reg_scale_input(design_m, intensity_v);
		if (!err) err = regularize(interp, &tcl_res, design_m, 
		                           intensity_v, error_v, param_v, weights_v, mode);
	}
	//sasfit_out("error type: %d\n",error_type);

	if (!err && tcl_res) {
		err = Tcl_ListObjLength(interp, tcl_res, &i);
		if (!err && (i%3) == 0) {
			Tcl_SetObjResult(interp, tcl_res);
		}
	}

	cleanup(&ap, &q, &y, &e, &res, 
		&design_m, &param_v, &distrib_v, &weights_v);
	return err;
}

