#pragma once

#ifndef SCREEN_H
#define SCREEN_H
#include "common.h"
#include "gdiplus.h"
#include "utils.h"

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

namespace screen {
	int Screenshot(char* folder);
}

#endif

