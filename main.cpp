#include <windows.h>
#include <tchar.h>
#include <iostream>

void CreateChildProcess(char *processPath);

int main(int argc, char *argv[])
{
  HANDLE childStdOut = NULL;

  CreateChildProcess(argv[1]);
}

void CreateChildProcess(char *processPath)
{
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;

  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);

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

  if (bSuccess)
  {
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
  }
}
