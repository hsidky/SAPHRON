#pragma once 

#include "../Utils/Rand.h"
#include "Move.h"
#include "../Worlds/WorldManager.h"
#include "../Simulation/SimInfo.h"
#include "../DensityOfStates/DOSOrderParameter.h"
#include "../ForceFields/ForceFieldManager.h"

namespace SAPHRON
{
	// Class for CBMC on a random particle with lower and upper bond limits between particles.
	// http://towhee.sourceforge.net/algorithm/cbmc.html
	// http://www.pa.msu.edu/~duxbury/courses/phy480/BaschnagelReview2004.pdf

	class CBMCMove : public Move
	{
	private: 

		int _numexploration;
		double _minr;
		double _maxr;
		int _startingbead;
		Rand _rand;
		int _rejected;
		int _performed;
		unsigned _seed;

		// Arrays for the CBMC trial positions
		std::array<EPTuple,8> _wenergy;
		std::array<Vector3D,8> _positions;
		std::array<double,8> _rosenbluths;

		// Explicit draw, use for low fraction particles.
		bool _explicit;

		// Vector of dx's based on species ID.
		std::vector<int> _species;
		std::vector<int> _sstartingbead;

	public: 
		// Initialize CBMC move with species based minr and maxr for bond lengths.
		// User supplies a starting bead as well as number of CBMC trials.
		// Anything not specified will initialize to zero.
		CBMCMove(double minr, double maxr, const std::map<int, int>& StartBead, int trials,  
		bool expl, unsigned seed = 2496) : 
		_numexploration(trials), _minr(minr), _maxr(maxr), _startingbead(-1), _rand(seed), _rejected(0), 
		_performed(0), _seed(seed), _explicit(expl), _species(0), _sstartingbead(-1)
		{
			// Get species and initialize all dx's with zeros.
			auto& list = Particle::GetSpeciesList();
			_startingbead.resize(list.size(), 0);

			for(auto& id : startbead)
			{	
				if(id.first >= (int)list.size())
				{
					std::cerr << "Species ID \"" 
							  << id.first << "\" provided does not exist." 
							  << std::endl;
					exit(-1);
				}
				_sstartingbead[id.first] = id.second;
				_species.push_back(id.first);
			}
		}

		// Initialize CBMC move with species based minr and maxr for bond lengths.
		// User supplies a starting bead as well as number of CBMC trials.
		// Anything not specified will initialize to zero.
		CBMCMove(double minr, double maxr, const std::map<std::string, int>& StartBead, int trials, 
		bool expl, unsigned seed = 2496) : 
		_numexploration(trials), _minr(minr), _maxr(maxr), _startingbead(-1), _rand(seed), _rejected(0), 
		_performed(0), _seed(seed), _explicit(expl), _species(0), _sstartingbead(-1)
		{
			// Get species and initialize all dx's with zeros.
			auto& list = Particle::GetSpeciesList();
			_startingbead.resize(list.size(), 0);
			_species.resize(StartBead.size());

			for(auto& id : StartBead)
			{
				auto it = std::find(list.begin(), list.end(), id.first);
				if(it == list.end())
				{
					std::cerr << "Species ID \""
							  << id.first << "\" provided does not exist."
							  << std::endl;
					exit(-1);
				}
				_startingbead[it - list.begin()] = id.second;
				_species.push_back(it - list.begin());
			}
		}

		CBMCMove(double minr, double maxr, int StartBead, int trials, 
		bool expl, unsigned seed = 2496) : 
		_numexploration(trials), _minr(minr), _maxr(maxr), _startingbead(StartBead), _rand(seed), _rejected(0), 
		_performed(0), _seed(seed), _explicit(expl), _species(0), _sstartingbead(-1)
		{
		}

		PlaceBeads(Particle* particle, double& rosenbluth, World* w, EPTuple& EP, bool retrace)
		{
			auto* bondedneighbor = particle.GetBondedNeighbors()

			for(size_t i = 0; i < bondedneighbor.size(); i++)
			{
				// If you have placed this particle, continue
				if(bondedneighbor[i]->GetVisit())
					continue;
				else
				{
					bondedneighbor[i]->SetVisit(true);
					w->AddParticle(bondedneighbor[i]);
				}
				
				double sumrosenbluth = 0;
				for (size_t i = 0; i < _numexploration; i++)
				{
					if(!retrace && i!=0)
					{
						double r = _rand.doub()*(_maxr - _minr) + _minr;

						// Random Placement on sphere, dont forget boundary conditions
						Position newPos({pos1, pos2, pos3});
						w->ApplyPeriodicBoundaries(&newPos);
						_positions[i] = newPos;

						bondedneighbor[i]->SetPosition(_positions[i]);
						w->CheckNeighborListUpdate(bondedneighbor[i]);
					}

					//Store trial widoms
					_wenergy[i] = ffm->EvaluateEnergy(*bondedneighbor[i]);
					_rosenbluths[i] = exp(-beta*_wenergy[i].energy.total());
					sumrosenbluth += _rosenbluths[i];
				}

				rosenbluth *= sumrosenbluth;
				// If getting old rosenbluth value return energy at current spot
				if(retrace)
				{
					EP += _wenergy[0];
				}
				else
				{
					double prob = sumrosenbluth*_rand.doub();

					int i = 0;
					sumrosenbluth = _rosenbluths[i];
					while(prob >= sumrosenbluth && i < _numexploration)
					{
						i++;
						sumrosenbluth += _rosenbluths[i];
					}

					bondedneighbor->SetPosition(_positions[i]);
					
					EP += _wenergy[i];
				}

				PlaceBeads(bondedneighbor[i], rosenbluth, w, EP, retrace);
			}
		}

		EPTuple PlaceFirstBead(Particle* particle, double& rosenbluth, double& beta, World* w, bool retrace)
		{

			auto& sim = SimInfo::Instance();
			auto beta = 1.0/(sim.GetkB()*w->GetTemperature());

			if(!retrace)
			{
				Vector3D pr{_rand.doub(), _rand.doub(), _rand.doub()};
				_positions[0] = H*pr;
				particle->SetPosition(_positions[0]);
			}
			
			w->AddParticle(particle);

			//Store trial widoms
			_wenergy[0] = ffm->EvaluateEnergy(*particle);
			_rosenbluths[0] = exp(-beta*_wenergy[0].energy.total());
			double sumrosenbluth = _rosenbluths[0];

			for (size_t i = 1; i < _numexploration; i++)
			{
				Vector3D pr{_rand.doub(), _rand.doub(), _rand.doub()};
				_positions[i] = H*pr;

				if(retrace && i==0)
					_positions[i] = particle->GetPosition();

				particle->SetPosition(_positions[i]);
				w->CheckNeighborListUpdate(particle);

				//Store trial widoms
				_wenergy[i] = ffm->EvaluateEnergy(*particle);
				_rosenbluths[i] = exp(-beta*_wenergy[i].energy.total());
				sumrosenbluth += _rosenbluths[i];
			}

			rosenbluth *= sumrosenbluth;
			particle->SetVisit(true);

			// If getting old rosenbluth value return energy at current spot
			if(retrace)
				return _wenergy[0];

			// Pick a move based on the boltzman weights
			double prob = sumrosenbluth*_rand.doub();

			int i = 0;
			sumrosenbluth = _rosenbluths[i];
			while(prob >= sumrosenbluth && i < _numexploration)
			{
				i++;
				sumrosenbluth += _rosenbluths[i];
			}

			particle->SetPosition(_positions[i]);
			
			return _wenergy[i];
		}

		// Perform CBMC on a random particle from a random world.
		virtual void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& override) override
		{

			// Initialize Temp informaton
			double oldrosenbluth=1.0;
			double newrosenbluth=1.0;
			std::vector<Position> oldpos;
			Particle* particle = nullptr;
			Particle* child = nullptr;

			// Get random particle from random world.
			World* w = wm->GetRandomWorld();

			if(w->GetParticleCount() == 0)
				return;
			
			// If we explicit draw. 
			if(_explicit)
				particle = w->DrawParticleFromSpeciesList(_species);
			else
				particle = w->DrawRandomParticle();

			// If draw is empty, return.
			if(particle == nullptr)
				return;

			auto StartBead = _startingbead;
			if(_startingbead < 0)
				StartBead = _sstartingbead[particle->GetSpeciesID()];	

			auto& children = particle->GetChildren();
			
			if(particle->HasChildren())
			{
				oldpos.resize(children.size());
				child = children[StartBead];
				for(size_t i = 0; i < children.size(); i++)
				{
					oldpos[i] = children[i]->GetPosition();
					children[i]->SetVisit(false);
				}
			}
			else
			{
				oldpos[0] = particle->GetPosition();
				child = particle;
				child->SetVisit(false);
			}
			
			// Remove particle from the world so you can regrow it
			w->RemoveParticle(particle);

			// Retrace old configuration (true)
			EPTuple ei = PlaceFirstBead(child, oldrosenbluth, w, true);
			PlaceBeads(child, oldrosenbluth, w, ei, true);
			
			// Grow new configuration (false)
			EPTuple ef = PlaceFirstBead(child, newrosenbluth, w, false);
			PlaceBeads(child, newrosenbluth, w, ef, false);
			
			++_performed;	

			ef.energy.constraint = ffm->EvaluateConstraintEnergy(*w);
			Energy de = ef.energy - ei.energy;

			auto& sim = SimInfo::Instance();
			auto beta = 1.0/(sim.GetkB()*w->GetTemperature());

			// Acceptance probability.
			double p = newrosenbluth/oldrosenbluth*exp(-beta*de.total());
			p = p > 1.0 ? 1.0 : p;

			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				for (size_t i = 0; i < children.size(); i++)
				{
					children[i]->SetPosition(oldpos[i]);
					// Update neighbor list if needed.
					w->CheckNeighborListUpdate(children[i]);
				}
				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w->IncrementEnergy(de);
				w->IncrementPressure(ef.pressure - ei.pressure);
			}	
		}
		
		// Perform move using DOS interface.
		virtual void Perform(World* w, ForceFieldManager* ffm, DOSOrderParameter* op , const MoveOverride& override) override
		{
			std::cout<<"Still need to implement DOS for CBMC!"<<std::endl;
			exit(0);	
		}

		virtual double GetAcceptanceRatio() const override
		{
			return 1.0-(double)_rejected/_performed;
		};

		virtual void ResetAcceptanceRatio() override
		{
			_performed = 0;
			_rejected = 0;
		}

		// Serialize.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "CBMC";
			json["seed"] = _seed;
			json["minr"] = _minr;
			json["maxr"] = _maxr;
			json["trials"] = _numexploration;

			if(_startingbead.size() != 0)
			{
				auto& species = Particle::GetSpeciesList();
				for(size_t i = 0; i < species.size(); ++i)
					json["starting bead"][species[i]] = _sstartingbead[i];
			}
			else
				json["_startingbead"] = _startingbead;


			json["explicit_draw"] = _explicit;
		}

		virtual std::string GetName() const override { return "CBMC"; }

		// Clone move.
		Move* Clone() const override
		{
			return new TranslateMove(static_cast<const TranslateMove&>(*this));
		}
	};
}