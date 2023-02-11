#include <sync/Context.h>
#include <leveldb/slice.h>
#include <string>

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
    void createDataSpace();

    // delete cur db
    // if exit
    // status created -> (on_delete) -> not_exist
    void deleteDataSpace();

    ~SyncDB();
};


} // namespace sync











