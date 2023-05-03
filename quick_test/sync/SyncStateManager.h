#pragma once

#include <memory>
#include <sync/SyncLogStore.h>
#include <libnuraft/nuraft.hxx>


using namespace nuraft;
namespace sync
{

class SyncStateManager;
using SyncStateManagerPtr = std::shared_ptr<SyncStateManager>;

class SyncStateManager : public state_mgr
{
private:
    /* data */
public:
    SyncStateManager(int server_id , const std::string& endpoint);

    ~SyncStateManager();

    std::shared_ptr<cluster_config> load_config() override;

    void save_config(const cluster_config& conf) override;

    void save_state(const srv_state& state) override;

    std::shared_ptr<srv_state> read_state() override;

    std::shared_ptr<log_store> load_log_store() override;

    int32 server_id() override {return own_id;}

    void system_exit(const int exit_code) override;

    std::shared_ptr<srv_config> get_srv_config() const;

private:
  int32 own_id ;
  std::string endpoint; 
  std::shared_ptr<SyncLogStore> cur_log_store;
  std::shared_ptr<srv_config> server_config;
  std::shared_ptr<cluster_config> saved_config;
  std::shared_ptr<srv_state> saved_state;
  
};

} // namespace sync
