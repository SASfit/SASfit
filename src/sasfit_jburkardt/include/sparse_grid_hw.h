int cce_order ( int l );
int ccl_order ( int l );
int ccs_order ( int l );
void cc ( int n, double x[], double w[] );
double cpu_time ( );
double fn_integral ( int d );
double *fn_value ( int d, int n, double x[] );
double fu_integral ( int d );
double *fu_value ( int d, int n, double x[] );
int *get_seq ( int d, int norm, int seq_num );
void gqn ( int n, double x[], double w[] );
int gqn_order ( int l );
int gqn2_order ( int l );
void gqu ( int n, double x[], double w[] );
int gqu_order ( int l );
void kpn ( int n, double x[], double w[] );
int kpn_order ( int l );
void kpu ( int n, double x[], double w[] );
int kpu_order ( int l );
int num_seq ( int n, int k );
void nwspgr ( void rule ( int n, double x[], double w[] ), 
  int rule_order ( int l ), int dim, int k, int r_size, int *s_size, 
  double nodes[], double weights[] );
int nwspgr_size ( int rule_order ( int l ), int dim, int k );
void quad_rule_print ( int m, int n, double x[], double w[], char *title );


void rule_adjust ( double a, double b, double c, double d, int n, double x[], 
  double w[] );
void rule_sort ( int m, int n, double x[], double w[] );