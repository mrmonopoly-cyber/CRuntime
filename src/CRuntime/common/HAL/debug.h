#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.


/**
 * \file debug.h
 * \brief entry point used for debugging
 * 
 * \author Alberto Damo
 * \date 2026
 */

#include <CResult.h>

#include <CRuntime/common/errors/errors.h>
#include <stddef.h>

void _todo(const char* const file, const int line, const char* masg);
#define TODO(msg) _todo(__FILE__, __LINE__, "not yet implemented: "msg)

typedef CRESULT_TEMPLATE(void*, CRStatus) CROpenFile;

CRESULT_RETURN(CROpenFile) CR_open_file(const char* path);

CRRETURN CR_write_to_file(void* file, const char* msg, const size_t size);

CRRETURN CR_close_file(void* file);
