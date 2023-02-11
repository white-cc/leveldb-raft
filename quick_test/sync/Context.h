#pragma once 

#include <sync/SyncStateManager.h>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace sync
{

class Context;
using ContextPtr = std::shared_ptr<Context>;



class Context
{
private:
    mutable std::mutex change_lock;
    std::unordered_map<std::string , DataSpaceStatus> data_space_status; 
    SyncStateManagerPtr state_manager; 

public:
    Context(/* args */);
    ~Context();
};



}




