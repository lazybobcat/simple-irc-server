#include "parser.h"
#include "server.h"
#include "mainframe.h"

Parser::Parser()
{

}


void Parser::parse(const std::string& message, User* user)
{
    // Here we want to parse the message recieved. It can contain a command and
    // any number of parameters. We wll first split our message by space caracter
    // into a vector of strings. Then it will be easier to parse the message

    // Split Vector<string>
    StrVec  split;
    // Boost do the split job for us, that's a good thing :p
    boost::split(split, message, boost::is_any_of(" \t"), boost::token_compress_on );

    boost::to_upper(split[0]);

    // Now we can parse the message
    if(split[0] == "NICK")
    {
        // Error if the number ofparameters is wrong
        if(split.size() < 2) {
            user->session()->sendAsServer(ToString(Response::Error::ERR_NONICKNAMEGIVEN) + " " + user->nick() + " No nickname given !" + Config::EOFMessage);
        }

        // Check for forbidden first caracters. Avoid a lot of bugs/exploits
        if(split[1][0] == '#' || split[1][0] == '&' || split[1][0] == '!' || split[1][0] == '@' || split[1][0] == ':')
        {
            user->session()->sendAsServer(ToString(Response::Error::ERR_ERRONEUSNICKNAME) + " " + user->nick() + " Wrong nickname format !" + Config::EOFMessage);
            return;
        }

        user->cmdNick(split[1]);
    }
    else if(split[0] == "USER")
    {
        // Error if the number of parameters is wrong
        if(split.size() < 5)
        {
            user->session()->sendAsServer(ToString(Response::Error::ERR_NEEDMOREPARAMS) + Config::EOFMessage);
            return;
        }

        // Extraction of the Realname (all parmeters from 5th to end)
        std::string realname = split[4];
        for(unsigned int i = 5; i < split.size(); ++i)
        {
            realname += " ";
            realname += split[i];
        }
        realname = realname.substr(1);

        user->cmdUser(split[2], realname);
    }
    else if(split[0] == "QUIT")
    {
        user->cmdQuit();
    }
    else if(split[0] == "JOIN")
    {
        // Error if the number of parameters is wrong
        if(split.size() < 2) return;

        // Non-correct chan name ?
        if((split[1][0] != '#' && split[1][0] != '&') || split[1].size() < 2) return;


        // Channel searching...
        Channel* chan = Mainframe::instance()->getChannelByName(split[1]);
        // Does channel exists and does the user isn't already on it ?
        if(chan)
        {
            //Does channel hasn't passkey OR user used the good passkey ?
            if (!chan->hasPass() || (split.size() >= 3 && split[2] == chan->password()))
            {
                if (!chan->limited() || !chan->full()) // If the chan hasn't limited slots or isn't full
                {
                    user->cmdJoin(chan);
                }
                else
                    user->session()->sendAsServer(ToString(Response::Error::ERR_CHANNELISFULL) + split[1] + " :Cannot join channel (+l)" + Config::EOFMessage);
            }
            else
                user->session()->sendAsServer(ToString(Response::Error::ERR_BADCHANNELKEY) + split[1] + " :Cannot join channel (+k)" + Config::EOFMessage);
        }
        else // The chan does not exists, we create it
        {
            chan = new Channel(user, split[1]);
            if(chan)
            {
                user->cmdJoin(chan);
                Mainframe::instance()->addChannel(chan);
            }
        }
    }
    else if(split[0] == "PART")
    {
        // Error if the number of parameters is wrong
        if(split.size() < 2) return;

        // Searching channel...
        Channel* chan = Mainframe::instance()->getChannelByName(split[1]);
        // Does channel exists ?
        if(chan)
        {
            user->cmdPart(chan);
        }
    }
    else if(split[0] == "TOPIC")
    {
        // Error if the number of parameters is wrong
        if(split.size() >= 2)
        {
            // Searching channel...
            Channel* chan = Mainframe::instance()->getChannelByName(split[1]);
            // Does channel exists ?
            if(chan)
            {
                // If the split size == 2, the Client asked to know what is the Channel topic (and not to set it)
                if(split.size() == 2)
                {
                    // If the channel topic isn't set to something, we send the NOTOPIC code
                    if(chan->topic().empty())
                        user->session()->sendAsServer(ToString(Response::Reply::RPL_NOTOPIC) + " " + split[1] + " :No topic is set !" + Config::EOFMessage);
                    else // Else the channel topic is set, we send it
                        user->session()->sendAsServer(ToString(Response::Reply::RPL_TOPIC) + " " + user->nick() + " " + split[1] + " :" + chan->topic() + Config::EOFMessage);
                }
                // If the split size is minimum 3, the Client want to set the channel topic
                else if(split.size() >= 3)
                {
                    // Get the channel topic from splitted vector
                    std::string topic;
                    for(unsigned int i = 2; i < split.size(); ++i)
                        topic += split[i] + " ";

                    // Removing the " : " caracter in first position
                    topic = topic.substr(1);

                    // If the user is Operator on the channel, he can modify the topic
                    if(chan->isOperator(user))
                    {
                        chan->cmdTopic(topic);
                        // Alert all channel users that the topic has been modified
                        chan->broadcast(user->messageHeader() + "TOPIC " + chan->name() + " :" + chan->topic() + Config::EOFMessage);
                    }
                }
            }
        }
    }
    else if(split[0] == "LIST")
    {
        // Returns a list of Channels created on the server, with the number of clients in them and their topic

        // First, we send the reply_code of beggining of list
        user->session()->sendAsServer(ToString(Response::Reply::RPL_LISTSTART) + " " + user->nick() + " Channel :Users Name" + Config::EOFMessage);

        // We get the channel map
        ChannelMap channels = Mainframe::instance()->channels();
        ChannelMap::iterator it = channels.begin();
        // Iteration on the channel map
        for(; it != channels.end(); ++it)
        {
            // For each channel we send a message containing the channel name, the number of users and the channel topic
            user->session()->sendAsServer(ToString(Response::Reply::RPL_LIST) +  " " + user->nick() + " " + it->first + " " + ToString(it->second->userCount()) + " :" + it->second->topic() + Config::EOFMessage);
        }
        // Finally, we send the reply_code of end of list
        user->session()->sendAsServer(ToString(Response::Reply::RPL_LISTEND) + " " + user->nick() + " :End of /LIST" + Config::EOFMessage);
    }
    else if(split[0] == "PRIVMSG")
    {
        // Error if the number of parameters is wrong
        if(split.size() < 3) return;

        // Get the message
        std::string message = "";
        for(unsigned int i = 2; i < split.size(); ++i)
        {
            message += split[i] + " ";
        }

        // If the message is for a channel
        if(split[1][0] == '#' || split[1][0] == '&')
        {
            Channel* chan = Mainframe::instance()->getChannelByName(split[1]);
            if(chan)
            {
                chan->broadcast(":" + user->nick() + " PRIVMSG " + chan->name() + " " + message + Config::EOFMessage);
            }
        }
        else // Private message for another user
        {
            User* target = Mainframe::instance()->getUserByName(split[1]);
            if(target)
            {
                target->session()->send(":" + user->nick() + " PRIVMSG " + target->nick() + " " + message + Config::EOFMessage);
            }
        }
    }
    else if(split[0] == "KICK")
    {
        // Error if the number of parameters is wrong
        if(split.size() < 3) return;

        Channel* chan = Mainframe::instance()->getChannelByName(split[1]); // Get channel
        User*  victim = Mainframe::instance()->getUserByName(split[2]); // Get User victim
        std::string reason = "";
        // Does channel and victim exists ?
        if(chan && victim)
        {
            for(unsigned int i = 3; i < split.size(); ++i)
            {
                reason += split[i] + " ";
            }
            // Check :
            // a) The user is Operator on the channel
            // b) The victim is on the channel
            // c) The victim isn't an operator
            if(chan->isOperator(user) && chan->hasUser(victim) && !chan->isOperator(victim))
            {
                user->cmdKick(victim, reason, chan);
                victim->cmdPart(chan);
            }
        }
    }
    else if(split[0] == "PING")
    {
        user->cmdPing();
    }
    else if (split[0] == "MODE")
    {
        ///< @todo : Guillaume, peux-tu commenter ton implémentation de MODE ?
        if(split.size() < 3) return;

        unsigned char nbParam = 0;
        bool bMode = true;

        Channel* chan = Mainframe::instance()->getChannelByName(split[1]);
        if (!chan)
        {
            user->session()->sendAsServer(ToString(Response::Error::ERR_NOSUCHCHANNEL) + Config::EOFMessage);
            return;
        }

        if (!(chan->hasUser(user)))
        {
            user->session()->sendAsServer(ToString(Response::Error::ERR_NOTONCHANNEL) + Config::EOFMessage);
            return;
        }

        if (!(chan->isOperator(user)))
        {
            user->session()->sendAsServer(ToString(Response::Error::ERR_NOTONCHANNEL) + Config::EOFMessage);
            return;
        }


        for(unsigned int i = 0; i<split[2].size(); ++i)
        {
            if (split[2][i] == '+')
                bMode = true;
            else if (split[2][i] == '-')
                bMode = false;
            else if ((split[2][i] == 'o' && !bMode) || bMode)
                ++nbParam;
            else if ((split[2][i]) != 'k' && split[2][i] != 'l')
            {
                user->session()->sendAsServer(ToString(Response::Error::ERR_UNKNOWNMODE) + Config::EOFMessage);
                return;
            }
        }

        if (nbParam > split.size()-3)
        {
            user->session()->sendAsServer(ToString(Response::Error::ERR_NEEDMOREPARAMS) + Config::EOFMessage);
            return;
        }

        nbParam = 3;

        for(unsigned int i = 0; i<split[2].size(); ++i)
        {
            if (split[2][i] == '+')
                bMode = true;
            else if (split[2][i] == '-')
                bMode = false;
            else if (bMode)
            {
                if (split[2][i] == 'o')
                {
                    User *victim = Mainframe::instance()->getUserByName(split[nbParam]);
                    chan->cmdOPlus(user, victim);
                }
                else if (split[2][i] == 'k')
                {
                    chan->cmdKPlus(split[nbParam]);
                    ++nbParam;
                }
                else if (split[2][i] == 'l')
                {
                    chan->cmdLPlus(split[nbParam]);
                    ++nbParam;
                }
                else
                {
                    user->session()->sendAsServer(ToString(Response::Error::ERR_UNKNOWNMODE) + Config::EOFMessage);
                    return;
                }
            }
            else if (!bMode)
            {
                if (split[2][i] == 'o')
                {
                    User *victim = Mainframe::instance()->getUserByName(split[nbParam]);
                    if (victim->name() == user->name())
                        chan->cmdOMinus(user);
                    ++nbParam;
                }
                else if (split[2][i] == 'k')
                    chan->cmdKMinus();
                else if (split[2][i] == 'l')
                    chan->cmdLMinus();
                else
                {
                    user->session()->sendAsServer(ToString(Response::Error::ERR_UNKNOWNMODE) + Config::EOFMessage);
                    return;
                }
            }
        }

    }
    else if(split[0] == "LAPIN") // Easter-Egg special Pr Delobel
    {
        for(unsigned int i = 0; i < 10; ++i)
            user->session()->send(":DarkRabbit PRIVMSG " + user->nick() + " Les lapins domineront le monde !" + Config::EOFMessage);
    }
    else
        user->session()->sendAsServer(ToString(Response::Error::ERR_UNKNOWNCOMMAND) + Config::EOFMessage);
}
