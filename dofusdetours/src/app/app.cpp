#include "../utils/TcpSocket.h"
#include "app.h"

static asio::io_context io_context;
static std::atomic<bool> terminated = false;
std::shared_ptr<tcp_client> client;
std::shared_ptr<std::thread> t;

MAKE_HOOK(
	recv_hook, utils::get_process_address_safe("Ws2_32.dll", "recv"),
	int, __stdcall, SOCKET p_socket, char* buffer, int length, int flags)
{
	//do stuff before the function is called:
	//..
	if (client != nullptr && client->is_connected())
	{
		client->send(buffer, length);
	}
	std::cout << std::format("recv_hook called => {}", length) << std::endl;
	const auto result{ CALL_ORIGINAL(p_socket, buffer, length, flags) };

	//do stuff after the function is called:
	//..

	return result;
}

MAKE_HOOK(
	send_hook, utils::get_process_address_safe("Ws2_32.dll", "send"),
	int, __stdcall, SOCKET p_socket, char* buffer, int length, int flags)
{
	std::cout << std::format("send_hook called => {}", length) << std::endl;

	//do stuff before the function is called:
	//..
	const auto result{ CALL_ORIGINAL(p_socket, buffer, length, flags) };
	//do stuff after the function is called:
	//..

	return result;
}

bool app::load()
{
	if (!get_hook_manager().initAllHooks())
	{
		return false;
	}
	
	t = std::make_shared<std::thread>([]()
		{
			client = std::make_shared<tcp_client>(io_context);
			asio::error_code ec = client->start();
			while (ec && !terminated)
			{
				std::cout << "Could not connect to bot retrying in 500ms" << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				ec = client->start();
			}

			if (!ec)
			{

				std::cout << std::format("Connected to {}", client->get_address()) << std::endl;
				io_context.run();
				std::cout << "Asio thread returned" << std::endl;
			}
		});
	//load stuff:
	//...

	return true;
}

bool app::unload()
{
	if (hook_manager::freeAllHooks() != MH_OK)
	{
		return false;
	}
	terminated = true;
	io_context.stop();
	if (t != nullptr && t->joinable())
		t->join();
	//unload stuff:
	//...

	return true;
}