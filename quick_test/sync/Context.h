#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include <sync/Log.h>
#include <db/Metadata.h>

namespace sync
{

class Context;
using ContextPtr = std::shared_ptr<Context>;
class SyncStateMachine;
using SyncStateMachinePtr = std::shared_ptr<SyncStateMachine>;
class SyncStateManager;
using SyncStateManagerPtr = std::shared_ptr<SyncStateManager>;


class Context
{
private:
    SyncStateMachinePtr state_machine;
    SyncStateManagerPtr state_manager;
    // runtime option
    std::shared_ptr<logger_wrapper> log; 

    std::shared_ptr<Metadata> metadata;
    
public:
    Context(/* args */);
    ~Context();

    std::shared_ptr<Metadata> getMetaData() ;

    std::shared_ptr<logger_wrapper> getLogger(){return log;}

    // DB option on open
    std::string db_path; 
    bool db_create_namespace_if_missing ;
    
    
};


}
