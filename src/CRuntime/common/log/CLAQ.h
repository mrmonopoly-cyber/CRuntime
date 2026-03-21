#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CLAQ.h
 * 
 * \brief Specialization of base template object CVAQ
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <CRuntime/common/errors/errors.h>

#define CVAQ_INTERNAL
#include <CRuntime/common/queue/CVAQ/CVAQ.h>
#undef CVAQ_INTERNAL


#ifndef INPUT_LOG_QUEUE_SIZE
#define  INPUT_LOG_QUEUE_SIZE (16)
#endif // !INPUT_LOG_QUEUE_SIZE

#ifndef MAX_LOG_MSG_LENGTH
#define MAX_LOG_MSG_LENGTH (256)
#endif // !MAX_LOG_MSG_LENGTH

typedef enum{
 Trace=0,
 Debug,
 Info,
 Warning,
 Error,
}CRLogLevel;

typedef struct{
  char msg[MAX_LOG_MSG_LENGTH];
}LogInfo;

typedef CR_ATOMIC_QUEUE_TEMPLATE(LogInfo, INPUT_LOG_QUEUE_SIZE) CLAQ;
typedef CR_QUEUE_POP_TEMPLATE(LogInfo) CLAQPopRes;

#define CLAQ_PREFIX static inline

/**
 * \brief initialize an already allocated CLAQ
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
CLAQ_PREFIX CRRETURN CLAQ_init(CLAQ* const self)
{
  return _CVAQ_init((CVAQ*)self);
}

/**
 * \brief add a reference of an existing object to CLAQ, by doing so the executor responsible for
 * \brief this particular instance will eventually manage the object
 *
 * @param self pointer to the instance
 * @param object pointer to an already allocated and instantiated object
 *
 * @return see \ref CRReturn in errors.h
 */
CLAQ_PREFIX CRRETURN CLAQ_push_try(CLAQ* const self, LogInfo* const ele)
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
CLAQ_PREFIX CRESULT_RETURN(CLAQPopRes) CLAQ_pop_try(CLAQ* const self)
{
  CRESULT_FULL_MATCH(_CVAQ_pop_try((CVAQ*)self, CR_ATOMIC_QUEUE_MAX_SIZE(self)),
      res,
      {
       return CRESULT_T_OK(CLAQPopRes, res);
      },
      {
       return CRESULT_T_ERR(CLAQPopRes, res);
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
CLAQ_PREFIX size_t CLAQ_size(const CLAQ* const self)
{
  return _CVAQ_size((CVAQ*)self, CR_ATOMIC_QUEUE_MAX_SIZE(self));
}
