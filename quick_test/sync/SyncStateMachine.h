#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <db/SyncDB.h>
#include <libnuraft/nuraft.hxx>

using namespace nuraft;

namespace sync
{

class SyncStateMachine;
using SyncStateMachinePtr = std::shared_ptr<SyncStateMachine>;

enum DataSpaceStateType
{
    on_create = 0X1,
    exist = 0X2,
    on_deleted = 0X3,
};

// todo
//  sync tmp store
//
enum OpType
{
    // Space op
    spcae_create = 0X1,
    space_delete = 0X2,
    // Data op
    data_put = 0X3,
    data_delete = 0x4
};

struct DataOpType
{
    OpType opt_type;
    buffer ;
};

class SyncStateMachine : public state_machine
{
public:
    SyncStateMachine();
    ~SyncStateMachine();

    ptr<buffer> commit(const ulong log_idx, buffer & data) override;

    ptr<buffer> pre_commit(const ulong log_idx, buffer & data) override;

    void rollback(const ulong log_idx, buffer& data) override;

    void commit_config(const ulong log_idx, ptr<cluster_config>& new_conf);
    
    bool apply_snapshot(snapshot & s) override;

    ptr<snapshot> last_snapshot() override;

    void save_snapshot_data(snapshot& s,
                                    const ulong offset,
                                    buffer& data);


private : 
    

    std::unordered_map<std::string, DataSpaceStateType> data_space_state;
};

} // namespace sync