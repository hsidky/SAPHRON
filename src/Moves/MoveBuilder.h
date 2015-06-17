#pragma once 


namespace SAPHRON
{
	class MoveBuilder
	{
	private:

	public:
		MoveBuilder();
 
		bool Validate(const char* json);

		~MoveBuilder()
		{
		}	
	};
}