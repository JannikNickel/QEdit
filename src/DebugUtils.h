#pragma once
#include <debugapi.h>

#define OutputDebugFloat(value) CString s; \
								s.Format(_T("%f\n"), (value)); \
								OutputDebugString(s);

#define OutputDebugInt(value) CString s; \
							  s.Format(_T("%d\n"), (value)); \
							  OutputDebugString(s);
