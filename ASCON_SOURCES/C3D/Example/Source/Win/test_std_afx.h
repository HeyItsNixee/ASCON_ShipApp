﻿// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers


// Windows Header Files:
#include <windows.h>
#include <commctrl.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// Local Header Files

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#if ( _MSC_VER >= 1400 ) // Использование движка REDWAY
  #ifdef __USE_RED_SDK
   #pragma comment( lib, "REDCore" )
   #pragma comment( lib, "REDValidation" )
   #pragma comment( lib, "Tools" )
  #endif // __USE_RED_SDK
#endif

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
