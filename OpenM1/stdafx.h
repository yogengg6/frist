// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include "StdString.h"
#include "StringSlots.h"

using namespace std;
#include <list>

#include "boost/dynamic_bitset/dynamic_bitset.hpp"

#include "OpenM1.h"
#include "M1Standard.h"
#include "M1Section.h"
#include "M1Image.h"
#include "Resource.h"
#include "FreeImage.h"

#define M1_EXCEPTION_TRY \
	try \
	{

#define M1_EXCEPTION_CATCH \
	} \
	catch (...) \
	{ \
		return M1_EXCEPTION; \
	}
