SAPHRON
==============

**SAPHRON** (**S**tatistical **A**pplied **PH**ysics through **R**andom **O**n-the-fly **N**umerization) is an open-source light-weight C++11 based Monte Carlo physics library. It is designed to be easy to use - providing a clean, extendable API with usage examples. Unlike most other packages, **SAPHRON** is not a standalone application. However, it is very simple to create usable applications, and many examples are provided. The library is currently in early develpoment, and as a result it currently lacks many basic features (namely off-lattice models). In addition to the C++ library, a number of helper MATLAB scripts are included to assist with data analysis.

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

The **SAPHRON** framework has a simple API structure. It is currently in development, so the API may change frequently. The main classes and how they interact are outlined below

***Site***

A `Site` is the simplest object representing a point in space, which would typically correspond to an atom in a molecule or site on a lattice. It is little more than a plain old data (POD) type.  The only requirement to initialize a site is a set of coordinates. Everything else is optional. An example of a typical constructor would be 
```C++ 
// Initialize a site with specified coordinates (x,y,z).
Site site(0.54, 2.27, 9.99);
```

Each `Site` is outfitted with property getters and setters, a few of which are listed below. 

- **Coordinates** - There are x,y and z coordinates of type `double` for a `Site`. There are get/set methods that allow individual or group assignment.
- **Unit vectors** - There are ux,uy,uz "unit vectors" of type `double` for a `Site`. Depending on the model, these can be used for velocities, orientation or anything else. There are get/set methods for both individual and group assignment. 
- **Species** - An integer specifying the species type for a `Site`. This is arbitrary and it is up to the caller to identify them with an actual species. In the future we plan on adding support for adding an associated string. 
- **Neighbors** - A vector of `int` containing indices of `Site` neighboring the current object in a vector of `Site`. A `Model` allocates a vector of `Site` and is typically responsible for assigning/maintaining the neighbor list (read below). 

It will soon be possible to group a set of sites together into a new type - for example, `Molecule`. The appropriate classes (below) are templated to allow for this, but the infastructure stil need more development. Specific applications of such a paradigm are described in the apppropriate sections.

***Move***

A `Move` is an object responsible for manipulating an object of type `T`. Currently, this is a `Site`, but in the future it could be a more complex type such as `Molecule` (described above). `Move` itself is abstract, and serves as an interface for implementations. 

Derived classes must implement the following:

- `void Perform(T& type)` - Performs a move on an object reference.
- `void Undo()` - Rolls back a move on an object.

The methodology for a `Move` object follows the command pattern closely (though not directly). When `Perform` is called, the `Move` should store a pointer to the object and the previous state of the object which it will change. It then changes the state and returns. When `Undo` is called, the object is simply returned back to its original state. 

To see a simple implementation of this, see `src/Moves/FlipSpinMove.h` or `src/Moves/SphereUnitVector.h`

***Model*** 

TBC 

***Ensemble***

TBC 

***Logger***

TBC

<a id="usage"></a>
## Usage 
Since **SAPHRON** is only a library, it needs to be linked to an application to run. Luckily, there are a number of examples to get someone started. The `examples` directory contains thoroughly documented working examples of varying complexity. To build the examples, simply run the command below in that directory.  

```bash
$ make
```

Each program details the appropriate syntax necessary for execution. Here's a list of current examples:

1. **lebwohl_lasher_nvt** - A 3D Lebwohl-Lasher model in a canonical NVT ensemble.
2. **ll_cell_twist_nvt** - A Lenwohl-Lasher model in a twist cell with wall anchoring. 
3. **wang_landau_ll** - Wang-Landau canonical sampling of the Lebwohl-Lasher model.

<a id="testing"></a>
## Testing 

**SAPHRON** uses the [Google C++ Testing Framework](https://code.google.com/p/googletest/) for unit testing. Instructions on setting up the testing framework to follow...

<a id="contributing"></a>
## Contributing

Feel free to fork this project on [GitHub](https://github.com/hsidky/SAPHRON). Any pull-requests, feature requests or other form of contributions are welcome.


