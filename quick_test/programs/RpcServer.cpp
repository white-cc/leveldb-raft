#include <programs/RpcServer.h>

#include <programs/RaftServer.h>
#include <programs/RpcService.h>

#include <sync/SyncStateMachine.h>

#include <db/Metadata.h>
#include <db/SyncDB.h>

#include <proto/database.grpc.pb.h>
#include <proto/database.pb.h>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <vector>


using database::Data;
using database::Database;
using database::Key;
using database::Result;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

namespace sync
{

Status RpcServerImpl::Put(ServerContext * context, const Data * request, Result * response)
{
    std::string name_space = request->key().name_space();
    std::string key = request->key().value();
    std::string data = request->value();
    if (raft_server->raft_instance->is_leader())
    {
        request->key().name_space();
        DataOption data_op{OpType::data_put, name_space, key, data};
        bool ret = raft_server->appendLog(data_op, database_context);

        if (!ret)
        {
            return Status::CANCELLED;
        }
        return Status::OK;
    }
    else
    {
        auto servers_lists = raft_server->listServer();
        RpcServiceImpl service(grpc::CreateChannel(servers_lists[0], grpc::InsecureChannelCredentials()));
        service.Put(name_space, key, data);
    }
}

Status RpcServerImpl::Get(ServerContext * context, const Key * request, Data * response)
{
    std::string name_space = request->name_space();
    std::string key = request->value();
    SyncDBPtr db = database_context->getMetaData()->getOrCreateNameSpace(name_space, database_context);
    if (db == nullptr)
    {
        response->set_value("");
        return Status::OK;
    }
    response->set_value(db->Get(key));
    return Status::OK;
}

Status RpcServerImpl::Delete(ServerContext * context, const Key * request, Result * response)
{
    std::string name_space = request->name_space();
    std::string key = request->value();
    ;
    if (raft_server->raft_instance->is_leader())
    {
        DataOption data_op{OpType::data_delete, name_space, key};
        bool ret = raft_server->appendLog(data_op, database_context);
        if (!ret)
        {
            return Status::CANCELLED;
        }
        return Status::OK;
    }
    else
    {
        auto servers_lists = raft_server->listServer();
        RpcServiceImpl service(grpc::CreateChannel(servers_lists[0], grpc::InsecureChannelCredentials()));
        service.Delete(name_space, key);
    }
}

std::unique_ptr<Server> RpcServerImpl::Run(std::string port, std::shared_ptr<RaftServer> raft_server, ContextPtr database_context)
{
    std::string server_address = "0.0.0.0:" + port;
    RpcServerImpl service(raft_server, database_context);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    builder.RegisterService(&service);

    return std::make_unique<Server>(builder.BuildAndStart());
}

}