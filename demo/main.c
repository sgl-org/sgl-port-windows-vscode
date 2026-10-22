#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <components/games/2048/sgl_2048.h>

#include <sgl.h>
#include <sgl_font.h>

#include <components/bench/sgl_bench.h>

/* ============================================================
 * SGL 性能基准测试（类似 LVGL 的 benchmark demo）
 *   - 置 1：运行 sgl/source/components/bench/sgl_bench.c 里的 benchmark
 *   - 置 0：回到下面原有的控件示例（win widget 等）
 * ============================================================ */
#define SGL_DEMO_USE_BENCHMARK      0

/* ============================================================
 * 手动测试哪个控件示例
 *   1 = filedialog  文件对话框（点按钮打开，模态，覆盖全屏）
 *   2 = filebrowser 文件浏览器（常驻，不覆盖全屏）
 *   0 = 都不创建
 * 改完重新 make run 即可。
 * ============================================================ */
#define SGL_DEMO_WIDGET_EXAMPLE     0

typedef struct sgl_port_sdl2 sgl_port_sdl2_t;

sgl_port_sdl2_t *sgl_port_sdl2_init(void);
size_t sgl_port_sdl2_get_frame_count(sgl_port_sdl2_t *sdl2_dev);
void sgl_port_sdl2_increase_frame_count(sgl_port_sdl2_t *sdl2_dev);
void sgl_port_sdl2_deinit(sgl_port_sdl2_t *sdl2_dev);
sgl_port_sdl2_t *sdl2_dev = NULL;
SDL_Event MouseEvent;

/* examples forward declarations */
void sgl_button_examples(sgl_obj_t *parent);
void sgl_label_examples(sgl_obj_t *parent);
void sgl_label_ext_examples(sgl_obj_t *parent);
void sgl_span_examples(sgl_obj_t *parent);
void sgl_rect_examples(sgl_obj_t *parent);
void sgl_rect_ext_examples(sgl_obj_t *parent);
void sgl_img_ext_examples(sgl_obj_t *parent);
void sgl_music_player_create(sgl_obj_t *parent);
void sgl_scrollview_demo(sgl_obj_t *parent);
void sgl_menu_demo(sgl_obj_t *parent, sgl_key_group_t *group);
void sgl_menu_examples(sgl_obj_t *parent);
void sgl_launcher_examples(void);
void sgl_2dball_examples(sgl_obj_t *parent);
void sgl_analogclock_examples(sgl_obj_t *parent);
void sgl_arc_examples(sgl_obj_t *parent);
void sgl_arc_battery_examples(sgl_obj_t *parent);
void sgl_bar_examples(sgl_obj_t *parent);
void sgl_battery_examples(sgl_obj_t *parent);
void sgl_charge_examples(sgl_obj_t *parent);
void sgl_dropdown_examples(sgl_obj_t *parent);
void sgl_stepper_examples(sgl_obj_t *parent);
void sgl_switch_examples(sgl_obj_t *parent);
void sgl_roller_examples(sgl_obj_t *parent);
void sgl_slider_examples(sgl_obj_t *parent);
void sgl_led_examples(sgl_obj_t *parent);
void sgl_scope_examples(sgl_obj_t *parent);
void sgl_qrcode_examples(sgl_obj_t *parent);
void sgl_checkbox_examples(sgl_obj_t *parent);
void sgl_textlist_examples(sgl_obj_t *parent);
void sgl_msgbox_examples(sgl_obj_t *parent);
void sgl_progress_examples(sgl_obj_t *parent);
void sgl_physical_key_examples(sgl_obj_t *parent);
void sgl_viewlist_examples(sgl_obj_t *parent);
void sgl_tabview_examples(sgl_obj_t *parent);
void sgl_textedit_examples(sgl_obj_t *parent);
void sgl_calendar_examples(sgl_obj_t *parent);
void sgl_gif_test(sgl_obj_t *parent);
void sgl_coverflow_examples(sgl_obj_t *parent);
void sgl_proplabel_examples(sgl_obj_t *parent);
void sgl_avi_examples(sgl_obj_t *parent);
void sgl_filedialog_examples(sgl_obj_t *parent);
void sgl_filebrowser_examples(sgl_obj_t *parent);
void sgl_win_examples(sgl_obj_t *parent);
void sgl_lv_music_create(sgl_obj_t *parent);
void sgl_demo_music_player_create(sgl_obj_t *parent);
void sgl_spectrum_examples(sgl_obj_t *parent);
void sgl_transit_warp_examples(sgl_obj_t *parent);
void sgl_warp_demo(sgl_obj_t *parent);

/* physical key handler from sgl/examples/physical_key.c */
typedef enum physical_key {
    PHYSICAL_KEY_UP,
    PHYSICAL_KEY_DOWN,
    PHYSICAL_KEY_LEFT,
    PHYSICAL_KEY_RIGHT,
    PHYSICAL_KEY_ENTER,
    PHYSICAL_KEY_ESC,
} physical_key_t;
void sgl_physical_key_handler(physical_key_t key, bool pressed);

/* Page-transition demo entry points: demo/transit_demo.c and demo/test.c
 * have been removed, so these are currently undefined; the sgl_task_handler()
 * driven transition API (sgl_screen_load) is what remains. */
//void transit_demo_init(void);
//void transit_demo_task(void);
//void test_demos_init(void);

int main(int argc, char *argv[])
{
    SGL_UNUSED(argc);
    SGL_UNUSED(argv);
    int quit = 0;

    sdl2_dev = sgl_port_sdl2_init();
    if (sdl2_dev == NULL) {
        return -1;
    }

    /* create widget examples on the active screen */
    // sgl_button_examples(NULL);
    // sgl_label_examples(NULL);
    //sgl_label_ext_examples(NULL);  /* label_ext rotation demo (45 deg grid, spin, live counter) */
    // sgl_span_examples(NULL);  /* span widget: rich inline text, clip/wrap/scroll, alpha */
    // sgl_rect_examples(NULL);
    // sgl_img_ext_examples(NULL);  /* img_ext rotation around center */
    //sgl_music_player_create(NULL);  /* music player UI demo */
    // sgl_scrollview_demo(NULL);
    // sgl_menu_examples(NULL);   /* menu widget: light + dark rounded card themes */
    // sgl_msgbox_examples(NULL);  /* msgbox widget examples */
    // sgl_charge_examples(NULL);  /* charge widget: seamless 3D cylinder + wave liquid */
    /* page-transition demos: demo/transit_demo.c and demo/test.c were
     * removed; page switching now goes through sgl_screen_load(obj, trans)
     * and is driven by sgl_task_handler() in the loop below. */
    //if (getenv("SGL_TRANSIT_AUTO") != NULL) {
    //    transit_demo_init();
    //} else {
    //    test_demos_init();
    //}
    // sgl_battery_examples(NULL);  /* battery widget: seamless 3D cylinder + wave liquid */
    //sgl_arc_examples(NULL);   /* arc widget: ring, gauge wrap, animated loader */
    //sgl_arc_battery_examples(NULL);  /* user test case: ring gauge + percent labels, 30 ms timer */
    // sgl_2dball_examples(NULL);
    /* analog clocks: sweep from 9:30 to current time, then tick every second */
    // sgl_analogclock_examples(NULL);
    // sgl_calendar_examples(NULL);  /* calendar widget: modern flat month view */
    // sgl_gif_test(NULL);  /* GIF streaming-decode test: embedded 80x80 animated GIF */
    // sgl_bar_examples(NULL);
    //sgl_dropdown_examples(NULL);
    // sgl_stepper_examples(NULL);
    // sgl_switch_examples(NULL);
    // sgl_roller_examples(NULL);
    // sgl_slider_examples(NULL);
    // sgl_led_examples(NULL);
    // sgl_scope_examples(NULL);
    // sgl_qrcode_examples(NULL);
    // sgl_checkbox_examples(NULL);
    // sgl_textlist_examples(NULL);
    // sgl_msgbox_examples(NULL);
    // sgl_progress_examples(NULL);
    // sgl_physical_key_examples(NULL);
    // sgl_coverflow_examples(NULL);  /* coverflow widget: stacked card gallery with drag/keys */
    // sgl_lv_music_create(NULL);  /* LVGL music player demo ported to SGL */
    // sgl_demo_music_player_create(NULL);  /* light-theme music player card demo (layout.html) */
    // sgl_viewlist_examples(NULL);
    // sgl_gif_test(NULL);  /* GIF streaming-decode test: embedded 80x80 animated GIF */
    // sgl_avi_examples(NULL);  /* AVI playback example; requires a local .avi file */
    // sgl_rect_ext_examples(NULL);
    /* sgl_launcher_examples(); */  /* standalone launcher, uncomment to use */
    //sgl_tabview_examples(NULL);  /* standalone tabview demo, uncomment to use (best with the other examples commented out) */

    //sgl_textedit_examples(NULL);  /* textedit demo with keyboard input */
    //sgl_game2048_start(sgl_screen_act(), SGL_SCREEN_WIDTH, SGL_SCREEN_HEIGHT, &consolas24, &consolas23, &consolas24);

    //sgl_spectrum_examples(NULL);  /* spectrum widget: gradient/bar/block/rounded + peak caps */
    // sgl_transit_warp_examples(NULL);  /* warp page-transition demo: genie / S-curve */
    sgl_warp_demo(NULL);  /* warp showcase: auto-cycling genie / S-curve over a checker grid */
    // sgl_avi_examples(NULL);
#if SGL_DEMO_WIDGET_EXAMPLE == 1
    sgl_filedialog_examples(NULL);  /* file dialog demo: two ramfs block devices (/sd, /flash) */
#elif SGL_DEMO_WIDGET_EXAMPLE == 2
    sgl_filebrowser_examples(NULL); /* filebrowser: browse devices via block registry */
#endif
#if SGL_DEMO_USE_BENCHMARK
    //sgl_bench_start();  /* SGL 性能基准测试：场景轮流跑，结束打印汇总表 */
#else
    //sgl_win_examples(NULL);  /* win widget: titled window with a vector close cross */
#endif

    while (!quit) {
        SDL_PollEvent(&MouseEvent);
        switch (MouseEvent.type) {
        case SDL_QUIT:
            quit = 1;
            break;
        case SDL_KEYDOWN:
            /* map the PC keyboard to physical key events */
            switch (MouseEvent.key.keysym.sym) {
            case SDLK_UP:
                sgl_physical_key_handler(PHYSICAL_KEY_UP, true);
                break;
            case SDLK_DOWN:
                sgl_physical_key_handler(PHYSICAL_KEY_DOWN, true);
                break;
            case SDLK_LEFT:
                sgl_physical_key_handler(PHYSICAL_KEY_LEFT, true);
                break;
            case SDLK_RIGHT:
                sgl_physical_key_handler(PHYSICAL_KEY_RIGHT, true);
                break;
            case SDLK_RETURN:
                sgl_physical_key_handler(PHYSICAL_KEY_ENTER, true);
                break;
            case SDLK_ESCAPE:
                sgl_physical_key_handler(PHYSICAL_KEY_ESC, true);
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            if (MouseEvent.key.keysym.sym == SDLK_RETURN) {
                sgl_physical_key_handler(PHYSICAL_KEY_ENTER, false);
            }
            break;
        }
#if SGL_DEMO_USE_BENCHMARK
        /* 驱动 benchmark：推进场景动画 / 采样 / 场景切换 */
        sgl_bench_loop();
        sgl_bench_frame_begin();
#endif
        /* sgl_task_handler() now drives any running page transition itself,
         * so an app only needs sgl_screen_load(obj, trans) to switch pages */
        sgl_task_handler();
        sgl_timer_handler();
        /* 无独立 present 步骤：端口的 flush_area 内部直接刷屏，
         * 节奏由 sgl_task_handler 的 tick 限制把关 */
#if SGL_DEMO_USE_BENCHMARK
        sgl_bench_frame_end();
#endif
    }

    sgl_port_sdl2_deinit(sdl2_dev);
    return 0;
}
