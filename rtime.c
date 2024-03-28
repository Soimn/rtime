#include <windows.h>
#include <timeapi.h>
#include <shlwapi.h>
#include <stdio.h>

int
wmain(int argc, WCHAR** argv)
{
  int error_code = 0;

  WCHAR* cmd_line = 0;

  if (argc < 2)
  {
    //// ERROR
    fprintf(stderr, "Illegal arguments. Expected: rtime path/to/executable arguments to pass\n");
    error_code = 1;
  }
  else cmd_line = PathGetArgsW(GetCommandLineW());

  if (cmd_line == 0)
  {
    //// ERROR
    fprintf(stderr, "Failed to parse arguments\n");
    error_code = 1;
  }
  else
  {
    STARTUPINFOW startup_info;
    GetStartupInfoW(&startup_info);

    PROCESS_INFORMATION proc_info;

    if (!CreateProcessW(argv[1], cmd_line, 0, 0, 0, 0, 0, 0, &startup_info, &proc_info))
    {
      //// ERROR
      fprintf(stderr, "Failed to create process\n");
      error_code = 1;
    }
    else
    {
      WaitForSingleObject(proc_info.hProcess, INFINITE);

      FILETIME creation_time, exit_time, kernel_time, user_time;
      BOOL succeded = GetProcessTimes(proc_info.hProcess, &creation_time, &exit_time, &kernel_time, &user_time);

      CloseHandle(proc_info.hProcess);
      CloseHandle(proc_info.hThread);

      if (!succeded)
      {
        //// ERROR
        fprintf(stderr, "Failed to retrieve timing information on process\n");
        error_code = 1;
      }
      else
      {
        ULONG64 creation_ticks = creation_time.dwLowDateTime | ((ULONG64)creation_time.dwHighDateTime << 32);
        ULONG64 exit_ticks     = exit_time.dwLowDateTime     | ((ULONG64)exit_time.dwHighDateTime     << 32);
        ULONG64 real_ticks = exit_ticks - creation_ticks;

        ULONG64 user_ticks = user_time.dwLowDateTime | ((ULONG64)user_time.dwHighDateTime << 32);
        ULONG64 sys_ticks  = kernel_time.dwLowDateTime  | ((ULONG64)kernel_time.dwHighDateTime  << 32);

        ULONG64 ticks_per_min  = 600000000;
        ULONG64 ticks_per_sec  = 10000000;
        ULONG64 ticks_per_msec = 10000;

        printf("real: %llu:%llu.%llu\n", real_ticks/ticks_per_min, (real_ticks/ticks_per_sec)%60, (real_ticks/ticks_per_msec)%1000);
        printf("user: %llu:%llu.%llu\n", user_ticks/ticks_per_min, (user_ticks/ticks_per_sec)%60, (user_ticks/ticks_per_msec)%1000);
        printf("sys: %llu:%llu.%llu\n",  sys_ticks /ticks_per_min, (sys_ticks /ticks_per_sec)%60, (sys_ticks /ticks_per_msec)%1000);
      }
    }
  }

  return error_code;
}
