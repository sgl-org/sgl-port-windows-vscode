/* source/port/sgl_port.c
 *
 * MIT License
 *
 * Copyright(c) 2023-present All contributors of SGL  
 * Document reference link: docs directory
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sgl.h>

/* Panel resolution. Override from the build system (-DCONFIG_SGL_PANEL_WIDTH=...)
 * to check that the demo (and the benchmark) adapt to other screen sizes. */
#ifndef  CONFIG_SGL_PANEL_WIDTH
#define  CONFIG_SGL_PANEL_WIDTH         800
#endif
#ifndef  CONFIG_SGL_PANEL_HEIGHT
#define  CONFIG_SGL_PANEL_HEIGHT        480
#endif
#ifndef  CONFIG_SGL_PANEL_BUFFER_LINE
#define  CONFIG_SGL_PANEL_BUFFER_LINE   10
#endif

static SDL_Renderer * m_renderer = NULL;

/* streaming texture created once, updated + presented inside panel_flush_area */
static SDL_Texture * m_texture = NULL;

typedef struct sgl_port_sdl2 {
    SDL_Window    *m_window;
    SDL_TimerID   systick;
    SDL_TimerID   anim_systick;
    uint32_t      frame_count;
} sgl_port_sdl2_t;

/* device instance, kept for the frame_count (FPS) bookkeeping in flush */
static sgl_port_sdl2_t * m_dev = NULL;

/* present coalescing during a WARP transition: cap the real SDL present rate
 * so hundreds of per-row pushes a frame collapse into ~60 uploads a second */
#define  SGL_PRESENT_MIN_MS       15u
static uint32_t          s_present_last = 0;

#if (CONFIG_SGL_FBDEV_PIXEL_DEPTH == 32)
#define  SDL_PIXEL_FORMAT       SDL_PIXELFORMAT_ARGB8888
#elif (CONFIG_SGL_FBDEV_PIXEL_DEPTH == 24)
#define  SDL_PIXEL_FORMAT       SDL_PIXELFORMAT_BGR24
#elif (CONFIG_SGL_FBDEV_PIXEL_DEPTH == 16)
#define  SDL_PIXEL_FORMAT       SDL_PIXELFORMAT_RGB565
#elif (CONFIG_SGL_FBDEV_PIXEL_DEPTH == 8)
#define  SDL_PIXEL_FORMAT       SDL_PIXELFORMAT_RGB332
#endif



sgl_color_t sdl2_frame_buffer[CONFIG_SGL_PANEL_WIDTH * CONFIG_SGL_PANEL_HEIGHT] = {0};


static int sdl_create_windows(SDL_Window **m_window, SDL_Renderer **m_renderer, const char *title)
{
    if ( SDL_CreateWindowAndRenderer( CONFIG_SGL_PANEL_WIDTH, CONFIG_SGL_PANEL_HEIGHT, SDL_WINDOW_SHOWN,
                                      &(*m_window), &(*m_renderer )) != 0 ) {
        return -1;
    }

    SDL_SetWindowTitle(*m_window, title);

    // Clear the window with a SGL_COLOR_WHITE background
    for(int i = 0; i< CONFIG_SGL_PANEL_WIDTH * CONFIG_SGL_PANEL_HEIGHT; i++) {
        sdl2_frame_buffer[i] = SGL_COLOR_WHITE;
    }

    SDL_SetRenderDrawColor( *m_renderer, 0, 0, 0, 255);
    SDL_RenderClear( *m_renderer );

    if (m_texture != NULL) {
        SDL_DestroyTexture(m_texture);
        m_texture = NULL;
    }
    m_texture = SDL_CreateTexture(*m_renderer, SDL_PIXEL_FORMAT, SDL_TEXTUREACCESS_STREAMING,
                                  CONFIG_SGL_PANEL_WIDTH, CONFIG_SGL_PANEL_HEIGHT);
    if (m_texture == NULL) {
        return -1;
    }
    return 0;
}



static uint32_t system_tick(uint32_t interval, void *param)
{
    sgl_mm_monitor_t mm = sgl_mm_get_monitor();
    sgl_port_sdl2_t *sdl2_dev = (sgl_port_sdl2_t*)param;
    SGL_UNUSED(mm);
    //printf("SGL SDL2 Frame = %d\n", sdl2_dev->frame_count);
    //printf("Memory: total: %d used: %d, free = %d\n", mm.total_size, mm.used_size, mm.free_size);
    sdl2_dev->frame_count = 0;
	return interval;
}


static uint32_t anim_systick(uint32_t interval, void *param)
{
    SGL_UNUSED(param);
    sgl_tick_inc(1);
    return interval;
}


static int mouse_event_interrupt(void *userdata, SDL_Event *event) 
{
    static bool mouse_press = false;
    bool pressed = false;
    SGL_UNUSED(userdata);

    sgl_event_pos_t pos;
    switch (event->type)
    {
    case SDL_MOUSEBUTTONDOWN:
        mouse_press = true;
        pos.x = event->button.x;
        pos.y = event->button.y;
        pressed = true;
        sgl_event_pos_input(pos.x, pos.y, pressed);
        break;

    case SDL_MOUSEBUTTONUP:
        pos.x = event->button.x;
        pos.y = event->button.y;
        mouse_press = false;
        sgl_event_pos_input(pos.x, pos.y, pressed);
        break;

    case SDL_MOUSEMOTION:
        if(mouse_press) {
            pos.x = event->motion.x;
            pos.y = event->motion.y;
            sgl_event_pos_input(pos.x, pos.y, true);
        }
        break;

    default: break;
    }

    return 0;
}


static void panel_flush_area(sgl_area_t *area, sgl_color_t *src)
{
    sgl_color_t *dest = sdl2_frame_buffer;
    int16_t w = area->x2 - area->x1 + 1;
    int16_t h = area->y2 - area->y1 + 1;
    dest += (area->x1 + area->y1 * CONFIG_SGL_PANEL_WIDTH);

    for(int i = 0; i < h; i ++) {
        memcpy(dest, src, w * sizeof(sgl_color_t));
        dest += CONFIG_SGL_PANEL_WIDTH;
        src += w;
    }

    /* flush == 刷屏：上传脏矩形后立即 RenderCopy+RenderPresent，
     * 渲染节奏由 sgl_task_handler 的 tick 限制把关，无独立 present 步骤 */
    if (m_texture != NULL && m_renderer != NULL) {
        /* Upload just the dirty rect every flush, but cap the real
         * RenderCopy+Present rate: a full-screen (or transition) refresh issues
         * one push per band (dozens a frame), and presenting each one hammers the
         * SDL swap chain -- a simulator-only cost (a real panel just DMA-pushes
         * each band). Pace is governed by sgl_task_handler's tick, not present. */
        SDL_Rect r = { area->x1, area->y1, w, h };
        SDL_UpdateTexture(m_texture, &r,
                          sdl2_frame_buffer + area->y1 * CONFIG_SGL_PANEL_WIDTH + area->x1,
                          CONFIG_SGL_PANEL_WIDTH * sizeof(sgl_color_t));
        uint32_t now = SDL_GetTicks();
        /* present on the bottom band (completes a top-to-bottom full refresh) or
         * whenever the throttle window elapsed, so the composited frame is shown */
        int last_band = (area->y2 >= (CONFIG_SGL_PANEL_HEIGHT - 1));
        if (last_band || (now - s_present_last) >= SGL_PRESENT_MIN_MS) {
            SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
            SDL_RenderPresent(m_renderer);
            s_present_last = now;
            if (m_dev != NULL) {
                m_dev->frame_count++;
            }
        }
    }

    sgl_fbdev_flush_ready();
}


static sgl_color_t panel_buffer0[CONFIG_SGL_PANEL_WIDTH * CONFIG_SGL_PANEL_BUFFER_LINE] = {0};
static sgl_color_t panel_buffer1[CONFIG_SGL_PANEL_WIDTH * CONFIG_SGL_PANEL_BUFFER_LINE] = {0};

void log_stdout(const char *str)
{
    printf(str);
    fflush(stdout);
}


sgl_port_sdl2_t* sgl_port_sdl2_init(void)
{
    sgl_port_sdl2_t *sdl2_dev = NULL;

    sgl_fbinfo_t fbinfo = {
        .xres = CONFIG_SGL_PANEL_WIDTH,
        .yres = CONFIG_SGL_PANEL_HEIGHT,
        .flush_area = panel_flush_area,
        .buffer[0] = panel_buffer0,
        .buffer[1] = panel_buffer1,
        .buffer_size = SGL_ARRAY_SIZE(panel_buffer0),
    };

    sgl_logdev_register(log_stdout);
    sgl_fbdev_register(&fbinfo);

    sdl2_dev = malloc(sizeof(sgl_port_sdl2_t));
    if(sdl2_dev == NULL) {
        SGL_LOG_ERROR("SGL SDL2 device alloc initialize failed");
        return NULL;
    }
    m_dev = sdl2_dev;

    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0 ) {
        SGL_LOG_ERROR("SGL SDL2 initialize failed");
        free(sdl2_dev);
        return NULL;
    }
    if(sdl_create_windows(&sdl2_dev->m_window, &m_renderer, "SGL SDL2 demo") < 0) {
        SGL_LOG_ERROR("SGL SDL2 create window failed");
        free(sdl2_dev);
        return NULL;
    }

    sdl2_dev->systick = SDL_AddTimer(1000, system_tick, sdl2_dev);
    sdl2_dev->anim_systick = SDL_AddTimer(1, anim_systick, sdl2_dev);
    sdl2_dev->frame_count = 0;

    /* init sgl */
    sgl_init();

    SDL_AddEventWatch(mouse_event_interrupt, NULL);

    return sdl2_dev;
}


size_t sgl_port_sdl2_get_frame_count(sgl_port_sdl2_t* sdl2_dev)
{
    return sdl2_dev->frame_count;
}

void sgl_port_sdl2_increase_frame_count(sgl_port_sdl2_t* sdl2_dev)
{
    sdl2_dev->frame_count ++;
}

void sgl_port_sdl2_deinit(sgl_port_sdl2_t* sdl2_dev)
{
    SDL_RemoveTimer(sdl2_dev->systick);
    SDL_RemoveTimer(sdl2_dev->anim_systick);
    if (m_texture != NULL) {
        SDL_DestroyTexture(m_texture);
        m_texture = NULL;
    }
    SDL_DestroyWindow(sdl2_dev->m_window);
    SDL_DestroyRenderer(m_renderer);
}
