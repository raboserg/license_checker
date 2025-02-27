#include "tools.h"
#include "message_sender.h"

#ifdef _WIN32
#include <Tlhelp32.h>
#else
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif
//#include <experimental/filesystem>
#include <memory>

namespace itvpnagent {

//namespace fs {
//namespace stdfs = std::experimental::filesystem;
//using stdfs::path;
//}

namespace System {

void sleep(const unsigned long ms) {
#ifdef WIN32
  Sleep(ms);
#else
  usleep(ms * 1000);
#endif
}

string__ current_module_path() {
  string__ service_path;
#ifdef _WIN32
  WCHAR szPath[MAX_PATH];
  if (!GetModuleFileName(NULL, szPath, MAX_PATH))
    wprintf(L"Cannot get service file name, error %u\n", GetLastError());
  const string__ module_path(szPath);
  service_path =
      module_path.substr(0, module_path.find_last_of(L"\\")).append(L"\\");
#else
  service_path = string__(getcwd(NULL, 0));
  service_path.append("/");
//  std::array<char, 1024 * 4> buf;
//  auto written = readlink("/proc/self/exe", buf.data(), buf.size());
//  // Checks::check_exit(VCPKG_LINE_INFO, written != -1, "Could not determine
//  // current executable path.");
//  service_path = fs::path(buf.data(), buf.data() + written);
#endif
  return service_path;
}

bool terminate_process(const string__ &procName) {
  bool result = false;
#ifdef WIN32
  WCHAR szPath[20];
  wcscpy_s(szPath, procName.c_str());
  HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
  PROCESSENTRY32 pEntry;
  pEntry.dwSize = sizeof(pEntry);
  int hRes = Process32First(hSnapShot, &pEntry);
  while (hRes) {
    if (wcscmp(pEntry.szExeFile, szPath) == 0) {
      HANDLE hProcess =
          OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
      if (hProcess != NULL) {
        TerminateProcess(hProcess, 9);
        CloseHandle(hProcess);
        result = true;
      }
    }
    hRes = Process32Next(hSnapShot, &pEntry);
  }
  CloseHandle(hSnapShot);
#else
  int pid = getProcIdByName(procName);
  if (pid > 0) {
    int ret = kill(pid, SIGTERM); //??? SIGINT
    if (ret == 0)
      result = true;
  }
#endif
  return result;
}

#ifndef _WIN32
int getProcIdByName(const std::string &procName) {
  int pid = -1;
  // Open the /proc directory
  DIR *dp = opendir("/proc");
  if (dp != NULL) {
    // Enumerate all entries in directory until process found
    struct dirent *dirp;
    while (pid < 0 && (dirp = readdir(dp))) {
      // Skip non-numeric entries
      int id = atoi(dirp->d_name);
      if (id > 0) {
        // Read contents of virtual /proc/{pid}/cmdline file
        std::string cmdPath = std::string("/proc/") + dirp->d_name + "/cmdline";
        std::ifstream cmdFile(cmdPath.c_str());
        std::string cmdLine;
        std::getline(cmdFile, cmdLine);
        if (!cmdLine.empty()) {
          // Keep first cmdline item which contains the program path
          size_t pos = cmdLine.find('\0');
          if (pos != std::string::npos)
            cmdLine = cmdLine.substr(0, pos);
          // Keep program name only, removing the path
          pos = cmdLine.rfind('/');
          if (pos != std::string::npos)
            cmdLine = cmdLine.substr(pos + 1);
          // Compare against requested process name
          if (procName == cmdLine)
            pid = id;
        }
      }
    }
  }
  closedir(dp);
  return pid;
}

#endif // !_WIN32

} // namespace System

namespace Files {

string__ split_file_name(const char__ *buffer) {
  string__ file_path(buffer);
#ifndef _WIN32
  return file_path.substr(file_path.find_last_of('/') + 1);
#else
  return file_path.substr(file_path.find_last_of('\\') + 1);
#endif
}

string__ split_file_path(const char__ *buffer) {
  string__ file_path;
  string__ full_file_path(buffer);
#ifndef _WIN32
  const int pos = full_file_path.find_last_of(_XPLATSTR("/"));
#else
  const int pos = full_file_path.find_last_of(_XPLATSTR("\\"));
#endif
  if (pos != string__::npos)
    file_path = full_file_path.substr(0, pos + 1);
  else
    file_path = file_path = _XPLATSTR(".");
  return file_path;
}
} // namespace Files

namespace Net {

int send_message(const string__ message) {
  const std::unique_ptr<Message_Sender> message_sender_ =
      std::make_unique<Message_Sender>(ACE_TEXT("127.0.0.1:8005"));
  message_sender_->send(message);
  return 0;
}

} // namespace Net
} // namespace itvpnagent
