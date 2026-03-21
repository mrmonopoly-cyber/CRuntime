#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CSQ.h
 * 
 * \brief Specialization of base template object CVQ
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <CResult.h>

#include <CRuntime/common/common.h>

#define CVQ_INTERNAL
#include "CRuntime/common/queue/CVQ/CVQ.h"
#undef CVQ_INTERNAL

#include "CTask.h"

#if !defined (CSQ_CAPACITY)
#define CSQ_CAPACITY_EXP (5)
WARNING("using default CSAQ_CAPACITY")
#endif // !defined (CSAQ_CAPACITY)

#define CSQ_CAPACITY (1<<CSQ_CAPACITY_EXP)

typedef CR_QUEUE_TEMPLATE(CTask, CSQ_CAPACITY) CSQ;
typedef CR_QUEUE_POP_TEMPLATE(CTask) CSQPopRes;

#define CSQ_PREFIX static inline

/**
 * \brief initialize an already allocated CSQ
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
CSQ_PREFIX CRRETURN CSQ_init(CSQ* const self)
{
  return _CVQ_init((CVQ*)self);
}

/**
 * \brief add a reference of an existing object to CSQ, by doing so the executor responsible for
 * \brief this particular instance will eventually manage the object
 *
 * @param self pointer to the instance
 * @param object pointer to an already allocated and instantiated object
 *
 * @return see \ref CRReturn in errors.h
 */
CSQ_PREFIX CRRETURN CSQ_push_try(CSQ* const self, CTask* const ele)
{
  return _CVQ_push_try((CVQ*)self, CR_QUEUE_MAX_SIZE(self), ele);
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
CSQ_PREFIX CRESULT_RETURN(CSQPopRes) CSQ_pop_try(CSQ* const self)
{
  CRESULT_FULL_MATCH(_CVQ_pop_try((CVQ*)self, CR_QUEUE_MAX_SIZE(self)),
      res,
      {
        return CRESULT_T_OK(CSQPopRes, res);
      },
      {
        return CRESULT_T_ERR(CSQPopRes, res);
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
CSQ_PREFIX size_t CSQ_size(const CSQ* const self)
{
  return _CVQ_size((CVQ*)self, CR_QUEUE_MAX_SIZE(self));
}
