/*
 * Centaurean Density
 *
 * Copyright (c) 2013, Guillaume Voirin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice, this
 *        list of conditions and the following disclaimer.
 *
 *     2. Redistributions in binary form must reproduce the above copyright notice,
 *        this list of conditions and the following disclaimer in the documentation
 *        and/or other materials provided with the distribution.
 *
 *     3. Neither the name of the copyright holder nor the names of its
 *        contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 30/06/13 11:01
 */

#include "stream.h"
#include "../density_api.h"

DENSITY_WINDOWS_EXPORT const DENSITY_STREAM_STATE density_stream_prepare(density_stream *stream) {
    return DENSITY_STREAM_STATE_READY;
}

DENSITY_WINDOWS_EXPORT const DENSITY_STREAM_STATE density_stream_compress_init(density_stream *const stream, const DENSITY_COMPRESSION_MODE compression_mode) {
    ((density_stream_state *) stream->internal_state)->internal_encode_state.compressionMode = compression_mode;
}

DENSITY_WINDOWS_EXPORT const DENSITY_STREAM_STATE density_stream_compress_continue(density_stream *const restrict stream, const uint8_t *const restrict input_buffer, const uint_fast64_t input_size, uint8_t *const restrict output_buffer, const uint_fast64_t output_size) {
    const uint8_t *in = input_buffer;
    uint8_t *out = output_buffer;

    uint_fast16_t *const temp_available = &((density_stream_state *) stream->internal_state)->available_bytes;
    density_byte *const temporary_buffer = ((density_stream_state *) stream->internal_state)->temporary_buffer;

    switch (((density_stream_state *) stream->internal_state)->internal_encode_state.compressionMode) {
        case DENSITY_COMPRESSION_MODE_CHAMELEON_ALGORITHM: {
            if (temp_available) {
                if ((*temp_available + input_size) > DENSITY_CHAMELEON_MAXIMUM_COMPRESSED_UNIT_SIZE) {
                    const uint_fast64_t fill_size = DENSITY_CHAMELEON_MAXIMUM_COMPRESSED_UNIT_SIZE - *temp_available;
                    DENSITY_MEMCPY(temporary_buffer, in, fill_size);    // position in temporary to check
                    in += fill_size;
                    //density_chameleon_encode(&temporary_buffer, DENSITY_CHAMELEON_MAXIMUM_COMPRESSED_UNIT_SIZE, &out, output_size, false);   // manage return status
                } else {
                    DENSITY_MEMCPY(temporary_buffer, in, input_size);    // position in temporary to check
                    in += input_size;
                    return DENSITY_STREAM_STATE_STALL_ON_INPUT;
                }
            }
            if (input_size < DENSITY_CHAMELEON_MAXIMUM_COMPRESSED_UNIT_SIZE) {   // check input size properly updated in previous branch
                DENSITY_MEMCPY(temporary_buffer, in, input_size);
                *temp_available = input_size;
                return DENSITY_STREAM_STATE_STALL_ON_INPUT;
            } else {
                //density_chameleon_encode(&in, input_size, &out, output_size, false);   // manage return status
                DENSITY_MEMCPY(temporary_buffer, in, input_size - (in - input_buffer));
                in += (input_size - (in - input_buffer));
            }
            break;
        }
        default:
            break;
    }

    /*if (temp_available) {
        // available is < to unit size by definition
        // fill to unit size if possible, if not fill input_size and exit
    } else {
        //if(input_size < unit_size) {
        // fill temp buffer and exit
        //} else {
        // encode temp buffer with restricted coarse encoding
        // encode input_buffer with restricted coarse encoding
        // put the remaining input size in temp buffer and exit
        //}
    }*/
}

DENSITY_WINDOWS_EXPORT const DENSITY_STREAM_STATE density_stream_decompress_continue(density_stream *const restrict stream, const uint8_t *const restrict input_buffer, const uint_fast64_t input_size, uint8_t *const restrict output_buffer, const uint_fast64_t output_size) {
    const uint8_t *in = input_buffer;
    uint8_t *out = output_buffer;

    const uint_fast16_t temp_available = ((density_stream_state *) stream->internal_state)->available_bytes;

    if (temp_available) {
        // available is < to unit size by definition
        // fill to max compressed unit size if possible, if not fill input_size and exit
    } else {
        //if(input_size < max_compressed_unit_size) {
        // fill temp buffer and exit
        //} else {
        // decode temp buffer with restricted coarse decoding
        // Mark the position at which we can start using input_buffer again
        // decode input_buffer with restricted coarse decoding at the new position
        // put the remaining input size in temp buffer and exit
        //}
    }
}
