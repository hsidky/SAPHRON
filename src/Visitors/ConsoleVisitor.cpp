#include "../Ensembles/NVTEnsemble.h"
#include "ConsoleVisitor.h"
#include <algorithm> 
#include <iterator>

using namespace Ensembles;
using namespace std;
using namespace Simulation;

namespace Visitors
{
	void ConsoleVisitor::VisitInternal(NVTEnsemble<Site>* e)
	{
		if (this->Flags.iterations)
			cout << "Iteration: " << this->GetIteration() << " "; 
		if (this->Flags.temperature)
			cout << "Temperature: " << e->GetTemperature() << " ";
		
		cout << endl;
	}

	void ConsoleVisitor::VisitInternal(Models::BaseModel*)
	{
	}

	void ConsoleVisitor::VisitInternal(Site* s)
	{
		if (this->Flags.site_coordinates)
			cout << "Coordinates: "  << s->GetXCoordinate() << " " <<
			s->GetYCoordinate() << " " << s->GetZCoordinate() << " ";
		if (this->Flags.site_unit_vectors)
			cout << "Unit Vectors: " << s->GetXUnitVector() << " " <<
			s->GetYUnitVector() << " " << s->GetZUnitVector() << " ";
		if (this->Flags.site_species)
			cout << "Species: " << s->GetSpecies() << " ";
		if (this->Flags.site_neighbors)
		{
			auto& neighbors = s->GetNeighbors();
			cout << "Neighbors: ";
			std::copy(neighbors.begin(), neighbors.end(), std::ostream_iterator<int>(std::cout, " "));
		}

		cout << endl;
	}
}
