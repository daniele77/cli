#pragma once

#include <exception>
#include <string>

#if __cplusplus < 201703
#include "string_veiw.h"
#endif

namespace dbj
{
	// NOTE: wherever possible I am using string_view
#if __cplusplus < 201703
	using bpstd::string_view; // back-port std project
#endif

    // this is just to make CLI transition easier
	struct bad_lexical_cast final : std::exception
	{
		explicit bad_lexical_cast(char const * msg_)
			: std::exception(msg_) {}
	};

	namespace  inner {
		
		inline int svtoi(string_view  text)
		{
			try {
				return std::stoi(text.data());
			}
			catch (std::exception & x) {
				throw bad_lexical_cast(x.what());
			}
		}

		inline int svtol(string_view  text)
		{
			try {
				return std::stol(text.data());
			}
			catch (std::exception & x) {
				throw bad_lexical_cast(x.what());
			}
		}

		inline double svtod(string_view  text)
		{
			try {
				return std::stod(text.data());
			}
			catch (std::exception & x) {
				throw bad_lexical_cast(x.what());
			}
		}
	} // 

	// locale aware trasformations from string to another type
	template < typename T > struct transformer {};

	template <> struct transformer< char > {
		char operator () (string_view arg_) {
			return arg_[0];
		}
	};

	template <> struct transformer< unsigned char > {
		unsigned char operator () (string_view arg_) { return (unsigned char)(arg_[0]); };
	};

	template <> struct transformer< short > {
		short operator () (string_view arg_) { return  short(inner::svtoi(arg_)); };
	};

	template <> struct transformer< unsigned short > {
		unsigned short operator () (string_view arg_) { return (unsigned short)(inner::svtoi(arg_)); };
	};

	template <> struct transformer< int > {
		int operator () (string_view arg_) { return (inner::svtoi(arg_)); };
	};

	template <> struct transformer< unsigned int > {
		unsigned int operator () (string_view arg_) { return (unsigned int)(inner::svtoi(arg_)); };
	};

	template <> struct transformer< long > {
		long operator () (string_view arg_) { return (long)(inner::svtol(arg_)); };
	};

	template <> struct transformer< unsigned long > {
		unsigned long operator () (string_view arg_) { return (unsigned long)(inner::svtol(arg_)); };
	};

	template <> struct transformer< float > {
		float operator () (string_view arg_) { return (float)(inner::svtod(arg_)); };
	};

	template <> struct transformer< double > {
		double operator () (string_view arg_) {
			return (double)(inner::svtod(arg_));
		};
	};

	template <> struct transformer< long double > {
		long double operator () (string_view arg_) { return (long double)(inner::svtod(arg_)); }
	};

	template <> struct transformer< bool > {
		bool operator () (string_view arg_) {

			if (arg_[0] == '0') return false;
			if (arg_[0] == '1') return true;

			if (arg_ ==  "true") return true;
			if (arg_ ==  "false") return false;

			if (arg_ ==  "T") return true;
			if (arg_ ==  "F") return false;

			return false;
		}
	};

	template <> struct transformer< std::string > {
		std::string operator () (string_view arg_) { return arg_.data(); };
	};

} // dbj