#pragma once

#include "../Simulation/SimObserver.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

namespace SAPHRON 
{
	class CSVObserver : public SimObserver
	{
	private: 
		std::unique_ptr<std::ofstream> _simfs;
        std::vector<std::unique_ptr<std::ofstream>> _worldfs;
        std::map<int,std::unique_ptr<std::ofstream>> _particlefs;
        std::unique_ptr<std::ofstream> _dosfs;
        std::unique_ptr<std::ofstream> _histfs;
		
        // Delimiter 
        std::string _dlm;
        std::string _ext;
        
        // Fixed length output? 
        bool _fixl;
        int _w;

    	// File prefix.
    	std::string _prefix;

        // Printed headers?
        bool _printedH;

        void InitializeEnsemble(const Ensemble& e);
        void InitializeWorlds(const WorldManager& wm);
        void InitializeParticles(const WorldManager& wm);

	public:
		CSVObserver(std::string prefix, SimFlags flags, unsigned int frequency = 1);

		virtual void Visit(const Ensemble& e) override;
		virtual void Visit(const DOSEnsemble& e) override;
		virtual void Visit(const WorldManager& wm) override;
		virtual void Visit(const MoveManager& mm) override;
		virtual void Visit(const ForceFieldManager& ffm) override;
		virtual void Visit(const Particle& p) override;
		

		// Post visit, mark printed bools as true.
		virtual void PostVisit() override
		{
			// Newline it out!
			*_simfs << std::endl;

			for(auto& w : _worldfs)
				*w << std::endl;

			for(auto& p : _particlefs)
				*p.second << std::endl;

			_printedH = true;
		}

		~CSVObserver()
		{
			if(_simfs)
				_simfs->close();
			
			for(auto& w : _worldfs)
				w->close();

			for(auto& p : _particlefs)
				p.second->close();

			if(_dosfs)
				_dosfs->close();
			if(_histfs)
				_histfs->close();
		}
	};
}
