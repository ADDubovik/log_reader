#include <Windows.h>

const int invalid_args_exit_code = 1;
const int cannot_open_file_exit_code = 2;

class HandleGuarded
{
public:
    explicit HandleGuarded(HANDLE handle)
        : _handle(handle)
    {}

    ~HandleGuarded()
    {
        CloseHandle(_handle);
    }

    operator bool() const { return _handle != NULL && _handle != INVALID_HANDLE_VALUE; }
    operator HANDLE() const { return _handle; }

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
            _handle,
            message,
            length,
            &written,
            NULL
        );

        return length == written;
    }

private:
    const HANDLE _handle;
};

class Console : public HandleGuarded
{
public:
    Console()
        : HandleGuarded(GetStdHandle(STD_OUTPUT_HANDLE))
    {}
};

class FileToBeRead : public HandleGuarded
{
public:
    FileToBeRead(LPCSTR  file_name)
        : HandleGuarded(
            CreateFileA(
                file_name,             // [in]           LPCSTR                lpFileName, 
                GENERIC_READ,          // [in]           DWORD                 dwDesiredAccess,
                FILE_SHARE_READ,       // [in]           DWORD                 dwShareMode,
                NULL,                  // [in, optional] LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                OPEN_EXISTING,         // [in]           DWORD                 dwCreationDisposition,
                FILE_ATTRIBUTE_NORMAL, // [in]           DWORD                 dwFlagsAndAttributes,
                NULL                   // [in, optional] HANDLE                hTemplateFile
        ))
    {};
};

int main(int argc, char* argv[])
{
    Console console;

    if (argc != 3)
    {
        console.print("Usage:\n  log_reader.exe <filename> <pattern>\n");

        exit(invalid_args_exit_code);
    }

    const auto file_name = argv[1];
    FileToBeRead file(file_name);

    if (!file)
    {
        console.print_multi("Can't open file ", file_name, "\n");

        exit(cannot_open_file_exit_code);
    }
}
