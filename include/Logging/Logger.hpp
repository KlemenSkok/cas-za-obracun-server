
#pragma once

#include "Config.hpp"

#ifdef SPDLOG
#define SPDLOG_HEADER_ONLY
#include <spdlog/spdlog.h>
#else
#include <iostream>
#endif


class Logger {
public:
    // support (almost) all spdlog overloads

    // single argument overloads
    template<typename T>
    static void info(const T* msg) {
#ifdef SPDLOG
        spdlog::info(msg);
#else
        std::cout << "[info] " << msg << '\n';
#endif
    }
        
    template<typename T>
    static void error(const T* msg) {
#ifdef SPDLOG
        spdlog::error(msg);
#else
        std::cout << "[error] " << msg << '\n';
#endif
    }

    template<typename T>
    static void warn(const T* msg) {
#ifdef SPDLOG
        spdlog::warn(msg);
#else
        std::cout << "[warning] " << msg << '\n';
#endif
    }

    template<typename T>
    static void debug(const T* msg) {
#ifdef SPDLOG
        spdlog::debug(msg);
#else
        std::cout << "[debug] " << msg << '\n';
#endif
    }


    // variable arguments oveloads (only for spdlog)
    // if spdlog is not defined, only the first argument will be printed
    template<typename... Args>
    static void info(const char* fmt, const Args&... args) {
#ifdef SPDLOG
        spdlog::info(fmt, args...);
#else
        std::cout << "[info] " << fmt << '\n';
#endif
    }

    template<typename... Args>
    static void error(const char* fmt, const Args&... args) {
#ifdef SPDLOG
        spdlog::error(fmt, args...);
#else
        std::cout << "[error] " << fmt << '\n';
#endif
    }

    template<typename... Args>
    static void warn(const char* fmt, const Args&... args) {
#ifdef SPDLOG
        spdlog::warn(fmt, args...);
#else
        std::cout << "[warning] " << fmt << '\n';
#endif
    }

    template<typename... Args>
    static void debug(const char* fmt, const Args&... args) {
#ifdef SPDLOG
        spdlog::debug(fmt, args...);
#else
        std::cout << "[debug] " << fmt << '\n';
#endif
    }
    
    
};