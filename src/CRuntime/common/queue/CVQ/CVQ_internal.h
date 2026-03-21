#pragma once

#include <stddef.h>

#include <CRuntime/common/errors/errors.h>

#include "CVQ_common.h"

typedef CR_QUEUE_TEMPLATE(void, ) CVQ;
typedef CR_QUEUE_POP_TEMPLATE(void) CVQPopRes;

/**
 * \brief initialize an already allocated CVQ
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
CRRETURN _CVQ_init(CVQ* const self);

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

/**
 * \brief return the amount of object references in self
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
size_t _CVQ_size(const CVQ* const self, const size_t size);
