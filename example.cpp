#include <Windows.h>

#include <iostream>

#include "handle_guarded.h"

#include "searcher.h"

#include "log_reader.h"

namespace
{
    const int invalid_args_exit_code = 1;
    const int cannot_set_filter_exit_code = 2;
    const int cannot_open_file_exit_code = 3;

    constexpr size_t buf_size = 32;

    template<size_t N>
    size_t line_length(const char (&buf)[N])
    {
        for (size_t i = 0; i < N; ++i)
        {
            if ('\x0a' == buf[i])
            {
                return i + 1;
            }
        }

        return N;
    }
} // namespace

class Console : public HandleGuarded
{
public:
    Console()
        : HandleGuarded(GetStdHandle(STD_OUTPUT_HANDLE))
    {}

    template<typename ... Args_t>
    bool print_multi(Args_t ...);

    template<typename Arg1_t, typename ... Rest_t>
    bool print_multi(Arg1_t arg1, Rest_t ... rest)
    {
        return print(arg1) && print_multi(rest ...);
    }

    template<typename Arg1_t>
    bool print_multi(Arg1_t arg1)
    {
        return print(arg1);
    }

    bool print(char const* message)
    {
        if (!*this)
        {
            return false;
        }

        const auto length = static_cast<DWORD>(strlen(message));
        DWORD written = 0;
        WriteConsoleA(
            *this,
            message,
            length,
            &written,
            NULL
        );

        return length == written;
    }
};

int main(int argc, char* argv[])
{
    //{
    //    {
    //        Searcher searcher;
    //        std::cout
    //            << std::boolalpha << searcher << std::endl;
    //    }
    //    Searcher searcher;
    //    searcher = Searcher("*abc*");
    //
    //    const auto chunk = "abgc\nlksadhgklfabpweuf\nityier3abcyrfdwueiacs\nlkqwhfutreiqwjd453\nabcfde\nabgc\nabchiuh\n";
    //    const auto str_length = strlen(chunk);
    //    const auto file_offset = 1000;
    //
    //    Searcher::Line line;
    //    size_t start_from_index = 0;
    //
    //    do
    //    {
    //        const bool process_result = searcher.process(
    //            chunk,
    //            start_from_index,
    //            str_length,
    //            file_offset,
    //            line
    //        );
    //
    //        std::cout
    //            << std::boolalpha << process_result << " " << line.start_from << " " << line.past_the_end << std::endl;
    //
    //        start_from_index = line.past_the_end - file_offset;
    //    }
    //    while (start_from_index != str_length);
    //
    //    return 0;
    //}

    Console console;

    if (argc != 3)
    {
        console.print("Usage:\n  log_reader.exe <filename> <pattern>\n");

        exit(invalid_args_exit_code);
    }

    CLogReader log_reader;

    const auto filter = argv[2];

    if (!log_reader.SetFilter(filter))
    {
        console.print("Can't set filter\n");

        exit(cannot_set_filter_exit_code);
    }

    const auto file_name = argv[1];

    if (!log_reader.Open(file_name))
    {
        console.print_multi("Can't open file ", file_name, "\n");

        exit(cannot_open_file_exit_code);
    }

    const auto out_file_name = "out.txt";
    HandleGuarded out_file{
        CreateFileA(
            out_file_name,         // [in]           LPCSTR                lpFileName, 
            GENERIC_WRITE,         // [in]           DWORD                 dwDesiredAccess,
            0,                     // [in]           DWORD                 dwShareMode,
            NULL,                  // [in, optional] LPSECURITY_ATTRIBUTES lpSecurityAttributes,
            CREATE_ALWAYS,         // [in]           DWORD                 dwCreationDisposition,
            FILE_ATTRIBUTE_NORMAL, // [in]           DWORD                 dwFlagsAndAttributes,
            NULL                   // [in, optional] HANDLE                hTemplateFile
        )
    };

    char buf[buf_size];
    while (log_reader.GetLineNext(&buf[0], buf_size))
    {
        const auto line_length = ::line_length(buf);
        const auto bytes_to_write = line_length < buf_size
            ? line_length
            : buf_size;

        DWORD bytes_written;

        WriteFile(
            out_file,                               // [in]                HANDLE       hFile,
            &buf[0],                                // [in]                LPCVOID      lpBuffer,
            static_cast<DWORD>(bytes_to_write),     // [in]                DWORD        nNumberOfBytesToWrite,
            &bytes_written,                         // [out, optional]     LPDWORD      lpNumberOfBytesWritten,
            NULL                                    // [in, out, optional] LPOVERLAPPED lpOverlapped
        );

        if (bytes_to_write != bytes_written)
        {
            console.print_multi("Can't write file\n");

            return -1;
        }
    }
}
