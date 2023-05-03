#include <iostream>
#include <programs/SyncServer.h>


int main()
{
    sync::SyncServer server;
    server.run();
    
    return 0;
}