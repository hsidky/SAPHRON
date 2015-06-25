#pragma once 

#include <iostream>

namespace SAPHRON
{
	class JsonSchema
	{
	public:
		//INSERT_DEC_HERE
		static std::string Worlds;
		static std::string SimpleWorld;
		static std::string Selector;
		static std::string Director;
		static std::string Observer;
		static std::string CSVObserver;
		static std::string TranslateMove;
		static std::string SphereUnitVectorMove;
		static std::string SpeciesSwapMove;
		static std::string ParticleSwapMove;
		static std::string Moves;
		static std::string IdentityChangeMove;
		static std::string FlipSpinMove;
		static std::string GibbsNVTEnsemble;
		static std::string NVTEnsemble;
		static std::string Ensembles;
		static std::string P2SAConnectivity;
		static std::string Connectivities;
		
	};
}

//INSERT_DEF_HERE
std::string SAPHRON::JsonSchema::Worlds = "{\"items\": {\"required\": [\"type\", \"dimensions\", \"nlist_cutoff\", \"skin_thickness\"], \"type\": \"object\", \"properties\": {\"skin_thickness\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"Simple\"], \"type\": \"string\"}, \"dimensions\": {\"minItems\": 3, \"items\": {\"minimum\": 0, \"type\": \"number\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 3}, \"nlist_cutoff\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}}}, \"type\": \"array\"}";
std::string SAPHRON::JsonSchema::SimpleWorld = "{\"required\": [\"type\", \"dimensions\", \"nlist_cutoff\", \"skin_thickness\"], \"type\": \"object\", \"properties\": {\"skin_thickness\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"Simple\"], \"type\": \"string\"}, \"dimensions\": {\"minItems\": 3, \"items\": {\"minimum\": 0, \"type\": \"number\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 3}, \"nlist_cutoff\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}}}";
std::string SAPHRON::JsonSchema::Selector = "{}";
std::string SAPHRON::JsonSchema::Director = "{\"minItems\": 3, \"items\": {\"type\": \"number\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 3}";
std::string SAPHRON::JsonSchema::Observer = "{\"oneOf\": [{\"required\": [\"file_prefix\"], \"properties\": {\"file_prefix\": {\"type\": \"string\"}}}], \"type\": \"object\", \"properties\": {\"frequency\": {\"minimum\": 1, \"type\": \"integer\"}, \"flags\": {\"type\": \"object\", \"properties\": {\"temperature\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world_density\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"energy\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"acceptance\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"sweeps\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"pressure\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world_count\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"identifier\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world_composition\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world_volume\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}}}}}";
std::string SAPHRON::JsonSchema::CSVObserver = "{\"required\": [\"file_prefix\"], \"properties\": {\"file_prefix\": {\"type\": \"string\"}}}";
std::string SAPHRON::JsonSchema::TranslateMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"dx\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"Translate\"], \"type\": \"string\"}, \"dx\": {\"minimum\": 0, \"type\": \"number\"}}}";
std::string SAPHRON::JsonSchema::SphereUnitVectorMove = "{\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"SphereUnitVector\"], \"type\": \"string\"}}}";
std::string SAPHRON::JsonSchema::SpeciesSwapMove = "{\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"type\": {\"enum\": [\"SpeciesSwap\"], \"type\": \"string\"}}}";
std::string SAPHRON::JsonSchema::ParticleSwapMove = "{\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"ParticleSwap\"], \"type\": \"string\"}}}";
std::string SAPHRON::JsonSchema::Moves = "{\"items\": {\"oneOf\": [{\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"type\": {\"enum\": [\"FlipSpin\"], \"type\": \"string\"}}}, {\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"type\": {\"enum\": [\"IdentityChange\"], \"type\": \"string\"}}}, {\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"ParticleSwap\"], \"type\": \"string\"}}}, {\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"type\": {\"enum\": [\"SpeciesSwap\"], \"type\": \"string\"}}}, {\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"SphereUnitVector\"], \"type\": \"string\"}}}, {\"additionalProperties\": false, \"required\": [\"type\", \"dx\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"Translate\"], \"type\": \"string\"}, \"dx\": {\"minimum\": 0, \"type\": \"number\"}}}, {\"additionalProperties\": false, \"required\": [\"type\", \"dv\"], \"type\": \"object\", \"properties\": {\"dv\": {\"minimum\": 0, \"type\": \"number\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"VolumeScale\"], \"type\": \"string\"}}}]}, \"type\": \"array\"}";
std::string SAPHRON::JsonSchema::IdentityChangeMove = "{\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"type\": {\"enum\": [\"IdentityChange\"], \"type\": \"string\"}}}";
std::string SAPHRON::JsonSchema::FlipSpinMove = "{\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"type\": {\"enum\": [\"FlipSpin\"], \"type\": \"string\"}}}";
std::string SAPHRON::JsonSchema::GibbsNVTEnsemble = "{\"additionalProperties\": false, \"required\": [\"type\", \"temperature\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"GibbsNVT\"], \"type\": \"string\"}, \"temperature\": {\"exclusiveMinimum\": \"true\", \"minimum\": 0, \"type\": \"number\"}, \"sweeps\": {\"minimum\": 1, \"type\": \"integer\"}}}";
std::string SAPHRON::JsonSchema::NVTEnsemble = "{\"additionalProperties\": false, \"required\": [\"type\", \"temperature\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"NVT\"], \"type\": \"string\"}, \"temperature\": {\"exclusiveMinimum\": \"true\", \"minimum\": 0, \"type\": \"number\"}, \"sweeps\": {\"minimum\": 1, \"type\": \"integer\"}}}";
std::string SAPHRON::JsonSchema::Ensembles = "{\"oneOf\": [{\"additionalProperties\": false, \"required\": [\"type\", \"temperature\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"NVT\"], \"type\": \"string\"}, \"temperature\": {\"exclusiveMinimum\": \"true\", \"minimum\": 0, \"type\": \"number\"}, \"sweeps\": {\"minimum\": 1, \"type\": \"integer\"}}}, {\"additionalProperties\": false, \"required\": [\"type\", \"temperature\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"GibbsNVT\"], \"type\": \"string\"}, \"temperature\": {\"exclusiveMinimum\": \"true\", \"minimum\": 0, \"type\": \"number\"}, \"sweeps\": {\"minimum\": 1, \"type\": \"integer\"}}}]}";
std::string SAPHRON::JsonSchema::P2SAConnectivity = "{\"additionalProperties\": false, \"required\": [\"type\", \"coefficient\", \"director\", \"selector\"], \"type\": \"object\", \"properties\": {\"coefficient\": {\"type\": \"number\"}, \"director\": {\"minItems\": 3, \"items\": {\"type\": \"number\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 3}, \"type\": {\"enum\": [\"P2SA\"], \"type\": \"string\"}, \"selector\": {}}}";
std::string SAPHRON::JsonSchema::Connectivities = "{\"items\": {\"oneOf\": [{\"additionalProperties\": false, \"required\": [\"type\", \"coefficient\", \"director\", \"selector\"], \"type\": \"object\", \"properties\": {\"coefficient\": {\"type\": \"number\"}, \"director\": {\"minItems\": 3, \"items\": {\"type\": \"number\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 3}, \"type\": {\"enum\": [\"P2SA\"], \"type\": \"string\"}, \"selector\": {}}}]}, \"type\": \"array\"}";
