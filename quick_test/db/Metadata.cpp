#include <memory>
#include <db/Metadata.h>
#include <db/SyncDB.h>
#include <common/DBException.h>

namespace sync
{

SyncDBPtr Metadata::getNameSpace(const std::string & name)
{
    if (name_space_lists.find(name) != name_space_lists.end())
    {
        return nullptr;
    }
    return name_space_lists[name];
}

SyncDBPtr Metadata::createNameSpace(const std::string & name, ContextPtr context)
{
    SyncDBPtr sync_db = std::make_shared<SyncDB>(name, context);
    name_space_lists.insert({name, sync_db});
    name_space_used_lists[name].store(false);
    return sync_db;
}


SyncDBPtr Metadata::getOrCreateNameSpace(const std::string & name, ContextPtr context)
{
    std::lock_guard<std::mutex> ll(metadata_lock);
    SyncDBPtr sync_db;
    sync_db = getNameSpace(name);
    if (sync_db == nullptr)
    {
        return createNameSpace(name, context);
    }
    return sync_db;
}

void Metadata::tryGetNameSpaceLock(const std::string& name)
{
    if(name_space_used_lists.find(name)==name_space_used_lists.end())
    {
        throw DBException("get namespace empty namespace: "+ name );
    }
    bool expect = false;
    while (!name_space_used_lists[name].compare_exchange_weak(expect, true))
    {
        expect = false;
    }
}

void Metadata::releaseNameSpaceLock(const std::string& name)
{   
    if(name_space_used_lists.find(name)==name_space_used_lists.end())
    {
        throw DBException("get namespace empty namespace: "+ name );
    }
    name_space_used_lists[name].store(false);
}


}
