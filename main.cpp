/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision: 7107 $
 * $Id: main.cpp 7107 2011-04-15 11:19:11Z mortenmacfly $
 * $HeadURL: http://svn.code.sf.net/p/codeblocks/code/branches/release-17.xx/src/tools/ConsoleRunner/main.cpp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#ifdef __WXMSW__
    #include <windows.h>
    #include <conio.h>
    #define wait_key getch
#else
    #define wait_key getchar
#endif
#if defined(__unix__) || defined(__unix)
    #include <sys/wait.h>
#endif
#include <string.h>

#ifdef __MINGW32__
int _CRT_glob = 0;
#endif

bool hasSpaces(const char* str)
{
    char last = 0;
    while (str && *str)
    {
        if ((*str == ' ' || *str == '\t') && last != '\\')
            return true;
        last = *str++;
    }
    return false;
}

int execute_command(char *cmdline)
{
#ifdef __WXMSW__
    //Windows's system() seems to not be able to handle parentheses in
    //the path, so we have to launch the program a different way.

    SetConsoleTitle(cmdline);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process.
    CreateProcess( NULL, TEXT(cmdline), NULL, NULL, FALSE, 0,
                   NULL, NULL, &si, &pi );

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Get the return value of the child process
    DWORD ret;
    GetExitCodeProcess( pi.hProcess, &ret );

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    return ret;
#else
    int ret = system(cmdline);
    if(WIFEXITED(ret))
    {
        return WEXITSTATUS(ret);
    }
    else
    {
        return -1;
    }
#endif
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: cb_console_runner <filename> <args ...>\n");
        return 1;
    }

    // count size of arguments
    size_t fullsize = 0;
    for (int i = 1; i < argc; ++i)
    {
        fullsize += strlen(argv[i]);
    }
    // add some slack for spaces between args plus quotes around executable
    fullsize += argc + 32;

    char* cmdline = new char[fullsize];
    memset(cmdline, 0, fullsize);

    // If the first argument is not LD_LIBRARY_PATH, then executable is the
    // first argument
    bool is_exec_first = strstr(argv[1], "LD_LIBRARY_PATH") == nullptr;
    int mpi_idx = -1;
    int num_proc = -1;
    bool use_mpi = false;
    for (int i = 2; i < argc; ++i)
    {
        // Check that user want to run the program with mpi
        if (strcmp(argv[i], "-mpi") == 0)
        {
            mpi_idx = i;
            break;
        }
    }
    // Check that there is enough arguments for us to check if
    // the mpi flags are in the right format:
    // -mpi </path/to/mpiexec> -n <number of processes>
    if (mpi_idx > -1 && mpi_idx + 3 < argc)
    {
        num_proc = atoi(argv[mpi_idx + 3]);
        // Accept both -n and -np as flags for number of processes
        if ((strcmp(argv[mpi_idx + 2], "-n") == 0 ||
                strcmp(argv[mpi_idx + 2], "-np") == 0) && num_proc > 1)
            use_mpi = true;
    }

    // Append mpiexec commands to cmdline
    if (use_mpi && is_exec_first)
    {
        strcat(cmdline, argv[mpi_idx + 1]);
        strcat(cmdline, " ");
        strcat(cmdline, argv[mpi_idx + 2]);
        sprintf(cmdline, "%s %d ", cmdline, num_proc);
    }

    // 1st arg (executable) enclosed in quotes to support filenames with spaces
    bool sp = hasSpaces(argv[1]);
    if (sp)
        strcat(cmdline, "\"");
    strcat(cmdline, argv[1]);
    if (sp)
        strcat(cmdline, "\"");
    strcat(cmdline, " ");

    // Append mpiexec commands to cmdline if program executable isn't the
    // first argument
    if (use_mpi && !is_exec_first)
    {
        strcat(cmdline, argv[mpi_idx + 1]);
        strcat(cmdline, " ");
        strcat(cmdline, argv[mpi_idx + 2]);
        sprintf(cmdline, "%s %d ", cmdline, num_proc);
    }

    for (int i = 2; i < argc; ++i)
    {
        // Skip the MPI setup parameters so we don't have to worry about
        // running out of storage
        if (!use_mpi || i < mpi_idx || i > mpi_idx + 3)
        {
            sp = hasSpaces(argv[i]);
            if (sp)
                strcat(cmdline, "\"");
            strcat(cmdline, argv[i]);
            if (sp)
                strcat(cmdline, "\"");
            strcat(cmdline, " ");
        }
    }

    timeval tv;
    gettimeofday(&tv, NULL);
    double cl = tv.tv_sec + (double)tv.tv_usec / 1000000;

    int ret = execute_command(cmdline);

    gettimeofday(&tv, NULL);
    cl = (tv.tv_sec + (double)tv.tv_usec / 1000000) - cl;

    printf("\nProcess returned %d (0x%X)   execution time : %0.3f s", ret, ret, cl);
    printf
    (
        "\nPress "
#ifdef __WXMSW__
        "any key"
#else
        "ENTER"
#endif
        " to continue.\n"
    );

    wait_key();

    delete[] cmdline;
    return ret;
}
