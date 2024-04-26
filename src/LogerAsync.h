#pragma once

#include "Commands.h"
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * @brief A shared command class that extends the CommandLog class and adds atomic flags for console and file completion.
 */
struct SharedCmd final : public CommandLog
{
public:
	/**
	 * @brief Atomic flag indicating if console output is complete.
	 */
	std::atomic_bool IsConsoleDone{ false };

	/**
	 * @brief Atomic flag indicating if file output is complete.
	 */
	std::atomic_bool IsFileDone{ false };
	
	/**
	 * @brief Default destructor.
	 */
	virtual ~SharedCmd() noexcept override = default;

	/**
	 * @brief Checks if the command is complete.
	 * @return True if both console and file flags are set, false otherwise.
	 */
	[[nodiscard]] bool IsComplete() const noexcept
	{
		return IsConsoleDone && IsFileDone;
	}

	/**
	 * @brief Resets the command.
	 */
	virtual void Reset() noexcept override
	{
		if (IsComplete())
		{
			CommandLog::Reset();
		}
	}

	/**
	 * @brief Assignment operator.
	 * @param other The command log to assign from.
	 * @return A reference to the assigned object.
	 */
	SharedCmd& operator=(const CommandLog& other) noexcept
	{
		if (this != &other)
		{
			Timestamp = other.Timestamp;
			Commands  = other.Commands;

			IsConsoleDone = false;
			IsFileDone    = false;
		}

		return *this;
	}
};

/**
 * @brief An asynchronous logger class that logs commands asynchronously to console and file.
 */
class LogerAsync final
{
public:

	/**
	 * @brief Default constructor.
	 */
	LogerAsync() noexcept;

	/**
	 * @brief Destructor.
	 */
	~LogerAsync() noexcept;

	/**
	 * @brief Logs a command asynchronously.
	 * @param cmd The command to log.
	 */
	void Log(const CommandLog& cmd) noexcept;

private:

	/**
	 * @brief Worker thread function that processes the command queue.
	 * @param stop_token The stop token for the thread.
	 */
	void QueueWorkerThread(const std::stop_token& stop_token) noexcept;

	/**
	 * @brief Worker thread function that outputs the command to the console.
	 * @param stop_token The stop token for the thread.
	 */
	void ConsoleWorkerThread(const std::stop_token& stop_token) noexcept;

	/**
	 * @brief Worker thread function that outputs the command to the file1.
	 * @param stop_token The stop token for the thread.
	 */
	void FileWorkerThread1(const std::stop_token& stop_token) noexcept;

	/**
	 * @brief Worker thread function that outputs the command to the file2.
	 * @param stop_token The stop token for the thread.
	 */
	void FileWorkerThread2(const std::stop_token& stop_token) noexcept;

	/**
	 * @brief Logs a command to a file.
	 * @param timestamp The timestamp of the command.
	 * @param cmds The list of commands.
	 */
	void LogToFile(const std::string& timestamp, const StringList& cmds) noexcept;

private:

	/**
	 * @brief Vector of worker threads.
	 */
	std::vector<std::jthread> m_WorkerThreads;

	/**
	 * @brief Stop source for the worker threads.
	 */
	std::stop_source m_StopToken;

	/**
	 * @brief Atomic counter for the number of commands processed.
	 */
	std::atomic_int m_Counter{};

	/**
	 * @brief Queue of commands to process.
	 */
	std::queue<CommandLog> m_Queue;

	/**
	 * @brief Mutex for the queue.
	 */
	std::mutex m_MutexQueue;

	/**
	 * @brief Condition variable for the queue.
	 */
	std::condition_variable m_CVQueue;

	/**
	 * @brief Shared command object.
	 */
	SharedCmd m_SharedCmd;

	/**
	 * @brief Mutex for the shared command object.
	 */
	std::mutex m_MutexWorker;

	/**
	 * @brief Condition variable for the shared command object.
	 */
	std::condition_variable m_CVWorker;
};
