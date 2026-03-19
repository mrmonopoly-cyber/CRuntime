#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CSAQueue.h
 *
 * \brief Single producer Single consumer queue
 * \important IT ONLY STORES POINTERS
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <stddef.h>

#include <CRuntime/common/errors/errors.h>

#define CR_QUEUE_TEMPLATE(TYPE, MAX_SIZE)\
  struct{\
    size_t write_cursor;\
    size_t read_cursor;\
    TYPE list[MAX_SIZE];\
  }

#define CR_QUEUE_MAX_SIZE(self) (sizeof((self)->list)/sizeof((self)->list[0]))


typedef CR_QUEUE_TEMPLATE(void*, ) CVQ;
typedef CRESULT_TEMPLATE(void*, CRStatus) CVQPopRes;

/**
 * \brief initialize an already allocated CVQ
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
CRRETURN _CVQ_init(CVQ* const self);
#define CVQ_init(self) _CVQ_init((CVQ*)(self))

/**
 * \brief add a reference of an existing object to CVQ, by doing so the executor responsible for
 * \brief this particular instance will eventually manage the object
 *
 * @param self pointer to the instance
 * @param object pointer to an already allocated and instantiated object
 *
 * @return see \ref CRReturn in errors.h
 */
CRRETURN _CVQ_push_try(CVQ* const self, const size_t size, void* const ele);
#define CVQ_push_try(self, ele) _CVQ_push_try((CVQ*)(self), CR_QUEUE_MAX_SIZE((self)), (ele))

/**
 * \brief return a reference to a object, it does not remove the reference from self
 *
 * @param self pointer to the instance
 *
 * @return If Ok the Result contains a valid reference to the object, it cannot be NULL
 * @return If err see \ref CRReturn in errors.h
 *
 */
CRESULT_RETURN(CVQPopRes) _CVQ_pop_try(CVQ* const self, const size_t size);
#define CVQ_pop_try(self) _CVQ_pop_try((CVQ*)(self), CR_QUEUE_MAX_SIZE((self)))

/**
 * \brief return the amount of object references in self
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
size_t _CVQ_size(const CVQ* const self, const size_t size);
#define CVQ_size(self) _CVQ_size((CVQ*)self, CR_QUEUE_MAX_SIZE((self)));
