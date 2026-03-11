#pragma once

// Copyright (c) 2026  Alberto Damo. All Rights Reserved.

/**
 * \file channel.h
 * \brief Interface for sync and async communication between processes
 * \important The data in the structs should not be modified by the user manually
 *
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <stddef.h>
#include <stdint.h>
#include <stdatomic.h>

#include <CResult.h>

#include <CRuntime/common/errors/errors.h>

typedef enum{
  Channel_ONE_TO_ONE=0,             /*!< 1 reader, 1 writer */
  Channel_ONE_WRITER_MANY_READER,   /*!< 1 writer, N reader */
  Channel_ONE_READER_MANY_WRITER,   /*!< N writer, 1 reader */
  Channel_MANY_TO_MANY,             /*!< N writer, N reader */
}ChannelType;

typedef struct{
  ChannelType type;
  size_t msg_size;
  atomic_size_t writers;
  atomic_size_t readers;
}Channel;

typedef struct {
  const Channel* metadata;
  int _fd;
}ChannelRead;

typedef struct {
  const Channel* metadata;
  int _fd;
}ChannelWrite;

typedef CRESULT_TEMPLATE(Channel, CRStatus) CInitResult;
typedef CRESULT_TEMPLATE(ChannelWrite, CRStatus) CCreateWriteResult;
typedef CRESULT_TEMPLATE(ChannelRead, CRStatus) CCreateReadResult;

CRESULT_RETURN(CInitResult) _Channel_new(const ChannelType type, const size_t msg_size);

/**
 * Create a new channel with a specific type
 * @param c_type type of the channel
 * @param msg_type type of messages used by the buffers
 * @return Result type with the Channel as ok value
 */
#define Channel_new(c_type, msg_type) _Channel_new((c_type), sizeof((msg_type)))

/**
 * Create a read endpoint from an already initialized channel.
 * Increments the channel reader count by one.
 * @param self already initialized channel
 * @return result type with ChannelRead type as ok value
 */
CRESULT_RETURN(CCreateReadResult) ChannelRead_create(Channel* const restrict self);

/**
 * Create a write endpoint from an already initialized channel
 * Increments the channel writer count by one.
 * @param self already initialized channel
 * @return result type with ChannelWrite type as ok value
 */
CRESULT_RETURN(CCreateWriteResult) ChannelWrite_create(Channel* const restrict self);

CRRETURN _ChannelRead_read(
    const ChannelRead* const restrict self,
    void* const buffer,
    const size_t buffer_size);
/**
 * Read a message from an, already initialized, ChannelRead endpoint
 * @param self read channel endpoint to use
 * @param o_msg message in which the data will be stored
 * @return return a CResult type. see \ref CRReturn for more info
 */
#define ChannelRead_read(self, o_msg) \
  _ChannelRead_read((self), &(o_msg), sizeof((o_msg)))

CRRETURN _ChannelWrite_write(
    const ChannelWrite* const restrict self,
    const void* const buffer,
    const size_t buffer_size);

/**
 * Write a message into an, already initialized, ChannelWrite endpoint
 * @param self write channel endpoint to use
 * @param msg message to send. It must be an lvalue object.
 * @return return a CResult type. see \ref CRReturn for more info
 */
#define ChannelWrite_write(self, msg) \
  _ChannelWrite_write((self), &(msg), sizeof((msg)))

/**
 * Close and ChannelRead endpoint.
 * Inform the Channel to decrease the number of reader by 1.
 * @return: return a CResult type. see \ref CRReturn for more info
 */
CRRETURN ChannelRead_close(ChannelRead* const restrict self);

/**
 * Close and ChannelWrite endpoint.
 * Inform the Channel to decrease the number of writer by 1.
 * @return: return a CResult type. see \ref CRReturn for more info
 */
CRRETURN ChannelWrite_close(ChannelWrite* const restrict self);

/**
 * Close the channel if no readers or writers are currently attached.
 * @return: return a CResult type. see \ref CRReturn for more info
 */
CRRETURN Channel_close(Channel* const restrict self);
