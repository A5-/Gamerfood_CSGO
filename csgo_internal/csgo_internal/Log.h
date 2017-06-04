#pragma once
#include <iostream>
#include <windows.h>

//#define LOG_ENABLED

#ifdef LOG_ENABLED
#define LOG_INIT Log::init()
#define LOG_FREE Log::free()
#define LOG(...) Log::Log(__FUNCTION__, __VA_ARGS__)

class Log
{
public:
    template< typename ... args >
    explicit Log(std::string function, args ... to_print)
    {
        std::cout << '[' << function << "] ";
        print(to_print ...);
    }

    static void init()
    {
        AllocConsole();
        freopen("CON", "w", stdout);
    }

    static void free()
    {
        std::cout << std::endl;
        FreeConsole();
    }

private:
    template< typename T >
    static void print(T only) { std::cout << only << std::endl; }

    template< typename T, typename ... args >
    void print(T current, args ... next)
    {
        std::cout << current << ' ';
        print(next...);
    }
};

#else
#define LOG_INIT
#define LOG_FREE
#define LOG
#endif
