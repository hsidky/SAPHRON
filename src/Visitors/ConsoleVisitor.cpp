#include "../Ensembles/NVTEnsemble.h"
#include "ConsoleVisitor.h"

using namespace Ensembles;
using namespace std;
using namespace Simulation;

namespace Visitors
{
	void ConsoleVisitor::Update(SimEvent&)
	{
	}

	void ConsoleVisitor::Visit(NVTEnsemble<Site>* e)
	{
		cout << "Temperature: " << e->GetTemperature() << ", Iterations: " <<
		e->GetSweepCount() << endl;
	}

	void ConsoleVisitor::Visit(Models::BaseModel*)
	{
	}

	void ConsoleVisitor::Visit(Site* s)
	{
		std::cout << "Coordinates: "  << s->GetXCoordinate() << " " <<
		s->GetYCoordinate() << " " << s->GetZCoordinate() << endl;
	}
}
