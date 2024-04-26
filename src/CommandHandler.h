#pragma once

#include "LogerAsync.h"

/**
 * @brief A command handler class that processes command blocks and logs them asynchronously.
 */
class CommandHandler final
{
public:

	/**
	 * @brief Handles a command block by processing it and logging it asynchronously.
	 * @param block The command block to handle.
	 */
	void Handle(const CommandBlock& block) noexcept;

private:

	/**
	 * @brief The asynchronous logger.
	 */
	LogerAsync m_LogerAsync{};
};
