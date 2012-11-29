#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "user.h"
#include <string>

/**
 This class parses a message from Client to extract a command and its arguments and call User or Channel methods in consequence.
*/
class Parser
{
    public:


        /**
         This method parses a message from a client and send a response via the User
        */
        static void parse(const std::string& message, User* user);

    private:
        /**
         Private constructor, this class will never been constructed
        */
        Parser();
};

#endif // PARSER_H_INCLUDED
