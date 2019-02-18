
### Replacements for external lib's

In the code 'boost' is replaced with 'dbj'stuff. 
Instead of global inclusion path is local.

Example. Instead of:

    #include <boost/lexical_cast.hpp>   
    #include <boost/algorithm/string.hpp>

I have done this:

    #include "dbj/lexical_cast.h"
    #include "dbj/algorithm_string.h"

#### Note: 

> Inside is mainly very simple stuff. 
> Thus it is irrelevent who is the author.
> Where applicable I have placed the links to the source.

The key aim is to have self contained lib. 