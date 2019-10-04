/**
 * @file dlldecoration.h
 * @brief Windows DLL handling macros
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

#if defined(SIMPLX_DLL_EXPORT) && defined(SIMPLX_DLL_IMPORT)
#error Simultaneous definition of SIMPLX_DLL_EXPORT and SIMPLX_DLL_IMPORT
#endif

#ifdef _MSC_VER // MS VISUAL C++ predefined macro
#ifdef SIMPLX_DLL_EXPORT
#undef SIMPLX_DLL
#define SIMPLX_DLL __declspec(dllexport)
#elif defined(SIMPLX_DLL_IMPORT)
#undef SIMPLX_DLL
#define SIMPLX_DLL __declspec(dllimport)
#endif
#else
#undef SIMPLX_DLL
#define SIMPLX_DLL
#endif