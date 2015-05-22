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
#include "Connectivities/Connectivity.h"
#include "Moves/MoveManager.h"
#include "SimObserver.h"

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

			struct ConnectivityProps
			{
				std::string type; // Connectivity type.
				std::vector<double> parameters; //Connectivity parameters (scalar).
				std::vector<std::vector<double>> vparameters; // Connectivity parameters (vector).
				std::vector<std::string> sparameters; // Connectivity parameters (string).
				
				std::vector<int> piselector; // Connectivity integer selectors for particles.
				std::vector<std::string> psselector; // Connectivity string selectors for particles.
				std::vector<std::string> ssselector; // Connectivity string selectors for species (parents).
			};

			struct MoveProps
			{
				std::string type; // Move type.
				int seed; // Move seed. 
				std::vector<double> parameters; // Move parameters.
			}; 


			struct ObserverProps 
			{
				std::string type; 
				SimFlags flags; 
				int frequency; 
				std::string prefix; 
			};

			// Observers
			std::vector<ObserverProps> _observers;

			// Moves.
			std::vector<MoveProps> _moves;

			int _moveseed;

			// Connectivities.
			std::vector<ConnectivityProps> _connectivities;

			// Forcefields.
			std::vector<ForceFieldProps> _forcefields;

			// Blueprint representing each unique species in the system.
			std::map<std::string, ParticleProps> _blueprint;

			// Pointer of initialized particles. 
			std::vector<Particle*> _ppointers;

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

			bool ValidateConnectivities(Json::Value connectivities);

			bool ValidateObservers(Json::Value observers);

			bool LookupParticleInConnectivity(ParticleProps& particle, ConnectivityProps& connectivity);

			bool LookupIndexInConnectivity(int index, ConnectivityProps& connectivity);

			bool LookupStringInConnectivity(std::string keyword, ConnectivityProps& connectivity);

			int ProcessFlag(Json::Value& flagtree, std::string flag, std::string observer, bool& err);

			bool ValidateMoves(Json::Value moves);

		public:
			SimBuilder() : 
				_worldprops(), _observers(0), _moves(0), _moveseed(0), _connectivities(0),
				_forcefields(0), _blueprint(), _ppointers(0), _particles(0), _reader(), 
				_root(), _errors(false), _emsgs(0), _nmsgs(0) 
			{
					srand(time(NULL));
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

			// Parse connectivities and store results in internal structure. Returns 
			// true if successful and false otherwise. Must be run after particle parse but 
			// before particlebuild!
			// Error(s) can be obtained via GetErrorMessages().
			bool ParseConnectivities()
			{
				if(!ValidateConnectivities(_root["connectivities"]))
				{
					_errors = true;
					return false;
				}
				return true;
			}

			// Parse moves and store results in internal stucture. Returns true if 
			// successful and false otherwise.
			bool ParseMoves()
			{
				if(!ValidateMoves(_root["moves"]))
				{
					_errors = true;
					return false;
				}
				return true;
			}

			// Parse observer and store results in internal structure. Returns true if 
			// successful and false otherwise.
			bool ParseObservers()
			{
				if(!ValidateObservers(_root["observers"]))
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
			void BuildParticles(World* world, std::vector<Connectivity*>& connectivities);

			// Builds forcefields and ads them to the forcefield vector. Object lifetime
			// is the caller's responsibility!
			void BuildForceFields(std::vector<ForceField*>& forcefields, ForceFieldManager& ffm);

			// Builds moves and adds them to the move vector and move manager. Object lifetime 
			// is the caller's responsibility! 
			void BuildMoves(std::vector<Move*>& moves, MoveManager& mm);

			// Builds observers and adds them to the observer vector. Object lifetime is the 
			// caller's responsibility!
			void BuildObservers(std::vector<SimObserver*>& observers);

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