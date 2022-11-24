/*
 * @fileoverview Copyright (c) 2021, Stefano Gualandi,
 *               via Ferrata, 5, I-27100, Pavia, Italy
 *
 * @author stefano.gualandi@gmail.com (Stefano Gualandi)
 *
 *
 * External libraries:
 * - Super fast sorting algorithm (C++11) 
 *    https://github.com/orlp/pdqsort
 *    (Zlib license)
 *
 * - Efficient parallel sorting algorithm (C++11)
 *    https://github.com/baserinia/parallel-sort
 *    (GPL-3.0 License)
 *    
 *
 * NOTE: 
 *   - https://encyclopediaofmath.org/index.php?title=Wasserstein_metric
 *   - https://www.euroscipy.org/2018/descriptions/Detecting%20anomalies%20using%20statistical%20distances.html
 *   - https://www.kaggle.com/nhan1212/some-statistical-distances/comments
 *   - https://www.datadoghq.com/blog/engineering/robust-statistical-distances-for-machine-learning/
 */

#pragma once

#include "math.h"

#include <vector>

// Super fast sorting algorithm (C++11)
// https://github.com/orlp/pdqsort
#include "pdqsort.h"

// Efficient parallel sorting algorithm (C++11)
// https://github.com/baserinia/parallel-sort
#include "parasort.h"

typedef std::pair<double, double>  dbl_pair_t;

struct dbl_idx_t
{
    dbl_idx_t() : xi(0.0), i(-1) {}
    dbl_idx_t(double _x, int _i) : xi(_x), i(_i) {}

    double xi;
    int i;

    const bool operator < ( const dbl_idx_t &r ) const {
        return xi < r.xi;
    }
};


struct dbl_pair_idx_t
{
    dbl_pair_idx_t() : xi(0.0), ai(0.0), i(-1) {}
    dbl_pair_idx_t(double _x, double _a, int _i) : xi(_x), ai(_a), i(_i) {}

    double xi;
    double ai;
    int i;

    const bool operator < ( const dbl_pair_idx_t &r ) const {
        if ( xi < r.xi ) return true;
        if ( xi > r.xi ) return false;
        return ai < r.ai;
    }
};
    
// Utilities: fast sort
void _parasort(int n, double* x, int threads=8) {
    parasort(n, x, threads);
}

void _parasort_mu(int n, double* x, double *a, int threads=8) {
    std::vector<dbl_pair_t> mu;
    mu.reserve(n);

    for (int i = 0; i < n; i++)
        mu.emplace_back(x[i], a[i]);
    
    parasort_mu(n, &mu[0], [](const dbl_pair_t& v, const dbl_pair_t& w) {
        return v.first < w.first;
    }, threads);

    for (int i = 0; i < n; i++) {
        x[i] = mu[i].first;
        a[i] = mu[i].second;
    }
}

// Wasserstein-1D, p=1, no weights, m=n (same size)
double OT1Da0(int n, double* x0, double* y0, bool sorting, int threads=8) {
    double z = 0.0;

    // Copy input vector
    double* x; 
    double* y;
    x = (double*)malloc(n*sizeof(double));
    y = (double*)malloc(n*sizeof(double));
    memcpy(x, x0, n*sizeof(double));
    memcpy(y, y0, n*sizeof(double));
    
    // It uses a fast parallel sort
    if (sorting) {
        if (threads == 1) {
            pdqsort(x, x+n);
            pdqsort(y, y+n);
        } else {
            parasort(n, x, threads);
            parasort(n, y, threads);
        }
    }

    for (int i = 0; i < n; i++)
        z += fabs(x[i] - y[i]);

    // Free local memory
    free(x);
    free(y);

    return z/n;
}

// Wasserstein-1D, p=1, no weights, m != n
double OT1Da(int m, int n, double* x0, double* y0, bool sorting, int threads=8) {
    int i = 0;
    int j = 0;
    double a = 1.0/m;
    double b = 1.0/n;
    double z = 0.0;
    double d = 0;

    // Copy input vector
    double* x; 
    double* y;
    x = (double*)malloc(m*sizeof(double));
    y = (double*)malloc(n*sizeof(double));
    memcpy(x, x0, m*sizeof(double));
    memcpy(y, y0, n*sizeof(double));
    
    // It uses a fast parallel sort
    if (sorting) {
        if (threads == 1) {
            pdqsort(x, x+m);
            pdqsort(y, y+n);
        } else {
            parasort(m, x, threads);
            parasort(n, y, threads);
        }
    }

    while (i < m && j < n) {
        d = fabs(x[i] - y[j]);
        if (a == b) {
            z += a*d;
            a = 1.0/m;
            b = 1.0/n;
            i = i+1;
            j = j+1;
        } else {
            if (a > b) {
                z += b*d;
                a = a - b;
                b = 1.0/n;
                j = j + 1;
            } else {
                z += a*d;
                b = b - a;
                a = 1.0/m;
                i = i + 1;
            }
        }
    }

    // Free local memory
    free(x);
    free(y);

    return z;
}

// Wasserstein-1D, p=2, no weights, m=n (same size)
double OT1Db0(int n, double* x0, double* y0, bool sorting, int threads=8) {
    double z = 0.0;

    // Copy input vector
    double* x; 
    double* y;
    x = (double*)malloc(n*sizeof(double));
    y = (double*)malloc(n*sizeof(double));
    memcpy(x, x0, n*sizeof(double));
    memcpy(y, y0, n*sizeof(double));
    
    // It uses a fast parallel sort
    if (sorting) {
        if (threads == 1) {
            pdqsort(x, x+n);
            pdqsort(y, y+n);
        } else {
            parasort(n, x, threads);
            parasort(n, y, threads);
        }
    }
    
    for (int i = 0; i < n; i++)
        z += (x[i] - y[i])*(x[i] - y[i]);

    // Free local memory
    free(x);
    free(y);

    return sqrt(z/n);
}

// Wasserstein-1D, p=2, no weights
double OT1Db(int m, int n, double* x0, double* y0, bool sorting, int threads=8) {
    int i = 0;
    int j = 0;
    double a = 1.0/m;
    double b = 1.0/n;
    double z = 0.0;
    double d = 0;

    // Copy input vector
    double* x; 
    double* y;
    x = (double*)malloc(m*sizeof(double));
    y = (double*)malloc(n*sizeof(double));
    memcpy(x, x0, m*sizeof(double));
    memcpy(y, y0, n*sizeof(double));

    // It uses a fast parallel sort
    if (sorting) {
        if (threads == 1) {
            pdqsort(x, x+m);
            pdqsort(y, y+n);
        } else {
            parasort(n, x, threads);
            parasort(n, y, threads);
        }
    }
    
    while (i < m && j < n) {
        d = (x[i] - y[j])*(x[i] - y[j]);
        if (a == b) {
            z += a*d;
            i = i+1;
            j = j+1;
            a = 1.0/m;
            b = 1.0/n;
        } else {
            if (a > b) {
                z += b*d;
                a = a - b;
                b = 1.0/n;
                j = j + 1;
            } else {
                z += a*d;
                b = b - a;
                a = 1.0/m;
                i = i + 1;
            }
        }
    }

    // Free local memory
    free(x);
    free(y);

    return sqrt(z);
}


// Wasserstein-1D, p=1, with weights
double OT1Dc(int m, int n, double* x, double* y, double* a, double* b, bool sorting, int threads=8) {
    std::vector<dbl_pair_t> mu;
    std::vector<dbl_pair_t> nu;
    mu.reserve(m);
    nu.reserve(n);

    for (int i = 0; i < m; i++)
        mu.emplace_back(x[i], a[i]);
    for (int j = 0; j < n; j++)
        nu.emplace_back(y[j], b[j]);

    if (sorting) {
        if (threads == 1) {
            pdqsort(mu.begin(), mu.end(), [](const dbl_pair_t& v, const dbl_pair_t& w) {
                return v.first < w.first;
            });
            pdqsort(nu.begin(), nu.end(), [](const dbl_pair_t& v, const dbl_pair_t& w) {
                return v.first < w.first;
            });
        } else {
            parasort_mu(m, &mu[0], [](const dbl_pair_t& v, const dbl_pair_t& w) {
                return v.first < w.first;
            }, threads);

            parasort_mu(n, &nu[0], [](const dbl_pair_t& v, const dbl_pair_t& w) {
                return v.first < w.first;
            }, threads);
        }
    }
    
    int i = 0;
    int j = 0;
    double z = 0.0;
    double d = 0;
    double X = mu[i].first;
    double A = mu[i].second;
    double Y = nu[j].first;
    double B = nu[j].second;
        
    while (i < m && j < n) {
        d = fabs(X - Y);
        if (A == B) {
            z += A*d;
            i = i+1;
            j = j+1;
            X = mu[i].first;
            A = mu[i].second;
            Y = nu[j].first;
            B = nu[j].second;
        } else {
            if (A > B) {
                z += B*d;
                A = A - B;
                j = j + 1;
                Y = nu[j].first;
                B = nu[j].second;
            } else {
                z += A*d;
                B = B - A;
                i = i + 1;
                X = mu[i].first;
                A = mu[i].second;
            }
        }
    }

    return z;
}

// Wasserstein-1D, p=2, with weights
double OT1Dd(int m, int n, double* x, double* y, double* a, double* b, bool sorting, int threads=8) {
    std::vector<dbl_pair_t> mu;
    std::vector<dbl_pair_t> nu;
    mu.reserve(m);
    nu.reserve(n);

    for (int i = 0; i < m; i++)
        mu.emplace_back(x[i], a[i]);
    for (int j = 0; j < n; j++)
        nu.emplace_back(y[j], b[j]);

    if (sorting) {
        if (threads == 1) {
            pdqsort(mu.begin(), mu.end(), [](const dbl_pair_t& v, const dbl_pair_t& w) {
                return v.first < w.first;
            });
            pdqsort(nu.begin(), nu.end(), [](const dbl_pair_t& v, const dbl_pair_t& w) {
                return v.first < w.first;
            });
        } else {
            parasort_mu(m, &mu[0], [](const dbl_pair_t& v, const dbl_pair_t& w) {
                return v.first < w.first;
            }, threads);

            parasort_mu(n, &nu[0], [](const dbl_pair_t& v, const dbl_pair_t& w) {
                return v.first < w.first;
            }, threads);
        }
    }

    int i = 0;
    int j = 0;
    double z = 0.0;
    double d = 0;
    double X = mu[i].first;
    double A = mu[i].second;
    double Y = nu[j].first;
    double B = nu[j].second;
        
    while (i < m && j < n) {
        d = (X - Y)*(X - Y);
        if (A == B) {
            z += A*d;
            i = i+1;
            j = j+1;
            X = mu[i].first;
            A = mu[i].second;
            Y = nu[j].first;
            B = nu[j].second;
        } else {
            if (A > B) {
                z += B*d;
                A = A - B;
                j = j + 1;
                Y = nu[j].first;
                B = nu[j].second;
            } else {
                z += A*d;
                B = B - A;
                i = i + 1;
                X = mu[i].first;
                A = mu[i].second;
            }
        }
    }

    return sqrt(z);
}

// Wasserstein-1D, p=1, without weights, n=m
double OT1Da0_withplan(int n, double* x, double* y, int* map, double* mass, int* map_size, bool sorting, int threads=8) {
    std::vector<dbl_idx_t> mu;
    std::vector<dbl_idx_t> nu;
    mu.reserve(n);
    nu.reserve(n);

    for (int i = 0; i < n; i++)
        mu.emplace_back(x[i], i);
    for (int j = 0; j < n; j++)
        nu.emplace_back(y[j], j);

    if (sorting) {
        if (threads == 1) {
            pdqsort(mu.begin(), mu.end(), [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            });
            pdqsort(nu.begin(), nu.end(), [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            });
        } else {
            parasort_mu(n, &mu[0], [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            }, threads);

            parasort_mu(n, &nu[0], [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            }, threads);
        }
    }
    
    double z = 0.0;
    double a = 1.0/n;
    
    double X = 0.0;
    double Y = 0.0;
        
    int* pi = (int*)malloc(2*n * sizeof(int));
    double* piw = (double*)calloc(n, sizeof(double));

    int idx = 0;
    int wdx = 0;

    for(int i = 0; i < n; i++) {
        X = mu[i].xi;
        Y = nu[i].xi;
 
        z += fabs(X - Y);

        pi[idx] = mu[i].i;    // From
        pi[idx+1] = nu[i].i;  // To
        piw[wdx++] = a;       // Quantity of mass transported

        idx += 2;
   }

    // Copy back the solutions
    memcpy(map,  pi,  idx*sizeof(int));
    memcpy(mass, piw, wdx*sizeof(double));
    map_size[0] = wdx;

    free(pi);
    free(piw);

    return z/n;
}

// Wasserstein-1D, p=2, without weights, n=m
double OT1Db0_withplan(int n, double* x, double* y, int* map, double* mass, int* map_size, bool sorting, int threads=8) {
    std::vector<dbl_idx_t> mu;
    std::vector<dbl_idx_t> nu;
    mu.reserve(n);
    nu.reserve(n);

    for (int i = 0; i < n; i++)
        mu.emplace_back(x[i], i);
    for (int j = 0; j < n; j++)
        nu.emplace_back(y[j], j);

    if (sorting) {
        if (threads == 1) {
            pdqsort(mu.begin(), mu.end(), [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            });
            pdqsort(nu.begin(), nu.end(), [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            });
        } else {
            parasort_mu(n, &mu[0], [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            }, threads);

            parasort_mu(n, &nu[0], [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            }, threads);
        }
    }
    
    double z = 0.0;
    double a = 1.0/n;
    
    double X = 0.0;
    double Y = 0.0;
        
    int* pi = (int*)malloc(2*n * sizeof(int));
    double* piw = (double*)calloc(n, sizeof(double));

    int idx = 0;
    int wdx = 0;

    for(int i = 0; i < n; i++) {
        X = mu[i].xi;
        Y = nu[i].xi;

        z += (X - Y)*(X - Y);

        pi[idx] = mu[i].i;    // From
        pi[idx+1] = nu[i].i;  // To
        piw[wdx++] = a;       // Quantity of mass transported

        idx += 2;
    }

    // Copy back the solutions
    memcpy(map,  pi,  idx*sizeof(int));
    memcpy(mass, piw, wdx*sizeof(double));
    map_size[0] = wdx;

    free(pi);
    free(piw);

    return sqrt(z/n);
}

// Wasserstein-1D, p=1, without weights
double OT1Da_withplan(int m, int n, double* x, double* y, int* map, double* mass, int* map_size, bool sorting, int threads=8) {
    std::vector<dbl_idx_t> mu;
    std::vector<dbl_idx_t> nu;
    mu.reserve(m);
    nu.reserve(n);

    for (int i = 0; i < m; i++)
        mu.emplace_back(x[i], i);
    for (int j = 0; j < n; j++)
        nu.emplace_back(y[j], j);

    if (sorting) {
        if (threads == 1) {
            pdqsort(mu.begin(), mu.end(), [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            });
            pdqsort(nu.begin(), nu.end(), [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            });
        } else {
            parasort_mu(m, &mu[0], [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            }, threads);

            parasort_mu(n, &nu[0], [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            }, threads);
        }
    }

    int i = 0;
    int j = 0;
    
    double z = 0.0;
    double d = 0;
    double a = 1.0/m;
    double b = 1.0/n;
    
    double X = mu[i].xi;
    double Y = nu[j].xi;
        
    int* pi = (int*)malloc(2*(n+m-1) * sizeof(int));
    double* piw = (double*)calloc(n+m-1, sizeof(double));

    int idx = 0;
    int wdx = 0;

    while (i < m && j < n) {
        d = fabs(X - Y);
        pi[idx] = mu[i].i;    // From
        pi[idx+1] = nu[j].i;  // To
        idx += 2;

        if (a == b) {
            z += a*d;
            piw[wdx++] = a;   // Quantity of mass transported

            i = i+1;
            j = j+1;
            X = mu[i].xi;
            Y = nu[j].xi;
            a = 1.0/m;
            b = 1.0/n;
        } else {
            if (a > b) {
                z += b*d;
                piw[wdx++] = b; // Quantity of mass transported

                a = a - b;
                j = j + 1;
                Y = nu[j].xi;
                b = 1.0/n;
            } else {
                z += a*d;
                piw[wdx++] = a; // Quantity of mass transported

                a = a - a;
                i = i + 1;
                X = mu[i].xi;
                a = 1.0/m;
            }
        }
    }

    // Copy back the solutions
    memcpy(map,  pi,  idx*sizeof(int));
    memcpy(mass, piw, wdx*sizeof(double));
    map_size[0] = wdx;

    free(pi);
    free(piw);

    return z;
}

// Wasserstein-1D, p=2, without weights
double OT1Db_withplan(int m, int n, double* x, double* y, int* map, double* mass, int* map_size, bool sorting, int threads=8) {
    std::vector<dbl_idx_t> mu;
    std::vector<dbl_idx_t> nu;
    mu.reserve(m);
    nu.reserve(n);

    for (int i = 0; i < m; i++)
        mu.emplace_back(x[i], i);
    for (int j = 0; j < n; j++)
        nu.emplace_back(y[j], j);

    if (sorting) {
        if (threads == 1) {
            pdqsort(mu.begin(), mu.end(), [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            });
            pdqsort(nu.begin(), nu.end(), [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            });
        } else {
            parasort_mu(m, &mu[0], [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            }, threads);

            parasort_mu(n, &nu[0], [](const dbl_idx_t& v, const dbl_idx_t& w) {
                return v.xi < w.xi;
            }, threads);
        }
    }

    int i = 0;
    int j = 0;
    
    double z = 0.0;
    double d = 0;
    double a = 1.0/m;
    double b = 1.0/n;
    
    double X = mu[i].xi;
    double Y = nu[j].xi;
        
    int* pi = (int*)malloc(2*(n+m-1) * sizeof(int));
    double* piw = (double*)calloc(n+m-1, sizeof(double));

    int idx = 0;
    int wdx = 0;

    while (i < m && j < n) {
        d = (X - Y)*(X - Y);
        pi[idx] = mu[i].i;    // From
        pi[idx+1] = nu[j].i;  // To
        idx += 2;

        if (a == b) {
            z += a*d;
            piw[wdx++] = a;   // Quantity of mass transported

            i = i+1;
            j = j+1;
            X = mu[i].xi;
            Y = nu[j].xi;
            a = 1.0/m;
            b = 1.0/n;
        } else {
            if (a > b) {
                z += b*d;
                piw[wdx++] = b; // Quantity of mass transported

                a = a - b;
                j = j + 1;
                Y = nu[j].xi;
                b = 1.0/n;
            } else {
                z += a*d;
                piw[wdx++] = a; // Quantity of mass transported

                a = a - a;
                i = i + 1;
                X = mu[i].xi;
                a = 1.0/m;
            }
        }
    }

    // Copy back the solutions
    memcpy(map,  pi,  idx*sizeof(int));
    memcpy(mass, piw, wdx*sizeof(double));
    map_size[0] = wdx;

    free(pi);
    free(piw);

    return sqrt(z);
}

// Wasserstein-1D, p=1, with weights
double OT1Dc_withplan(int m, int n, double* x, double* y, double* a, double* b, int* map, double* mass, int* map_size, bool sorting, int threads=8) {
    std::vector<dbl_pair_idx_t> mu;
    std::vector<dbl_pair_idx_t> nu;
    mu.reserve(m);
    nu.reserve(n);

    for (int i = 0; i < m; i++)
        mu.emplace_back(x[i], a[i], i);
    for (int j = 0; j < n; j++)
        nu.emplace_back(y[j], b[j], j);

    if (sorting) {
        if (threads == 1) {
            pdqsort(mu.begin(), mu.end(), [](const dbl_pair_idx_t& v, const dbl_pair_idx_t& w) {
                return v.xi < w.xi;
            });
            pdqsort(nu.begin(), nu.end(), [](const dbl_pair_idx_t& v, const dbl_pair_idx_t& w) {
                return v.xi < w.xi;
            });
        } else {
            parasort_mu(m, &mu[0], [](const dbl_pair_idx_t& v, const dbl_pair_idx_t& w) {
                return v.xi < w.xi;
            }, threads);

            parasort_mu(n, &nu[0], [](const dbl_pair_idx_t& v, const dbl_pair_idx_t& w) {
                return v.xi < w.xi;
            }, threads);
        }
    }

    int i = 0;
    int j = 0;
    double z = 0.0;
    double d = 0;
    double X = mu[i].xi;
    double A = mu[i].ai;
    double Y = nu[j].xi;
    double B = nu[j].ai;
        
    int* pi = (int*)malloc(2*(n+m-1) * sizeof(int));
    double* piw = (double*)calloc(n+m-1, sizeof(double));

    int idx = 0;
    int wdx = 0;

    while (i < m && j < n) {
        d = fabs(X - Y);
        pi[idx] = mu[i].i;    // From
        pi[idx+1] = nu[j].i;  // To
        idx += 2;

        if (A == B) {
            z += A*d;
            piw[wdx++] = A;   // Quantity of mass transported

            i = i+1;
            j = j+1;
            X = mu[i].xi;
            A = mu[i].ai;
            Y = nu[j].xi;
            B = nu[j].ai;
        } else {
            if (A > B) {
                z += B*d;
                piw[wdx++] = B; // Quantity of mass transported

                A = A - B;
                j = j + 1;
                Y = nu[j].xi;
                B = nu[j].ai;
            } else {
                z += A*d;
                piw[wdx++] = A; // Quantity of mass transported

                B = B - A;
                i = i + 1;
                X = mu[i].xi;
                A = mu[i].ai;
            }
        }
    }

    // Copy back the solutions
    memcpy(map,  pi,  idx*sizeof(int));
    memcpy(mass, piw, wdx*sizeof(double));
    map_size[0] = wdx;

    free(pi);
    free(piw);

    return z;
}


// Wasserstein-1D, p=2, with weights
double OT1Dd_withplan(int m, int n, double* x, double* y, double* a, double* b, int* map, double* mass, int* map_size, bool sorting, int threads=8) {
    std::vector<dbl_pair_idx_t> mu;
    std::vector<dbl_pair_idx_t> nu;
    mu.reserve(m);
    nu.reserve(n);

    for (int i = 0; i < m; i++)
        mu.emplace_back(x[i], a[i], i);
    for (int j = 0; j < n; j++)
        nu.emplace_back(y[j], b[j], j);

    if (sorting) {
        if (threads == 1) {
            pdqsort(mu.begin(), mu.end(), [](const dbl_pair_idx_t& v, const dbl_pair_idx_t& w) {
                return v.xi < w.xi;
            });
            pdqsort(nu.begin(), nu.end(), [](const dbl_pair_idx_t& v, const dbl_pair_idx_t& w) {
                return v.xi < w.xi;
            });
        } else {
            parasort_mu(m, &mu[0], [](const dbl_pair_idx_t& v, const dbl_pair_idx_t& w) {
                return v.xi < w.xi;
            }, threads);

            parasort_mu(n, &nu[0], [](const dbl_pair_idx_t& v, const dbl_pair_idx_t& w) {
                return v.xi < w.xi;
            }, threads);
        }
    }

    int i = 0;
    int j = 0;
    double z = 0.0;
    double d = 0;
    double X = mu[i].xi;
    double A = mu[i].ai;
    double Y = nu[j].xi;
    double B = nu[j].ai;
        
    int* pi = (int*)malloc(2*(n+m-1) * sizeof(int));
    double* piw = (double*)calloc(n+m-1, sizeof(double));

    int idx = 0;
    int wdx = 0;

    while (i < m && j < n) {
        d = (X - Y)*(X - Y);
        pi[idx] = mu[i].i;    // From
        pi[idx+1] = nu[j].i;  // To
        idx += 2;

        if (A == B) {
            z += A*d;
            piw[wdx++] = A;   // Quantity of mass transported

            i = i+1;
            j = j+1;
            X = mu[i].xi;
            A = mu[i].ai;
            Y = nu[j].xi;
            B = nu[j].ai;
        } else {
            if (A > B) {
                z += B*d;
                piw[wdx++] = B; // Quantity of mass transported

                A = A - B;
                j = j + 1;
                Y = nu[j].xi;
                B = nu[j].ai;
            } else {
                z += A*d;
                piw[wdx++] = A; // Quantity of mass transported

                B = B - A;
                i = i + 1;
                X = mu[i].xi;
                A = mu[i].ai;
            }
        }
    }

    // Copy back the solutions
    memcpy(map,  pi,  idx*sizeof(int));
    memcpy(mass, piw, wdx*sizeof(double));
    map_size[0] = wdx;

    free(pi);
    free(piw);

    return sqrt(z);
}
