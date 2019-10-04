/**
 * @file dlldecorationrestore.h
 * @brief Windows DLL decoration restore macros
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

#undef SIMPLX_DLL_EXPORT
#ifdef SIMPLX_DLL_EXPORT_SAVE
#define SIMPLX_DLL_EXPORT
#endif

#undef SIMPLX_DLL_IMPORT
#ifdef SIMPLX_DLL_IMPORT_SAVE
#define SIMPLX_DLL_IMPORT
#endif

#undef SIMPLX_DLL_EXPORT_SAVE
#undef SIMPLX_DLL_IMPORT_SAVE