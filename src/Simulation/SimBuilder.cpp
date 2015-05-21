#include "SimBuilder.h"
#include "ForceFields/LebwohlLasherFF.h"

namespace SAPHRON
{
	bool SimBuilder::ValidateWorld(Json::Value world)
	{
		bool err = false;
		// World tag exists.
		if(!world)
		{
			_emsgs.push_back("No world has been specified.");
			return false;
		}

		// World type specified.
		std::string type = [&]() -> std::string {
			try{
				return world.get("type", "").asString();
			} catch(std::exception& e) {
				_emsgs.push_back("Type unserialization error: " + std::string(e.what()));
				err = true;
				return "";
			}
		}();

		if(type.length() == 0)
		{
			_emsgs.push_back("No world type has been specified.");
			err = true;
		}

		// Validate that the world type is a valid entry.
		std::vector<std::string> types =  {"Simple"};
		auto it = std::find(types.begin(), types.end(), type);
		if(it == types.end())
		{
			_emsgs.push_back("The type of world specified is invalid.");

			std::ostringstream s;
			std::copy(types.begin(), types.end(), 
					  std::ostream_iterator<std::string>(s," "));
			_emsgs.push_back("    Valid entries are: " + s.str());

			err = true;
		}

		// Assign world type.
		_worldprops.type = type;

		// Simple world type.
		auto wsize = world["size"];
		if(type == types[0])
		{
			if(!wsize.isArray() || wsize.size() != 3)
			{
				_emsgs.push_back("The world size must be a 1x3 array.");
				err = true;
			}

			double xlength = [&]() -> double {
				try{
					return wsize[0].asDouble();
				} catch(std::exception& e)
				{
					_emsgs.push_back("X-length unserialization error : " + std::string(e.what()));
					err = true;
					return 0;
				}
			}();

			double ylength = [&]() -> double {
				try{
					return wsize[1].asDouble();
				} catch(std::exception& e)
				{
					_emsgs.push_back("Y-length unserialization error : " + std::string(e.what()));
					err = true;
					return 0;
				}
			}();

			double zlength = [&]() -> double {
				try{
					return wsize[2].asDouble();
				} catch(std::exception& e)
				{
					_emsgs.push_back("Z-length unserialization error : " + std::string(e.what()));
					err = true;
					return 0;
				}
			}();

			if(xlength <= 0 || 
			   ylength <= 0 || 
			   zlength <= 0)
			{
				_emsgs.push_back("All world size elements must be greater than zero.");
				err = true;	
			}

			// Assign world size. 
			_worldprops.xlength = xlength;
			_worldprops.ylength = ylength;
			_worldprops.zlength = zlength;
		}

		double rcutoff = [&]()-> double { 
			try {	
				return world.get("cutoff_radius", 0).asDouble(); 
			} catch(std::exception& e) {
				_emsgs.push_back("Cutoff radius unserialization error : " + std::string(e.what()));
				err = true;
				return 0;
			}
		}();
		
		if(!rcutoff)
		{
			_emsgs.push_back("A positive cutoff radius must be specified.");
			err = true;
		}
		if(rcutoff > wsize[0].asDouble()/2.0 || 
		   rcutoff > wsize[1].asDouble()/2.0 || 
		   rcutoff > wsize[2].asDouble()/2.0)
		{
			_emsgs.push_back("The cutoff radius cannot exceed half the shortest world size vector.");
			err = true;
		}

		// Assign cutoff radius. 
		_worldprops.rcutoff = rcutoff;

		int seed = [&]()-> int { 
			try {	
				return world.get("seed", 0).asInt(); 
			} catch(std::exception& e) {
				_emsgs.push_back("Seed unserialization error : " + std::string(e.what()));
				err = true;
				return 0;
			}
		}();

		if(!seed)
		{
			_nmsgs.push_back("No seed provided. Generating a random number.");
			srand(time(NULL));
			_worldprops.seed = rand();
		}
		else if(seed <= 0)
		{
			_emsgs.push_back("Invalid seed specified. Seed cannot be less than or equal to zero.");
			err = true;
		}
		else 
			_worldprops.seed = seed;

		return !err;
	}

	bool SimBuilder::ValidateParticles(Json::Value components, Json::Value particles)
	{
		bool err = false;
		assert(_errors == false);

		// Necessary tags exist.
		if(!components)
		{
			_emsgs.push_back("No components have been specified.");
			return false;
		}

		if(!particles)
		{
			_emsgs.push_back("No particles have been specified.");
			return false;
		}

		// Build blueprint of "components" section.
		ConstructBlueprint(components, "");

		// Parse particles.
		for (int i = 0; i < (int)particles.size(); ++i)
		{
			std::string istr = std::to_string(i);

			std::string species = particles[i].getMemberNames()[0];
			auto particle = particles[i][species];

			// Make sure the species exists as a component.
			// TODO: check children.
			if(_blueprint.find(species) == _blueprint.end())
			{
				_emsgs.push_back("Particle " + istr + 
					" species, " + species + ", has not been delcared as a component.");
				return false;
			}

			// Get blueprint.
			auto blueprint = _blueprint[species];

			// Validate parent. 
			auto parent = [&]() -> std::string {
				try {
					return particle.get("parent", "").asString();
				} catch(std::exception& e) {
					err = true;
					_emsgs.push_back("Particle " + istr + 
					" parent unserialization error : " + std::string(e.what()));
					return "";
				}
			}();

			if(parent != blueprint.parent)
			{
				std::cout << "Parent: " << parent << " " << blueprint.parent << std::endl;
				_emsgs.push_back("Particle " + istr + " parent must match component definition.");
				err = true;
			}

			// Validate residue.
			auto residue = particle.get("residue", "").asString();

			// Make sure molecule is not specified. Only primitives/atoms. 
			if((int)blueprint.children.size() != 0)
			{
				_emsgs.push_back("Particle " + istr + " cannot have children. Check definition.");
				err = true;
			}

			// Validate position.
			if(!particle["position"].isArray() || particle["position"].size() != 3)
			{
				_emsgs.push_back("Particle " + istr + " position must be a 1x3 array.");
				err = true;
			}

			Position position = [&]() -> Position {
				try{
					double x = particle["position"][0].asDouble();
					double y = particle["position"][1].asDouble();
					double z = particle["position"][2].asDouble();
					return Position{x,y,z};

				} catch(std::exception& e)
				{
					_emsgs.push_back("Particle "  + 
						istr + " position unserialization error : " + 
						std::string(e.what()));
					err = true;
					return Position{NAN, NAN, NAN};
				}
			}();

			if(position.x == NAN || position.y == NAN || position.z == NAN)
			{
				_emsgs.push_back("Particle positions must be valid doubles.");
				err = true;	
			}

			if(position.x < 0 || 
			   position.y < 0 || 
			   position.z < 0 || 
			   position.x > _worldprops.xlength || 
			   position.y > _worldprops.ylength || 
			   position.z > _worldprops.zlength)
			{
				_emsgs.push_back("Particle " + istr + " positions must be between 0 and world size.");
				err = true;	
			}

			// Validate director.
			Director director({0.0, 0.0, 0.0});
			if(!particle["director"])
			{
			} 
			else 
			{
				if(!particle["director"].isArray() || particle["director"].size() != 3)
				{
					_emsgs.push_back("Particle " + istr + " director must be a 1x3 array.");
					err = true;
				}
				else 
				{
					director = [&]() -> Director {
						try{
							double x = particle["director"][0].asDouble();
							double y = particle["director"][1].asDouble();
							double z = particle["director"][2].asDouble();
							return Director{x,y,z};

						} catch(std::exception& e)
						{
							_emsgs.push_back("Particle "  + 
								istr + " director unserialization error : " + 
								std::string(e.what()));
							err = true;
							return Director{NAN, NAN, NAN};
						}
					}();
				}
			}

			// Check norm.
			double norm = sqrt(director[0]*director[0] + 
							   director[1]*director[1] + 
							   director[2]*director[2]);
			if(std::abs(norm - 1.0) > 1e-7 && norm != 0)
			{
				_nmsgs.push_back("Particle " + istr + " norm is not equal to 1. Normalizing vector.");
				director[0] /= norm;
				director[1] /= norm;
				director[2] /= norm;
			}

			// Push back struct 
			_particles.push_back({position, director, species, parent, residue, 0, {}});
		}

		return !err;
	}

	bool SimBuilder::ValidateForceFields(Json::Value forcefields)
	{
		bool err = false;
		assert(_errors == false);

		// Necessary tags exist.
		if(!forcefields)
		{
			_emsgs.push_back("No forcefields have been specified.");
			return false;
		}

		// Validate type.
		if(!forcefields.isArray())
		{
			_emsgs.push_back("Forcefield specification must be an array.");
			return false;
		}

		// Valid forcefields  . 
		std::vector<std::string> types = {"LebwohlLasher"};


		for (int i = 0; i < (int)forcefields.size(); ++i)
		{
			
			// forcefield type specified.
			std::string type = [&]() -> std::string {
				try{
					return forcefields[i].getMemberNames()[0];
				} catch(std::exception& e) {
					_emsgs.push_back("Type unserialization error: " + std::string(e.what()));
					err = true;
					return "";
				}
			}();

			auto forcefield = forcefields[i][type];

			if(type.length() == 0)
			{
				_emsgs.push_back("A forcefield has been incorrectly specified.");
				err = true;
			}
			else
			{
				// Validate that the forcefield type is a valid entry.
				auto it = std::find(types.begin(), types.end(), type);
				if(it == types.end())
				{
					_emsgs.push_back("The type of forcefield specified is invalid.");

					std::ostringstream s;
					std::copy(types.begin(), types.end(), 
							  std::ostream_iterator<std::string>(s," "));
					_emsgs.push_back("    Valid entries are: " + s.str());
					err = true;
				}
				else
				{
					ForceFieldProps ff;
					// Lebwohl Lasher
					if(type == types[0])
					{
						if(!forcefield["epsilon"] || !forcefield["gamma"])
						{
							_emsgs.push_back("Both epsilon and gamma must be specified for the " + type + " forcefield.");
						}

						double epsilon = [&]()-> double { 
							try {	
								return forcefield.get("epsilon", 0).asDouble(); 
							} catch(std::exception& e) {
								_emsgs.push_back("Epsilon unserialization error : " + std::string(e.what()));
								err = true;
								return 0;
							}
						}();	

						double gamma = [&]()-> double { 
							try {	
								return forcefield.get("gamma", 0).asDouble(); 
							} catch(std::exception& e) {
								_emsgs.push_back("Epsilon unserialization error : " + std::string(e.what()));
								err = true;
								return 0;
							}
						}();

						ff.type = type;
						ff.parameters.push_back(epsilon);
						ff.parameters.push_back(gamma);
					}

					// Validate species 
					auto species = forcefield["species"];
					if(!species || !species.isArray() || species.size() != 2)
					{
						_emsgs.push_back("Forcefield " + type  + " species is required and must be a 1x2 array.");
						err = true;
					}

					std::string species1 = [&]() -> std::string {
						try {
							return species[0].asString();
						} catch(std::exception& e) {
							err = true;
							_emsgs.push_back("Forcefield " + type + 
							" species unserialization error : " + std::string(e.what()));
							return "";
						}
					}();

					std::string species2 = [&]() -> std::string {
						try {
							return species[1].asString();
						} catch(std::exception& e) {
							err = true;
							_emsgs.push_back("Forcefield " + type + 
							" species unserialization error : " + std::string(e.what()));
							return "";
						}
					}();

					// Make sure species exist in our blueprint. 
					if(_blueprint.find(species1) == _blueprint.end())
					{
						_emsgs.push_back("Forcefield " + type + 
							" is defined for an undefined species " + species1 + ".");
						err = true;
					}

					if(_blueprint.find(species2) == _blueprint.end())
					{
						_emsgs.push_back("Forcefield " + type + 
							" is defined for an undefined species " + species2 + ".");
						err = true;
					}

					ff.species1 = species1;
					ff.species2 = species2;

					_forcefields.push_back(ff);
				}
			}
		}

		return !err;
	}

	bool SimBuilder::ConstructBlueprint(Json::Value components, std::string parent)
	{
		bool err = false;

		auto species = components.getMemberNames();
		for(int i = 0; i < (int)components.size(); ++i)
		{
			auto component = components[species[i]];
			// Species is new, add to blueprint.
			if(_blueprint.find(species[i]) == _blueprint.end())
			{
				ParticleProps blueprint;
				blueprint.species = species[i];
				blueprint.parent = parent;

				blueprint.index = [&]() -> int {
					try {
						return component.get("index", 0).asInt();
					} catch(std::exception& e) {
						err = true;
						_emsgs.push_back("Component" + species[i] + "index unserialization error : " + 
							std::string(e.what()));
						return 0;
					}
				}();

				// Enumerate children.
				if(component.isMember("children"))
				{
					auto children = component["children"];
					if(!children.isArray())
					{
						_emsgs.push_back("Component " + species[i] + " children must be an array.");
						err = true;
					}
					else 
					{
						for(int j = 0; j < (int)children.size(); ++j)
						{							
							ConstructBlueprint(children[j], species[i]);
							blueprint.children.push_back(children[j].getMemberNames()[0]);
						}
					}
				}
				_blueprint[species[i]] = blueprint;
				_nmsgs.push_back("Created blueprint for " + species[i] + ".");
			}
		}

		return !err;
	}


	World* SimBuilder::BuildWorld()
	{
		assert(_errors == false);

		if(_worldprops.type == "Simple")
		{
			_nmsgs.push_back("Setting size to [" +  
				std::to_string(_worldprops.xlength) + ", " + 
				std::to_string(_worldprops.ylength) + ", " + 
				std::to_string(_worldprops.zlength) + "] \u212B.");
			_nmsgs.push_back("Setting cutoff radius to " + std::to_string(_worldprops.rcutoff) + " \u212B.");
			_nmsgs.push_back("Setting seed to " + std::to_string(_worldprops.seed) + ".");
			return new SimpleWorld(_worldprops.xlength, 
								   _worldprops.ylength, 
								   _worldprops.zlength, 
								   _worldprops.rcutoff,
								   _worldprops.seed);
		}

		return nullptr;
	}

	void SimBuilder::BuildParticles(World* world)
	{
		assert(_errors = false);
		
		std::map<std::string, int> counts;

		for (int i = 0; i < (int)_particles.size(); ++i)
		{
			auto particle = _particles[i];

			// Count.
			if(counts.find(particle.species) == counts.end())
				counts[particle.species] = 1;
			else
				++counts[particle.species];

			// Initialize particles.
			world->AddParticle(new Site(particle.position, particle.director, particle.species));
		}

		for(auto& count : counts)
			_nmsgs.push_back("Initialized " + 
				std::to_string(count.second) + " particle(s) of type " + count.first + ".");
	}

	void SimBuilder::BuildForceFields(std::vector<ForceField*>& forcefields, ForceFieldManager& ffm)
	{
		assert(_errors = false);

		for(auto& forcefield : _forcefields)
		{
			if(forcefield.type == "LebwohlLasher")
			{
				ForceField* ff = new LebwohlLasherFF(forcefield.parameters[0], 
													 forcefield.parameters[1]);
				
				ffm.AddForceField(forcefield.species1, forcefield.species2, *ff);
				forcefields.push_back(ff);
				_nmsgs.push_back("Initialized " + forcefield.type + " force field for [" + 
					forcefield.species1 + "," + forcefield.species2 + 
					"]\n     with epsilon = "  + std::to_string(forcefield.parameters[0]) + 
					" and gamma = " + std::to_string(forcefield.parameters[1]) + ".");
			}
		}
	}
}