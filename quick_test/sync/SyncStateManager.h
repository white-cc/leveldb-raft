#pragma once

#include <libnuraft/nuraft.hxx>
#include <memory>

namespace sync {

class SyncStateManager;
using SyncStateManagerPtr = std::shared_ptr<SyncStateManager>;

class SyncStateManager : public nuraft::state_mgr {
 private:
  /* data */
 public:
  SyncStateManager(/* args */);
  ~SyncStateManager();
};

SyncStateManager::SyncStateManager(/* args */) {}

SyncStateManager::~SyncStateManager() {}

}  // namespace sync