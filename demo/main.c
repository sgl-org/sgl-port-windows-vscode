#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

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
void sgl_rect_examples(sgl_obj_t *parent);
void sgl_scrollview_demo(sgl_obj_t *parent);
void sgl_menu_demo(sgl_obj_t *parent, sgl_key_group_t *group);
void sgl_launcher_examples(void);
void sgl_2dball_examples(sgl_obj_t *parent);
void sgl_analogclock_examples(sgl_obj_t *parent);
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
    sgl_button_examples(NULL);
    sgl_label_examples(NULL);
    sgl_rect_examples(NULL);
    sgl_scrollview_demo(NULL);
    sgl_menu_demo(NULL, NULL);
    sgl_2dball_examples(NULL);
    /* analog clocks: sweep from 9:30 to current time, then tick every second */
    sgl_analogclock_examples(NULL);
    sgl_bar_examples(NULL);
    sgl_battery_examples(NULL);
    sgl_dropdown_examples(NULL);
    sgl_stepper_examples(NULL);
    sgl_switch_examples(NULL);
    sgl_roller_examples(NULL);
    sgl_slider_examples(NULL);
    sgl_led_examples(NULL);
    sgl_scope_examples(NULL);
    sgl_qrcode_examples(NULL);
    sgl_checkbox_examples(NULL);
    sgl_textlist_examples(NULL);
    sgl_msgbox_examples(NULL);
    sgl_progress_examples(NULL);
    /* sgl_launcher_examples(); */  /* standalone launcher, uncomment to use */

    while (!quit) {
        SDL_PollEvent(&MouseEvent);
        switch (MouseEvent.type) {
        case SDL_QUIT:
            quit = 1;
            break;
        }
        sgl_task_handler();
        sgl_timer_handler();
    }

    sgl_port_sdl2_deinit(sdl2_dev);

    return 0;
}
