#pragma once

#include <CResult.h>

#include "CVAQ_common.h"

typedef CR_ATOMIC_QUEUE_TEMPLATE(void, ) CVAQ;
typedef CR_QUEUE_POP_TEMPLATE(void) CVAQPopRes;

/**
 * \brief initialize an already allocated CVAQ
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
CRRETURN _CVAQ_init(CVAQ* const self);

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

/**
 * \brief return the amount of object references in self
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
size_t _CVAQ_size(const CVAQ* const self, const size_t size);
