#pragma once

#include <memory>
#include <vector>
#include <libnuraft/internal_timer.hxx>
#include <libnuraft/nuraft.hxx>

using namespace nuraft;
namespace sync
{

class SyncLogStore;

class SyncLogStore : public log_store
{
public:
    SyncLogStore();

    ~SyncLogStore();

    ulong next_slot() const override;

    ulong start_index() const override;

    ptr<log_entry> last_entry() const override;

    ulong append(ptr<log_entry>& entry) override;

    void write_at(ulong index, ptr<log_entry>& entry);

    ptr<std::vector<ptr<log_entry>>> log_entries(ulong start, ulong end);

    ptr<std::vector<ptr<log_entry>>> log_entries_ext(
            ulong start, ulong end, int64 batch_size_hint_in_bytes = 0);

    ptr<log_entry> entry_at(ulong index);

    ulong term_at(ulong index);

    ptr<buffer> pack(ulong index, int32 cnt);

    void apply_pack(ulong index, buffer& pack);

    bool compact(ulong last_log_index);

    bool flush();

    void close();

    ulong last_durable_index();

    void set_disk_delay(raft_server* raft, size_t delay_ms);

private:
    SyncLogStore(const SyncLogStore&) = delete;
    SyncLogStore& operator=(const SyncLogStore&) = delete;

    static ptr<log_entry> make_clone(const ptr<log_entry>& entry);
    void disk_emul_loop();

    std::map<ulong, ptr<log_entry>> logs;

    mutable std::mutex logs_lock;
    std::atomic<ulong> start_idx;

    raft_server* raft_server_bwd_pointer;
    std::atomic<size_t> disk_emul_delay;
    std::map<uint64_t, uint64_t> disk_emul_logs_being_written;
    std::unique_ptr<std::thread> disk_emul_thread;
    std::atomic<bool> disk_emul_thread_stop_signal;
    std::atomic<uint64_t> disk_emul_last_durable_index;
};

}