#include <windows.h>
#include <tchar.h>
#include <iostream>

void CreateChildProcess(char *processPath);
void readFromPipe(HANDLE &childCoutReader);

int main(int argc, char *argv[])
{
  HANDLE childStdOut = NULL;

  CreateChildProcess(argv[1]);
}

void CreateChildProcess(char *processPath)
{
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;

  SECURITY_ATTRIBUTES secureAttr;

  secureAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  secureAttr.bInheritHandle = TRUE;
  secureAttr.lpSecurityDescriptor = NULL;

  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  // siStartInfo.dwFlags = STARTF_USESTDHANDLES;

  HANDLE coutPipeWriter = NULL;
  HANDLE coutPipeReader = NULL;

  CreatePipe(&coutPipeReader, &coutPipeWriter, &secureAttr, 0);

  std::cout << GetLastError() << std::endl;

  siStartInfo.hStdOutput = coutPipeWriter;
  siStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

  BOOL bSuccess = FALSE;

  bSuccess = CreateProcess(
      NULL,
      (LPSTR)processPath,
      NULL,
      NULL,
      TRUE,
      0,
      NULL,
      NULL,
      &siStartInfo,
      &piProcInfo);

  std::cout << "1" << std::endl;
  if (bSuccess)
  {
    std::cout << "sus" << std::endl;
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    CloseHandle(coutPipeWriter);
  }
  readFromPipe(coutPipeReader);
  std::cout << "2" << std::endl;
  WaitForSingleObject(piProcInfo.hProcess, INFINITE);
  std::cout << "3" << std::endl;
}

void readFromPipe(HANDLE &childCoutReader)
{
  const size_t BUFSIZE = 4096;

  DWORD dwRead, dwWritten;
  CHAR chBuf[BUFSIZE];
  BOOL bSuccess = FALSE;
  HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

  for (;;)
  {
    bSuccess = ReadFile(childCoutReader, chBuf, BUFSIZE, &dwRead, NULL);
    if (!bSuccess || dwRead == 0)
      break;

    bSuccess = WriteFile(hParentStdOut, chBuf,
                         dwRead, &dwWritten, NULL);
    if (!bSuccess)
      break;
  }
  return;
}
