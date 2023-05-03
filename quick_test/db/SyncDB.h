#include <sync/Context.h>
#include <sync/log/logger.h>
#include <leveldb/slice.h>
#include <leveldb/options.h>
#include <leveldb/db.h>
#include <string>
#include <memory>

namespace sync
{
class SyncDB
{
private:
    ContextPtr context ;
    std::string data_space; 
public:
    SyncDB(const std::string & data_space, ContextPtr Context);
    
    void Put(const std::string & key, const std::string & value);

    std::string Get(const std::string & key);

    void Delete(const std::string & key);

    // check db status 
    // if not exist
    // status not_exit -> (on_create) -> created 
    std::string getDataSpace(){return data_space;}

    ~SyncDB();

private:
    std::string data_space;
    ContextPtr context; 
    leveldb::DB* db;
    leveldb::Options db_option;
    leveldb::WriteOptions db_write_option;
    leveldb::ReadOptions db_read_option;
};

using SyncDBPtr = std::shared_ptr<SyncDB>;

} // namespace sync











