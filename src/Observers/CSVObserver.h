#pragma once

#include "../Simulation/SimObserver.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>

namespace SAPHRON 
{
	class CSVObserver : public SimObserver
	{
	private: 
		std::unique_ptr<std::ofstream> _simfs;
        std::vector<std::unique_ptr<std::ofstream>> _worldfs;
        std::unique_ptr<std::ofstream> _particlefs;
        std::unique_ptr<std::ofstream> _dosfs;
        std::unique_ptr<std::ofstream> _histfs;
		
        // Delimiter 
        std::string _dlm;
        std::string _ext;
        
        // Fixed length output? 
        bool _fixl;

        // Printed headers?
        bool _printedH;

		void PrintEnsembleHeader(const Ensemble& e);

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
			for(auto& w : _worldfs)
				*w << std::endl;

			_printedH = true;
		}

		~CSVObserver()
		{
			if(_simfs)
				_simfs->close();
			
			for(auto& w : _worldfs)
				w->close();

			if(_particlefs)
				_particlefs->close();
			if(_dosfs)
				_dosfs->close();
			if(_histfs)
				_histfs->close();
		}
	};
}
