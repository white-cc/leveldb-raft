#pragma once

#include <sync/log/logger.h>
#include <libnuraft/nuraft.hxx>
#include <memory>

using namespace nuraft;

namespace sync
{

#define LOG_USE(logger, level, span, msg) \
    logger->put_details(level, __FILE__, span, __LINE__, msg)

#define LOG_ERROR(logger, span, msg) \
    LOG_USE(logger, SimpleLogger::Levels::ERROR, span, msg)

#define LOG_DEBUG(logger, span, msg) \
    LOG_USE(logger, SimpleLogger::Levels::DEBUG, span, msg)

#define LOG_TRACE(logger, span, msg) \
    LOG_USE(logger, SimpleLogger::Levels::TRACE, span, msg)

class logger_wrapper : public logger
{
public:
    logger_wrapper(const std::string & log_file, int log_level = 6)
    {
        my_log = std::make_shared<SimpleLogger>(log_file, 1024, 32 * 1024 * 1024, 10);
        my_log->setLogLevel(log_level);
        my_log->setDispLevel(-1);
        my_log->setCrashDumpPath("./", true);
        my_log->start();
    }

    ~logger_wrapper() { destroy(); }

    void destroy()
    {
        if (my_log)
        {
            my_log->flushAll();
            my_log->stop();
        }
    }

    void put_details(int level, const char * source_file, const char * func_name, size_t line_number, const std::string & msg)
    {
        if (my_log)
        {
            my_log->put(level, source_file, func_name, line_number, "%s", msg.c_str());
        }
    }

    void set_level(int l)
    {
        if (!my_log)
            return;

        if (l < 0)
            l = 1;
        if (l > 6)
            l = 6;
        my_log->setLogLevel(l);
    }

    int get_level()
    {
        if (!my_log)
            return 0;
        return my_log->getLogLevel();
    }

    std::shared_ptr<SimpleLogger> getLogger() const { return my_log; }

private:
    std::shared_ptr<SimpleLogger>  my_log;
};

}
