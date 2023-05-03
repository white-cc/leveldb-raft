#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include <sync/Log.h>
#include <sync/SyncStateManager.h>

namespace sync
{

class Context;
using ContextPtr = std::shared_ptr<Context>;


class Context
{
private:
    mutable std::mutex change_lock;
    SyncStateManagerPtr state_manager;

    // runtime option
    std::shared_ptr<logger_wrapper> log; 
    

public:
    Context(/* args */);
    ~Context();

    std::shared_ptr<logger_wrapper> getLogger(){return log};

    // DB option on open
    std::string db_path; 
    bool db_create_namespace_if_missing ;




};


}
