#pragma once

#include <exception>
#include <string>

namespace sync
{

class DBException : public std::exception
{
private:
    std::string msg;

public:
    DBException(const std::string msg_) : msg(msg_) { }

    virtual const char * what() const noexcept
    {
        std::string error_msg = "DBException()" + msg;
        return error_msg.c_str();
    }

    virtual ~DBException() { }
};


}