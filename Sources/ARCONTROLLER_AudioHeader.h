/*
  Copyright (C) 2015 Parrot SA

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.
  * Neither the name of Parrot nor the names
  of its contributors may be used to endorse or promote products
  derived from this software without specific prior written
  permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.
*/
#ifndef _ARCONTROLLER_AUDIO_HEADER_H_
#define _ARCONTROLLER_AUDIO_HEADER_H_

#include <inttypes.h>

/*
 * Masks for ARDiscovery JSON "audio_codec" key
 * Legacy can be assumed to be supported even if not included in the bitfield
 */
#define ARAUDIO_CODEC_LEGACY_MASK (1 << 0)
#define ARAUDIO_CODEC_CONTROLLER_APA_MASK (1 << 1)

// Keep this up-to-date, and don't forget the ARAudioFrame.java file !
#define ARAUDIO_CODEC_ALL_SUPPORTED (           \
        ARAUDIO_CODEC_LEGACY_MASK |             \
        ARAUDIO_CODEC_CONTROLLER_APA_MASK       \
        )

/*
 * ARAudio frame header
 */

#define ARCONTROLLER_AUDIO_HEADER_FMT_CODEC_SHIFT (12)
#define ARCONTROLLER_AUDIO_HEADER_FMT_CHANNELS_SHIFT (11)
#define ARCONTROLLER_AUDIO_HEADER_FMT_BASE_RATE_SHIFT (10)
#define ARCONTROLLER_AUDIO_HEADER_FMT_RATE_SHIFT_SHIFT (8)
#define ARCONTROLLER_AUDIO_HEADER_FMT_HAS_FEEDBACK_SHIFT (7)

#define ARCONTROLLER_AUDIO_HEADER_FMT_MONO (0)
#define ARCONTROLLER_AUDIO_HEADER_FMT_STEREO (1)

#define ARCONTROLLER_AUDIO_HEADER_FMT_8000_HZ (0)
#define ARCONTROLLER_AUDIO_HEADER_FMT_11025_HZ (1)

#define ARCONTROLLER_AUDIO_HEADER_FMT_RATE_X1 (0)
#define ARCONTROLLER_AUDIO_HEADER_FMT_RATE_X2 (1)
#define ARCONTROLLER_AUDIO_HEADER_FMT_RATE_X4 (2)
#define ARCONTROLLER_AUDIO_HEADER_FMT_RATE_X8 (3)

typedef enum {
    ARCONTROLLER_AUDIO_CODEC_PCM16LE = 0,

    ARCONTROLLER_AUDIO_CODEC_MAX
} eARCONTROLLER_STREAM_CODEC;

typedef struct {
    uint64_t timestamp; /**< Timestamp in tenth of ms at which the captured audio samples starts. */
    uint16_t sampleFormat; /**< Sample format used in this frame.
                            * 15-12: Codec type (0 for PCM16_LE, interleaved)
                            * 11: Channels number: 0 = mono, 1 = stereo
                            * 10: Sample rate base value. 0 = 8000 Hz, 1 = 11025 Hz
                            * 9-8: Sample rate shift value. sample_rate = base_value << shift_value
                            * 7: Whether the stream has feedback data.
                            * 6-0: Reserved for future use.
                            */
    uint16_t codecData1; /**< Codec-specific data. */
    uint32_t codecData2; /**< Codec-specific data. */
} __attribute__((packed)) ARCONTROLLER_AudioHeader_t;

#define ARCONTROLLER_AUDIO_HEADER_SIZE (sizeof (ARCONTROLLER_AudioHeader_t))
#define ARCONTROLLER_AUDIO_DATA_SIZE 256
#define ARCONTROLLER_AUDIO_FRAME_SIZE ARCONTROLLER_AUDIO_HEADER_SIZE + ARCONTROLLER_AUDIO_DATA_SIZE

#endif /* _ARCONTROLLER_AUDIO_HEADER_H_ */
