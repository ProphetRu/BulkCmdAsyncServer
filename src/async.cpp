#include "async.h"

namespace async
{
    namespace internal
    {
        CommandReceiverSafe& GetInstance() noexcept
    	{
            static CommandReceiverSafe instance;
            return instance;
        }
    }
	
    handle_t connect(std::size_t bulk)
    {
        return internal::GetInstance().Connect((unsigned)bulk);
    }

    void receive(handle_t handle, const char* data, std::size_t size)
    {
        return internal::GetInstance().Receive(handle, data, size);
    }

    void disconnect(handle_t handle)
    {
        return internal::GetInstance().Disconnect(handle);
    }
}
