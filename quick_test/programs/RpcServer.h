#pragma once

#include <proto/database.grpc.pb.h>
#include <proto/database.pb.h>

#include <sync/Context.h>
#include <programs/RaftServer.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using database::Database;
using database::Data;
using database::Key;
using database::Result;

namespace sync
{

class RpcServerImpl final : public Database::Service
{
public:
    RpcServerImpl(std::shared_ptr<RaftServer> raft_server_, ContextPtr database_context_)
    :raft_server(raft_server_), database_context(database_context_){}

    Status Put(ServerContext* context, const  Data* request, Result* response) override;

    Status Get(ServerContext* context, const Key* request, Data* response) override;
    
    Status Delete(ServerContext* context, const Key* request, Result* response) override;

    static std::unique_ptr<Server> Run(std::string port, std::shared_ptr<RaftServer> raft_server, ContextPtr database_context);

private:
    std::shared_ptr<RaftServer> raft_server;
    ContextPtr database_context;
};

}

