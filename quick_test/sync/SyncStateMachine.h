#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
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

enum DataOpType
{

};

class SyncStateMachine : public state_machine
{
public:
    SyncStateMachine();
    ~SyncStateMachine();

private:
    bool apply_snapshot(snapshot & s) override;

    std::unordered_map<std::string, DataSpaceStateType> data_space_state;
};

} // namespace sync