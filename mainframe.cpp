#include "mainframe.h"


Mainframe *Mainframe::mInstance = NULL;


Mainframe* Mainframe::instance()
{
    // If Mainframe has never been constructed, let's do it ONCE
    if(!mInstance)
        mInstance = new Mainframe();

    return mInstance;
}

Mainframe::Mainframe()
{

}

Mainframe::~Mainframe()
{
    // Properly remove all channels
    removeAllChannels();
}

void Mainframe::start()
{
    // Déclaration du service principal
    boost::asio::io_service ios;

    Server server(ios);
    server.start();

    ios.run();
}



/////////////////////////////////////////////////////////////////////////
//                           USERS                                     //
/////////////////////////////////////////////////////////////////////////

bool Mainframe::doesNicknameExists(const std::string& nick)
{
    return ((mUsers.find(nick)) != mUsers.end());
}

bool Mainframe::addUser(User* user)
{
    if(doesNicknameExists(user->nick()))    return false;

    mUsers[user->nick()] = user;
    return true;
}

bool Mainframe::changeNickname(const std::string& old, const std::string& recent)
{
    if(doesNicknameExists(recent))    return false;
    if(!doesNicknameExists(old))    return false;

    User* tmp = mUsers[old];
    mUsers.erase(old);
    mUsers[recent] = tmp;
    return true;
}

void Mainframe::removeUser(const std::string& nick)
{
    mUsers.erase(nick);
}

User* Mainframe::getUserByName(const std::string& nick)
{
    if(! doesNicknameExists(nick) ) return NULL;

    return mUsers[nick];
}


/////////////////////////////////////////////////////////////////////////
//                           CHANNELS                                  //
/////////////////////////////////////////////////////////////////////////


bool    Mainframe::doesChannelExists(const std::string& name)
{
    ChannelMap::iterator it = mChannels.begin();
    return ((mChannels.find(name)) != mChannels.end());
}

void    Mainframe::addChannel(Channel* chan)
{
    std::string channame = chan->name();
    if(!doesChannelExists(channame))
    {
        mChannels[channame] = chan;
    }
}

void    Mainframe::removeChannel(const std::string& name)
{
    mChannels.erase(name);
}

Channel* Mainframe::getChannelByName(const std::string& name)
{
    if(!doesChannelExists(name))    return NULL;

    return mChannels[name];
}

void Mainframe::removeAllChannels()
{
    ChannelMap::iterator it = mChannels.begin();
    for(; it != mChannels.end(); ++it)
    {
        delete (it->second);
    }
}

void Mainframe::updateChannels()
{
    ChannelMap::iterator it = mChannels.begin();
    while(it != mChannels.end())
    {
        if(it->second->empty())
        {
            delete (it->second);
            mChannels.erase(it);
	    it++;
        }
        else
        {
            it++;
        }

    }
}

ChannelMap Mainframe::channels() const
{
    return mChannels;
}




















