SAPHRON
==============

`SAPHRON` (**S**tatistical **A**pplied **PH**ysics through **R**andom **O**n-the-fly **N**umerization) is an open-source light-weight C++11 based Monte Carlo physics library. Its designed to be easy to use - providing a clean, extendable API with usage examples. Unlike most other packages, `SAPHRON` is not a standalone application. However, it is very simple to create usable applications, and many examples are provided. The library is currently in early develpoment, and as a result it currently lacks many basic features (namely off-lattice models). In addition to the C++ library, a number of helper MATLAB scripts are included to assist with data analysis.

<a id="features"></a>
## Features
- *A very simple API*.
- 3D lattice models including Ising and Lebwohl-Lasher. 
- Mixture support.
- Canonical (NVT) ensemble.
- Density of states (DOS) flat-histogram sampling including Wang-Landau and Expanded Ensemble DOS (in development). 

<a id="installation"></a>
## Installation 
The library must be built before it can be used. The first step is to clone the repository locally.

```bash
$ git clone https://github.com/hsidky/SAPHRON.git
```

To build the actual library, cd into the `src` directory. Clang is the default compiler specified in the Makefile. If you have it installed, simply run the command below.

```bash
$ make
```
Any compatible C++11 compiler should work in principle. If you would like to use, say gcc, this can be done by specifying the compiler flag

```bash
$ make CXX=g++
```
After the library is built, it should create a static library `SAPHRON.a` in the `lib` folder that can be linked against a program of your choosing.  

That's it! 

<a id="structure"></a>
## Structure 

The `SAPHRON` framework has a simple API structure. It is currently in development, so the API may change frequently. The main classes and how they interact are outlined below

***Site***

TBC 

***Move***

TBC 

***Model*** 

TBC 

***Ensemble***

TBC 

***Logger***

TBC

<a id="usage"></a>
## Usage 
Since `SAPHRON` is only a library, it needs to be linked to an application to run. Luckily, there are a number of examples to get someone started. The `examples` directory contains thoroughly documented working examples of varying complexity. To build the examples, simply run the command below in that directory.  

```bash
$ make
```

Each program details the appropriate syntax necessary for execution. Here's a list of current examples:

1. **lebwohl_lasher_nvt** - A 3D Lebwohl-Lasher model in a canonical NVT ensemble.
2. **ll_cell_twist_nvt** - A Lenwohl-Lasher model in a twist cell with wall anchoring. 
3. **wang_landau_ll** - Wang-Landau canonical sampling of the Lebwohl-Lasher model.

<a id="testing"></a>
## Testing 

`SAPHRON` uses the [Google C++ Testing Framework](https://code.google.com/p/googletest/) for unit testing. Instructions on setting up the testing framework to follow...

<a id="contributing"></a>
## Contributing

Feel free to fork this project on [GitHub](https://github.com/hsidky/SAPHRON). Any pull-requests, feature requests or other form of contributions are welcome.


