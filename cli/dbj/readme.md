
### Replacements for external lib's and other dramatic changes

In the original CLI code, I have done various changes.

For example: beside 'boos asio', all the other  'boost' dependancies are cut and replaced with 'dbj' stuff, to be found in this folder. 

My aim is to improve the resilience and performance by replacing std::string wherever possoible with string_view, and especially with 
light and fast buffers based on `std::unique_ptr<char[]n>`.

In `cli.h` , instead of global inclusions path is now local.

Thus,  instead of:

    #include <boost/lexical_cast.hpp>   
    #include <boost/algorithm/string.hpp>

I have done this:

    // transformation from cli string
    // to various types as required
    #include "dbj/type_trans.h"

    // curently string related
    // algoithms previously used
    // from boost
    #include "dbj/dbj_algo.h"

    // simple and fast buffering 
    // based on std::unique_ptr<char[]>_
    // for details please review the tests
    // inside
    #include "dbj/dbj_buffer.h"

    // formated output into the dbj buffers
    // please review the test inside
    #include "dbj/dbj_format.h"

#### Notes 

> Where applicable I have placed the links to the source.

>GPLv3 licence for this part is in the file LICENCE.h

>string_view.h implementation is enitrely external. Please respect authors copyright.

The key aim is to have self contained, light and fast CLI. 