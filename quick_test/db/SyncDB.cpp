#include <db/SyncDB.h>

#include <leveldb/slice.h>
#include <sync/Context.h>
#include <sync/Log.h>
#include <leveldb/db.h>
#include <leveldb/options.h>
#include <leveldb/status.h>
#include <string>
#include <memory>

namespace sync
{

SyncDB::SyncDB(const std::string & data_space_, ContextPtr context_) 
    : data_space(data_space_), context(context_)
{
    db_option.create_if_missing = context->db_create_namespace_if_missing;
    std::string db_path = context->db_path + data_space;
    leveldb::Status status = leveldb::DB::Open(db_option, db_path, &db);

    if (!status.ok())
    {
        LOG_ERROR(context->getLogger(), "SyncDB()", status.ToString());
    }
}

SyncDB::~SyncDB()
{
    delete(db);
}

void SyncDB::Put(const leveldb::Slice & key, const leveldb::Slice & value)
{
    
}

std::string SyncDB::Get(const leveldb::Slice & key)
{

}

void SyncDB::Delete(const leveldb::Slice & key)
{

}



}


