int *abscissa_level_closed_nd ( int level_max, int dim_num, int test_num, 
  int test_val[] );
double cc_abscissa ( int order, int i );
double *cc_weights ( int n );
int index_to_level_closed ( int dim_num, int t[], int order, int level_max );
void level_to_order_closed ( int dim_num, int level[], int order[] );
double monomial_int01 ( int dim_num, int expon[] );
double monomial_quadrature ( int dim_num, int expon[], int point_num, 
  double w[], double x[] );
double *monomial_value ( int dim_num, int point_num, double x[], int expon[] );
int *multigrid_index0 ( int dim_num, int order_1d[], int order_nd );
void multigrid_scale_closed ( int dim_num, int order_nd, int level_max, 
  int level_1d[], int grid_index[] );
double *product_weights_cc ( int dim_num, int order_1d[], int order_nd );
void sparse_grid_cc ( int dim_num, int level_max, int point_num, 
  double grid_weight[], double grid_point[] );
int *sparse_grid_cc_index ( int dim_num, int level_max, int point_num );
int sparse_grid_cfn_size ( int dim_num, int level_max );
int sparse_grid_cc_size_old ( int dim_num, int level_max );
void sparse_grid_cc_weights ( int dim_num, int level_max, int point_num, 
  int grid_index[], double grid_weight[] );
int sparse_grid_ccs_size ( int dim_num, int level_max );

