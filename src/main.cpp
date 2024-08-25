#include <print>
#include <string>
#include <chrono>
#include <thread>

#include "serial.hpp"


int main()
{
	boost::asio::io_context cnt;

	if (auto device = Serial::openSerialPort("COM11", cnt); device)
	{
		device->set_option(boost::asio::serial_port_base::baud_rate(9600));

		// A few seconds for the connection.
		std::this_thread::sleep_for(std::chrono::seconds(1));

		auto tx_data = Serial::writeToSerialPort(*device, "red");
		auto rx_data = Serial::readFromSerialPort(*device);
		if (rx_data)
		{
			std::println("Received: {}", *rx_data);
		}

		// NOTE: ALWAYS REMEMBER TO CLOSE THE SERIAL PORT WHEN DONE!!!!
		// HINT: May be This can be sloved automatically by RAII
		device->close();
	}
}
