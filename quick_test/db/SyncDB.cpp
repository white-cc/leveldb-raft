#include <db/SyncDB.h>

#include <sync/Context.h>
#include <sync/Log.h>
#include <common/DBException.h>
#include <leveldb/db.h>
#include <leveldb/slice.h>
#include <leveldb/options.h>
#include <leveldb/status.h>
#include <string>
#include <memory>
#include <exception>

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
        throw DBException("Error on open DB: "+status.ToString());
    }
}

SyncDB::~SyncDB()
{
    delete(db);
}

void SyncDB::Put(const std::string & key, const std::string & value)
{   
    leveldb::Slice skey(key);
    leveldb::Slice svalue(value);
    db->Put(db_write_option, skey, svalue);
}

std::string SyncDB::Get(const std::string & key)
{   
    leveldb::Slice skey(key);
    std::string value; 
    db->Get(db_read_option, skey, &value);
    return value;
}

void SyncDB::Delete(const std::string & key)
{   
    leveldb::Slice skey(key);
    db->Delete(db_write_option, key);
}

}


