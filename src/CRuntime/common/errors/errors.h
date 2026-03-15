#pragma once

// Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file error.h
 * \brief Error definitions and functions. 
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <stdint.h>

#include <CResult.h>

#define CR_STATUS_OK 0
#define CR_STATUS_ERR_PROCESS 1
#define CR_STATUS_ERR_EMPTY_STACK 2
#define CR_STATUS_ERR_STACK 3
#define CR_STATUS_ERR_EMPTY 4
#define CR_STATUS_ERR_FULL 5
#define CR_STATUS_ERR_UNREACHABLE_CODE 6
#define CR_STATUS_ERR_INVALID_INPUT 7

#define CR_STATUS_ERR_UNKNOWN 99

/**
 * \brief Error type of the library
 * \brief status: category of the error
 * \brief description: description of the error
 */
typedef struct{
  uint16_t status;
  const char* description;
}CRStatus;

/**
 * \brief CRReturn is a specialization of the CResult type.
 * \brief It's used all around the library as error handling and it has different meaning depending
 * \brief on the context.
 * \brief IF Ok: the value can be ignored
 * \brief IF Err: the status indicates the category of the error in CR_STATUS_****
 * \brief         the description indicates with greater details the nature of the error
 */
typedef CRESULT_TEMPLATE(bool, CRStatus) CRReturn;

#define CRRETURN CRESULT_RETURN(CRReturn)

/**
 *\brief construct an Ok result with CR_STATUS_OK
 */
#define OK() CRESULT_T_OK(CRReturn, CR_STATUS_OK)

/**
 *\brief construct an Err result using status and description from the user
 *
 * @param ... : user inputs to construct the error
 */
#define ERR(...) CRESULT_T_ERR(CRReturn, ((CRStatus){__VA_ARGS__}))

/**
 * \brief evaluate the result
 * \brief If there is ar error makes the function return that error
 *
 * @param res result type, it can also be an r-value (call of a function)
 */
#define TRY(res) do{ if(CRESULT_IS_ERR((res))) return res; }while(0)
