#ifndef TRA_ECS_EXPORT_HPP
#define TRA_ECS_EXPORT_HPP

#ifdef _WIN32
#ifdef TRA_ECS_EXPORTS
#define TRA_ECS_API __declspec(dllexport)
#else
#define TRA_ECS_API __declspec(dllimport)
#endif
#else
#define TRA_ECS_API __attribute__((visibility("default")))
#endif

#endif