#include "IOCPServer/stdafx.h"
#include "global_header.hh"

namespace ERROR_UTIL
{
	_NORETURN static void Error(const std::string_view msg)
	{
		LPVOID lpMsgBuf;
		int errorCode = WSAGetLastError();
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);

		std::cout << "[" << msg << "(" << errorCode << ")] "
			<< ((LPCTSTR)lpMsgBuf) << "\n";

		LocalFree(lpMsgBuf);

		while (true) {}
	}
}
