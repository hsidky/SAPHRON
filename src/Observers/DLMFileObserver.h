#pragma once

#include "../Simulation/SimObserver.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

namespace SAPHRON 
{
	class DLMFileObserver : public SimObserver
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

        void InitializeEnsemble(const Simulation& e);
        void InitializeDOSSimulation(const DOSSimulation& e);
        void InitializeWorlds(const WorldManager& wm);
        void InitializeParticles(const WorldManager& wm);
        void InitializeHistogram(const Histogram& hist);

        template<typename T>
        void WriteStream(std::ofstream& stream, const T& data);

        template<typename T>
        void WriteStream(std::ofstream& stream, const T& data, int width);

	public:
		DLMFileObserver(const std::string& prefix, 
						const SimFlags& flags, 
						unsigned int frequency = 1);

		// Sets the column width in fixed with mode.
		void SetWidth(unsigned int width) { _w = width; }

		// Set fixed width mode (true = on, false = off).
		void SetFixedWithMode(bool fixed) { _fixl = fixed; }

		// Set delimiter.
		void SetDelimiter(const std::string& dlm) { _dlm = dlm; }

		// Set file extension
		void SetExtension(const std::string& ext) { _ext = ext; }

		// Get Observer name.
		virtual std::string GetName() const override{ return "DLMFile"; }

		virtual void Visit(const Simulation& e) override;
		virtual void Visit(const DOSSimulation& e) override;
		virtual void Visit(const WorldManager& wm) override;
		virtual void Visit(const MoveManager& mm) override;
		virtual void Visit(const ForceFieldManager& ffm) override;
		virtual void Visit(const Particle& p) override;
		virtual void Visit(const Histogram& hist) override;

		virtual void Serialize(Json::Value& json) const override;

		// Post visit, mark printed bools as true.
		virtual void PostVisit() override
		{
			if(_simfs)
				*_simfs << std::endl;

			if(_histfs)
				*_histfs << std::endl;

			for(auto& w : _worldfs)
				*w << std::endl;

			_printedH = true;
		}

		~DLMFileObserver()
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
