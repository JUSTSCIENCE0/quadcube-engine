// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#if defined(_MSC_VER)
#  ifdef quadcube_engine_EXPORTS
#    define QCE_API extern "C" __declspec(dllexport)
#  else
#    define QCE_API extern "C" __declspec(dllimport)
#  endif
#elif defined(__GNUC__) || defined(__clang__)
# define  QCE_API // TODO
#else
# error "Unsupported compiler"
#endif

