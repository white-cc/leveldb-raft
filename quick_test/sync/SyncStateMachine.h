#pragma once

#include <libnuraft/nuraft.hxx>

using namespace nuraft;

namespace sync
{

class SyncStateMachine : public state_machine 
{
public:
    SyncStateMachine();

private:

    bool apply_snapshot(snapshot& s) override;
};



}