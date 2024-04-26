#include "LogerAsync.h"
//#include <format>
#include <iostream>
#include <fstream>

constexpr auto FILE_NAME      { "bulk" };
constexpr auto FILE_EXTENSION { ".log" };

LogerAsync::LogerAsync() noexcept
{
	m_WorkerThreads.reserve(4);

	m_WorkerThreads.emplace_back(std::jthread{ &LogerAsync::QueueWorkerThread,   this, m_StopToken.get_token() });
	m_WorkerThreads.emplace_back(std::jthread{ &LogerAsync::ConsoleWorkerThread, this, m_StopToken.get_token() });
	m_WorkerThreads.emplace_back(std::jthread{ &LogerAsync::FileWorkerThread1,   this, m_StopToken.get_token() });
	m_WorkerThreads.emplace_back(std::jthread{ &LogerAsync::FileWorkerThread2,   this, m_StopToken.get_token() });
}

LogerAsync::~LogerAsync() noexcept
{
	// crutch :(
	// wait until the queue is empty
	while (!m_Queue.empty())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	m_StopToken.request_stop();
	m_CVQueue.notify_all();
	m_CVWorker.notify_all();

	for (auto&& thr : m_WorkerThreads)
	{
		if (thr.joinable())
		{
			thr.request_stop();
			thr.join();
		}
	}

	m_WorkerThreads.clear();
}

void LogerAsync::Log(const CommandLog& cmd) noexcept
{
	if (cmd.IsValid())
	{
		std::unique_lock lck(m_MutexQueue);
		m_Queue.emplace(cmd);
		m_CVQueue.notify_all();
	}
}

void LogerAsync::QueueWorkerThread(const std::stop_token& stop_token) noexcept
{
	while (!stop_token.stop_requested())
	{
		std::unique_lock lck(m_MutexQueue);
		m_CVQueue.wait(lck, [&]() noexcept { return !m_Queue.empty() || stop_token.stop_requested(); });
		if (!m_Queue.empty() && !stop_token.stop_requested())
		{
			// get cmd from queue
			m_SharedCmd = m_Queue.front();
			m_Queue.pop();

			// notify worker threads
			m_CVWorker.notify_all();

			// waiting for the worker threads to complete the current shared cmd
			m_CVQueue.wait(lck, [&]() noexcept { return m_SharedCmd.IsComplete(); });
		}
	}
}

void LogerAsync::ConsoleWorkerThread(const std::stop_token& stop_token) noexcept
{
	auto log = [](const StringList& cmds) noexcept
	{
		if (!cmds.empty())
		{
			std::cout << "bulk: ";
			for (const auto& cmd : cmds)
			{
				std::cout << cmd << ", ";
			}
			std::cout << std::endl;
		}
	};

	while (!stop_token.stop_requested())
	{
		std::unique_lock lck(m_MutexWorker);
		m_CVWorker.wait(lck, [&]() noexcept { return m_SharedCmd.IsValid() || stop_token.stop_requested(); });
		if (m_SharedCmd.IsValid() && !m_SharedCmd.IsConsoleDone && !stop_token.stop_requested())
		{
			log(m_SharedCmd.Commands);

			// mark cmd as completed
			m_SharedCmd.IsConsoleDone = true;
			const auto flag{ m_SharedCmd.IsComplete() };
			m_SharedCmd.Reset();

			if (flag)
			{
				// if task completed, notify QueueWorkerThread
				m_CVQueue.notify_all();
			}
		}
	}
}

void LogerAsync::FileWorkerThread1(const std::stop_token& stop_token) noexcept
{
	while (!stop_token.stop_requested())
	{
		std::unique_lock lck(m_MutexWorker);
		m_CVWorker.wait(lck, [&]() noexcept { return m_SharedCmd.IsValid() || stop_token.stop_requested(); });
		if (m_SharedCmd.IsValid() && !m_SharedCmd.IsFileDone && !stop_token.stop_requested())
		{
			if (m_Counter % 2 == 0)
			{
				LogToFile(m_SharedCmd.Timestamp, m_SharedCmd.Commands);

				++m_Counter;

				// mark cmd as completed
				m_SharedCmd.IsFileDone = true;
				const auto flag{ m_SharedCmd.IsComplete() };
				m_SharedCmd.Reset();

				if (flag)
				{
					// if task completed, notify QueueWorkerThread
					m_CVQueue.notify_all();
				}
			}
		}
	}
}

void LogerAsync::FileWorkerThread2(const std::stop_token& stop_token) noexcept
{
	while (!stop_token.stop_requested())
	{
		std::unique_lock lck(m_MutexWorker);
		m_CVWorker.wait(lck, [&]() noexcept { return m_SharedCmd.IsValid() || stop_token.stop_requested(); });
		if (m_SharedCmd.IsValid() && !m_SharedCmd.IsFileDone && !stop_token.stop_requested())
		{
			if (m_Counter % 2 != 0)
			{
				LogToFile(m_SharedCmd.Timestamp, m_SharedCmd.Commands);
				
				++m_Counter;

				// mark cmd as completed
				m_SharedCmd.IsFileDone = true;
				const auto flag{ m_SharedCmd.IsComplete() };
				m_SharedCmd.Reset();

				if (flag)
				{
					// if task completed, notify QueueWorkerThread
					m_CVQueue.notify_all();
				}
			}
		}
	}
}

void LogerAsync::LogToFile(const std::string& timestamp, const StringList& cmds) noexcept
{
	auto get_file_name = [&]() noexcept
	{
		//return std::format("{}_{}_{}_{}", FILE_NAME, timestamp, std::to_string(m_Counter), FILE_EXTENSION);
		return FILE_NAME + timestamp +  std::to_string(m_Counter) + FILE_EXTENSION;
	};

	if (!timestamp.empty() && !cmds.empty())
	{
		if (std::ofstream log_file(get_file_name()); log_file.is_open())
		{
			log_file << "bulk: ";

			for (const auto& cmd : cmds)
			{
				log_file << cmd << ", ";
			}

			log_file << std::endl;
			log_file.close();
		}
	}
}
