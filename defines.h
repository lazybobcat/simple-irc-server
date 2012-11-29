#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

#include "config.h"
#define TESTMODE

#include <string>
#include <sstream>

/**
 A usefull function to convert anything into a std::string
*/
template<typename T>
std::string ToString( const T & Value )
{
    // utiliser un flux de sortie pour créer la chaîne
    std::ostringstream oss;
    // écrire la valeur dans le flux
    oss << Value;
    // renvoyer une string
    return oss.str();
}


#endif // DEFINES_H_INCLUDED
