/**
 * @file dlldecorationsave.h
 * @brief Windows DLL decoration save macros
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

#if defined(SIMPLX_DLL_EXPORT_SAVE) || defined(SIMPLX_DLL_IMPORT_SAVE)
#error Illegal definition of SIMPLX_DLL_EXPORT_SAVE and/or SIMPLX_DLL_IMPORT_SAVE
#endif

#ifdef SIMPLX_DLL_EXPORT
#define SIMPLX_DLL_EXPORT_SAVE
#endif

#ifdef SIMPLX_DLL_IMPORT
#define SIMPLX_DLL_IMPORT_SAVE
#endif

#undef SIMPLX_DLL_EXPORT
#undef SIMPLX_DLL_IMPORT