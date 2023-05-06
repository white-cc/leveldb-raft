#pragma once

#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include <proto/database.grpc.pb.h>
#include <proto/database.pb.h>


using database::Data;
using database::Database;
using database::Key;
using database::Result;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

namespace sync
{

class RpcServiceImpl
{
public:
    RpcServiceImpl(std::shared_ptr<Channel> channel_) 
        : stub(Database::NewStub(channel_)) {}

    std::string Get(const std::string & name_space, const std::string & key)
    {
        ClientContext context;
        Key request_key;
        Data response_data;

        request_key.set_name_space(name_space);
        request_key.set_value(key);

        Status status = stub->Get(&context, request_key, &response_data);

        if (status.ok())
        {
            return response_data.value();
        }

        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "";
        
    }

    bool Put(const std::string & name_space, const std::string & key, const std::string & value)
    {
        ClientContext context;
        Key request_key;
        Data request_data;
        Result response_result;

        request_key.set_name_space(name_space);
        request_key.set_value(key);

        request_data.set_allocated_key(&request_key);
        request_data.set_value(value);

        Status status = stub->Put(&context, request_data, &response_result);

        if (status.ok()&& !response_result.error())
        {   
            return true;
        }
        else if (! status.ok())
        {   
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return false;
        }
        else
        {   
            std::cout << response_result.value() << std::endl;
            return false;
        }
    }

    bool Delete(const std::string & name_space, const std::string & key) 
    { 
        ClientContext context;
        Key request_key;
        Result response_result;

        request_key.set_name_space(name_space);
        request_key.set_value(key);

        Status status = stub->Delete(&context, request_key, &response_result);

        if (status.ok()&& !response_result.error())
        {   
            return true;
        }
        else if (!status.ok())
        {   
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return false;
        }
        else
        {   
            std::cout << response_result.value() << std::endl;
            return false;
        }
    }

private:
    std::unique_ptr<Database::Stub> stub;
};

}
