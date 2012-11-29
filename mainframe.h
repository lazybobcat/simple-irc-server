#ifndef MAINFRAME_H_INCLUDED
#define MAINFRAME_H_INCLUDED

#include <map>
#include <string>

#include "server.h"
#include "user.h"
#include "channel.h"


typedef std::map<std::string, User*>    UserMap; ///< Make life simple
typedef std::map<std::string, Channel*> ChannelMap; ///< Make life simple

/**
 This class is a Singleton in order to be accessed from anywhere. It contains Users and Channels.
*/
class Mainframe
{
    public:
        /**
         Returns an unique instance on Manframe
        */
        static Mainframe*   instance();

        /**
         Shoud be call in main ONCE, starts the whole IRC Server
        */
        void    start();


        /////////////////////////////////////////////////////////////////////////
        //                           USERS                                     //
        /////////////////////////////////////////////////////////////////////////

        /**
         Return true if the given nickname is already registered
        */
        bool    doesNicknameExists(const std::string& nick);

        /**
         Try to add a User into the UserMap.
         Return true if succeed, else the Nickname is already used
        */
        bool    addUser(User* user);

        /**
         Remove User by nickname
        */
        void    removeUser(const std::string& nick);

        /**
         Try to change a User nickname
         Return true if succeed, else the Nickname is already used
        */
        bool    changeNickname(const std::string& old, const std::string& recent);

        /**
         Returns a pointer on the User that hasthe given nickname. Returns NULL if User does not exists
        */
        User*   getUserByName(const std::string& nick);



        /////////////////////////////////////////////////////////////////////////
        //                           CHANNELS                                  //
        /////////////////////////////////////////////////////////////////////////

        /**
         Returns true if the given channel exists
        */
        bool    doesChannelExists(const std::string& name);

        /**
         Adds a new channels to the Channel Map
        */
        void    addChannel(Channel* chan);

        /**
         Remove a Channel by name
        */
        void    removeChannel(const std::string& name);

        /**
         Returns a pointer on the Channel that has the given name. Returns NULL if channel does not exists
        */
        Channel* getChannelByName(const std::string& name);

        /**
         Do operations on Channels. For now, it remove all empty channels
        */
        void    updateChannels();

        /**
         Get a copy of channel map
        */
        ChannelMap channels() const;



    private:
        Mainframe(); ///< Private constructor because this class is a singleton
        ~Mainframe(); ///< Private destructor
        Mainframe(const Mainframe&); ///< Private copy constructor to prevent copies
        Mainframe& operator=(Mainframe&); ///< Private operator= to prevent copies

        /**
         Delete all channels at destruction
        */
        void removeAllChannels();


        static Mainframe*       mInstance; ///< Unique instance of this class
        UserMap                 mUsers; ///< Map of users by Nickname
        ChannelMap              mChannels; ///< Map of Channels by channel name
};

#endif // MAINFRAME_H_INCLUDED
