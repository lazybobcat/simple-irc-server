#include "session.h"
#include "parser.h"

Session::Session(boost::asio::io_service& io_service)
:   mUser(this),
    mSocket(io_service)
{

}


Session::pointer Session::create(boost::asio::io_service& io_service)
{
    return Session::pointer(new Session(io_service));
}



void Session::start()
{
    // Hi Session ! This is your initialisation here...
    // You will start to read asynchroneously and as it
    // is your first time around here, a automatic message
    // will be sent to you ! Have fun !

    // Do an async read
    read();
}


void Session::close()
{
    // Close the socket and remove all async actions related to our socket
    mSocket.cancel();
    mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
}





void Session::read()
{
    // Async read until the EndCaracter defined in Config (by default: \r\n)
    // We call handleRead() when thee is something to read
    boost::asio::async_read_until(mSocket, mBuffer, Config::EOFMessage,
                                  boost::bind(&Session::handleRead, shared_from_this(),
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred)
    );
}

void Session::handleRead(const boost::system::error_code& error, std::size_t bytes)
{
    if(error)
    {
            if(error == boost::asio::error::eof) { // This isn't a real error, the client just leaved (disconnected)
                std::cout << "[Serveur] " << ip() << " nous a quitté :(" << std::endl;
                mSocket.close();
            }
            else // Real error, real management : write the error on cerr :p
                std::cerr << "[Serveur] Erreur (l" << __LINE__ <<") : " << error.message() << " par " << ip() << std::endl;
    }
    else if(bytes == 0) // Shouldn't happen, but we are't enough careful with a Cient
    {
        std::cerr << "[Serveur] Erreur : message vide envoyé par " << ip() << std::endl;
    }
    else
    {
        // We put the message from the buffer to a string
        std::string message;
        std::istream istream(&mBuffer);
        std::getline(istream, message);
        // We remove the termaison caracters from the string
        message = message.substr(0, message.size()-1);

        #ifdef TESTMODE
        std::cout << "[" << ip() << "] : " << message << std::endl;
        #endif

        // Call the Parser to parse the message
        Parser::parse(message, &mUser);

        // We async_read again
        read();
    }
}

void Session::send(const std::string& message)
{
    // Make sure the buffer source of the message lives long enough by putting message into mMessage.
    mMessage = message;
    boost::asio::write(mSocket, boost::asio::buffer(mMessage));
}

void Session::sendAsUser(const std::string& message)
{
    send(mUser.messageHeader() + message);
}

void Session::sendAsServer(const std::string& message)
{
    send(Config::FormatServerName + message);
}








tcp::socket& Session::socket()
{
    return mSocket;
}

std::string Session::ip() const
{
    return mSocket.remote_endpoint().address().to_string();
}

