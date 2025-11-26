#ifndef TME_EXPORT_HPP
#define TME_EXPORT_HPP

#ifdef _WIN32
#ifdef TME_EXPORTS
#define TME_API __declspec(dllexport)
#else
#define TME_API __declspec(dllimport)
#endif
#else
#define TME_API __attribute__((visibility("default")))
#endif

#endif