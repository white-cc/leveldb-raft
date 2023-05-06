#include <memory>
#include <string>
#include <thread>
#include <iostream>
#include <sync/Context.h>
#include <grpcpp/grpcpp.h>
#include <programs/RaftServer.h>
#include <programs/RpcServer.h>

using namespace sync;

int main(int argc, char** argv)
{   
    ContextPtr context = std::make_shared<Context>();
    std::shared_ptr<RaftServer> raft_server(0,"127.0.0.1",9001);
    raft_server->init_raft(context);
    auto rpc_server = RpcServerImpl::Run("9000", raft_server,context);
    rpc_server->Wait();
    return 0;
}