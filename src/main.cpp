#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif // _WIN32_WINNT
#endif // _WIN32

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <iostream>

#include "async.h"

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;

struct Config final
{
	unsigned short Port{};
	unsigned short BulkSize{};
} g_Config;

constexpr auto BUFF_SIZE{ 4096 };

awaitable<void> HandleClient(tcp::socket client_socket) noexcept
{
	const auto handle = async::connect(g_Config.BulkSize);

	try
	{
		char data[BUFF_SIZE]{ 0 };

		for (;;)
		{
			const auto n = co_await client_socket.async_read_some(boost::asio::buffer(data), use_awaitable);
            
			async::receive(handle, data, n);
		}
	}
	catch (const std::exception& ex)
	{
        std::cerr << "Exception: " << ex.what() << std::endl;
	}

	async::disconnect(handle);
}

awaitable<void> Listener() noexcept
{
	auto executor = co_await this_coro::executor;
	tcp::acceptor acceptor(executor, { tcp::v4(), g_Config.Port });

	for (;;)
	{
		auto client_socket = co_await acceptor.async_accept(use_awaitable);

		co_spawn(executor, HandleClient(std::move(client_socket)), detached);
	}
}

int main(int argc, char* argv[]) noexcept
{
	try
	{
		if (argc != 3)
		{
			std::cout << "Usage: " << argv[0] << " <port> <bulk_size>\n";
			return -1;
		}

		g_Config.Port     = static_cast<unsigned short>(std::stoi(argv[1]));
		g_Config.BulkSize = static_cast<unsigned short>(std::stoi(argv[2]));

		boost::asio::io_context io_context(1);

		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);

		signals.async_wait([&](auto, auto){ io_context.stop(); });

		co_spawn(io_context, Listener(), detached);

		io_context.run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception: " << ex.what() << std::endl;
	}

	return 0;
}
