SAPHRON
==============

**SAPHRON** (**S**tatistical **A**pplied **PH**ysics through **R**andomized **O**n-the-fly **N**umerics) is a modern, open-source, light-weight C++11 based Monte Carlo physics package. It is designed to be easy to use, extendable and extremely versatile. As the name suggests, one of the primary features is the inclusion of various Density-of-States (DOS) sampling algorithms. **SAPHRON** is currently in early development, and as a result it currently lacks many basic features; this README is also likely to be out of date. In addition to the C++ application, a number of helper MATLAB scripts are included to assist with data analysis. 

**The documentation can be found [here](https://hsidky.github.io/SAPHRON)**

<a id="features"></a>
## Features
- *A very simple API* that is user and developer friendly
- Lattice and off-lattice simulations.
- Anisotropic potentials including Gay-Berne.
- Arbitrary ensembles defined by collections of moves.
- OpenMP parallelization.
- Multi-walker Density of states (DOS) flat-histogram sampling along many order parameters.
- Much more!

Check out the documentation for a more detailed list of features

https://hsidky.github.io/SAPHRON

<a id="dependencies"></a>
## Dependencies
SAPHRON requires some linear alegra routines to get going. It's easy enough to install the required packages. On any Debian-based distro simply type:

```bash
$ sudo apt-get install libblas3 liblapack3 libarmadillo-dev
```
BLAS and LAPACK are the default linear algebra providers, though **SAPHRON** supports the Intel MKL and searches for it. If found it will use it instead of LAPACK, so you don't need to do anything. [Armadillo](http://arma.sourceforge.net/) provides C++ headers for interfacing with the libraries.

##### To get SAPHRON to compile it is crucial that you perform the following step

By default, Armadillo assumes you want to link against its shared library. SAPHRON does not do this, but instead links directly to BLAS and LAPACK or MKL as mentioned above. To fix this, use your editor of choice (here we use vim) to modify the Armadillo config header

```bash
$ vim /usr/include/armadillo_bits/config.hpp
```

Simply comment out the line that says `#define ARMA_USE_WRAPPER` and you're good to go!

##### OpenMP 

SAPHRON uses OpenMP to parallelize energy and neighbor list calculations. It relies on compilers that support the `-fopenmp` flag. Any recent C++11 compatible compiler should support this, including GCC 4.9.x at **minimum** (required for regex support) and Clang 3.4+.

##### Additional information

If you wish to link **SAPHRON** against the static BLAS/LAPACK libraries you can install those instead. However, you will also need to install `libgfortran` and `libquadmath`. **SAPHRON** will look for and link against them as well.

<a id="installation"></a>
## Installation
The first step is to clone the repository locally.

```bash
$ git clone https://github.com/hsidky/SAPHRON.git
```
**SAPHRON** uses a CMake build system. To build on a UNIX-style platform enter the following commands

```bash
$ cd SAPHRON
$ mkdir build && cd build
$ cmake .. 
$ make
$ sudo make install
```
This will install a `saphron` binary into `/usr/local/bin` and a static library in `/usr/local/lib/libsaphron.a`.

<a id="testing"></a>
## Testing

**SAPHRON** uses the [Google C++ Testing Framework](https://code.google.com/p/googletest/) for unit testing. It is downloaded as part of the build process. To run the unit tests after building, type

```bash
$ make test
```
<a id="acknowledgements"></a>
## Acknowledgements 

**SAPHRON** utilizes the following packages: 

Armadillo (http://arma.sourceforge.net/)

JSONCpp (https://github.com/open-source-parsers/jsoncpp)

Sitmo Random Number Generator (http://www.sitmo.com/article/multi-threaded-random-number-generation-in-c11/)

**SAPHRON** would also not be possible without the support and expertise of [Eliseo Marin-Rimoldi](https://github.com/emarinri), developer of [Cassandra](http://cassandra.nd.edu/).

<a id="contributing"></a>
## Contributing

Feel free to fork this project on [GitHub](https://github.com/hsidky/SAPHRON). Any pull-requests, feature requests or other form of contributions are welcome.
