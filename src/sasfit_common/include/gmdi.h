/*! \file gmdi.h */

/*
 * Copyright (C) 2013 Hong Xu
 * All rights reserved.
 *
 * This file is part of MultiDimInte.
 *
 * MultiDimInte is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * MultiDimInte is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with MultiDimInte.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef GMDI_H_
# define GMDI_H_

#include <gsl/gsl_integration.h>

/*!
 * The integration function of each dimension of the integration. Used in gmdi_handle_set_inte_func().
 */
typedef enum
{
    GMDI_INTE_FUNCTIONS_QNG = 1, /*!< [gsl_integration_qng](http://www.gnu.org/software/gsl/manual/html_node/QNG-non_002dadaptive-Gauss_002dKronrod-integration.html) */
    GMDI_INTE_FUNCTIONS_QAG, /*!< [gsl_integration_qag](http://www.gnu.org/software/gsl/manual/html_node/QAG-adaptive-integration.html#QAG-adaptive-integration) */
    GMDI_INTE_FUNCTIONS_QAGS, /*!< [gsl_integration_qags](https://www.gnu.org/software/gsl/doc/html/integration.html#qags-adaptive-integration-with-singularities */
    GDMI_INTE_FUNCTIONS_TANHSINH,
    GDMI_INTE_FUNCTIONS_LEBEDEV,
    GDMI_INTE_FUNCTIONS_OUURA_DE,
    GDMI_INTE_FUNCTIONS_OOURS_CC,
} GMDI_INTE_FUNCTIONS;

/*!
 * the type of gmdi_function_or_constant::type.
 */
typedef enum
{
    GMDI_FUNCTION_OR_CONSTANT_TYPE_CONSTANT = 1, /*!< Constant type. (double) */
    GMDI_FUNCTION_OR_CONSTANT_TYPE_FUNCTION,    /*!< Function type. (gsl_function) */
    GMDI_FUNCTION_OR_CONSTANT_TYPE_MULTI_VAR_FUNCTION,  /*!< Multivariate function type. (gmdi_multi_var_function) */
} GMDI_FUNCTION_OR_CONSTANT_TYPE;

/*!
 * Multivariate function. Similar to gsl_function, but for multivariate function.
 * */
typedef struct
{
    double (*function) (const double* v, size_t n, void* p); /*!< The pointer to the multivariate itself. */
    size_t   n;     /*!< Number of variables. */
    void*    params; /*!< Parameters to be passed in when calling the function. */
} gmdi_multi_var_function;

/*!
 * Used to specify a function or a constant.
 */
typedef struct
{
    GMDI_FUNCTION_OR_CONSTANT_TYPE  type; /*!< Type of the object (constant, function or multivariate function?) */
    union
    {
        gsl_function                gf; /*!< The function if the type is GMDI_FUNCTION_OR_CONSTANT_TYPE_FUNCTION */
        gmdi_multi_var_function     mf; /*!< The multivariate function if the type is
                                          GMDI_FUNCTION_OR_CONSTANT_TYPE_MULTI_VAR_FUNCTION */
        double                      c;  /*!< The constant if the type is GMDI_FUNCTION_OR_CONSTANT_TYPE_CONSTANT */
    } content; /*!< The content of the object. The type of content is specified by type. */
} gmdi_function_or_constant;

/*!
 * Handle of an integration.
 */
typedef void *      gmdi_inte_handle;

/*!
 * \brief Create an handle used for integration.
 *
 * \param n The number of dimensions of integration.
 *
 * \return The integration handle.
 */
gmdi_inte_handle gmdi_create_inte_handle(size_t n);

/*!
 * \brief Free the handle when it will be no longer used.
 *
 * \param handle The handle to free.
 *
 * \return None.
 */
void gmdi_free_inte_handle(gmdi_inte_handle * handle);

/*!
 * \brief Start doing the integration
 *
 * \param handle The handle of which we need to perform integration.
 *
 * \return The error code the integration. `GSL_SUCCESS` is returned if no error occurs.
 */
int gmdi_multi_dimensional_integration(gmdi_inte_handle handle);

/*!
 * \brief Set the key of the integration of the nth dimension
 *
 * \param handle The handle of the integration.
 *
 * \param key The value of the key to set, e.g. `GSL_INTEG_GAUSS61`, etc.
 *
 * \param n The dimension of the integration which this setting applies to.
 *
 * \return None.
 */
void gmdi_handle_set_key(gmdi_inte_handle handle, int key, size_t n);

/*!
 * \brief Set the epsabs of the integration of the nth dimension
 *
 * \param handle The handle of the integration.
 *
 * \param epsabs The value of the epsabs to set.
 *
 * \param n The dimension of the integration which this setting applies to.
 *
 * \return None.
 */
void gmdi_handle_set_epsabs(gmdi_inte_handle handle, double epsabs, size_t n);

/*!
 * \brief Set the epsrel of the integration of the nth dimension
 *
 * \param handle The handle of the integration.
 *
 * \param epsrel The value of the epsrel to set.
 *
 * \param n The dimension of the integration which this setting applies to.
 *
 * \return None.
 */
void gmdi_handle_set_epsrel(gmdi_inte_handle handle, double epsrel, size_t n);

/*!
 * \brief Set the limit of the integration of the nth dimension
 *
 * \param handle The handle of the integration.
 *
 * \param limit The value of the limit to set.
 *
 * \param n The dimension of the integration which this setting applies to.
 *
 * \return None.
 */
void gmdi_handle_set_limit(gmdi_inte_handle handle, double limit, size_t n);

/*!
 * \brief Set the inte_func of the integration of the nth dimension
 *
 * \param handle The handle of the integration.
 *
 * \param inte_func The value of the inte_func to set. Must be one of GMDI_INTE_FUNCTIONS.
 *
 * \param n The dimension of the integration which this setting applies to.
 *
 * \return None.
 */
void gmdi_handle_set_inte_func(gmdi_inte_handle handle, GMDI_INTE_FUNCTIONS inte_func, size_t n);

/*!
 * \brief Set the x0 of the integration of the nth dimension
 *
 * \param handle The handle of the integration.
 *
 * \param x0 The value of the x0 to set.
 *
 * \param n The dimension of the integration which this setting applies to.
 *
 * \return None.
 */
void gmdi_handle_set_x0(gmdi_inte_handle handle, const gmdi_function_or_constant * x0, size_t n);

/*!
 * \brief Set the x0 of the integration of the nth dimension as a constant.
 *
 * \param handle The handle of the integration.
 *
 * \param x0 The value of the x0 to set.
 *
 * \param n The dimension of the integration which this setting applies to.
 *
 * \return None.
 */
void gmdi_handle_set_x0_constant(gmdi_inte_handle handle, double x0, size_t n);

/*!
 * \brief Set the x1 of the integration of the nth dimension
 *
 * \param handle The handle of the integration.
 *
 * \param x1 The value of the x1 to set.
 *
 * \param n The dimension of the integration which this setting applies to.
 *
 * \return None.
 */
void gmdi_handle_set_x1(gmdi_inte_handle handle, const gmdi_function_or_constant * x1, size_t n);

/*!
 * \brief Set the x1 of the integration of the nth dimension as a constant.
 *
 * \param handle The handle of the integration.
 *
 * \param x1 The value of the x1 to set.
 *
 * \param n The dimension of the integration which this setting applies to.
 *
 * \return None.
 */
void gmdi_handle_set_x1_constant(gmdi_inte_handle handle, double x1, size_t n);

/*!
 * \brief Set the f of the integration of the nth dimension as a constant.
 *
 * \param handle The handle of the integration.
 *
 * \param f The value of the f to set.
 *
 * \param n The dimension of the integration which this setting applies to.
 *
 * \return None.
 */
void gmdi_handle_set_f(gmdi_inte_handle handle, const gmdi_multi_var_function * f, size_t n);

/*!
 * \brief Set the main integral kernel of the integration of the nth dimension as a constant.
 *
 * \param handle The handle of the integration.
 *
 * \param f The value of the integral kernel to set.
 *
 * \return None.
 */
void gmdi_handle_set_kernel(gmdi_inte_handle handle, const gmdi_multi_var_function * f);

/*!
 * \brief Obtain the result.
 *
 * \param handle The handle of the integration.
 *
 * \return The result.
 */
double gmdi_handle_get_result(gmdi_inte_handle handle);

/*!
 * \brief Obtain the abserr.
 *
 * \param handle The handle of the integration.
 *
 * \return The abserr.
 */
double gmdi_handle_get_abserr(gmdi_inte_handle handle);

/*!
 * \brief Obtain the version number of the library currently in use. You can pass in a NULL pointer if you do not need a
 * specific version number.
 *
 * \param major Major version number.
 *
 * \param minor Minor version number.
 *
 * \param subminor Subminor version number
 *
 * \return None.
 */
void gmdi_get_version(int * major, int * minor, int * subminor);

#endif /* GMDI_H_ */
