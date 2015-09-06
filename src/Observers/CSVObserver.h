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
		
        // Delimiter 
        std::string _dlm;
        std::string _ext;
        bool _fixl;

		bool _printedE;
		bool _printedW;

		void PrintEnsembleHeader(Ensemble* e);

	public:
		CSVObserver(std::string prefix, SimFlags flags, unsigned int frequency = 1);

		virtual void Visit(const Ensemble& e) override;
		virtual void Visit(const DOSEnsemble& e) override;
		virtual void Visit(const WorldManager& w) override;
		virtual void Visit(const Particle& p) override;
		virtual void Visit(const MoveManager& mm) override;
		virtual void Visit(const ForceFieldManager& ffm) override;

		// Post visit, mark printed bools as true.
		virtual void PostVisit() override
		{
			if(_worldfs)
				*_worldfs << std::endl;
			_printedW = true;
			_printedE = true;
		}

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
