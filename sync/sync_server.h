#include <atomic>
#include <mutex>
#include <libnuraft/nuraft.hxx>

namespace leveldb {

namespace sync {
class sync_server : public nuraft::state_machine {
 public:
  sync_server() {}

  ~sync_server() {}

 private:
  std::atomic<uint64_t> last_commit_index_;


};

}  // namespace sync
}  // namespace leveldb