#ifndef TRA_EXPORT_HPP
#define TRA_EXPORT_HPP

#ifdef _WIN32
#ifdef TRA_EXPORTS
#define TRA_API __declspec(dllexport)
#else
#define TRA_API __declspec(dllimport)
#endif
#else
#define TRA_API __attribute__((visibility("default")))
#endif

#endif