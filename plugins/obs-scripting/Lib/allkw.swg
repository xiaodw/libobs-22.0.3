#ifndef __Lib_allkw_swg__
#define __Lib_allkw_swg__


/*  
  Include all the known keyword warnings.  Very useful for adding test
  files to the test-suite, or checking if your own library is ok for all
  the swig supported languages.

  Use as 

    swig -Wallkw ...

  If you add a new language, remember to create a separate languagekw.swg
  file, and add it here.
  
*/

%include <lua/luakw.swg>



#endif //__Lib_allkw_swg__
