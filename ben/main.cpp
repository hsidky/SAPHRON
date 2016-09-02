#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "mpi.h"
#include <omp.h>
#include <time.h>
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/ForceFields/FENEFF.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Particles/Particle.h"
#include "../src/Worlds/World.h"

#include "lammps.h"         // these are LAMMPS include files
#include "input.h"
#include "atom.h"
#include "library.h"

using namespace std;
using namespace SAPHRON;
using namespace LAMMPS_NS;

int main(int narg, char **arg) 
{
  // setup MPI and various communicators
  // driver runs on all procs in MPI_COMM_WORLD
  // comm_lammps only has 1st P procs (could be all or any subset)

  //args[0] = mpistuff
  //args[1] = number processors lammps will use
  //args[2] = Lammps input file name

  MPI_Init(&narg,&arg);

  int me,nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  int nprocs_lammps = atoi(arg[1]);

  if (nprocs_lammps > nprocs) {
    if (me == 0)
      printf("ERROR: LAMMPS cannot use more procs than available\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  int lammps;
  if (me < nprocs_lammps) lammps = 1;
  else lammps = MPI_UNDEFINED;
  MPI_Comm comm_lammps;
  MPI_Comm_split(MPI_COMM_WORLD,lammps,0,&comm_lammps);
  
  // open LAMMPS input script

  FILE *fp;
  if (me == 0) {
    fp = fopen(arg[2],"r");
    if (fp == NULL) {
      printf("ERROR: Could not open LAMMPS input script\n");
      MPI_Abort(MPI_COMM_WORLD,1);
    }
  }

  // run the input script thru LAMMPS one line at a time until end-of-file
  // driver proc 0 reads a line, Bcasts it to all procs
  // (could just send it to proc 0 of comm_lammps and let it Bcast)
  // all LAMMPS procs call input->one() on the line
  
  LAMMPS *lmp;
  if (lammps == 1) lmp = new LAMMPS(0,NULL,comm_lammps);

  int n;
  char line[1024];
  while (1) 
  {
    if (me == 0) 
    {
      if (fgets(line,1024,fp) == NULL) n = 0;
      else n = strlen(line) + 1;
      if (n == 0) fclose(fp);
    }

    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
    if (n == 0) break;
    MPI_Bcast(line,n,MPI_CHAR,0,MPI_COMM_WORLD);
    if (lammps == 1) lmp->input->one(line);

  }

  if (lammps == 1) // if lammps has right number of processors
  {
    string eachLine;
    ifstream infile;
    infile.open ("in.lj");
    
    while(!infile.eof()) // get all lines
    {
      getline(infile,eachLine); // Saves the line in eachLine.
      lmp->input->one((char*)eachLine.c_str());
    }
    
    infile.close();

    int natoms = static_cast<int> (lmp->atom->natoms); // get total numb atoms in simulation
    double *x = new double[3*natoms]; // single array 3* times cause x,y and z
    lammps_gather_atoms(lmp,"x",1,3,x); //  gathers all coords , for type double, 3 elements per id and fill x

    Rand _rand(time(NULL));   // seeding random number generator with time (every time you run it's gonna be diff)
    // can replace with seed value
    lmp->input->one("run 1");
    //*******************************************************************************
    // set rcut
    double rcut = 12.0;   // ???

    //Set simulation
    World world(100, 100, 100, rcut, 46732); //create a world

    SAPHRON::Particle poly("Polymer"); //Create the polymer 
    ParticleList Monomers;

    for(int i=0; i<natoms*3;i=i+3){
      Monomers.push_back(new Particle({x[i],x[i+1],x[i+2]},{0.0,0.0,0.0}, "Monomer")); //Particle constructor
    }

    for(int i=1; i<natoms-1;i++){
      Monomers[i]->AddNeighbor(Monomers[i+1]);
      Monomers[i]->AddNeighbor(Monomers[i-1]);
    }
    Monomers[0]->AddNeighbor(Monomers[1]);
    Monomers[natoms-1]->AddNeighbor(Monomers[natoms-2]);

    for(auto& c : Monomers)
    {
      poly.AddChild(c);
    }

    world.PackWorld({&poly}, {1.0}, 1, 0.0001); // polymer, number fractions, n = total number, density=resize box

    ForceFieldManager ffm;

    LennardJonesFF lj(10.0, 1.0, {4.0}); // eps, sigma, cutoff
    FENEFF fene(10.0, 1.0, 4.0, 1.0); // eps, sigma, kspring, rmax, will get value from lammps

    //SAPHTRON forcefield
    ffm.AddNonBondedForceField("Monomer", "Monomer", lj);
    ffm.AddBondedForceField("Monomer", "Monomer", fene);

    //Gather the energy from saphron
    auto H1 = ffm.EvaluateEnergy(world);

//********************************************************************//

    // End Reference & run lammps
    delete [] x;

    MPI_Barrier(MPI_COMM_WORLD);
    
    // I dont know if this works but you can try it Vik
    double* lammps_energy = ((double*) lammps_extract_variable(lmp,"t","all"));
      
    MPI_Barrier(MPI_COMM_WORLD);

  }

  if (lammps == 1) delete lmp;

  // close down MPI
  MPI_Finalize();
}