#pragma once

#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////

namespace plog
{
    static std::wstring sprintf(const wchar_t* format, ...);
    static std::string  sprintf(const char* format, ...);
}

#define logv( ... ) LOG( plog::verbose ) << plog::sprintf( __VA_ARGS__ )
#define logd( ... ) LOG( plog::debug )   << plog::sprintf( __VA_ARGS__ )
#define logi( ... ) LOG( plog::info )    << plog::sprintf( __VA_ARGS__ )
#define logw( ... ) LOG( plog::warning ) << plog::sprintf( __VA_ARGS__ )
#define loge( ... ) LOG( plog::error )   << plog::sprintf( __VA_ARGS__ )
#define logf( ... ) LOG( plog::fatal )   << plog::sprintf( __VA_ARGS__ )
#define logn( ... ) LOG( plog::none )    << plog::sprintf( __VA_ARGS__ )

//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

namespace plog
{
    std::wstring sprintf(const wchar_t* format, ...)
    {
        va_list args, args_copy;
        va_start(args, format);
        va_copy(args_copy, args);

        int sz = _vsnwprintf(nullptr, 0, format, args);
        size_t max_len = static_cast<size_t>(sz) + 1;
        try {
            std::wstring result(max_len, ' ');
            _vsnwprintf(&result.front(), max_len, format, args_copy);
            va_end(args_copy);
            va_end(args);

            if (result.size() > 2)
            {
                if (result[result.size() - 2] == '\n')
                {
                    result.pop_back(); // extra space for '\0'
                    result.pop_back(); // if contexts ends with '\n'
                }
            }

            return result;
        }
        catch (const std::bad_alloc&) {
            va_end(args_copy);
            va_end(args);
            throw;
        }

        return std::wstring();
    }

    std::string sprintf(const char* format, ...)
    {
        va_list args, args_copy;
        va_start(args, format);
        va_copy(args_copy, args);

        int sz = _vsnprintf(nullptr, 0, format, args);
        size_t max_len = static_cast<size_t>(sz) + 1;
        try {
            std::string result(max_len, ' ');
            _vsnprintf(&result.front(), max_len, format, args_copy);
            va_end(args_copy);
            va_end(args);

            if (result.size() > 2)
            {
                if (result[result.size() - 2] == '\n')
                {
                    result.pop_back(); // extra space for '\0'
                    result.pop_back(); // if contexts ends with '\n'
                }
            }

            return result;
        }
        catch (const std::bad_alloc&) {
            va_end(args_copy);
            va_end(args);
            throw;
        }

        return std::string();
    }
}

#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
