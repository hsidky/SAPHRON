#pragma once 

#include <istream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include "json/json.h"

namespace SAPHRON
{
	class SimBuilder 
	{
		private: 
			
			struct WorldProps
			{
				std::string type;
				std::vector<double> size;
				double seed;
				double rcutoff;
			} _worldprops;

			Json::Reader _reader;
			Json::Value _root;

			bool _errors;

			// Error messages.
			std::vector<std::string> _emsgs;

			// Note messages.
			std::vector<std::string> _nmsgs;

			bool ValidateWorld(Json::Value world)
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
					_worldprops.size.push_back(xlength);
					_worldprops.size.push_back(ylength);
					_worldprops.size.push_back(zlength);
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

				double seed = [&]()-> double { 
					try {	
						return world.get("seed", 0).asDouble(); 
					} catch(std::exception& e) {
						_emsgs.push_back("Seed unserialization error : " + std::string(e.what()));
						err = true;
						return 0;
					}
				}();

				if(!seed)
				{
					_nmsgs.push_back("No seed provided. Generating random number.");
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

		public:
			SimBuilder() : _worldprops(), _reader(), _root(), _errors(false), _emsgs(0), _nmsgs(0) {}

			// Parse stream input. Returns true if parse was successful or 
			// false if otherwise. Error(s) can be obtained via GetParsingErrors().
			bool ParseInput(std::istream& is)
			{
				if(!_reader.parse(is, _root))
				{
					_errors = true;
					_emsgs.push_back(_reader.getFormattedErrorMessages());
					return false;
				}

				// Validate world. 
				if(!ValidateWorld(_root["world"]))
				{
					_errors = true;
					return false;
				}

				return true;
			} 

			std::vector<std::string> GetErrorMessages()
			{
				return _emsgs;
			}



	};
}