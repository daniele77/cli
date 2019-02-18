#pragma once

#include <exception>
#include <string>

namespace dbj {

	using namespace std;

	struct bad_lexical_cast final : std::exception 
	{
		explicit bad_lexical_cast( char const * msg_)
			: std::exception(msg_) {}
	};
	 
	// std::stoi() is locale aware and replaces nicely 
	// boost::lexical_cast<T>

	int stoi (string  text )
	{
		try {
			return std::stoi(text);
		}
		catch (exception & x) {
			throw bad_lexical_cast(x.what());
		}
	}

} // dbj