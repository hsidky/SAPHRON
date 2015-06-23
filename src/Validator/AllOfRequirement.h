#pragma once 

#include "Requirement.h"
#include "RequirementLoader.h"

namespace Json
{
	class AllOfRequirement : public Requirement
	{
	private:
		std::vector<Requirement*> _reqs;

	public:
		AllOfRequirement() : _reqs(0) {}

		virtual void ClearErrors() override
		{
			for(auto& r : _reqs)
				r->ClearErrors();

			Requirement::ClearErrors();
		}

		virtual void ClearNotices() override
		{
			for(auto& r : _reqs)
				r->ClearNotices();

			Requirement::ClearNotices();
		} 

		virtual void Reset() override
		{
			ClearErrors();
			ClearNotices();
		
			for(auto& r : _reqs)
				delete r;
			_reqs.clear();
		}

		virtual void Parse(Value json, std::string path) override
		{
			Reset();
			RequirementLoader loader;

			auto& head = json.isMember("allOf") ? json["allOf"] : json;

			for(auto& val : head)
				if(auto* req = loader.LoadRequirement(val))
				{
					_reqs.push_back(req);
					_reqs.back()->Parse(val, path);
				}

		}

		virtual void Validate(const Value& json, std::string path) override
		{
			for(auto& r : _reqs)
			{
				r->Validate(json, path);
		
				if(r->HasErrors())
					for(const auto& error : r->GetErrors())
						PushError(error);
		
				if(r->HasNotices())
					for(const auto& notice : r->GetNotices())
						PushNotice(notice);
			}
		}
	};
}