#pragma once

#include <Windows.h>

#include "memory.h"

class Searcher
{
public:
    explicit Searcher(char const* pattern = "");

    Searcher(Searcher&&) = default;
    Searcher(const Searcher&) = delete;

    Searcher& operator=(Searcher&&) = default;
    Searcher& operator=(const Searcher&) = delete;

    struct Line
    {
        ULONG64 start_from = 0;
        ULONG64 past_the_end = 0;
    };

    bool process(
        char const* const chunk,
        const size_t start_from_index,
        const size_t past_the_end_index,
        const size_t chunk_position_in_file,
        Line& line);

    operator bool() const;

private:
    enum class LineStatus
    {
        unknown,
        suitable,
        non_suitable,
    };

private:
    size_t _length;
    Memory _pattern;

    Line _line;
    size_t _pattern_current_index;
    bool _has_roll;
    size_t _pattern_roll_index;
    LineStatus _line_status;
};
