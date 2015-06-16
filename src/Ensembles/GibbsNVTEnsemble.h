#pragma once 

#include "Ensemble.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Moves/MoveManager.h"
#include "../Moves/VolumeScaleMove.h"
#include "../Moves/ParticleSwapMove.h"
#include "../Worlds/World.h"
#include "../Rand.h"

namespace SAPHRON 
{

	typedef std::vector<ForceFieldManager*> FFManagerList;

	class GibbsNVTEnsemble : public Ensemble
	{
	private:

		// System temperature.
		double _temperature;

		// Total number of particles in the system. This is constant.
		unsigned int _ntotal;

		// Total volume. This is conserved.
		double _vtotal;

		// System energy and pressure
		std::vector<EPTuple> _eptuple;

		// List of worlds.
		const WorldList& _worlds;

		// Particle Swap move. 
		ParticleSwapMove* _psmove;

		// Volume scaling move.
		VolumeScaleMove* _vsmove;

		// List of forcefield managers.
		const FFManagerList& _ffmanagers;

		// Move manager.
		MoveManager& _mmanager;

		// Random number generator.
		Rand _rand; 

		// Particle list for draws.
		ParticleList _particles;

		// World index list for draws. 
		WorldIndexList _windex;

		// Acceptance map.
		AcceptanceMap _accmap;

		// Rand seed.
		int _seed;

		inline void UpdateAcceptances()
		{
			for(auto& move : _mmanager)
				_accmap[move->GetName()] = move->GetAcceptanceRatio();

			_accmap[_psmove->GetName()] = _psmove->GetAcceptanceRatio();
			_accmap[_vsmove->GetName()] = _vsmove->GetAcceptanceRatio();
		}
	
		// Cannonical acceptance probability.
		inline double AcceptanceProbability(double prevH, double currH)
		{
			double p = exp(-(currH - prevH) / (_temperature*this->GetBoltzmannConstant()));
			return p > 1.0 ? 1.0 : p;
		}

		// Volume scale acceptance rules. 
		// Frenkel & Smit Eq. 8.3.3 (Algorithm 18).
		inline double VolumeScaleAcceptance(double prevH1, double prevV1,
											double prevH2, double prevV2,
											double currH1, double currV1, 
											double currH2, double currV2,
											int N1, int N2)
		{
			double beta = 1.0/(GetBoltzmannConstant()*_temperature);
			double arg1 = -beta*((currH1 - prevH1) - (N1 + 1)*log(currV1/prevV1)/beta);
			double arg2 = -beta*((currH2 - prevH2) - (N2 + 1)*log(currV2/prevV2)/beta);
			double p = exp(arg1+arg2);
			return p > 1.0 ? 1.0 : p;
		}

		// Particle swap accetance rule. Note that here we provide the *final*
		// particle numbers so it is modified to acoomodate this. 
		// Frenkel & Smit Eq. 8.3.4.
		inline double ParticleSwapAcceptance(double prevH, double currH,
											 double V1, double V2,
											 int N1, int N2)
		{
			double beta = 1.0/(GetBoltzmannConstant()*_temperature);
			double p = (N1-1.0)*V2/(N2*V1)*exp(-beta*(currH - prevH));
			return p > 1.0 ? 1.0 : p;
		}

		void Iterate();
	
	protected:
			
			// Visit children.
			virtual void VisitChildren(Visitor& v) override
			{
				_mmanager.AcceptVisitor(v);			
				for(auto& ffmanager : _ffmanagers)
					ffmanager->AcceptVisitor(v);
				for(auto& world : _worlds)
					world->AcceptVisitor(v);
			}

	public:
		GibbsNVTEnsemble(const WorldList& worlds, 
						 const FFManagerList& ffmanagers,
						 MoveManager& mmanager,
						 double temperature, 
						 int seed = 1) : 
		_temperature(temperature), _ntotal(0), _vtotal(0), _eptuple(worlds.size()), 
		_worlds(worlds), _psmove(nullptr), _vsmove(nullptr), _ffmanagers(ffmanagers), 
		_mmanager(mmanager), _rand(seed), _particles(0), _windex(0), _accmap(), 
		_seed(seed)
		{
			assert(_worlds.size() == _ffmanagers.size());

			// We want to collect the volume scaling and particle swap 
			// moves independently because they have a unique acceptance
			// rule. TODO: find a better alternative :)
			for(auto& move : _mmanager)
			{
				if(auto* pmove = dynamic_cast<ParticleSwapMove*>(move))
					_psmove = pmove; 
				else if (auto* vmove = dynamic_cast<VolumeScaleMove*>(move))
					_vsmove = vmove;
			}

			if(_psmove == nullptr || _vsmove == nullptr)
			{
				std::cerr 
				<< "ERROR: Move manager must contain volume scaling " 
				<< "and particle swap moves." 
				<< std::endl; 
				exit(-1);
			}

			// Remove the moves from the move manager. We want to handle them
			// separately.
			_mmanager.RemoveMove(_psmove);
			_mmanager.RemoveMove(_vsmove);

			_particles.reserve(10);
			_windex.reserve(10);
			for(size_t i = 0; i < _worlds.size(); ++i)
			{
				_ntotal += _worlds[i]->GetParticleCount();
				_vtotal += _worlds[i]->GetVolume();
				_eptuple[i] = _ffmanagers[i]->EvaluateHamiltonian(*_worlds[i]);
			}

			UpdateAcceptances();
		}

		// Run the Gibbs NVT ensemble for a specified number of iterations.
		virtual void Run(int iterations) override;

		// Get temperature.
		virtual double GetTemperature() override 
		{
			return _temperature;
		}

		// Get energy.
		virtual Energy GetEnergy() override
		{
			EPTuple sum;
			for(auto& ep : _eptuple)
				sum += ep;
			
			return sum.energy;
		}

		// Get ratio of accepted moves.
		virtual AcceptanceMap GetAcceptanceRatio() override
		{
			return _accmap;
		}

		// Get seed.
		virtual int GetSeed() override { return _seed; }
		
		// Set seed.
		virtual void SetSeed(int seed) override { _rand.seed(seed); _seed = seed; }

		virtual std::string GetName() override { return "GibbsNVT"; }
	};
}