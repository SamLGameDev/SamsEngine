#pragma once
#include <format>
#include <iostream>


template<typename... Args>
void CheckF(
    bool Condition,
    std::format_string<Args...> Format,
    Args&&... args)
{
#if _DEBUG

    if (!Condition)
    {
        std::runtime_error(std::format(Format, std::forward<Args>(args)...) + '\n');
    }

#endif

}
