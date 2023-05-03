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
    
    void Put(const leveldb::Slice & key, const leveldb::Slice & value);

    std::string Get(const leveldb::Slice & key);

    void Delete(const leveldb::Slice & key);

    // check db status 
    // if not exist
    // status not_exit -> (on_create) -> created 
    std::string getDataSpace(){return data_space;}

    // delete cur db
    // if exit
    // status created -> (on_delete) -> not_exist
    void deleteDataSpace();

    ~SyncDB();

private:
    std::string data_space;
    ContextPtr context; 
    leveldb::DB* db;
    leveldb::Options db_option;
};


} // namespace sync











