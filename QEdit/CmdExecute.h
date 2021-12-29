#pragma once

#include <Windows.h>
#include <string>
#include <thread>
#include <functional>

#pragma comment(lib, "Shell32.lib")

void CreateStdBufferThread(std::thread& thread, HANDLE& stdRead, std::function<void(std::string)>& readCallback)
{
	if(stdRead)
	{
		thread = std::thread([&]
		{
			const int bufferSize = 1024;
			char buffer[bufferSize];
			while(true)
			{
				DWORD n = 0;
				int success = ReadFile(stdRead, buffer, (DWORD)bufferSize, &n, NULL);
				if(!success || n == 0)
				{
					break;
				}
				if(readCallback != NULL)
				{
					std::string s = std::string(buffer, n);
					readCallback(s);
				}
			}
		});
	}
}

bool CmdExecute(std::string cmd, std::function<void(std::string)> stdOutReadCallback, std::function<void(std::string)> stdErrReadCallback)
{
	SECURITY_ATTRIBUTES sa = SECURITY_ATTRIBUTES();
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = nullptr;

	//Create pipes for read/write out/err
	HANDLE stdOutRead = INVALID_HANDLE_VALUE;
	HANDLE stdOutWrite = INVALID_HANDLE_VALUE;
	if(CreatePipe(&stdOutRead, &stdOutWrite, &sa, 0) == false || SetHandleInformation(stdOutRead, HANDLE_FLAG_INHERIT, 0) == false)
	{
		return false;
	}
	HANDLE stdErrRead = INVALID_HANDLE_VALUE;
	HANDLE stdErrWrite = INVALID_HANDLE_VALUE;
	if(CreatePipe(&stdErrRead, &stdErrWrite, &sa, 0) == false || SetHandleInformation(stdErrRead, HANDLE_FLAG_INHERIT, 0) == false)
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

	//Prepare process start
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

	//Copy of the cmd which will be modified
	char cmdStr[8096];//Max length of a cmd command since win xp
	strncpy_s(cmdStr, cmd.c_str(), 8096);
	cmdStr[8096 - 1] = 0;

	//Start process
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	int success = CreateProcessA(NULL, cmdStr, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	//Write handles are not required
	CloseHandle(stdOutWrite);
	CloseHandle(stdErrWrite);

	//Failed to create the cmd process
	if(!success)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(stdOutRead);
		CloseHandle(stdErrRead);
		return false;
	}
	CloseHandle(pi.hThread);
	
	//Create threads which read the incoming pipe data
	std::thread stdOutT;
	CreateStdBufferThread(stdOutT, stdOutRead, stdOutReadCallback);
	std::thread stdErrT;
	CreateStdBufferThread(stdErrT, stdErrRead, stdErrReadCallback);

	//Wait for the process to complete
	WaitForSingleObject(pi.hProcess, INFINITE);

	//Cleanup
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