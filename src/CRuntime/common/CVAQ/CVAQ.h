#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CSAQueue.h
 *
 * \brief Single producer Single consumer lock-free queue
 * \important IT ONLY STORES POINTERS
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <stddef.h>
#include <stdatomic.h>

#include <CRuntime/common/errors/errors.h>

#define CR_ATOMIC_QUEUE_TEMPLATE(TYPE, MAX_SIZE)\
  struct{\
    atomic_size_t write_cursor;\
    atomic_size_t read_cursor;\
    TYPE list[MAX_SIZE];\
  }

#define CR_ATOMIC_QUEUE_MAX_SIZE(self) (sizeof((self)->list)/sizeof((self)->list[0]))

typedef CR_ATOMIC_QUEUE_TEMPLATE(void*, ) CVAQ;
typedef CRESULT_TEMPLATE(void*, CRStatus) CVAQPopRes;

/**
 * \brief initialize an already allocated CVAQ
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
CRRETURN _CVAQ_init(CVAQ* const self);
#define CVAQ_init(self) _CVAQ_init((CVAQ*)(self))

/**
 * \brief add a reference of an existing object to CVAQ, by doing so the executor responsible for
 * \brief this particular instance will eventually manage the object
 *
 * @param self pointer to the instance
 * @param object pointer to an already allocated and instantiated object
 *
 * @return see \ref CRReturn in errors.h
 */
CRRETURN _CVAQ_push_try(CVAQ* const self, const size_t size, void* const ele);
#define CVAQ_push_try(self, ele) _CVAQ_push_try((CVAQ*)(self), CR_ATOMIC_QUEUE_MAX_SIZE((self)), (ele))

/**
 * \brief return a reference to a object, it does not remove the reference from self
 *
 * @param self pointer to the instance
 *
 * @return If Ok the Result contains a valid reference to the object, it cannot be NULL
 * @return If err see \ref CRReturn in errors.h
 *
 */
CRESULT_RETURN(CVAQPopRes) _CVAQ_pop_try(CVAQ* const self, const size_t size);
#define CVAQ_pop_try(self) _CVAQ_pop_try((CVAQ*)(self), CR_ATOMIC_QUEUE_MAX_SIZE((self)))

/**
 * \brief return the amount of object references in self
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
size_t _CVAQ_size(const CVAQ* const self, const size_t size);
#define CVAQ_size(self) _CVAQ_size((CVAQ*)self, CR_ATOMIC_QUEUE_MAX_SIZE((self)))
