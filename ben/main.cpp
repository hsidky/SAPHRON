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
#include "../src/ForceFields/LennardJonesTSFF.h"
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
    for(int i=0; i<20;i++)
      {// THIS FOR LOOP SIMULATES RUN (FOR E.G. RUN 100 IN LAMMPS WOULD CORRESPOND TO i<100 HERE), CAN LATER BE PUT AS ARGUMENT
        lmp->input->one("run 1");
        int natoms = static_cast<int> (lmp->atom->natoms); // get total numb atoms in simulation
        double *x = new double[3*natoms]; // single array 3* times cause x,y and z
        lammps_gather_atoms(lmp,"x",1,3,x); //  gathers all coords arranged by id , for type double, 3 elements per id and fill x
        Rand _rand(time(NULL));   // seeding random number generator with time (every time you run it's gonna be diff)
        // can replace with seed value
        double* lammps_energy = ((double*) lammps_extract_variable(lmp,"tot","0")); //THIS WORKS
        

        cout << "*******************************************"<<endl;
        cout <<"LAMMPS TOTAL ENERGY (ke+pe)"<<*lammps_energy<<endl; 
        cout<<"TOTAL NO. OF ATOMS FROM LAMMPS "<<natoms<<endl;

        //*******************************************************************************
        // set rcut WORLD
        double rcut = 3.0;   // ???

        //Set simulation
        World world(10.38498, 10.38498, 10.38498, rcut, 46732); //create a world simualar to lammps

        SAPHRON::Particle poly("Polymer"); //Create the polymer 
        ParticleList Monomers;

        for(int i=0; i<natoms*3;i=i+3){
          Monomers.push_back(new Particle({x[i],x[i+1],x[i+2]},{0.0,0.0,0.0}, "Monomer")); //Particle constructor
        }
  
        for(int i=1; i<natoms-1;i++){
          Monomers[i]->AddBondedNeighbor(Monomers[i+1]); // AddBondedNeighbor instead of AddNeighbor
          Monomers[i]->AddBondedNeighbor(Monomers[i-1]);
        }
        Monomers[0]->AddBondedNeighbor(Monomers[1]);
        Monomers[natoms-1]->AddBondedNeighbor(Monomers[natoms-2]);
  
        for(auto& c : Monomers)
        {
          poly.AddChild(c);
        }
  
  
        world.AddParticle(&poly);   // instead of PackWorld, packWorld was filling up the box
        world.UpdateNeighborList();
        //world.PackWorld({&poly}, {1.0}, 1, 0.00357); // polymer, number fractions, n = total number, density=resize box
        // ABOVE VALUES IN PACKWORLD CORRESPOND SYSTEM IN LAMMPS

        ForceFieldManager ffm;

        LennardJonesTSFF lj(0.8, 1.0, {1.122}); // eps, sigma, cutoff. SIMILAR TO LAMMPS
        FENEFF fene(1.0, 1.0, 30.0, 2.0); // eps, sigma, kspring, rmax, will get value from lammps. SIMILAR TO LAMMPS

        //SAPHTRON forcefield
        ffm.AddNonBondedForceField("Monomer", "Monomer", lj);
        ffm.AddBondedForceField("Monomer", "Monomer", fene);

        //Gather the energy from saphron
        auto H1 = ffm.EvaluateEnergy(world);

        cout<<"TOTAL ENERGY OF TEH WORLD FROM SAPHRON "<<H1.energy.total()<<endl;
  
        // TESTING COORDS saphron gets
        for(auto& p : world)
        {
          for(auto& c1 : *p) {
            Position pos = c1->GetPosition();
            std::cout << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
          }
        }

    //********************************************************************//

        // PRINTING OUT VALUES OF x
        for(int i = 0; i < 12; i++) {
          cout << *(x + i) << endl;
        }

        delete [] x;
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
  }
}

  if (lammps == 1) delete lmp;
  // close down MPI
  MPI_Finalize();
}
