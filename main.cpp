#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class Pipe
{
public:
  Pipe(HANDLE pipeReader = NULL, HANDLE pipeWriter = NULL)
  {
    SECURITY_ATTRIBUTES secureAttr;

    secureAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    secureAttr.bInheritHandle = TRUE;
    secureAttr.lpSecurityDescriptor = NULL;

    CreatePipe(&pipeReader, &pipeWriter, &secureAttr, 0);

    this->reader = pipeReader;
    this->writer = pipeWriter;
  };
  BOOL CloseReader()
  {
    return CloseHandle(this->reader);
  };
  BOOL CloseWriter()
  {
    return CloseHandle(this->writer);
  };
  HANDLE reader;
  HANDLE writer;
};

template <size_t N>
void redirectToFileHandle(Pipe &childCout, HANDLE (&destHandle)[N])
{
  const size_t BUFSIZE = 4096;

  DWORD dwRead, dwWritten;
  CHAR chBuf[BUFSIZE];

  for (;;)
  {
    BOOL bSuccess = ReadFile(childCout.reader, chBuf, BUFSIZE, &dwRead, NULL);
    if (!bSuccess || dwRead == 0)
    {
      break;
    }
    for (size_t i = 0; i < N; i++)
    {
      bSuccess = WriteFile(destHandle[i], chBuf, dwRead, &dwWritten, NULL);
    }
  }
  return;
}

void CreateChildProcess(char *, Pipe &);
void saveCmdOut(const char *cmd, const char *filename);
std::vector<std::string> parseArgv(int argc, char *argv[]);
std::string GetLastErrorAsString();

int main(int argc, char *argv[])
{
  std::vector<std::string> argvec = parseArgv(argc, argv);

  Pipe childStdOut;
  std::string outFileName = "./cout.txt";

  if (argvec.size() > 2)
  {
    outFileName = argvec[argvec.size() - 1];
  }

  if (argvec[1] == "-c")
  {
    saveCmdOut(argvec[2].c_str(), outFileName.c_str());
    return 0;
  };

  HANDLE coutResultTxt = CreateFile(
      outFileName.c_str(),
      GENERIC_WRITE,
      0,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      NULL);

  CreateChildProcess(argv[1], childStdOut);

  HANDLE destHandles[2] = {GetStdHandle(STD_OUTPUT_HANDLE), coutResultTxt};

  redirectToFileHandle(childStdOut, destHandles);
  return 0;
}

std::string GetLastErrorAsString()
{
  DWORD errorMessageID = ::GetLastError();
  if (errorMessageID == 0)
  {
    return std::string();
  }

  LPSTR messageBuffer = nullptr;
  size_t size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      errorMessageID,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR)&messageBuffer,
      0,
      NULL);

  std::string message(messageBuffer, size);
  LocalFree(messageBuffer);

  return message;
}

void CreateChildProcess(char *processPath, Pipe &childStdout)
{
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;

  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);

  siStartInfo.hStdOutput = childStdout.writer;
  siStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

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

    childStdout.CloseWriter();
  }
  else
  {
    std::cout << "Cout2txt-Error:" << GetLastErrorAsString() << std::endl;
    exit(1);
    return;
  }
}

void saveCmdOut(const char *cmd, const char *filename)
{
  FILE *fp;
  char buf[4096];
  std::cout << filename << std::endl;
  std::ofstream outFile(filename);

  if ((fp = popen(cmd, "r")) != NULL)
  {
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
      std::cout << buf;
      outFile << buf;
    }
    pclose(fp);
  }

  return;
}

std::vector<std::string> parseArgv(int argc, char *argv[])
{
  std::vector<std::string> argvec;

  for (size_t i = 0; i < argc; i++)
  {
    argvec.push_back(std::string(argv[i]));
  }
  return argvec;
};
