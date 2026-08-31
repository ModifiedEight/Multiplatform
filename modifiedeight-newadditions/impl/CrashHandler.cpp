#include <CrashHandler.hpp>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <util/Common.hpp>

#ifndef _WIN32
#include <execinfo.h>
#include <unistd.h>
#endif

static std::string g_lastCrash = "";

static void writeCrashReport(const char *sigName, void *addr) {
  std::ostringstream ss;
  time_t now = time(0);
  char timeBuf[64];
  char fileTimeBuf[64];
  strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", localtime(&now));
  strftime(fileTimeBuf, sizeof(fileTimeBuf), "%Y-%m-%d_%H-%M-%S",
           localtime(&now));

  ss << "====================================================\n";
  ss << "                    CRASH REPORT                    \n";
  ss << "====================================================\n";
  ss << "Client Version: ModifiedEight New Additions 1.6.5.1pre1\n";
  ss << "Time: " << timeBuf << "\n";
  ss << "Signal: " << sigName << "\n";
  if (addr) {
    ss << "Fault Address: " << addr << "\n";
  }
  ss << "\n--- STACK TRACE ---\n";

#ifndef _WIN32
  void *callstack[64];
  int frames = backtrace(callstack, 64);
  char **strs = backtrace_symbols(callstack, frames);
  if (strs) {
    for (int i = 0; i < frames; ++i) {
      ss << "#" << i << " " << strs[i] << "\n";
    }
    free(strs);
  } else {
    ss << "(Unable to resolve backtrace symbols)\n";
  }
#endif

  ss << "====================================================\n";

  g_lastCrash = ss.str();

  std::string reportFileName =
      std::string("crash_report_") + fileTimeBuf + ".txt";
  FILE *f = fopen(reportFileName.c_str(), "w");
  if (f) {
    fputs(g_lastCrash.c_str(), f);
    fclose(f);
  }

  fprintf(stderr, "%s\n", g_lastCrash.c_str());

#ifndef _WIN32
  FILE *pipe = popen("xclip -selection clipboard 2>/dev/null", "w");
  if (pipe) {
    fputs(g_lastCrash.c_str(), pipe);
    pclose(pipe);
  }
#endif
}

static void sigHandler(int sig, siginfo_t *info, void *ucontext) {
  const char *name = "UNKNOWN_SIGNAL";
  switch (sig) {
  case SIGSEGV:
    name = "SIGSEGV (Segmentation Fault)";
    break;
  case SIGABRT:
    name = "SIGABRT (Abort)";
    break;
  case SIGFPE:
    name = "SIGFPE (Arithmetic Exception)";
    break;
  case SIGILL:
    name = "SIGILL (Illegal Instruction)";
    break;
  case SIGBUS:
    name = "SIGBUS (Bus Error)";
    break;
  }
  writeCrashReport(name, info ? info->si_addr : 0);
  _exit(sig);
}

void CrashHandler::install() {
#ifndef _WIN32
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = sigHandler;
  sa.sa_flags = SA_SIGINFO;

  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGABRT, &sa, NULL);
  sigaction(SIGFPE, &sa, NULL);
  sigaction(SIGILL, &sa, NULL);
  sigaction(SIGBUS, &sa, NULL);
#endif
}

std::string CrashHandler::getLastCrashReport() {
  if (g_lastCrash.empty()) {
    std::ifstream ifs("crash_report.txt");
    if (ifs.is_open()) {
      std::string content((std::istreambuf_iterator<char>(ifs)),
                          (std::istreambuf_iterator<char>()));
      return content;
    }
  }
  return g_lastCrash;
}

void CrashHandler::triggerReport(const std::string &customError) {
  writeCrashReport(customError.c_str(), 0);
}
