#pragma once

// Forward declare.
namespace Ensembles
{
	template <class T>
	class NVTEnsemble;

	template <class T>
	class WangLandauDOS;
}

namespace Models
{
	class BaseModel;
}

class Site;
class Histogram;

namespace Visitors
{
	class Visitor
	{
		public:
			virtual void Visit(Histogram* h) = 0;
			virtual void Visit(Ensembles::WangLandauDOS<Site>* e) = 0;
			virtual void Visit(Ensembles::NVTEnsemble<Site>* e) = 0;
			virtual void Visit(Models::BaseModel* m) = 0;
			virtual void Visit(Site* s) = 0;
	};
}
