#include <sync/SyncLogStore.h>
#include <libnuraft/nuraft.hxx>



using namespace nuraft;
namespace sync
{

SyncLogStore::SyncLogStore()
    : start_idx(1)
    , raft_server_bwd_pointer(nullptr)
    , disk_emul_delay(0)
    , disk_emul_thread(nullptr)
    , disk_emul_thread_stop_signal(false)
    , disk_emul_last_durable_index(0)
{
    // Dummy entry for index 0.
    ptr<buffer> buf = buffer::alloc(sz_ulong);
    logs[0] = cs_new<log_entry>(0, buf);
}

SyncLogStore::~SyncLogStore()
{
    if (disk_emul_thread)
    {
        disk_emul_thread_stop_signal = true;
        if (disk_emul_thread->joinable())
        {
            disk_emul_thread->join();
        }
    }
}

ptr<log_entry> SyncLogStore::make_clone(const ptr<log_entry> & entry)
{
    // NOTE:
    //   Timestamp is used only when `replicate_log_timestamp_` option is on.
    //   Otherwise, log store does not need to store or load it.
    ptr<log_entry> clone
        = cs_new<log_entry>(entry->get_term(), buffer::clone(entry->get_buf()), entry->get_val_type(), entry->get_timestamp());
    return clone;
}

ulong SyncLogStore::next_slot() const
{
    std::lock_guard<std::mutex> l(logs_lock);
    // Exclude the dummy entry.
    return start_idx + logs.size() - 1;
}

ulong SyncLogStore::start_index() const
{
    return start_idx;
}

ptr<log_entry> SyncLogStore::last_entry() const
{
    ulong next_idx = next_slot();
    std::lock_guard<std::mutex> l(logs_lock);
    auto entry = logs.find(next_idx - 1);
    if (entry == logs.end())
    {
        entry = logs.find(0);
    }

    return make_clone(entry->second);
}

ulong SyncLogStore::append(ptr<log_entry> & entry)
{
    ptr<log_entry> clone = make_clone(entry);

    std::lock_guard<std::mutex> l(logs_lock);
    size_t idx = start_idx + logs.size() - 1;
    logs[idx] = clone;

    if (disk_emul_delay)
    {
        uint64_t cur_time = timer_helper::get_timeofday_us();
        disk_emul_logs_being_written[cur_time + disk_emul_delay * 1000] = idx;
    }

    return idx;
}

void SyncLogStore::write_at(ulong index, ptr<log_entry> & entry)
{
    ptr<log_entry> clone = make_clone(entry);

    // Discard all logs equal to or greater than `index.
    std::lock_guard<std::mutex> l(logs_lock);
    auto itr = logs.lower_bound(index);
    while (itr != logs.end())
    {
        itr = logs.erase(itr);
    }
    logs[index] = clone;

    if (disk_emul_delay)
    {
        uint64_t cur_time = timer_helper::get_timeofday_us();
        disk_emul_logs_being_written[cur_time + disk_emul_delay * 1000] = index;

        // Remove entries greater than `index`.
        auto entry = disk_emul_logs_being_written.begin();
        while (entry != disk_emul_logs_being_written.end())
        {
            if (entry->second > index)
            {
                entry = disk_emul_logs_being_written.erase(entry);
            }
            else
            {
                entry++;
            }
        }
    }
}

ptr<std::vector<ptr<log_entry>>> SyncLogStore::log_entries(ulong start, ulong end)
{
    ptr<std::vector<ptr<log_entry>>> ret = cs_new<std::vector<ptr<log_entry>>>();

    ret->resize(end - start);
    ulong cc = 0;
    for (ulong ii = start; ii < end; ++ii)
    {
        ptr<log_entry> src = nullptr;
        {
            std::lock_guard<std::mutex> l(logs_lock);
            auto entry = logs.find(ii);
            if (entry == logs.end())
            {
                entry = logs.find(0);
                assert(0);
            }
            src = entry->second;
        }
        (*ret)[cc++] = make_clone(src);
    }
    return ret;
}

ptr<std::vector<ptr<log_entry>>> SyncLogStore::log_entries_ext(ulong start, ulong end, int64 batch_size_hint_in_bytes)
{
    ptr<std::vector<ptr<log_entry>>> ret = cs_new<std::vector<ptr<log_entry>>>();

    if (batch_size_hint_in_bytes < 0)
    {
        return ret;
    }

    size_t accum_size = 0;
    for (ulong ii = start; ii < end; ++ii)
    {
        ptr<log_entry> src = nullptr;
        {
            std::lock_guard<std::mutex> l(logs_lock);
            auto entry = logs.find(ii);
            if (entry == logs.end())
            {
                entry = logs.find(0);
                assert(0);
            }
            src = entry->second;
        }
        ret->push_back(make_clone(src));
        accum_size += src->get_buf().size();
        if (batch_size_hint_in_bytes && accum_size >= (ulong)batch_size_hint_in_bytes)
            break;
    }
    return ret;
}

ptr<log_entry> SyncLogStore::entry_at(ulong index)
{
    ptr<log_entry> src = nullptr;
    {
        std::lock_guard<std::mutex> l(logs_lock);
        auto entry = logs.find(index);
        if (entry == logs.end())
        {
            entry = logs.find(0);
        }
        src = entry->second;
    }
    return make_clone(src);
}

ulong SyncLogStore::term_at(ulong index)
{
    ulong term = 0;
    {
        std::lock_guard<std::mutex> l(logs_lock);
        auto entry = logs.find(index);
        if (entry == logs.end())
        {
            entry = logs.find(0);
        }
        term = entry->second->get_term();
    }
    return term;
}

ptr<buffer> SyncLogStore::pack(ulong index, int32 cnt)
{
    std::vector<ptr<buffer>> tmp_logs;

    size_t size_total = 0;
    for (ulong ii = index; ii < index + cnt; ++ii)
    {
        ptr<log_entry> le = nullptr;
        {
            std::lock_guard<std::mutex> l(logs_lock);
            le = logs[ii];
        }
        assert(le.get());
        ptr<buffer> buf = le->serialize();
        size_total += buf->size();
        tmp_logs.push_back(buf);
    }

    ptr<buffer> buf_out = buffer::alloc(sizeof(int32) + cnt * sizeof(int32) + size_total);
    buf_out->pos(0);
    buf_out->put((int32)cnt);

    for (auto & entry : tmp_logs)
    {
        ptr<buffer> & bb = entry;
        buf_out->put((int32)bb->size());
        buf_out->put(*bb);
    }
    return buf_out;
}

void SyncLogStore::apply_pack(ulong index, buffer & pack)
{
    pack.pos(0);
    int32 num_logs = pack.get_int();

    for (int32 ii = 0; ii < num_logs; ++ii)
    {
        ulong cur_idx = index + ii;
        int32 buf_size = pack.get_int();

        ptr<buffer> buf_local = buffer::alloc(buf_size);
        pack.get(buf_local);

        ptr<log_entry> le = log_entry::deserialize(*buf_local);
        {
            std::lock_guard<std::mutex> l(logs_lock);
            logs[cur_idx] = le;
        }
    }

    {
        std::lock_guard<std::mutex> l(logs_lock);
        auto entry = logs.upper_bound(0);
        if (entry != logs.end())
        {
            start_idx = entry->first;
        }
        else
        {
            start_idx = 1;
        }
    }
}

bool SyncLogStore::compact(ulong last_log_index)
{
    std::lock_guard<std::mutex> l(logs_lock);
    for (ulong ii = start_idx; ii <= last_log_index; ++ii)
    {
        auto entry = logs.find(ii);
        if (entry != logs.end())
        {
            logs.erase(entry);
        }
    }

    if (start_idx <= last_log_index)
    {
        start_idx = last_log_index + 1;
    }
    return true;
}

bool SyncLogStore::flush()
{
    disk_emul_last_durable_index = next_slot() - 1;
    return true;
}

void SyncLogStore::close()
{
}

void SyncLogStore::set_disk_delay(raft_server * raft, size_t delay_ms)
{
    disk_emul_delay = delay_ms;
    raft_server_bwd_pointer = raft;

    if (!disk_emul_thread)
    {
        disk_emul_thread = std::unique_ptr<std::thread>(new std::thread(&SyncLogStore::disk_emul_loop, this));
    }
}

ulong SyncLogStore::last_durable_index()
{
    uint64_t last_log = next_slot() - 1;
    if (!disk_emul_delay)
    {
        return last_log;
    }

    return disk_emul_last_durable_index;
}

void SyncLogStore::disk_emul_loop()
{
    // This thread mimics async disk writes.

    size_t next_sleep_us = 100 * 1000;
    while (!disk_emul_thread_stop_signal)
    {
        if (disk_emul_thread_stop_signal)
            break;

        uint64_t cur_time = timer_helper::get_timeofday_us();
        next_sleep_us = 100 * 1000;

        bool call_notification = false;
        {
            std::lock_guard<std::mutex> l(logs_lock);
            // Remove all timestamps equal to or smaller than `cur_time`,
            // and pick the greatest one among them.
            auto entry = disk_emul_logs_being_written.begin();
            while (entry != disk_emul_logs_being_written.end())
            {
                if (entry->first <= cur_time)
                {
                    disk_emul_last_durable_index = entry->second;
                    entry = disk_emul_logs_being_written.erase(entry);
                    call_notification = true;
                }
                else
                {
                    break;
                }
            }

            entry = disk_emul_logs_being_written.begin();
            if (entry != disk_emul_logs_being_written.end())
            {
                next_sleep_us = entry->first - cur_time;
            }
        }

        if (call_notification)
        {
            raft_server_bwd_pointer->notify_log_append_completion(true);
        }
    }
}

}
