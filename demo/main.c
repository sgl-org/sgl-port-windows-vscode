#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <components/games/2048/sgl_2048.h>

#include <sgl.h>
#include <sgl_font.h>

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
void sgl_rect_examples(sgl_obj_t *parent);
void sgl_rect_ext_examples(sgl_obj_t *parent);
void sgl_img_ext_examples(sgl_obj_t *parent);
void sgl_music_player_create(sgl_obj_t *parent);
void sgl_scrollview_demo(sgl_obj_t *parent);
void sgl_menu_demo(sgl_obj_t *parent, sgl_key_group_t *group);
void sgl_launcher_examples(void);
void sgl_2dball_examples(sgl_obj_t *parent);
void sgl_analogclock_examples(sgl_obj_t *parent);
void sgl_arc_examples(sgl_obj_t *parent);
void sgl_bar_examples(sgl_obj_t *parent);
void sgl_battery_examples(sgl_obj_t *parent);
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
    // sgl_rect_examples(NULL);
    // sgl_img_ext_examples(NULL);  /* img_ext rotation around center */
    //sgl_music_player_create(NULL);  /* music player UI demo */
    // sgl_scrollview_demo(NULL);
    // sgl_menu_demo(NULL, NULL);
    // sgl_arc_examples(NULL);   /* arc widget: ring, gauge wrap, animated loader */
    // sgl_2dball_examples(NULL);
    // /* analog clocks: sweep from 9:30 to current time, then tick every second */
    // sgl_analogclock_examples(NULL);
    // sgl_bar_examples(NULL);
    // sgl_battery_examples(NULL);
    // sgl_dropdown_examples(NULL);
    // sgl_stepper_examples(NULL);
    // sgl_switch_examples(NULL);
    // sgl_roller_examples(NULL);
    // sgl_slider_examples(NULL);
    // sgl_led_examples(NULL);
    // sgl_scope_examples(NULL);
    // sgl_qrcode_examples(NULL);
    // sgl_checkbox_examples(NULL);
    // sgl_textlist_examples(NULL);
    //sgl_msgbox_examples(NULL);
    // sgl_progress_examples(NULL);
    // sgl_physical_key_examples(NULL);
    // sgl_viewlist_examples(NULL);
    // sgl_rect_ext_examples(NULL);
    /* sgl_launcher_examples(); */  /* standalone launcher, uncomment to use */
    //sgl_tabview_examples(NULL);  /* standalone tabview demo, uncomment to use (best with the other examples commented out) */

    //sgl_textedit_examples(NULL);  /* textedit demo with keyboard input */
    sgl_game2048_start(sgl_screen_act(), SGL_SCREEN_WIDTH, SGL_SCREEN_HEIGHT, &consolas24, &consolas23, &consolas24);

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
        sgl_task_handler();
        sgl_timer_handler();
    }

    sgl_port_sdl2_deinit(sdl2_dev);

    return 0;
}
