#pragma once

#include <sync/Context.h>
#include <sync/SyncStateMachine.h>

namespace sync
{

// grpc server and syncdb server
class SyncServer
{
private:
    
public:
    SyncServer();
    ~SyncServer();
    run();
};


}
