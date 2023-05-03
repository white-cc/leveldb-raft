#include <sync/SyncStateMachine.h>

#include <libnuraft/nuraft.hxx>
#include <db/Metadata.h>
#include <db/SyncDB.h>
#include <sync/Log.h>
#include <sync/Context.h>
#include <proto/database.pb.h>

namespace sync
{

SyncStateMachine::SyncStateMachine() : last_committed_idx_(0)
{
}
SyncStateMachine::~SyncStateMachine()
{
}

ptr<buffer> SyncStateMachine::encode_log(const DataOption& payload, ContextPtr context)
{
    database::DataOptionPayload raw_payload;
    raw_payload.set_opt_type(payload.opt_type);
    raw_payload.set_space(payload.space);
    raw_payload.set_key(payload.key);
    raw_payload.set_value(payload.value);
    
    std::string raw_payload_str;
    if (raw_payload.SerializeToString(&raw_payload_str))
    {
        LOG_ERROR(context->getLogger(), "SyncStateMachine::encode_log()", "error on serialize log");
    }

    ptr<buffer> ret_buffer = buffer::alloc(raw_payload_str.size());
    buffer_serializer bs(ret_buffer);
    
    bs.put_raw(raw_payload_str.c_str(), raw_payload_str.size());

    return ret_buffer;
}
    
void SyncStateMachine::decode_log(buffer& log, DataOption& payload_out, ContextPtr context)
{   
    buffer_serializer bs(log);
    std::string raw_payload_str(bs.get_str());
    database::DataOptionPayload raw_payload;

    if (!raw_payload.ParseFromString(raw_payload_str))
    {
        LOG_ERROR(context->getLogger(),"SyncStateMachine::decode_log()", "error on Parse log");
    }

    payload_out.opt_type =  static_cast<OpType>(raw_payload.opt_type());
    payload_out.key = raw_payload.key();
    payload_out.space = raw_payload.space();
    payload_out.value = raw_payload.value();
}


ptr<buffer> SyncStateMachine::commit(const ulong log_idx, buffer & data)
{   
    DataOption payload_out;
    decode_log(data, payload_out, context);

    SyncDBPtr db = Metadata::getOrCreateNameSpace(payload_out.space, context);

    switch (payload_out.opt_type)
    {
    case OpType::data_put:
        db->Put(payload_out.key, payload_out.value);
        break;
    case OpType::data_delete:
        db->Delete(payload_out.key);
        break;
    default:
        break;
    }

    LOG_TRACE(context->getLogger(), "commit", "commit log_idx:"+log_idx);
    last_committed_idx_ = log_idx;

    Metadata::releaseNameSpaceLock(payload_out.space);

    return nullptr;
}

ptr<buffer> SyncStateMachine::pre_commit(const ulong log_idx, buffer & data)
{   
    DataOption payload_out;
    decode_log(data, payload_out, context);

    Metadata::getOrCreateNameSpace(payload_out.space, context);
    Metadata::tryGetNameSpaceLock(payload_out.space);

    return nullptr;
}

void SyncStateMachine::rollback(const ulong log_idx, buffer & data)
{   
    DataOption payload_out;
    decode_log(data, payload_out, context);
    Metadata::releaseNameSpaceLock(payload_out.space);
}   

void SyncStateMachine::commit_config(const ulong log_idx, std::shared_ptr<cluster_config>& new_conf)
{
    last_committed_idx_ = log_idx;
}

bool SyncStateMachine::apply_snapshot(snapshot & s)
{
    {
        std::lock_guard<std::mutex> l(last_snapshot_lock_);
        ptr<buffer> snp_buf = s.serialize();
        last_snapshot_ = snapshot::deserialize(*snp_buf);
    }
    return true;
}

ptr<snapshot> SyncStateMachine::last_snapshot()
{
    std::lock_guard<std::mutex> l(last_snapshot_lock_);
    return last_snapshot_;

}

void SyncStateMachine::create_snapshot(snapshot & s, async_result<bool>::handler_type & when_done)
{
    {
        std::lock_guard<std::mutex> l(last_snapshot_lock_);
        std::shared_ptr<buffer> snp_buf = s.serialize();
        last_snapshot_ = snapshot::deserialize(*snp_buf);
    }
    std::shared_ptr<std::exception> except(nullptr);
    bool ret = true;
    when_done(ret, except);
}

void SyncStateMachine::save_snapshot_data(snapshot & s, const ulong offset, buffer & data)
{
    // will be used in dump to snapshot to disk
}


}