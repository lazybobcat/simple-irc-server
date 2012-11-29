#include "server.h"


Server::Server(boost::asio::io_service& io_service)
:   mAcceptor(io_service, tcp::endpoint(tcp::v4(), Config::Port))
{
    mAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    mAcceptor.listen();
}


void Server::start()
{
    startAccept();
}




void Server::startAccept()
{
    // First, we create a new Session
    Session::pointer newclient = Session::create(mAcceptor.get_io_service());

    // Then, we do an async accept
    mAcceptor.async_accept(newclient->socket(),
                           boost::bind(&Server::handleAccept,   this,   newclient,  boost::asio::placeholders::error));
}


void Server::handleAccept(Session::pointer newclient, const boost::system::error_code& error)
{
    if(error)
    {
        std::cerr << "[Serveur] Erreur : " << error.message() << std::endl;
    }
    else
    {
        #ifdef TESTMODE
        std::cout << "[Serveur] Nouvelle connexion : " << newclient->ip() << std::endl;
        #endif

        newclient->start();
    }

    startAccept();
}
