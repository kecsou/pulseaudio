/***
  This file is part of PulseAudio.

  Copyright 2019 Pali Rohár <pali.rohar@gmail.com>

  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.

  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with PulseAudio; if not, see <http://www.gnu.org/licenses/>.
***/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <pulsecore/core.h>
#include <pulsecore/core-util.h>
#if defined(HAVE_GSTAPTX) || defined(HAVE_GSTLDAC)
#include <gst/gst.h>
#endif

#include "a2dp-codec-util.h"

extern const pa_a2dp_codec pa_a2dp_codec_sbc;
#ifdef HAVE_GSTAPTX
extern const pa_a2dp_codec pa_a2dp_codec_aptx;
extern const pa_a2dp_codec pa_a2dp_codec_aptx_hd;
#endif
#ifdef HAVE_GSTLDAC
extern const pa_a2dp_codec pa_a2dp_codec_ldac_eqmid_hq;
extern const pa_a2dp_codec pa_a2dp_codec_ldac_eqmid_sq;
extern const pa_a2dp_codec pa_a2dp_codec_ldac_eqmid_mq;
#endif

/* This is list of supported codecs. Their order is important.
 * Codec with lower index has higher priority. */
static const pa_a2dp_codec *pa_a2dp_codecs[] = {
#ifdef HAVE_GSTLDAC
    &pa_a2dp_codec_ldac_eqmid_hq,
    &pa_a2dp_codec_ldac_eqmid_sq,
    &pa_a2dp_codec_ldac_eqmid_mq,
#endif
#ifdef HAVE_GSTAPTX
    &pa_a2dp_codec_aptx_hd,
    &pa_a2dp_codec_aptx,
#endif
    &pa_a2dp_codec_sbc,
};

unsigned int pa_bluetooth_a2dp_codec_count(void) {
    return PA_ELEMENTSOF(pa_a2dp_codecs);
}

const pa_a2dp_codec *pa_bluetooth_a2dp_codec_iter(unsigned int i) {
    pa_assert(i < pa_bluetooth_a2dp_codec_count());
    return pa_a2dp_codecs[i];
}

const pa_a2dp_codec *pa_bluetooth_get_a2dp_codec(const char *name) {
    unsigned int i;
    unsigned int count = pa_bluetooth_a2dp_codec_count();

    for (i = 0; i < count; i++) {
        if (pa_streq(pa_a2dp_codecs[i]->name, name))
            return pa_a2dp_codecs[i];
    }

    return NULL;
}

void pa_bluetooth_a2dp_codec_gst_init(void) {
#if defined(HAVE_GSTAPTX) || defined(HAVE_GSTLDAC)
    GError *error = NULL;

    if (!gst_init_check(NULL, NULL, &error)) {
        pa_log_error("Could not initialise GStreamer: %s", error->message);
        g_error_free(error);
        return;
    }
    pa_log_info("GStreamer initialisation done");
#endif
}

bool pa_bluetooth_a2dp_codec_is_available(const pa_a2dp_codec_id *id, bool is_a2dp_sink) {
    unsigned int i;
    unsigned int count = pa_bluetooth_a2dp_codec_count();
    const pa_a2dp_codec *a2dp_codec;

    for (i = 0; i < count; i++) {
        a2dp_codec = pa_bluetooth_a2dp_codec_iter(i);
        if (memcmp(id, &a2dp_codec->id, sizeof(pa_a2dp_codec_id)) == 0
                && a2dp_codec->can_be_supported(is_a2dp_sink))
            return true;
    }

    return false;
}
