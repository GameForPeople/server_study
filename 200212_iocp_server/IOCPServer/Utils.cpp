#include "stdafx.h"
#include "Utils.h"

_NORETURN void ERROR_UTIL::Error(const std::string_view msg)
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