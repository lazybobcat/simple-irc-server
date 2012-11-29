#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <set>

class Session;
class Channel;

typedef std::vector<std::string> StrVec; ///< Will be usefull to parse a message string into command+arguments array
typedef std::set<Channel*>       ChannelSet; ///< Make our life simple

/**
 The User class represent the Client data and possible actions.
 So it contains the nickname, realname, channels etc.. and a set of commands that modify these datas.
*/
class User
{
    friend class Session; ///< Session and User are very linked and together represent the Client. So Session may need to access to some methods/attributes directly

    public:
        /**
         Constructor : set mSession to mysession and booleans to false
         The User class should be only constructed by Session class
        */
        User(Session*   mysession);

        /**
         Destructor : make sure the User has been removed from Mainframe users and from Channels users
        */
        ~User();


        /************************************************************
                                COUCHE METIER
        ************************************************************/

        /**
         The User want to change or set its nickname.
         This will check if the new nick is available.
         If yes, it will alert all channels the user has changed its nick
        */
        void    cmdNick(const std::string& newnick);

        /**
         The User is logging in, setting the realname and host.
         If the User is well logged, the server will send the Motd
        */
        void    cmdUser(const std::string& host, const std::string& realname);

        /**
         This command make the User to close the socket connection.
         It also remove the User from Mainfram and all its channels
        */
        void    cmdQuit();

        /**
         Simply add the User into the Channel "channel" and
         send a confirmation message to the Client.
         Note : The channel creation/search is done in the Parser
         @see Parser::parse()
        */
        void    cmdJoin(Channel* channel);

        /**
         Simply remove the User from the Channel "channel" and
         send a confirmation message to the Client.
         Note : The channel search is done in the Parser
         @see Parser::parse()
        */
        void    cmdPart(Channel* channel);

        /**
         Simply alerts the Channel we kicked someone.
         All verifications are done in the Parser
         @see Parser::parse()
        */
        void    cmdKick(User* victim, const std::string& reason, Channel* channel);

        /**
         Send Pong !
        */
        void    cmdPing();



        /************************************************************
                                ACCESSORS
        ************************************************************/

        Session*        session() const; ///< Returns a pointer on the User's Session

        std::string     name() const;   ///< Returns the Realname
        std::string     nick() const;   ///< Returns the Nickname
        std::string     host() const;   ///< Returns the User host
        std::string     messageHeader() const;  ///< Returns a RFC1459 Header of a message that come from this client (:nick!nick@host )


        void            setName(const std::string& name);   ///< Sets the User Realname
        void            setNick(const std::string& nick);   ///< Sets the User Nickname
        void            setHost(const std::string& host);   ///< Sets the User Host

    private:
        Session*            mSession;   ///< Pointer on owner Session

        std::string         mRealName;  ///< Realname
        std::string         mNickName;  ///< Nickname
        std::string         mHost;      ///< Host (here, client IP)

        bool                bSentUser;  ///< True when the User has sent USER command
        bool                bSentNick;  ///< True when the User has sent NICK command the first time
        bool                bSentMotd;  ///< True when the User has sent USER and NICK and Motd has been sent
        bool                bProperlyQuit;  ///< True only if the User used QUIT command before disconnecting

        ChannelSet          mChannels;  ///< Set of all Channels the User is connected to.

};

#endif // USER_H_INCLUDED
