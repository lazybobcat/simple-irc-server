#include "user.h"
#include "session.h"
#include "mainframe.h"

User::User(Session*     mysession)
:   mSession(mysession),
    bSentUser(false),
    bSentNick(false),
    bSentMotd(false),
    bProperlyQuit(false)
{

}

User::~User()
{
    // Make sure we remove ourself from the list of Users and from Channels
    // If the QUIT command has already been sent, this will do nothing
    if(!bProperlyQuit)
    {
        ChannelSet::iterator it = mChannels.begin();
        for(; it != mChannels.end(); ++it) // Remove User from all its channels and alert the each channel
        {
            (*it)->removeUser(this);
            (*it)->broadcast(messageHeader() + "PART " + (*it)->name() + " :Quitte le canal" + Config::EOFMessage);
            mChannels.erase((*it));
        }
        Mainframe::instance()->removeUser(mNickName); // Removes the User from the Mainframe (its nick will be avalaible)
        Mainframe::instance()->updateChannels(); // Check all channels to look for empty ones. Empty channels are deleted
    }
}


/************************************************************
                        COUCHE METIER
************************************************************/

void User::cmdNick(const std::string& newnick)
{
    if(bSentNick) // if it is NOT the first time we choose a nickname
    {
        if(Mainframe::instance()->changeNickname(mNickName, newnick)) // True if the new nickname is available
        {
            mSession->sendAsUser("NICK :"+ newnick + Config::EOFMessage);
            // Tell this to all our channels
            ChannelSet::iterator it = mChannels.begin();
            for(; it != mChannels.end(); ++it)
            {
                (*it)->broadcast(messageHeader() + "NICK " + newnick + Config::EOFMessage);
                (*it)->sendWhoList();
            }
            setNick(newnick);
        }
        else
        {
            // Else, the nick is already used
            mSession->sendAsServer(ToString(Response::Error::ERR_NICKCOLLISION) + " " + mNickName + " " + newnick + " :Nickname is already in use." + Config::EOFMessage);
        }

    }
    else // if it is the first time
    {
        if(!Mainframe::instance()->doesNicknameExists(newnick)) // True if the new nickname is available
        {
            setNick(newnick);
            bSentNick = true;
        }
        else
        {
            // Else, the nick is already used
            mSession->sendAsServer(ToString(Response::Error::ERR_NICKCOLLISION) + " " + mNickName + " " + newnick + " :Nickname is already in use." + Config::EOFMessage);
        }
    }
}

void User::cmdUser(const std::string& host, const std::string& realname)
{
    if(bSentUser) // Error if the user is already registered
    {
        mSession->sendAsServer(ToString(Response::Error::ERR_ALREADYREGISTRED) + " " + mNickName + " You are already registered !" + Config::EOFMessage);
    }
    else
    {
        // the NICK command should be used before USER
        if(!bSentNick)
        {
            mSession->sendAsServer(ToString(Response::Error::ERR_NONICKNAMEGIVEN) + " No nickname given, use NICK first !" + Config::EOFMessage);
            return;
        }

        // True if the user has been added, false if the nick is used
        // This check dobles the check done with NICK command, but sometimes the User wait a long time between these two steps,
        // so a second check is required.
        if(!Mainframe::instance()->addUser(this))
        {
            mSession->sendAsServer(ToString(Response::Error::ERR_NICKCOLLISION) + " " + mNickName + " This nickname is already used !" + Config::EOFMessage);
            return;
        }

        // For a real IRC, we need a real Host Extraction here
        //setHost(host);
        // We just gonna take the IP for the host to simplify
        mHost = mSession->ip();
        mRealName = realname;

        // Ther user sent command USER
        bSentUser = true;
    }

    if(bSentNick && bSentUser && !bSentMotd) // Send the MOTD
    {
        bSentMotd = true;

        mSession->sendAsServer("001 " + mNickName + " :Welcome to " + Config::ServerName + " IRC network, " + mNickName + Config::EOFMessage);
        mSession->sendAsServer("002 " + mNickName + " :Your host is " + mHost + Config::EOFMessage);
        mSession->sendAsServer("003 " + mNickName + " :This server is " + Config::ServerName + Config::EOFMessage);
    }
}

void User::cmdQuit()
{
    ChannelSet::iterator it = mChannels.begin();
    for(; it != mChannels.end(); ++it)
    {
        (*it)->broadcast(messageHeader() + "PART " + (*it)->name() + " :Quitte le canal" + Config::EOFMessage);
        (*it)->removeUser(this);
        mChannels.erase((*it));
    }
    Mainframe::instance()->removeUser(mNickName);
    Mainframe::instance()->updateChannels();
    mSession->close();
    bProperlyQuit = true;
}

void User::cmdPart(Channel* channel)
{
    channel->broadcast(messageHeader() + "PART " + channel->name() + " :Quitte le canal" + Config::EOFMessage);
    channel->removeUser(this);
    mChannels.erase(channel);
    Mainframe::instance()->updateChannels();
}

void User::cmdJoin(Channel* channel)
{
    mSession->sendAsUser("JOIN " + channel->name() + Config::EOFMessage);
    mChannels.insert(channel);
    channel->addUser(this);
}

void User::cmdKick(User* victim, const std::string& reason, Channel* channel)
{
    channel->broadcast(":" + mNickName + " KICK " + channel->name() + " " + victim->nick() + " :" + reason + Config::EOFMessage);
}

void User::cmdPing()
{
    mSession->sendAsServer("PONG" + Config::EOFMessage);
}



/************************************************************
                        ACCESSORS
************************************************************/

Session*    User::session() const
{
    return mSession;
}

std::string User::name() const
{
    return mRealName;
}

std::string User::nick() const
{
    return mNickName;
}

std::string User::host() const
{
    return mHost;
}

void User::setName(const std::string& name)
{
    mRealName = name;
}
void User::setNick(const std::string& nick)
{
    mNickName = nick;
}
void User::setHost(const std::string& host)
{
    mHost = host;
}

std::string User::messageHeader() const
{
    return std::string(":"+mNickName+"!"+mNickName+"@"+mHost+" ");
}
