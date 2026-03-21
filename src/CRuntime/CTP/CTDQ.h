#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CTDQ.h
 * 
 * \brief Specialization of base template object CVQ
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <CResult.h>

#include <CRuntime/common/common.h>

#define CVQ_INTERNAL
#include <CRuntime/common/queue/CVQ/CVQ.h>
#undef CVQ_INTERNAL

#include <CRuntime/CScheduler/CTask.h>

#if !defined (CTDQ_CAPACITY)
#define CTDQ_CAPACITY_EXP (3)
WARNING("using default CSAQ_CAPACITY")
#endif // !defined (CSAQ_CAPACITY)

#define CTDQ_CAPACITY (1<<CTDQ_CAPACITY_EXP)

typedef CR_QUEUE_TEMPLATE(CTaskDescription*, CTDQ_CAPACITY) CTDQ;
typedef CRESULT_TEMPLATE(CTaskDescription*, CRStatus) CTDQPopRes;

#define CTDQ_PREFIX static inline

/**
 * \brief initialize an already allocated CTDQ
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
CTDQ_PREFIX CRRETURN CTDQ_init(CTDQ* const self)
{
  return _CVQ_init((CVQ*)self);
}

/**
 * \brief add a reference of an existing object to CTDQ, by doing so the executor responsible for
 * \brief this particular instance will eventually manage the object
 *
 * @param self pointer to the instance
 * @param object pointer to an already allocated and instantiated object
 *
 * @return see \ref CRReturn in errors.h
 */
CTDQ_PREFIX CRRETURN CTDQ_push_try(CTDQ* const self, CTaskDescription* const ele)
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
CTDQ_PREFIX CRESULT_RETURN(CTDQPopRes) CTDQ_pop_try(CTDQ* const self)
{
  CRESULT_FULL_MATCH(_CVQ_pop_try((CVQ*)self, CR_QUEUE_MAX_SIZE(self)),
      res,
      {
        return CRESULT_T_OK(CTDQPopRes, res);
      },
      {
        return CRESULT_T_ERR(CTDQPopRes, res);
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
CTDQ_PREFIX size_t CTDQ_size(const CTDQ* const self)
{
  return _CVQ_size((CVQ*)self, CR_QUEUE_MAX_SIZE(self));
}
