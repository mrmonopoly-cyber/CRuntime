#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CSAQ.h
 * 
 * \brief Specialization of base template object CVAQ
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <CRuntime/common/common.h>

#include "CResult.h"
#include "CRuntime/common/errors/errors.h"
#include "CTask.h"

#define  CVAQ_INTERNAL
#include <CRuntime/common/queue/CVAQ/CVAQ.h>
#undef CVAQ_INTERNAL

#if !defined (CSAQ_CAPACITY)
#define CSAQ_CAPACITY_EXP (5)
WARNING("using default CSAQ_CAPACITY")
#endif // !defined (CSAQ_CAPACITY)

#define CSAQ_CAPACITY (1<<CSAQ_CAPACITY_EXP)

typedef CR_ATOMIC_QUEUE_TEMPLATE(CTask, CSAQ_CAPACITY) CSAQ;
typedef CR_QUEUE_POP_TEMPLATE(CTask) CSAQPopRes;

#define CSAQ_PREFIX static inline

/**
 * \brief initialize an already allocated CSAQ
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
CSAQ_PREFIX CRRETURN CSAQ_init(CSAQ* const self)
{
  return _CVAQ_init((CVAQ*)self);
}

/**
 * \brief add a reference of an existing object to CSAQ, by doing so the executor responsible for
 * \brief this particular instance will eventually manage the object
 *
 * @param self pointer to the instance
 * @param object pointer to an already allocated and instantiated object
 *
 * @return see \ref CRReturn in errors.h
 */
CSAQ_PREFIX CRRETURN CSAQ_push_try(CSAQ* const self, CTask* const ele)
{
  return _CVAQ_push_try((CVAQ*)self, CR_ATOMIC_QUEUE_MAX_SIZE(self), ele);
}

/**
 * \brief return a reference to a object, it does not remove the reference from self
 *
 * @param self pointer to the instance
 *
 * @return If Ok the Result contains a valid reference to the object, it cannot be NULL
 * @return If err see \ref CRReturn in errors.h
 *
 */
CSAQ_PREFIX CRESULT_RETURN(CSAQPopRes) CSAQ_pop_try(CSAQ* const self)
{
  CRESULT_FULL_MATCH(_CVAQ_pop_try((CVAQ*)self, CR_ATOMIC_QUEUE_MAX_SIZE(self)),
      res,
      {
       return CRESULT_T_OK(CSAQPopRes, res);
      },
      {
       return CRESULT_T_ERR(CSAQPopRes, res);
      }
  );
}

/**
 * \brief return the amount of object references in self
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
CSAQ_PREFIX size_t CSAQ_size(const CSAQ* const self)
{
  return _CVAQ_size((CVAQ*)self, CR_ATOMIC_QUEUE_MAX_SIZE(self));
}
