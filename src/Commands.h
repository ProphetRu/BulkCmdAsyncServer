#pragma once

#include <string>
#include <vector>

/**
 * @brief A type alias for a vector of strings.
 */
using StringList = std::vector<std::string>;

/**
 * @brief An interface for a command.
 */
struct ICommand
{
public:
	/**
	 * @brief The list of commands.
	 */
	StringList Commands{};

	/**
	 * @brief Default constructor.
	 */
	ICommand() = default;

	/**
	 * @brief Constructs an instance with the given list of commands.
	 * @param cmds The list of commands.
	 */
	explicit ICommand(const StringList& cmds) :
		Commands{ cmds }
	{
	}

	/**
	 * @brief Destructor.
	 */
	virtual ~ICommand() noexcept = default;

	/**
	 * @brief Checks if the command is valid.
	 * @return True if the command is valid, false otherwise.
	 */
	[[nodiscard]] virtual bool IsValid() const noexcept
	{
		return !Commands.empty();
	}
};

/**
 * @brief A command block that contains a list of commands and a bulk value.
 */
struct CommandBlock final : public ICommand
{
public:
	/**
	 * @brief The bulk value.
	 */
	unsigned Bulk{};

	/**
	 * @brief Default constructor.
	 */
	CommandBlock() :
		ICommand(),
		Bulk{}
	{
	}

	/**
	 * @brief Constructs an instance with the given bulk value.
	 * @param bulk The bulk value.
	 */
	explicit CommandBlock(unsigned bulk) :
		ICommand(),
		Bulk{ bulk }
	{
	}

	/**
	 * @brief Destructor.
	 */
	virtual ~CommandBlock() noexcept override = default;

	/**
	 * @brief Checks if the command block is valid.
	 * @return True if the command block is valid, false otherwise.
	 */
	[[nodiscard]] virtual bool IsValid() const noexcept override
	{
		return ICommand::IsValid() && Bulk > 0;
	}
};

/**
 * @brief A command log that contains a timestamp and a list of commands.
 */
struct CommandLog : public ICommand
{
public:
	/**
	 * @brief The timestamp.
	 */
	std::string Timestamp;

	/**
	 * @brief Default constructor.
	 */
	CommandLog() = default;

	/**
	 * @brief Constructs an instance with the given timestamp and list of commands.
	 * @param timestamp The timestamp.
	 * @param cmds The list of commands.
	 */
	CommandLog(const std::string& timestamp, const StringList& cmds) :
		ICommand(cmds),
		Timestamp{ timestamp }
	{
	}

	/**
	 * @brief Destructor.
	 */
	virtual ~CommandLog() noexcept override = default;

	/**
	 * @brief Checks if the command log is valid.
	 * @return True if the command log is valid, false otherwise.
	 */
	[[nodiscard]] virtual bool IsValid() const noexcept override
	{
		return ICommand::IsValid() && !Timestamp.empty();
	}

	/**
	 * @brief Resets the command log.
	 */
	virtual void Reset() noexcept
	{
		Timestamp.clear();
		Commands.clear();
	}
};
