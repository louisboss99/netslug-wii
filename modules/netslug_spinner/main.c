/* main.c
 *   by Alex Chadwick
 *
 * Copyright (C) 2015, Alex Chadwick
 *
 * Permission  is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in  the Software without restriction, including without limitation the rights
 * to  use,  copy, modify, merge, publish, distribute, sublicense,  and/or  sell
 * copies  of  the  Software,  and to permit persons to  whom  the  Software  is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE  SOFTWARE  IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  OR
 * IMPLIED,  INCLUDING  BUT  NOT LIMITED TO THE WARRANTIES  OF  MERCHANTABILITY,
 * FITNESS  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL  THE
 * AUTHORS  OR  COPYRIGHT  HOLDERS  BE LIABLE FOR ANY CLAIM,  DAMAGES  OR  OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <bslug.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <rvl/dwc.h>
#include <rvl/GXFrameBuf.h>
#include <rvl/vi.h>

/* support any game id */
BSLUG_MODULE_GAME("????");
BSLUG_MODULE_NAME("NetPlay's Progress Light");
BSLUG_MODULE_VERSION("v0.1");
BSLUG_MODULE_AUTHOR("Chadderz");
BSLUG_MODULE_LICENSE("BSD");

/* Replacement for VISetNextFrameBuffer.
 *  Notes down the location of all of the games frame buffers so the console can
 *  be displayed to them. Calls the real VISetNextFrameBuffer. */
static void Spinner_VISetNextFrameBuffer(void *frame_buffer);
/* Replacement for GXSetDispCopyDst.
 *  Sets the copy height for the EFB to XFB copy to half the normal value. Calls
 *  the real GXSetDispCopyDst with half the height parameter. */
static void Spinner_GXSetDispCopyDst(unsigned short width, unsigned short height);
static void My__VIRetraceHandler(int isr, void *context);

BSLUG_MUST_REPLACE(VISetNextFrameBuffer, Spinner_VISetNextFrameBuffer);
BSLUG_MUST_REPLACE(GXSetDispCopyDst, Spinner_GXSetDispCopyDst);
BSLUG_MUST_REPLACE(__VIRetraceHandler, My__VIRetraceHandler);

/* Power of 2 is more efficient. */
#define SPINNER_SIZE 32
#define SPINNER_DOT_SIZE (SPINNER_SIZE / 4)
#define SPINNER_DOT_SEPARATION ((SPINNER_SIZE - SPINNER_DOT_SIZE) / 2)
/* Maximum number of frame buffers a game can use. */
#define MAX_FRAME_BUFFER_COUNT 2
/* Define this as 0 if you don't want a cache flush! */
#define FLUSH_CACHE 1

#define _CPU_FP_Enable( _isr_cookie ) \
  { register unsigned long _disable_mask = 0; \
    _isr_cookie = 0; \
    __asm__ __volatile__ ( \
      "mfmsr %0\n" \
      "ori %1,%0,0x2000\n" \
      "mtmsr %1\n" \
      "extrwi %0,%0,1,18" \
      : "=&r" ((_isr_cookie)), "=&r" ((_disable_mask)) \
      : "0" ((_isr_cookie)), "1" ((_disable_mask)) \
    ); \
  }

#define _CPU_FP_Restore( _isr_cookie )  \
  { register unsigned long _enable_mask = 0; \
    __asm__ __volatile__ ( \
    "    cmpwi %0,0\n" \
    "    bne 1f\n" \
    "    mfmsr %1\n" \
    "    rlwinm %1,%1,0,19,17\n" \
    "    mtmsr %1\n" \
    "1:" \
    : "=r"((_isr_cookie)),"=&r" ((_enable_mask)) \
    : "0"((_isr_cookie)),"1" ((_enable_mask)) \
    ); \
  }

/* Current screen dimensions. */
static int spinner_fb_width;
static int spinner_fb_height;
/* Detected frame buffer. */
static void *spinner_frame_buffers[2];
/* State of the animation. */
static int spinner_state;

static void Spinner_VISetNextFrameBuffer(void *frame_buffer) {
    int i;

    // Note down any frame buffers we haven't seen before.
    for (i = 0; i < MAX_FRAME_BUFFER_COUNT; i++) {
        if (spinner_frame_buffers[i] == NULL) {
            spinner_frame_buffers[i] = frame_buffer;
            break;
        } else if (spinner_frame_buffers[i] == frame_buffer) {
            break;
        }
    }
    /* If we've not seen this frame buffer and we have no  room,  we  reset  all
     * memory of frame buffers. This is here in case a game allocates new  frame
     * buffers part way in. */
    if (i == MAX_FRAME_BUFFER_COUNT) {
        for (i = 0; i < MAX_FRAME_BUFFER_COUNT; i++) {
            spinner_frame_buffers[i] = NULL;
        }
        spinner_frame_buffers[0] = frame_buffer;
    }

    // Game must have advanced, so reset animation.
    spinner_state = 0;

    // Call down to the real VISetNextFrameBuffer.
    VISetNextFrameBuffer(frame_buffer);
}
static void Spinner_GXSetDispCopyDst(
    unsigned short width, unsigned short height
) {
    spinner_fb_width = width;
    spinner_fb_height = height;
    /* call down to the real GXSetDispCopyDst */
    GXSetDispCopyDst(width, height);
}

static inline void Spinner_DimPixelPair(int x, int y) {
    for (int fb = 0; fb < MAX_FRAME_BUFFER_COUNT; fb++) {
        if (!spinner_frame_buffers[fb]) continue;
        unsigned char *pixel_ptr =
            (unsigned char *)spinner_frame_buffers[fb]
                + spinner_fb_width * y * 2
                + x * 2;
        pixel_ptr[0] = pixel_ptr[0] > 64 ? pixel_ptr[0] - 64 : 0;
        pixel_ptr[2] = pixel_ptr[2] > 64 ? pixel_ptr[2] - 64 : 0;
#if FLUSH_CACHE
        asm ("dcbf 0, %0" : : "r"((int)pixel_ptr & ~0x1f));
#endif
    }
}
static inline void Spinner_PutPixelPair(
    int x, int y,
    unsigned char y1, unsigned char y2, unsigned char u, unsigned char v
) {
    for (int fb = 0; fb < MAX_FRAME_BUFFER_COUNT; fb++) {
        if (!spinner_frame_buffers[fb]) continue;
        unsigned char *pixel_ptr =
            (unsigned char *)spinner_frame_buffers[fb]
                + spinner_fb_width * y * 2
                + x * 2;
        pixel_ptr[0] = y1;
        pixel_ptr[1] = u;
        pixel_ptr[2] = y2;
        pixel_ptr[3] = v;
#if FLUSH_CACHE
        asm ("dcbf 0, %0" : : "r"((int)pixel_ptr & ~0x1f));
#endif
    }
}

/* type: Colour of the spinner (0 green, 1 blue, 2 red, 3 magenta). */
static void Spinner_Draw(int state, int type) {
    if (spinner_fb_width < SPINNER_SIZE ||
        spinner_fb_height < SPINNER_SIZE)
        return;
    
    if (state < (SPINNER_SIZE / 2 + SPINNER_DOT_SIZE) * 2) {
        int dx = state * 2;
        int dy = 0;
        const int midx = spinner_fb_width / 2;
        const int midy = spinner_fb_height / 2;
        for (int i = 0; i <= state; i++) {
            if (dx < SPINNER_SIZE / 2 + SPINNER_DOT_SIZE &&
                dy < SPINNER_SIZE / 2 + SPINNER_DOT_SIZE
            ) {
                Spinner_DimPixelPair(midx + dx, midy + dy);
                Spinner_DimPixelPair(midx - dx - 2, midy + dy);
                Spinner_DimPixelPair(midx + dx, midy - dy - 1);
                Spinner_DimPixelPair(midx - dx - 2, midy - dy - 1);
            }
            dx -= 2;
            dy++;
        }
    }

    // Slow the animation down.
    if (state % 2) return;
    int colour_state = (state / 2) % 2;
    int dot_number = (state / 4) % 8;

    if (colour_state)
        dot_number = (dot_number + 4) % 8;

    int start_x = (spinner_fb_width - SPINNER_SIZE) / 2;
    int start_y = (spinner_fb_height - SPINNER_SIZE) / 2;

    // 0 1 2
    // 7   3
    // 6 5 4
    int dot_x = dot_number % 4;
    if (dot_x > 2) dot_x = 2;
    if (dot_number > 3) dot_x = 2 - dot_x;
    int dot_y = dot_number / 2;
    if (dot_number == 2) dot_y = 0;
    if (dot_number > 5) dot_y = 8 - dot_number;

    start_x += dot_x * SPINNER_DOT_SEPARATION;
    start_y += dot_y * SPINNER_DOT_SEPARATION;

    int u = 128 + ((type & 1) ? 48 : -48) * (colour_state ? 2 : 0);
    int v = 128 + ((type & 2) ? 48 : -48) * (colour_state ? 2 : 0);
    for (int x = 0; x < SPINNER_DOT_SIZE; x += 2) {
        int x_y1 = (x == 0 ? 32 : 0);
        int x_y2 = (x + 2 >= SPINNER_DOT_SIZE ? -32 : 0);
        if (x == 0) {
            x_y1 = 31;
            x_y2 = 16;
        }
        if (x + 2 >= SPINNER_DOT_SIZE) {
            x_y1 = -16;
            x_y2 = -31;
        }
        for (int y = 0; y < SPINNER_DOT_SIZE; y++) {
            int y_y = 0;
            if (y < 3)
                y_y += 32 * (3 - y);
            if (y + 3 >= SPINNER_DOT_SIZE)
                y_y -= 32 * (4 - (SPINNER_DOT_SIZE - y));
            Spinner_PutPixelPair(
                start_x + x, start_y + y,
                128 + x_y1 + y_y, 128 + x_y2 + y_y, u, v
            );
        }
    }
}
static int frames = 0;
static int type = 0;
static void My__VIRetraceHandler(int isr, void *context)
{
    int fp_isr;
    _CPU_FP_Enable(fp_isr);

    if (frames++ < 900)
    {
        _CPU_FP_Restore(fp_isr);
        __VIRetraceHandler(isr, context);
    }
    else
    {
        Spinner_Draw(spinner_state++, (type++ / 300) % 4);
        static volatile unsigned short* const _viReg = (volatile unsigned short *)0xCC002000;
        unsigned int intr;
        // WE must clear the interrupt.

        intr = _viReg[24];
        if(intr&0x8000) {
            _viReg[24] = intr&~0x8000;
        }

        intr = _viReg[26];
        if(intr&0x8000) {
            _viReg[26] = intr&~0x8000;
        }

        intr = _viReg[28];
        if(intr&0x8000) {
            _viReg[28] = intr&~0x8000;
        }

        intr = _viReg[30];
        if(intr&0x8000) {
            _viReg[30] = intr&~0x8000;
        }
        _CPU_FP_Restore(fp_isr);
    }
}