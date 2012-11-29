#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "defines.h"
#include "session.h"

using boost::asio::ip::tcp;

class Server
{
    public:
        /**
         Construct the Server and initialize the tcp acceptor
        */
        Server(boost::asio::io_service& io_service);


        /**
         Starts the server, will call startAccept(). Should be called once in the mainframe
        */
        void    start();



    private:
        /**
         The Server creates a new Session and do an async_accept on it. This async_accept will call handleAccept() when a someone ask for a new connection.
        */
        void    startAccept();

        /**
         Start the new connection with the Session and redo the async_accept (by calling startAccept() )
        */
        void    handleAccept(Session::pointer newclient, const boost::system::error_code& error);



        tcp::acceptor       mAcceptor; ///< Acceptor

};

#endif // SERVER_H_INCLUDED
