#include "CommandReceiverSafe.h"

unsigned CommandReceiverSafe::Connect(unsigned bulk) noexcept
{
    std::unique_lock lck(m_Mutex);

    const auto [handle, _]{ m_CommandBlocks.emplace(std::make_pair(++m_Handle, bulk)) };

    return handle->first;
}

void CommandReceiverSafe::Receive(unsigned handle, const char* data, std::size_t size) noexcept
{
	if (handle > 0 && data && size > 0)
	{
        std::unique_lock lck(m_Mutex);
        if (m_CommandBlocks.contains(handle))
        {
            m_CommandBlocks[handle].Commands.emplace_back(data, size);
            m_CommandHandler.Handle(m_CommandBlocks[handle]);
        }
	}
}

void CommandReceiverSafe::Disconnect(unsigned handle) noexcept
{
    if (handle > 0)
    {
        std::unique_lock lck(m_Mutex);
        if (m_CommandBlocks.contains(handle))
        {
            m_CommandBlocks.erase(handle);
        }
    }
}
