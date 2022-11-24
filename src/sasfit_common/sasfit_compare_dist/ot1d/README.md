# OT1D: Discrete Optimal Transport in 1D by Linear Programming

[![build-update Actions Status](https://github.com/stegua/ot1d/workflows/build-update/badge.svg)](https://github.com/stegua/ot1d/actions)  [![Downloads](https://pepy.tech/badge/ot1d)](https://pepy.tech/project/ot1d) [![Downloads](https://pepy.tech/badge/ot1d/month)](https://pepy.tech/project/ot1d) [![Downloads](https://pepy.tech/badge/ot1d/week)](https://pepy.tech/project/ot1d)

The OT1D library offers a simple but efficient implementation of an algorithm to compute the Kantorovich-Wasserstein distance between two empirical measures defined in dimension 1, that is, the support points of the measures are in **R**.
We have designed the algorithm by directly exploiting the [Complementary slackness](https://en.wikipedia.org/wiki/Linear_programming#Complementary_slackness) conditions of Linear Programming. 
The implementation focuses more on efficiency than genericity, and we try to be as efficient as possible in several notable cases.
We implemented the core algorithm in standard ANSI C++11, and we provide a python3 wrapper, which can be installed with:

```bash
pip3 install ot1d
```

The **OT1D** library provides an implementation of Optimal Transport in 1D that is faster than:

1. [Scipy](https://www.scipy.org/): it is at least 6x faster than [scipy.stats.wasserstein_distance](https://docs.scipy.org/doc/scipy/reference/generated/scipy.stats.wasserstein_distance.html), but it can be up to 11x faster
2.  [POT](https://pythonot.github.io/): it is at least 2x faster than [ot.lp.wasserstein_1d](https://pythonot.github.io/gen_modules/ot.lp.html#ot.lp.wasserstein_1d), but it can be up to 7x faster

The real speedup will depend on your computer platform (i.e., numebr of cores), your OS, and compiler. For running a performance test on your computer, see below, or run the python script [OT1D_test](https://github.com/stegua/ot1d/blob/main/python/OT1D_test.py). For some strange reason, the speed ups on Mac laptops are larger than for other architectures.

**REMARK:** If you find instances where **OT1D** is slower, please, let us know.

### DotLIB
This tiny library is part of [dotlib](https://github.com/stegua/dotlib), a large project to develop Optimal Transport algorithms based on efficient Linear Programming implementations.

## Basic Usage: Colab Notebook

The simplest way to test this library is to run the following notebook on Colab:

| Data | Notebook | Link |
|:-|:-|:-|
|**[2021/06/21]**|*Testing and evaluating OT1D*|[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/stegua/ot1d/blob/main/notebooks/Testing_OT1D.ipynb)|


## Usage
The main function of the **OT1D** library is the following:

```python
z = OT1D(x, y, mu=None, nu=None, p=2, sorting=True, threads=8, plan=False)
```

The parameters of the function are:

* `x`: the support points of the first measure
* `y`: the support points of the second measure
* `mu`: the weights of the first measure. If equal to `None`, all the samples have the same mass.
* `nu`: the weights of the second measure. If equal to `None`, all the samples have the same mass.
* `p`: the order of the Wasserstein distance (p=1 or p=2)
* `sorting`: if equal to `True`, the function sorts the support points given in input
* `threads`: number of threads to use by the parallel sorting algorithm
* `plan`: if equal to `True` , the function returns the optimal transportation plan (see example [`interpolate.py`](https://github.com/stegua/ot1d/blob/main/python/interpolate.py))

The first four parameters can be given in input as numpy arrays (*preferred*) or python lists.

### Sorting at the speed of light
In addition, we expose the following in-place parallel sorting function:
```python
parasort(x, mu=None, threads=8)
```

The parameters of the function are:

* `x`: the support points of a given measure
* `mu`: the weights of the given measure. If equal to`None`, only the support points are sorted
* `threads`: number of threads to use by the parallel sorting algorithm

The first two parameters can be given in input as numpy arrays (*preferred*) or python lists.

## Details
Given two empirical distributions, the Kantorovich-Wasserstein distance is the given by optimal solution of a linear program, known as the transportation problem.
While this is a general linear program, when the costs are defined among points belonging to the real line, 
the problem can be solved with an algorithm having worst-case time complexity of *O(n log n)*.
This can be shown by writing first the dual linear program, and then the slackness condition.

The key step of the algorithm is sorting of the two arrays of support points *x* and *y*.
We sort the arrays by using a customized parallel sorting algorithm implemented in C++, which combines the very fast [pdqsort](https://github.com/orlp/pdqsort)
with [parasort](https://github.com/baserinia/parallel-sort). See the linked webpages for the license type of these two libraries.


### Prerequisities for compilation

You want to compile the source code and the python wrapper, you only need the following two standard python libraries:

* A C++ compiler compliant with the `C++11` standard.
* [cython](https://cython.org/)
* [numpy](https://numpy.org/)

You might need to install `python-dev` library, which on Linux can be installed by:

```bash
apt install python3-dev  # Ubuntu
```

### Installation

To install `OT1D` you can run the following command:

```bash
pip3 install ot1d
```

### Testing

For testing the library, you can run the following command:

```bash
python3 basic_test.py
```

The basic test snippet is the following:

```python
import numpy as np
from OT1D import OT1D, parasort

np.random.seed(13)

N = 1000000

# Uniform samples
x = np.random.uniform(1, 2, N)
y = np.random.uniform(0, 1, N)

z = OT1D(x, y, p=2, sorting=True, threads=16)

print('Wasserstein distance of order 2, W2(x,y) =', z)
```
and the output should be similar to:
```
Wasserstein distance of order 2, W2(x,y) = 1.0002549459050794
```

### Testing for Performance
These results can be reproduced running the following command (you need to have installed [scipy](https://scipy.org/) and [pot](https://pythonot.github.io/)):
```bash
python3 OT1D_test.py
```
which output is should be similar to the following (but it depends on your platform):
```
--------------- TEST 3: Unsorted input (average runtime) --------------------
For OT1D using 8 threads

running test . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

Testing W1, samples of deltas, n=m
Scipy: average time = 0.214 speedup = 11.0
POT  : average time = 0.122 speedup = 6.3O
OT1D : average time = 0.019 speedup = 1.0

Testing W2, samples of deltas, n=m
POT  : average time = 0.12 speedup = 6.1
OT1D : average time = 0.02 speedup = 1.0

Testing W1, samples with weights
Scipy: average time = 0.225 speedup = 7.7
POT  : average time = 0.121 speedup = 4.2
OT1D : average time = 0.029 speedup = 1.0

Testing W2, samples with weights
POT  : average time = 0.119 speedup = 4.1
OT1D : average time = 0.029 speedup = 1.0


--------------- TEST 4: Sorted input (average runtime) --------------------
For OT1D using 8 threads

running test . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

Parallel sorting: time = 0.023 sec

Testing W1, samples of deltas, n=m
Scipy: average time = 0.07 speedup = 11.4
POT  : average time = 0.043 speedup = 7.1
OT1D : average time = 0.006 speedup = 1.0

Testing W2, samples of deltas, n=m
POT  : average time = 0.042 speedup = 7.0
OT1D : average time = 0.006 speedup = 1.0

Testing W1, samples with weights
Scipy: average time = 0.078 speedup = 5.9
POT  : average time = 0.042 speedup = 3.1
OT1D : average time = 0.013 speedup = 1.0

Testing W2, samples with weights
POT  : average time = 0.039 speedup = 3.0
OT1D : average time = 0.013 speedup = 1.0```
```

Please, contact us by email if you encounter any issues.

### Author and maintainer
Stefano Gualandi, stefano.gualandi@gmail.com.

Maintainer: Stefano Gualandi <stefano.gualandi@gmail.com>
