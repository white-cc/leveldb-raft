#ifndef _LEVELDB_SYNC_SERVER_H_
#define _LEVELDB_SYNC_SERVER_H_

#include <atomic>
#include <libnuraft/nuraft.hxx>
#include <mutex>

namespace leveldb {

namespace sync {

using namespace nuraft;

class sync_server : public state_machine {
 public:
  sync_server();

  ~sync_server();

  ptr<buffer> pre_commit(const ulong log_idx, buffer& data) override;

  ptr<buffer> commit(const ulong log_idx, buffer& data) override;

  void commit_config(const ulong log_idx,
                     ptr<cluster_config>& new_conf) override;

  void rollback(const ulong log_idx, buffer& data) override;

 private:
  std::atomic<uint64_t> last_commit_index_;

  // The begin commit index for snapshot
  std::atomic<uint64_t> fist_snapshot_index_;

  // Last raft snapshot
  ptr<nuraft::snapshot> last_snapshot_;

  // Mutex for last snapshot.
  std::mutex last_snapshot_lock_;
};

}  // namespace sync
}  // namespace leveldb

#endif  //_LEVELDB_SYNC_SERVER_H_