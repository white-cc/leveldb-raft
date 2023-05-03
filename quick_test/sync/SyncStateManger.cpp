#include <memory>
#include <string>
#include <sync/SyncLogStore.h>
#include <sync/SyncStateManager.h>

using namespace nuraft;

namespace sync
{

SyncStateManager::SyncStateManager(int server_id_, const std::string & endpoint_) : server_id(server_id_), endpoint(endpoint_)
{
    cur_log_store = std::make_shared<SyncLogStore>();
    server_config = std::make_shared<srv_config>(server_id, endpoint);
    saved_config = std::make_shared<cluster_config>();
    saved_config->get_servers().push_back(server_config);
}

SyncStateManager::~SyncStateManager()
{
}

std::shared_ptr<cluster_config> SyncStateManager::load_config()
{   
    //todo 这里准备用写入文件替代
    return saved_config;
}                  

void SyncStateManager::save_config(const cluster_config & conf)
{   
    // todo 这里用持久化替代内存保存
    std::shared_ptr<buffer> buf = conf.serialize();
    saved_config = cluster_config::deserialize(*buf);
}

void SyncStateManager::save_state(const srv_state & state)
{
    // todo写入磁盘保证持久化
    std::shared_ptr<buffer> buf = state.serialize();
    saved_state = srv_state::deserialize(*buf); 
}

std::shared_ptr<srv_state> SyncStateManager::read_state()
{
    return saved_state;
}

std::shared_ptr<log_store> SyncStateManager::load_log_store()
{
    return cur_log_store;
}


void SyncStateManager::system_exit(const int exit_code)
{
    //todo 文件持久化
}

std::shared_ptr<srv_config> SyncStateManager::get_srv_config() const
{
    return server_config;
}


}
