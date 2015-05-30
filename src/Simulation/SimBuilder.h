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
#include "DensityOfStates/DOSOrderParameter.h"

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

			struct EnsembleProps
			{
				std::string type;
				int sweeps;
				std::vector<double> parameters; // Scalar parameters for ensembles.
				double pressure; 
				int seed;
			} _ensemble;

			struct DOSProps
			{
				std::string type;
				std::pair<double, double> interval;
				int bincount;
				double binwidth;
				double scalefactor;
				double targetflatness;
				std::vector<double> parameters; // scalar parameters for order parameters.

				// Selectors which may be used by some OP.
				std::vector<int> piselector; // Connectivity integer selectors for particles.
				std::vector<std::string> psselector; // Connectivity string selectors for particles.
				std::vector<std::string> ssselector; // Connectivity string selectors for species (parents).
			} _dosprops;

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

			bool ValidateEnsemble(Json::Value ensemble);

			bool ValidateDOS(Json::Value dos);

			bool CheckType(std::string type, std::vector<std::string> types);

			bool LookupParticleInConnectivity(ParticleProps& particle, std::vector<std::string>& psselector);

			bool LookupIndexInConnectivity(int index, std::vector<int>& piselector);

			bool LookupStringInConnectivity(std::string keyword, std::vector<int>& piselector, std::vector<std::string>& psselector);

			int ProcessFlag(Json::Value& flagtree, std::string flag, std::string observer, bool& err);

			bool ValidateMoves(Json::Value moves);

			bool ParseSelectors(Json::Value particles, std::string type, std::vector<int>& piselector, std::vector<std::string>& psselector);

		public:
			SimBuilder() : 
				_worldprops(), _ensemble(), _dosprops(), _observers(0), 
				_moves(0), _moveseed(0), _connectivities(0), _forcefields(0), 
				_blueprint(), _ppointers(0), _particles(0), _reader(), _root(), _errors(false), 
				_emsgs(0), _nmsgs(0) 
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

			// Parse ensemble and store results in internal structure. Returns true if 
			// successful and false otherwise.
			bool ParseEnsemble()
			{
				if(!ValidateEnsemble(_root["ensemble"]))
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

			// Builds ensemble using world, forcefield manager, move manager and observers.
			// Returns pointer to ensemble object. Also returns pointer to DOSOrderParameter if it is used, 
			// otherwise returns nullptr.
			Ensemble* BuildEnsemble(World& world, 
									ForceFieldManager& ffm, 
									MoveManager& mm, 
									std::vector<SimObserver*>& observers, 
									DOSOrderParameter*& dosop, int& sweeps);

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