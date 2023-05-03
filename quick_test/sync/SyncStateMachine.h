#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <db/SyncDB.h>
#include <sync/Log.h>
#include <sync/Context.h>
#include <libnuraft/nuraft.hxx>

using namespace nuraft;

namespace sync
{

class SyncStateMachine;
using SyncStateMachinePtr = std::shared_ptr<SyncStateMachine>;

enum OpType
{
    // Data op
    data_put = 1,
    data_delete = 2
};

struct DataOption
{
    OpType opt_type;
    std::string space;
    std::string key;
    std::string value; 
};

class SyncStateMachine : public state_machine
{
public:
    SyncStateMachine();
    ~SyncStateMachine();

    static std::shared_ptr<buffer> encode_log(const DataOption& payload, ContextPtr context);

    static void decode_log(buffer& log, DataOption& payload_out, ContextPtr context);

    std::shared_ptr<buffer> commit(const ulong log_idx, buffer & data) override;

    std::shared_ptr<buffer> pre_commit(const ulong log_idx, buffer & data) override;

    void rollback(const ulong log_idx, buffer& data) override;

    void commit_config(const ulong log_idx, std::shared_ptr<cluster_config>& new_conf);
    
    bool apply_snapshot(snapshot & s) override;

    ulong last_commit_index() override { return last_committed_idx_; }

    std::shared_ptr<snapshot> last_snapshot() override;

    void create_snapshot(snapshot& s, async_result<bool>::handler_type& when_done) override;

    void save_snapshot_data(snapshot& s, const ulong offset, buffer& data);

private:
    // db
    ContextPtr context;
    // raft
    std::atomic<uint64_t> last_committed_idx_;
    ptr<snapshot> last_snapshot_;
    std::mutex last_snapshot_lock_;
};

using SyncStateMachinePtr = std::shared_ptr<SyncStateMachine>;

} // namespace sync
