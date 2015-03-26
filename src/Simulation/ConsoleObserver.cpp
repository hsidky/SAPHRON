#include "../Ensembles/Ensemble.h"
#include "../Particles/Particle.h"
#include "ConsoleObserver.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>

using namespace std;

namespace SAPHRON
{
	void ConsoleObserver::Visit(Ensemble* e)
	{
		cout << endl << endl;
		if (this->Flags.iterations)
			cout << setw(20) << left << "Iteration";
		if (this->Flags.energy)
			cout << setw(20) << left << "Energy";
		if (this->Flags.temperature)
			cout << setw(20) << left << "Temperature";
		cout << endl;
		
		
		if (this->Flags.iterations)
			cout << setw(20) << left << e->GetIteration();
		if (this->Flags.energy)
			cout << setw(20) << setprecision(5) << left << scientific << e->GetEnergy();
		if (this->Flags.temperature)
			cout << setw(20) << left << e->GetTemperature();
		 
		cout << endl;
	}

	void ConsoleObserver::Visit(World*)
	{
	}

	void ConsoleObserver::Visit(Particle* p)
	{
 		if(!this->Flags.particle)
	    	return;
		
		cout << "Global ID: " << p->GetGlobalIdentifier() << " ";

	    if (this->Flags.particle_position)
	    {
	    	auto coords = p->GetPosition();
	    	cout << "Coordinates: " << coords.x << " " << coords.y << " " << coords.z << " ";
		}

	    if (this->Flags.particle_director)
	    {
	    	auto& dir = p->GetDirectorRef();
	    	cout << "Director: " << dir[0] << " " << dir[1] << " " << dir[2] << " ";
	    }
	    
	    if (this->Flags.particle_species)
	    	cout << "Species: " << p->GetIdentifierString() << " ";
	    if (this->Flags.particle_neighbors)
	    {
	    	auto& neighbors = p->GetNeighborList();
	        cout << "Neighbors: ";
	        for(auto& neighbor : neighbors)
	        	{
	            	auto* part = neighbor.GetParticle();
	             	cout << part->GetGlobalIdentifier() << " ";
	            }
	   	}
	   	cout << endl;
	}


	//
	// void ConsoleObserver::VisitInternal(WangLandauDOSEnsemble<Site>* e)
	// {
	//      cout << endl << endl;
	//      if (this->Flags.identifier)
	//              cout << setw(20) << left << "Identifier";
	//      if (this->Flags.iterations)
	//              cout << setw(20) << left << "Iteration";
	//      if (this->Flags.energy)
	//              cout << setw(20) << left << "Energy";
	//      cout << endl;
	//
	//      if (this->Flags.identifier)
	//              cout << setw(20) << left << this->GetObservableID();
	//      if (this->Flags.iterations)
	//              cout << setw(20) << left << this->GetIteration();
	//      if (this->Flags.energy)
	//              cout << setw(20) << setprecision(5) << left << scientific << e->GetEnergy();
	//      cout << endl;
	//
	//      // Get all DOS properties.
	//      VisitInternal(static_cast<DensityOfStatesEnsemble<Site>*>(e));
	//
	//      cout << endl;
	// }
	//
	// void ConsoleObserver::VisitInternal(SemiGrandDOSEnsemble<Site>* e)
	// {
	//      cout << endl << endl;
	//      if (this->Flags.identifier)
	//              cout << setw(20) << left << "Identifier";
	//      if (this->Flags.iterations)
	//              cout << setw(20) << left << "Iteration";
	//      if (this->Flags.energy)
	//              cout << setw(20) << left << "Energy";
	//      if (this->Flags.temperature)
	//              cout << setw(20) << left << "Temperature";
	//      if (this->Flags.composition)
	//              cout << setw(20) << left << "Composition";
	//      cout << endl;
	//
	//      if (this->Flags.identifier)
	//              cout << setw(20) << left << this->GetObservableID();
	//      if (this->Flags.iterations)
	//              cout << setw(20) << left << this->GetIteration();
	//      if (this->Flags.energy)
	//              cout << setw(20) << setprecision(5) << left << scientific << e->GetEnergy();
	//      if (this->Flags.temperature)
	//              cout << setw(20) << left << e->GetTemperature();
	//      if(this->Flags.composition)
	//      {
	//              auto comp = e->GetComposition();
	//              std::copy(comp.begin(), comp.end(),
	//                        std::ostream_iterator<int>(std::cout << setw(20/comp.size()), " "));
	//      }
	//      cout << endl;
	//
	//      // Get all DOS properties.
	//      VisitInternal(static_cast<DensityOfStatesEnsemble<Site>*>(e));
	// }
	//
	// void ConsoleObserver::VisitInternal(DensityOfStatesEnsemble<Site>* e)
	// {
	//      if(!this->Flags.dos)
	//              return;
	//
	//      cout << endl;
	//      cout << setw(20) << "";
	//      if (this->Flags.dos_walker)
	//              cout << setw(20) << left << "Walker ID";
	//      if (this->Flags.dos_scale_factor)
	//              cout << setw(20) << left << "Scale Factor";
	//      if (this->Flags.dos_flatness)
	//              cout << setw(20) << left << "Flatness";
	//      if (this->Flags.dos_interval)
	//              cout << setw(20) << left << "Interval";
	//      cout << endl;
	//      // No DOS values output to console. That would be insane!
	//
	//      cout << setw(20) << "";
	//      if (this->Flags.dos_walker)
	//              cout << setw(20) << left << e->GetWalkerID();
	//      if(this->Flags.dos_scale_factor)
	//              cout << setw(20) << left << setprecision(5) << scientific << e->GetScaleFactor();
	//      if (this->Flags.dos_flatness)
	//              cout << setw(20) << left << fixed << setprecision(3) << e->GetFlatness();
	//      if(this->Flags.dos_interval)
	//      {
	//              auto interval = e->GetParameterInterval();
	//              cout << setw(10) << fixed << setprecision(7) << left << interval.first << " " <<
	//              interval.second;
	//      }
	//      cout << endl;
	// }
	//
	// void ConsoleObserver::VisitInternal(Models::BaseModel*)
	// {
	// }
	//
	// void ConsoleObserver::VisitInternal(Histogram* h)
	// {
	//      if(!this->Flags.histogram)
	//              return;
	//
	//      cout << endl;
	//      cout << setw(20) << "";
	//      if (this->Flags.hist_bin_count)
	//              cout << setw(20) << left << "Bin Count";
	//      if (this->Flags.hist_lower_outliers)
	//              cout << setw(20) << left << "Lower Outliers";
	//      if (this->Flags.hist_upper_outliers)
	//              cout << setw(20) << left << "Upper Outliers";
	//      cout << endl;
	//
	//      cout << setw(20) << "";
	//      if(this->Flags.hist_bin_count)
	//              cout << setw(20) << left << h->GetBinCount();
	//      if(this->Flags.hist_lower_outliers)
	//              cout << setw(20) << left << h->GetLowerOutlierCount();
	//      if(this->Flags.hist_upper_outliers)
	//              cout << setw(20) << left << h->GetUpperOutlierCount();
	//
	//      cout << endl;
	// }
	//
	// void ConsoleObserver::VisitInternal(Site* s)
	// {
	//      if(!this->Flags.site)
	//              return;
	//
	//      if (this->Flags.site_coordinates)
	//              cout << "Coordinates: "  << s->GetXCoordinate() << " " <<
	//              s->GetYCoordinate() << " " << s->GetZCoordinate() << " ";
	//      if (this->Flags.site_unit_vectors)
	//              cout << "Unit Vectors: " << s->GetXUnitVector() << " " <<
	//              s->GetYUnitVector() << " " << s->GetZUnitVector() << " ";
	//      if (this->Flags.site_species)
	//              cout << "Species: " << s->GetSpecies() << " ";
	//      if (this->Flags.site_neighbors)
	//      {
	//              auto& neighbors = s->GetNeighbors();
	//              cout << "Neighbors: ";
	//              std::copy(neighbors.begin(), neighbors.end(),
	//                        std::ostream_iterator<int>(std::cout, " "));
	//      }
	//
	//      cout << endl;
	// }
}
