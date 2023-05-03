#pragma once

#include <memory>
#include <unordered_map>
#include <db/SyncDB.h>
#include <sync/Context.h>

namespace sync
{

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Metadata
{
private:
    static SyncDBPtr getNameSpace(const std::string & name);
    static SyncDBPtr createNameSpace(const std::string & name, ContextPtr context);

    static std::mutex metadata_lock;
    static std::unordered_map<std::string, SyncDBPtr> name_space_lists;
    static std::unordered_map<std::string, std::atomic<bool>> name_space_used_lists;

public:
    static SyncDBPtr getOrCreateNameSpace(const std::string & name, ContextPtr context);
    static void tryGetNameSpaceLock(const std::string& name);
    static void releaseNameSpaceLock(const std::string& name);

};
}
