#pragma once 

#include <vector>
#include <map>
#include "json/json.h"

namespace Json
{
	class Requirement
	{
	private:
		std::vector<std::string> _errors;
		std::vector<std::string> _notices;

	protected:
		void PushError(std::string error) { _errors.push_back(error); }

		void PushNotice(std::string notice) { _notices.push_back(notice); }
	
	public:
		virtual void Parse(Value json, std::string path) = 0;

		virtual void Validate(const Value& json, std::string path) = 0;

		virtual void Reset() = 0;

		bool HasErrors() { return _errors.size() != 0; };

		std::vector<std::string> GetErrors() { return _errors; };

		virtual void ClearErrors() { _errors.clear(); }

		virtual bool HasNotices() {return _notices.size() != 0; };

		std::vector<std::string> GetNotices() {return _notices; };

		virtual void ClearNotices() { _notices.clear(); }

		virtual ~Requirement() {}
	};
}