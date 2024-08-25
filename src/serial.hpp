/// DEPENDENCIES:
/// 1 BOOST ASIO
/// 
/// 
/// ==================================================================================================
/// NOTE: ALWAYS REMEMBER TO CLOSE THE SERIAL PORT WHEN DONE!!!!
/// HINT: May be This can be sloved automatically by RAII
/// ==================================================================================================

#pragma once

#include <boost/asio.hpp>
#include <expected>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <devguid.h>

#pragma comment(lib, "setupapi.lib")

namespace Serial
{
	/**
	 * @brief Lists all available serial ports on a Windows system.
	 *
	 * This function uses the SetupAPI to enumerate all available serial ports
	 * and returns their friendly names.
	 *
	 * @return std::vector<std::string> A vector of strings containing the names of available serial ports.
	 */
	[[nodiscard("listSerialPorts returns a vector of strings of names of available serial ports")]]
	auto listSerialPorts() -> std::vector<std::string>
	{
		std::vector<std::string> ports;

		HDEVINFO device_info_set = SetupDiGetClassDevs(
			&GUID_DEVCLASS_PORTS,
			nullptr,
			nullptr,
			DIGCF_PRESENT);

		if (device_info_set == INVALID_HANDLE_VALUE)
		{
			return ports;
		}

		SP_DEVINFO_DATA device_info_data;
		device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

		for (DWORD i{ 0 };
			SetupDiEnumDeviceInfo(
				device_info_set,
				i,
				&device_info_data);
			++i)
		{
			DWORD data_type;
			char buffer[256];
			DWORD buffer_size{ 0 };

			if (SetupDiGetDeviceRegistryProperty(
				device_info_set,
				&device_info_data,
				SPDRP_FRIENDLYNAME,
				&data_type,
				(PBYTE)buffer,
				sizeof(buffer),
				&buffer_size))
			{
				ports.push_back(buffer);
			}
		}

		SetupDiDestroyDeviceInfoList(device_info_set);
		return ports;
	}
}

#elif defined(__linux__)
#include <filesystem>

namespace Serial
{
	/**
	 * @brief Lists all available serial ports on a Linux system.
	 *
	 * This function iterates through the /dev directory and finds all entries
	 * that contain "tty" in their names, which typically represent serial ports.
	 *
	 * @return std::vector<std::string> A vector of strings containing the paths of available serial ports.
	 */
	[[nodiscard("listSerialPorts returns a vector of strings of names of available serial ports")]]
	auto listSerialPorts() -> std::vector<std::string>
	{
		std::vector<std::string> ports;
		for (const auto& entry : std::filesystem::directory_iterator("/dev"))
		{
			std::string path = entry.path().string();
			if (path.find("tty") != std::string::npos)
			{
				ports.push_back(path);
			}
		}
		return ports;
	}
}

#else
#error "UNSUPPORTED OPERATING SYSTEM"
#endif


namespace Serial
{
	/**
	 * @brief Opens a serial port.
	 *
	 * This function attempts to open a serial port with the specified name.
	 *
	 * @param port The name of the serial port to open.
	 * @param ctx The Boost.Asio I/O context.
	 * @return std::expected<boost::asio::serial_port, boost::system::error_code>
	 *         An expected object containing the opened serial port or an error code.
	 */
	[[nodiscard]]
	auto openSerialPort(const std::string& port, boost::asio::io_context& ctx)
		-> std::expected<boost::asio::serial_port, boost::system::error_code>
	{
		boost::system::error_code err;
		boost::asio::serial_port device(ctx);
		device.open(port, err);
		if (err) { return std::unexpected(err); }
		return device;
	}

	/**
	 * @brief Reads data from a serial port.
	 *
	 * This function reads data from the specified serial port until a newline character is encountered.
	 *
	 * @param port The serial port to read from.
	 * @return std::expected<std::string, boost::system::error_code>
	 *         An expected object containing the read data as a string or an error code.
	 */
	[[nodiscard]]
	auto readFromSerialPort(boost::asio::serial_port& port)
		-> std::expected<std::string, boost::system::error_code>
	{
		boost::asio::streambuf buf;
		boost::system::error_code err;
		// The newline character signals the end of the message
		boost::asio::read_until(port, buf, '\n', err);
		if (err) { return std::unexpected(err); }
		// The buf is converted as returned as a std::string object.
		return std::string(boost::asio::buffer_cast<const char*>(buf.data()), buf.size());
	}

	/**
	 * @brief Writes data to a serial port.
	 *
	 * This function writes the specified message to the serial port.
	 *
	 * @param port The serial port to write to.
	 * @param msg The message to write to the serial port.
	 * @return std::expected<bool, boost::system::error_code>
	 *         An expected object containing true if the write was successful or an error code.
	 */
	[[nodiscard]]
	auto writeToSerialPort(boost::asio::serial_port& port, const std::string& msg)
		-> std::expected<bool, boost::system::error_code>
	{
		boost::system::error_code err;
		// Write Data to the serial port
		boost::asio::write(port, boost::asio::buffer(msg), err);
		if (err) { return std::unexpected(err); }
		return true;
	}
}