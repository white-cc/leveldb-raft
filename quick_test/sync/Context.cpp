#include <sync/Context.h>


namespace sync
{
Context::Context() : db_path("./"), db_create_namespace_if_missing(false)
{
}
Context::~Context()
{
}
} // namespace sync
