#pragma once 

#include <functional>
#include <armadillo>
#include <algorithm>
#include <map>
#include "Connectivity.h"
#include "../Particles/Particle.h"
#include "../Worlds/World.h"

namespace SAPHRON
{
	typedef std::function<void(Particle*, Director&)> DirectorFunc;
	typedef std::function<int(const Particle*)> PFilterFunc;
	// Class representing De-Localized Spin Anchoring (DLSA) on a particle.
	// The de-localization is w.r.t. a user-supplied filter that uses a 
	// de-localized director to compute the anchoring potential rather than 
	// the particle potential itself. This allows for more particle mobility.
	// The Hamiltonian H = coeff*dot(n,d) where n is the de-localized director 
	// and d is the director from a user-supplied function.
	class DLSAConnectivity : public Connectivity
	{
	private:
		double _coeff;
		Director _dir;
		DirectorFunc _dfunc;
		PFilterFunc _pfunc;
		std::map<int, int> _groupMap;
		std::vector<arma::mat> _u;
		std::vector<std::vector<int>> _idmap;

		arma::cx_vec _eigval;
		arma::cx_mat _eigvec;
		arma::uword _imax;

	public:
		DLSAConnectivity(const World& world, double coeff, DirectorFunc dfunc, PFilterFunc pfunc) :
			_dir({ 0.0, 0.0, 0.0 }), _coeff(coeff), _dfunc(dfunc), _pfunc(pfunc), _u(0), _idmap(0)
		{
			std::vector<int> groups;
			std::map<int, int> counts;
			// Generate all possible groupings.
			for (int i = 0; i < world.GetParticleCount(); ++i)
			{
				auto* particle = world.SelectParticle(i);
				int group = particle->GetGlobalIdentifier();
				_groupMap.insert(
					std::pair<int, int>(group,_pfunc(particle))
				);

				if (counts.find(group) == counts.end())
					counts[group] = 0;
				else
					counts[group]++;

				groups.push_back(group);
			}

			// Determine unique groups.
			std::sort(groups.begin(), groups.end());
			auto last = std::unique(groups.begin(), groups.end());
			groups.erase(last, groups.end());

			// Update group map to match vector indices.
			for (auto it = _groupMap.begin(); it != _groupMap.end(); ++it)
			{
				auto loc = std::find(groups.begin(), groups.end(), it->second);
				if (loc == groups.end())
				{
					std::cerr << "ERROR: Invalid group type. "
						      << "Please report this error."
						      << std::endl;
					exit(-1);
				}
				it->second = *loc;
			}

			// Allocate directors.
			for (auto& group : groups)
			{
				_u.push_back(arma::mat(counts[group], 3, arma::fill::zeros));
				_idmap.push_back(std::vector<int>(0));
			}

			// Fill in particle directors.
			for (int i = 0; i < world.GetParticleCount(); ++i)
			{
				auto* particle = world.SelectParticle(i);
				auto& dir = particle->GetDirectorRef();
				int id = particle->GetGlobalIdentifier();
				int idx1 = _groupMap[id];
				int idx2 = _idmap[idx1].size();
				for (int j = 0; j < 3; ++j)
					_u[idx1](idx2, j) = dir[j];
				
				_idmap[idx1].push_back(id);
			}
		}

		// Evaluate Hamiltonian.
		virtual double EvaluateHamiltonian(Particle* p) override
		{
			int id = p->GetGlobalIdentifier();
			auto& dir = p->GetDirectorRef();
			int idx1 = _groupMap[id];
			
			auto search = std::find(_idmap[idx1].begin(), _idmap[idx1].end(), id);
			if (search == _idmap[idx1].end())
			{
				std::cerr << "ERROR: Particle not found in map."	
					      << std::endl;
			}

			int idx2 = search - _idmap[idx1].begin();
			_u[idx1](idx2, 0) = dir[0];
			_u[idx1](idx2, 1) = dir[1];
			_u[idx1](idx2, 2) = dir[2];		

			auto Q = 3.0 / (2.0*_u[idx1].n_rows)*(_u[idx1].t()*_u[idx1] - 1.0 / 3.0 * arma::eye(3, 3));
			arma::eig_gen(_eigval, _eigvec, Q);
			_eigval.max(_imax);

			// Calculate director based on user supplied func.
			_dfunc(p, _dir);

			double dot = _dir[0] * _eigvec(0, _imax).real 
					   + _dir[1] * _eigvec(1, _imax).real 
					   + _dir[2] * _eigvec(2, _imax).real;

			return _coeff*dot;
		}
	};
}