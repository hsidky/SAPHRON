#pragma once

#include "../Simulation/SimObserver.h"
#include <memory>
#include <iostream>
#include <fstream>

namespace SAPHRON 
{
	class CSVObserver : public SimObserver
	{
		private: 
			std::unique_ptr<std::ofstream> _ensemblefs;
	        std::unique_ptr<std::ofstream> _worldfs;
	        std::unique_ptr<std::ofstream> _particlefs;
	        std::unique_ptr<std::ofstream> _dosfs;
	        std::unique_ptr<std::ofstream> _histfs;
			
			bool _printedE;

			void PrintEnsembleHeader(Ensemble* e);

		public:
			CSVObserver(std::string prefix, SimFlags flags, unsigned int frequency = 1);

			virtual void Visit(Ensemble* e) override;
			virtual void Visit(DOSEnsemble* e) override;
			virtual void Visit(World* w) override;
			virtual void Visit(Particle* p) override;
			virtual void Visit(MoveManager* mm) override;
			virtual void Visit(ForceFieldManager* ffm) override;

			~CSVObserver()
			{
				if(_ensemblefs)
					_ensemblefs->close();
				if(_worldfs)
					_worldfs->close();
				if(_particlefs)
					_particlefs->close();
				if(_dosfs)
					_dosfs->close();
				if(_histfs)
					_histfs->close();
			}


	};
}
