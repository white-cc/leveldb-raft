#pragma once

#include <libnuraft/nuraft.hxx>
#include <sync/Context.h>
#include <sync/SyncStateMachine.h>
#include <sync/SyncStateManager.h>

#include <memory>
#include <string>
#include <vector>

namespace sync
{
class RaftServer
{
public:
    int server_id;
    std::string addr;
    int port;
    std::string endpoint;
    SyncStateMachinePtr state_machine;
    SyncStateManagerPtr state_manager;
    raft_launcher launcher;
    std::shared_ptr<raft_server> raft_instance;

    RaftServer(
        int server_id_, 
        const std::string& addr_, 
        int port_) 
        : server_id(server_id_)
        , addr(addr_)
        , port(port_) 
        {}

    void reset();

    bool appendLog(DataOption data_op, ContextPtr context);

    void addServer(int sevrer_id_to_add, const std::string & endpoint_to_add, ContextPtr context);

    // server_list[0] server is leader;
    std::vector<std::string> listServer();

    void init_raft(ContextPtr context);
};
}

