#pragma once 

#include <istream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <time.h>
#include "json/json.h"
#include "Worlds/SimpleWorld.h"
#include "Particles/Site.h"
#include "ForceFields/ForceFieldManager.h"

namespace SAPHRON
{
	class SimBuilder 
	{
		private: 
			
			struct WorldProps
			{
				std::string type;
				double xlength;
				double ylength;
				double zlength;
				int seed;
				double rcutoff;
			} _worldprops;

			struct ParticleProps
			{
				Position position; // Particle position.
				Director director; // Particle director
				std::string species; // Particle species/type.
				std::string parent; // Particle parent species/type.
				std::string residue;  // Paricle parent id. 
				int index; // Particle index relative to parent (1-n).
				std::vector<std::string> children; // Particle children species/type.
			};

			struct ForceFieldProps
			{
				std::string type;
				std::string species1;
				std::string species2;
				std::vector<double> parameters;
			};

			std::vector<ForceFieldProps> _forcefields;

			// Blueprint representing each unique species in the system.
			std::map<std::string, ParticleProps> _blueprint;

			// Vector of particles (sites really). These are the actual particles 
			// that will enter.
			std::vector<ParticleProps> _particles;

			Json::Reader _reader;
			Json::Value _root;

			bool _errors;

			// Error messages.
			std::vector<std::string> _emsgs;

			// Note messages.
			std::vector<std::string> _nmsgs;

			bool ValidateForceFields(Json::Value forcefields);

			bool ConstructBlueprint(Json::Value components, std::string parent);

			bool ValidateWorld(Json::Value world);
			
			bool ValidateParticles(Json::Value components, Json::Value particles);

		public:
			SimBuilder() : 
				_worldprops(), _forcefields(0), _blueprint(), _particles(0), _reader(), 
				_root(), _errors(false), _emsgs(0), _nmsgs(0) 
			{
			}

			// Parse stream input. Returns true if parse was successful or 
			// false if otherwise. Error(s) can be obtained via GetErrorMessages().
			bool ParseInput(std::istream& is)
			{
				if(!_reader.parse(is, _root))
				{
					_errors = true;
					_emsgs.push_back(_reader.getFormattedErrorMessages());
					return false;
				}

				return true;
			} 

			// Parse "world" node and store results in internal structure. Returns true 
			// if successful and false if otherwise. Error(s) can be obtained via GetErrorMessages().
			bool ParseWorld()
			{
				// Validate world. 
				if(!ValidateWorld(_root["world"]))
				{
					_errors = true;
					return false;
				}

				return true;
			}

			// Parse "particles" node and store results in internal structure. Returns true 
			// if successful and false if otherwise. Must be run after world parse! 
			// Error(s) can be obtained via GetErrorMessages().
			bool ParseParticles()
			{
				if(!ValidateParticles(_root["components"], _root["particles"]))
				{
					_errors = true;
					return false;
				}
				return true;
			}

			// Parse forcefields node and store results in internal structure. Returns 
			// true if successful and false otherwise. Must be run after particle parse!
			// Error(s) can be obtained via GetErrorMessages().
			bool ParseForceFields()
			{
				if(!ValidateForceFields(_root["forcefields"]))
				{
					_errors = true; 
					return false;
				}
				return true;
			}

			// Builds a world object from parsed data (see ParseWorld()). 
			// Returns pointer to newly created World object. Object lifetime is 
			// caller's responsibility.
			World* BuildWorld();

			// Builds particles by adding them to a World object. Must be called after 
			// ParseParticles().
			void BuildParticles(World* world);


			// Builds forcefields and ads them to the forcefield vector. Object lifetime
			// is the caller's responsibility!
			void BuildForceFields(std::vector<ForceField*>& forcefields, ForceFieldManager& ffm);

			std::vector<std::string> GetErrorMessages()
			{
				return _emsgs;
			}

			std::vector<std::string> GetNotices()
			{
				return _nmsgs;
			}

			void ResetNotices()
			{
				_nmsgs.clear();
			}
	};
}