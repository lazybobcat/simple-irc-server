#ifndef CHANNEL_H_INCLUDED
#define CHANNEL_H_INCLUDED

#include <set>
#include <string>
#include "user.h"

typedef std::set<User*>     UserSet; ///< Make life simple

/**
 Channel implementation.
 A Channel has a pointer on its Users, on its Operators, has a topic and sometimes a password, limited slots
 A lot of actions can be performed on Channels : commands from users/operators or User management.
 The channel is also responsible for sending messages to all its Users (broadcast).
*/
class Channel
{
    public:
        /**
         Constructor : Sets booleans to false, without topic, without password and without limited slots
        */
        Channel(User* creator, const std::string& name, const std::string& topic = "");

        /**
         Default destructor
        */
        ~Channel();

        /************************************************************
                                COUCHE METIER
        ************************************************************/

        /**
         Promotes a User (here: victim) as Operator.
         Requires the victim to be in the channel and the user that promoted it to be Operator
        */
        void    cmdOPlus(User *user, User *victim);

        /**
         De-promote the User to simple User.
         Note : for now, the User can only de-promote himself because an OP cannot depromote another OP and
         there is only one rank.
        */
        void    cmdOMinus(User *user);

        /**
         Set the channel password
         Next Users that will try to join the channel will need the password
        */
        void    cmdKPlus(const std::string& newPass);

        /**
         Removes the password
        */
        void    cmdKMinus();

        /**
         Limits the slots for the channel. Users can join the channel until it's full.
         Note : if there are already more Users than the limit, there will be no kicks :p
        */
        void    cmdLPlus(const std::string& newNbPlace);

        /**
         Removes the slot limit
        */
        void    cmdLMinus();

        /**
         Sets the Channel topic to "topic"
        */
        void    cmdTopic(const std::string& topic);


        /**
         Adds the User to the channel and send the new user list to all channel clients
        */
        void    addUser(User* user);

        /**
         Removes the user from channel uses and ops and send the new user list to all channel clients
        */
        void    removeUser(User* user);

        /**
         Returns true if the channel contains that User, else returns false
        */
        bool    hasUser(User* user);

        /**
         Returns true if the given User is Operator on that channel, else return false
        */
        bool    isOperator(User* user);


        /**
         Send the channel user list to all channel users
        */
        void    sendUserList();

        /**
         Send the channel user list to all channel users formated for a WHO command
        */
        void    sendWhoList();

        /**
        Send the given message to all channel users
        */
        void    broadcast(const std::string& message);



        std::string password() const;   ///< Returns the channel password
        std::string name() const;       ///< Returns the channel name
        std::string topic() const;      ///< Returns the channel topic
        unsigned int userCount() const;      ///< Returns the number of Users on the channel
        bool        empty() const;      ///< Returns true if the channel is empty (no users)
        bool        full() const;       ///< Returns true if the channel is full (limited slots reached)
        bool        hasPass() const;    ///< Returns true if the channel has a password
        bool        limited() const;    ///< Returns true if the channel has limited slots

    private:

        const std::string   mName;
        std::string         mTopic;
        UserSet             mUsers;
        UserSet             mOperators;
        std::string         mPassword;
        unsigned int        mSlots;
        bool                bPassword;
        bool                bLimited;
};

#endif // CHANNEL_H_INCLUDED
