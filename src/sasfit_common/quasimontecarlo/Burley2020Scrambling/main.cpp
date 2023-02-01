#include <cstdlib>
#include <cstdint>
#include <vector>
#include <stdio.h>
#include "genpoints.h"

void help()
{
    printf("Usage: genpoints [seq] [N=16] [dim=0] [seed=1]\n"
           "seq is one of:\n"
           "   random\n"
           "   sobol\n"
           "   sobol_rds\n"
           "   sobol_owen\n"
           "   laine_karras\n"
           "   faure05\n");
 exit(1);
}

int main(int argc, const char** argv)
{
    argc--; argv++;

    if (argc < 1) help();
    const char* seq = *argv++;
    int n = argc < 2 ? 16 : std::atoi(*argv++);
    int dim = argc < 3 ? 0 : std::atoi(*argv++);
    int seed = argc < 4 ? 1 : std::atoi(*argv++);

    if (n < 0) n = 0;
    if (dim < 0 || dim > 4) dim = 0;

    std::vector<double> x(n);
    genpoints(seq, n, dim, seed, x.data());
    for (int i = 0; i < n; i++) {
        printf("%lg\n", x[i]);
    }
    return 0;
}
