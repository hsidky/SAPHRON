#include "ConsoleVisitor.h"
#include "../Ensembles/NVTEnsemble.h"

using namespace Ensembles;
using namespace std;
using namespace Simulation;
namespace Visitors
{
	void ConsoleVisitor::Update(SimEvent& ev)
	{
		cout << "Console called" << endl;
	}

	void ConsoleVisitor::Visit(NVTEnsemble<Site>* e)
	{
		cout << "Temperature: " << e->GetTemperature() << endl;
	}
}