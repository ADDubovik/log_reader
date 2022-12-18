#include <Windows.h>

const int invalid_args_exit_code = 1;

struct Console
{
    Console()
        : _handle(GetStdHandle(STD_OUTPUT_HANDLE))
    {};

    operator bool() const { return _handle != NULL && _handle != INVALID_HANDLE_VALUE; }
    operator HANDLE() const { return _handle; }

private:
    const HANDLE _handle;
};

int main(int argc, char* argv[])
{
    Console console;

    if (argc != 3)
    {
        if (console)
        {
            DWORD written = 0;
            const auto message = "Usage:\n  log_reader.exe <filename> <pattern>\n";
            WriteConsoleA(console, message, static_cast<DWORD>(strlen(message)), &written, NULL);
        }

        exit(invalid_args_exit_code);
    }
}
