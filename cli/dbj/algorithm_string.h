#pragma once

#if __cplusplus < 201703
#include "string_veiw.hpp"
#endif

#include <algorithm>

namespace dbj {
	namespace algorithm {

#if __cplusplus < 201703
		using bpstd::string_view ; // back-port std project
#endif

		// NOTE: wherever possible I am using string_view

		using namespace std;
		using string_vector = vector<string>;

		// synopsis: 
		// dbj::algorithm::starts_with(name, line)
		// returns true if line strate with name
		// NOTE: this should work with C++11 also
		inline bool starts_with(string_view  prefix, string_view text)
		{
			if ((prefix.size() > text.size())) return false;
			return (
				std::equal(prefix.begin(), prefix.end(), text.begin())
				);
		}

		namespace {
			// anonymous ns makes this static in essence
			char const * white_space = " \t\n\r\f\v";
		}

		inline string trim_left(string_view text, char const * t = white_space) {
			text.remove_prefix(text.find_first_not_of(t));
			return text.data();
		}

		inline string trim_right(string_view text, char const * t = white_space) {
			text.remove_suffix(text.find_last_not_of(t) + 1);
			return text.data();
		}

		inline string trim(string_view text, char const * t = white_space) {

			return trim_left(trim_right(text, t), t);
		}

		// https://github.com/fenbf/StringViewTests/blob/master/StringViewTest.cpp
		// strings, but works on pointers rather than iterators
		// code by JFT
		// DBJ: changed argument types to be string_view, not string
		string_vector fast_string_split(
			const string_view & str, 
			const string_view & delims = " \t\v\n\r\f"
		)
		{
			string_vector output;
			//output.reserve(str.size() / 2);

			for (auto first = str.data(), second = str.data(), last = first + str.size(); second != last && first != last; first = second + 1) {
				second = std::find_first_of(first, last, std::cbegin(delims), std::cend(delims));

				if (first != second)
					output.emplace_back(first, second);
			}

			return output;
		}
	}
} // dbj