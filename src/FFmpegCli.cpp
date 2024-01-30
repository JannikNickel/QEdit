#include "pch.h"
#include "FFmpegCli.h"
#include <windows.h>
#include <string>
#include <thread>

#pragma comment(lib, "shell32.lib")

static void CreateStdBufferThread(std::thread& thread, HANDLE& stdRead, const std::function<void(std::string)>& readCallback)
{
	thread = std::thread([&]
	{
		const int bufferSize = 1024;
		char buffer[bufferSize] = { 0 };
		while(true)
		{
			DWORD n = 0;
			if(!ReadFile(stdRead, buffer, (DWORD)bufferSize, &n, NULL) || n == 0)
			{
				break;
			}
			readCallback(std::string(buffer, n));
		}
	});
}

bool FFmpegCli::RunCmd(std::string cmd, std::function<void(std::string)> stdOutReadCallback, std::function<void(std::string)> stdErrReadCallback)
{
	SECURITY_ATTRIBUTES sa = SECURITY_ATTRIBUTES();
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = nullptr;

	HANDLE stdOutRead = INVALID_HANDLE_VALUE;
	HANDLE stdOutWrite = INVALID_HANDLE_VALUE;
	if(!CreatePipe(&stdOutRead, &stdOutWrite, &sa, 0) || !SetHandleInformation(stdOutRead, HANDLE_FLAG_INHERIT, 0))
	{
		return false;
	}
	HANDLE stdErrRead = INVALID_HANDLE_VALUE;
	HANDLE stdErrWrite = INVALID_HANDLE_VALUE;
	if(!CreatePipe(&stdErrRead, &stdErrWrite, &sa, 0) || !SetHandleInformation(stdErrRead, HANDLE_FLAG_INHERIT, 0))
	{
		if(stdOutRead != INVALID_HANDLE_VALUE)
		{
			CloseHandle(stdOutRead);
		}
		if(stdOutWrite != INVALID_HANDLE_VALUE)
		{
			CloseHandle(stdOutWrite);
		}
		return false;
	}

	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFOA);
	si.hStdInput = 0;
	si.hStdOutput = stdOutWrite;
	si.hStdError = stdErrWrite;
	if(stdOutRead || stdErrRead)
	{
		si.dwFlags |= STARTF_USESTDHANDLES;
	}

	char cmdStr[8096];
	strncpy_s(cmdStr, cmd.c_str(), 8096);
	cmdStr[8096 - 1] = '\0';

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	int success = CreateProcessA(NULL, cmdStr, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	CloseHandle(stdOutWrite);
	CloseHandle(stdErrWrite);

	if(!success)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(stdOutRead);
		CloseHandle(stdErrRead);
		return false;
	}
	CloseHandle(pi.hThread);

	std::thread stdOutT;
	CreateStdBufferThread(stdOutT, stdOutRead, stdOutReadCallback);
	std::thread stdErrT;
	CreateStdBufferThread(stdErrT, stdErrRead, stdErrReadCallback);

	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	if(stdOutT.joinable())
	{
		stdOutT.join();
	}
	if(stdErrT.joinable())
	{
		stdErrT.join();
	}
	CloseHandle(stdOutRead);
	CloseHandle(stdErrRead);
	return true;
}
