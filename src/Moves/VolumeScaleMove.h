#pragma once 

#include "GibbsMove.h"
#include "../Rand.h"

namespace SAPHRON
{
	// Class for a volume scaling move which is implemented for both "standard"
	// and Gibbs ensembles. Note that the scaling is performed w.r.t. log(V).
	// Currently only does cubic volume scales. For Gibbs volume scaling, the 
	// total volume is preserved and re-distributed between the two.
	// Follows Algorithm 18 in Frenkel & Smit.
	class VolumeScaleMove : public GibbsMove
	{
	private:
		double _vmax;
		Rand _rand;
		std::vector<double> _oldv;
		WorldList _worlds;
		int _rejected;
		int _performed;
		int _seed;

	public:
		VolumeScaleMove(double vmax, int seed = 64866) : 
		_vmax(vmax), _rand(seed), _oldv(0), _worlds(0), 
		_rejected(0), _performed(0), _seed(seed)
		{
			_worlds.reserve(10);
			_oldv.reserve(10);
		}

		// Nothing happens on the draw since we are scaling world volume.
		virtual void Draw(World& world, ParticleList&) override {}

		// Perform volume scaling move.
		virtual bool Perform(World& world, ParticleList&) override
		{
			_worlds.resize(1);
			_worlds[0] = &world;

			_oldv.resize(1);
			_oldv[0] = world.GetVolume();

			double lnvn = log(_oldv[0]) + (_rand.doub() - 0.5)*_vmax;
			double boxlen = pow(exp(lnvn), 1.0/3.0);
			
			world.SetBoxVectors(boxlen, boxlen, boxlen, true);
			++_performed;
			
			return true;
		}

		virtual void Draw(const WorldList&, ParticleList&) override {}

		// TODO: currently only implemented for two worlds. Generalize to multiple.
		virtual bool Perform(const WorldList& worlds, ParticleList&) override
		{
			_worlds.resize(2);
			_worlds[0] = worlds[0];
			_worlds[1] = worlds[1];

			_oldv.resize(2);
			_oldv[0] = _worlds[0]->GetVolume();
			_oldv[1] = _worlds[1]->GetVolume();

			double v = (_oldv[0] + _oldv[1]);
			double lnvn = log(_oldv[0]/_oldv[1]) + (_rand.doub() - 0.5)*_vmax;
			double v1n = v*exp(lnvn)/(1+exp(lnvn));
			double v2n = v - v1n;
			
			double b1 = pow(v1n, 1.0/3.0);
			double b2 = pow(v2n, 1.0/3.0);
			_worlds[0]->SetBoxVectors(b1, b1, b1, true);
			_worlds[1]->SetBoxVectors(b2, b2, b2, true);

			return true;
		}

		// Get maximum volume change.
		double GetMaxVolumeChange()
		{
			return _vmax;
		}
	
		virtual double GetAcceptanceRatio() override
		{
			return 1.0-(double)_rejected/_performed;
		};

		virtual void ResetAcceptanceRatio() override
		{
			_performed = 0;
			_rejected = 0;
		}

		// Undo move.
		virtual void Undo() override
		{
			for(int i = 0; i < _worlds.size(); ++i)
			{
				double len = pow(_oldv[i], 1.0/3.0);
				_worlds[i]->SetBoxVectors(len, len, len, true);
			}

			++_rejected;
		}

		// Get seed.
		virtual int GetSeed() override { return _seed; }

		virtual std::string GetName() override { return "VolumeScale"; }

		// Clone move.
		Move* Clone() const override
		{
			return new VolumeScaleMove(static_cast<const VolumeScaleMove&>(*this));
		}
	};
}