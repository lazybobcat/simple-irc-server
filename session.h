#ifndef SESSION_H_INCLUDED
#define SESSION_H_INCLUDED

#ifndef Session_H_INCLUDED
#define Session_H_INCLUDED

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include <string>

#include <iostream>

#include "defines.h"
#include "user.h"

using boost::asio::ip::tcp;

/**
 A Session handle the communication with the Client via its socket.
 So it can read incoming data or send a message
*/
class Session : public boost::enable_shared_from_this<Session>
{
    public:
        /**
         A typedef to make our life simplest
        */
        typedef boost::shared_ptr<Session> pointer;



        /**
         Creates a Session and returns a shared_pointer on it
        */
        static pointer  create(boost::asio::io_service& io_service);



        /**
         Starts the Session, starts an async_read by calling read()
        */
        void start();


        /**
         Send a message to the client (async_write) and call handleWrite. Put the server header in the message.
        */
        void sendAsServer(const std::string& message);

        /**
         Send a message to the client (async_write) and call handleWrite. Put the USer header in the message.
        */
        void sendAsUser(const std::string& message);

        /**
         Send a message to the client (async_write) and call handleWrite. No header will be added to the message.
        */
        void send(const std::string& message);


        /**
         Stops the Session, close the socket
        */
        void close();


        /**
         Returns the Session Socket
        */
        tcp::socket&    socket();

        /**
         Returns the Session IP in a std::string
        */
        std::string     ip() const;



    private:
        /**
         Session constructor, initialize the Session socket
        */
        Session(boost::asio::io_service& io_service);



        /**
         Starts an async_read_until that will read a message and call handleRead
        */
        void read();

        /**
         Handle the data that has been read
        */
        void handleRead(const boost::system::error_code& error, std::size_t bytes);



        User                        mUser;      ///< User that is linked to this connection
        tcp::socket                 mSocket;    ///< TCP Socket
        boost::asio::streambuf      mBuffer;    ///< Buffer used to read incoming data
        std::string                 mMessage;   ///< A string buffer used to write (send) data to client
};

#endif // Session_H_INCLUDED


#endif // SESSION_H_INCLUDED
