#pragma once

#include "CommandHandler.h"
#include <unordered_map>
#include <mutex>

/**
 * @brief A safe command receiver class that handles the receiving of commands.
 */
class CommandReceiverSafe final
{
public:
	/**
	 * @brief Connects to the command receiver with a specified buffer by handle.
	 * @param bulk The bulk size.
	 * @return The handle to the connection.
	 */
	unsigned Connect(unsigned bulk) noexcept;

	/**
	 * @brief Receives data and processes it as a command.
	 * @param handle The handle to the connection.
	 * @param data The data to receive.
	 * @param size The size of the data.
	 */
	void Receive(unsigned handle, const char* data, std::size_t size) noexcept;

	/**
	 * @brief Disconnects from the command receiver.
	 * @param handle The handle to the connection.
	 */
	void Disconnect(unsigned handle) noexcept;

private:

	/**
	 * @brief The unordered map of handles to command blocks.
	 */
	std::unordered_map<unsigned, CommandBlock> m_CommandBlocks;

	/**
	 * @brief The command handler.
	 */
	CommandHandler m_CommandHandler{};

	/**
	 * @brief The current handle.
	 */
	unsigned m_Handle{};

	/**
	 * @brief The mutex for synchronization.
	 */
	std::mutex m_Mutex;
};
