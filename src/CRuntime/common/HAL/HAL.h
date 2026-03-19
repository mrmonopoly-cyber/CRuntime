#pragma once

// Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/*
 * \file HAL.h
 *
 * \brief the Hardware Abstraction Layer (HAL) is an interface with all the function which must be
 * \brief implemented to deploy CRuntime on a specific vendor (OS, RTOS, bare-metal,...).
 *
 * \author Alberto Damo
 * \date 2026
 */


#include "channel.h"
#include "debug.h"
#include "context.h"
#include "thread.h"
