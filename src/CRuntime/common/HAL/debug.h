#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.


/**
 * \file debug.h
 * \brief entry point used for debugging
 * 
 * \author Alberto Damo
 * \date 2026
 */

void
_todo(const char* const file, const int line, const char* masg);

#define TODO(msg) _todo(__FILE__, __LINE__, "not yet implemented: "msg)
