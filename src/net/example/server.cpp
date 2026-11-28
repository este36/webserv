#include "net/example/EchoTaskFactory.hpp"
#include "net/example/EchoTask.hpp"
#include "net/TCPListener.hpp"
#include "net/PollScheduler.hpp"

int main(void)
{
	net::PollScheduler scheduler;

	scheduler.setup_signal_handlers();

	scheduler.add(
		new net::TCPListener(
			"localhost",
			8080,
			EchoTaskFactory()
		)
	);

	scheduler.run();
}
