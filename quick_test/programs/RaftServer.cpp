#include <programs/RaftServer.h>

#include <libnuraft/nuraft.hxx>
#include <sync/Log.h>
#include <sync/Context.h>
#include <sync/SyncStateMachine.h>
#include <sync/SyncStateManager.h>

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <iostream>

using raft_result = nuraft::cmd_result<std::shared_ptr<nuraft::buffer>>;

namespace sync
{

void RaftServer::reset()
{
    state_machine.reset();
    state_manager.reset();
    raft_instance.reset();
}


void RaftServer::addServer(int server_id_to_add, const std::string & endpoint_to_add, ContextPtr context)
{
    nuraft::srv_config srv_config_to_add(server_id_to_add, endpoint_to_add);
    if ( !server_id_to_add || server_id_to_add == server_id ) {
        LOG_ERROR(context->getLogger(),"RaftServer::addServer()","add server error with id: "+ server_id_to_add);
        return;
    }
    std::shared_ptr<raft_result> ret = raft_instance->add_srv(srv_config_to_add);
    if (!ret->get_accepted())
    {
        LOG_ERROR(context->getLogger(),"RaftServer::addServer()","add server error with error_code: " + ret->get_result_code());
    }
    LOG_TRACE(context->getLogger(), "RaftServer::addServer()","async request is in progress");
}

std::vector<std::string> RaftServer::listServer()
{   
    std::vector< std::shared_ptr<nuraft::srv_config> > configs;
    raft_instance->get_srv_config_all(configs);

    std::vector<std::string> server_list(configs.size());

    int leader_id = raft_instance->get_leader();
    int i = 1;
    for (auto& entry: configs) {
        ptr<srv_config>& srv = entry;
        if (srv->get_id() == leader_id) {
            server_list[0] = srv->get_endpoint();
            continue;
        }
        server_list[0] = srv->get_endpoint();
        i++;
    }
    return server_list;
}

bool RaftServer::appendLog(DataOption data_op, ContextPtr context)
{
    std::shared_ptr<nuraft::buffer> new_log = SyncStateMachine::encode_log(data_op, context);
    std::shared_ptr<raft_result> ret = raft_instance->append_entries({new_log});

    if(!ret->get_accepted())
    {
        return false;
    }

    return true;
}


void RaftServer::init_raft(ContextPtr context)
{   
    state_machine = nuraft::cs_new<SyncStateMachine>();
    state_manager = nuraft::cs_new<SyncStateManager>(server_id,endpoint);

    asio_service::options asio_opt;
    asio_opt.thread_pool_size_ = 4;

    nuraft::raft_params params;

    // heartbeat: 100 ms, election timeout: 200 - 400 ms.
    params.heart_beat_interval_ = 100;
    params.election_timeout_lower_bound_ = 200;
    params.election_timeout_upper_bound_ = 400;

    // Upto 5 logs will be preserved ahead the last snapshot.
    params.reserved_log_items_ = 5;
    // Snapshot will be created for every 5 log appends.
    params.snapshot_distance_ = 5;
    // Client timeout: 3000 ms.
    params.client_req_timeout_ = 3000;
    // According to this method, `append_log` function
    // should be handled differently.
    params.return_method_ = nuraft::raft_params::blocking;

    raft_instance = launcher.init(
        state_machine, state_manager, context->getLogger(), port, asio_opt, params);

    if (!raft_instance) 
    {   
        LOG_ERROR(context->getLogger(), "RaftServer::init_raft()", "error on init raft server");
        exit(1);
    }
    
    // Wait until Raft server is ready (upto 5 seconds).
    const size_t MAX_TRY = 20;
    LOG_DEBUG(context->getLogger(), "RaftServer::init_raft()","init Raft instance");
    for (size_t ii=0; ii<MAX_TRY; ++ii) {
        if (raft_instance->is_initialized()) {
            LOG_DEBUG(context->getLogger(), "RaftServer::init_raft()","init Raft instance done");
            return;
        }
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    LOG_ERROR(context->getLogger(), "RaftServer::init_raft()","init Raft instance error");
    exit(-1);
}

}