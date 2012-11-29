#include <iostream>

#include "mainframe.h"

// IRC by Boutter Loïc and Masson Guillaume
// Configuration of Port, Server name, others... can be set in config.cpp
// By default, Port = 4321

int main()
{
    try
    {
        // Start the IRC Server
        Mainframe* frame = Mainframe::instance();
        frame->start();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }



    return 0;
}
