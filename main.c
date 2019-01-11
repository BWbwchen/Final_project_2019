#include "initial.h"

int main(int argc, char *argv[]) {
    int msg = 0;

    game_init();
    game_begin();

    while (msg != GAME_TERMINATE) {
        msg = game_run();
        if (msg == GAME_TERMINATE){
                printf("Game Over\n");
        }else if(msg == PLAY_AGAIN){
            game_init();
            game_begin();
        }
    }

    game_destroy();
    return 0;
}



void game_init() {
    if (!al_init()) {
        show_err_msg(-1);
    }
    if(!al_install_audio()){
        fprintf(stderr, "failed to initialize audio!\n");
        show_err_msg(-2);
    }
    if(!al_init_acodec_addon()){
        fprintf(stderr, "failed to initialize audio codecs!\n");
        show_err_msg(-3);
    }
    if (!al_reserve_samples(1)){
        fprintf(stderr, "failed to reserve samples!\n");
        show_err_msg(-4);
    }
    // Create display
    display = al_create_display(WIDTH, HEIGHT);
    event_queue = al_create_event_queue();
    if (display == NULL || event_queue == NULL) {
        show_err_msg(-5);
    }
    // Initialize Allegro settings
    al_set_window_position(display, 0, 0);
    al_set_window_title(display, title);
    al_init_primitives_addon();
    al_install_keyboard();
    al_install_audio();
    al_init_image_addon();
    al_init_acodec_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    // Register event
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    memset (keys, false, Keynumber * sizeof(bool ));


}

void game_begin() {
    // Load sound
    song = al_load_sample( "hello.wav" );
    if (!song){
        printf( "Audio clip sample not loaded!\n" );
        show_err_msg(-6);
    }
    // Loop the song until the display closes
    al_play_sample(song, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
    al_clear_to_color(al_map_rgb(100,100,100));
    // Load and draw text
    font = al_load_ttf_font("pirulen.ttf",12,0);
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+220 , ALLEGRO_ALIGN_CENTRE, "Press 'Enter' to start");
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+200 , ALLEGRO_ALIGN_CENTRE, "Press 'f' to input the file");
    al_draw_rectangle(WIDTH/2-150, 510, WIDTH/2+150, 550, al_map_rgb(255, 255, 255), 0);
    al_flip_display();
}

int process_event(){
    // Request the event
    
    ALLEGRO_EVENT event;
    al_wait_for_event(event_queue, &event);

    // Our setting for controlling animation
    if(event.timer.source == timer){
        if(character2.x < -150) dir = false;
        else if(character2.x > WIDTH+50) dir = true;

        if(dir) character2.x -= 10;
        else character2.x += 10;
    }
    if(event.timer.source == timer2){
        ture = false;
        next = true;
    }
    if(event.timer.source == timer3){
        if(next) next = false;
        else ture = true;
    }

    // Keyboard
    if(event.type == ALLEGRO_EVENT_KEY_DOWN)
    {
        switch(event.keyboard.keycode)
        {
            // Control
            case ALLEGRO_KEY_W:
                keys[UP] = true;
                break;
            case ALLEGRO_KEY_S:
                keys[DOWN] = true;
                break;
            case ALLEGRO_KEY_A:
                keys[LEFT] = true;
                break;
            case ALLEGRO_KEY_D:
                keys[RIGHT] = true;
                break;
            case ALLEGRO_KEY_SPACE:
                keys[SPACE] = true;
                break;
            case ALLEGRO_KEY_ESCAPE:
                return GAME_TERMINATE;
            // For Start Menu
            case ALLEGRO_KEY_ENTER:
                judge_next_window = true;
                break;
            case ALLEGRO_KEY_F:
                input_file = 1;
                break;

        }
    }else if(event.type == ALLEGRO_EVENT_KEY_UP){
        switch(event.keyboard.keycode)
        {
            // Control
            case ALLEGRO_KEY_W:
                keys[UP] = false;
                break;
            case ALLEGRO_KEY_S:
                keys[DOWN] = false;
                break;
            case ALLEGRO_KEY_A:
                keys[LEFT] = false;
                break;
            case ALLEGRO_KEY_D:
                keys[RIGHT] = false;
                break;
            case ALLEGRO_KEY_SPACE:
                keys[SPACE] = false;
                break;
        }
    }


    // Shutdown our program
    else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        return GAME_TERMINATE;

    //up, down left right
    character1.y -= keys[UP] * Velocity;
    character1.y += keys[DOWN]  * Velocity;
    character1.x -= keys[LEFT] * Velocity;
    character1.x += keys[RIGHT] * Velocity;

    if (character1.x <= 0)
        character1.x = 0;
    else if (character1.x + character1.width >= WIDTH)
        character1.x = WIDTH - character1.width;
    if (character1.y <= 0)
        character1.y = 0;
    else if (character1.y + character1.height >= HEIGHT)
        character1.y = HEIGHT - character1.height;

    // [HACKATHON 2-6]
    // TODO: Update bullet coordinates.
    // 1) For each bullets, if it's not hidden, update x, y
    // according to vx, vy.
    // 2) If the bullet is out of the screen, hide it.
    // Uncomment and fill in the code below.
    int i;
    for (i = 0; i < MAX_BULLET; i++) {
        if (bullet[i].hidden)
            continue;
        bullet[i].x += bullet[i].vx;
        bullet[i].y += bullet[i].vy;
        if (bullet[i].y  <=  0)
            bullet[i].hidden = true;
    }
     // [HACKATHON 2-7
    // TODO: Shoot if key is down and cool-down is over.
    // 1) Get the time now using 'al_get_time'.
    // 2) If Space key is down in 'key_state' and the time
    //    between now and last shoot is not less that cool
    //    down time.
    // 3) Loop through the bullet array and find one that is hidden.
    //    (This part can be optimized.)
    // 4) The bullet will be found if your array is large enough.
    // 5) Set the last shoot time to now.
    // 6) Set hidden to false (recycle the bullet) and set its x, y to the
    //    front part of your plane.
    // Uncomment and fill in the code below.
    double now = al_get_time();
    if (keys[SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN) {
        for (i = 0; i < MAX_BULLET; i++) {
            if (bullet[i].hidden)
                break;
        }
        if (i == MAX_BULLET)
            return 0;
        last_shoot_timestamp = now;
        bullet[i].hidden = false;
        bullet[i].x = character1.x;
        bullet[i].y = character1.y - bullet[i].height;
    }


    return 0;
}

int game_run() {
    int error = 0;
    // First window(Menu)
    if(window == 1){
        if (!al_is_event_queue_empty(event_queue)) {
            error = process_event();
            if(judge_next_window) {
                window = 2;
                // Setting Character
                // setting character1
                character1.x = WIDTH / 2;
                character1.y = HEIGHT / 2 + 150;
                character1.height = 162;
                character1.width = 100;
                // setting character2
                character2.x = WIDTH + 100;
                character2.y = HEIGHT / 2 - 280;
                character1.image_path = al_load_bitmap("tower.png");
                character2.image_path = al_load_bitmap("teemo_left.png");
                character3.image_path = al_load_bitmap("teemo_right.png");
                background = al_load_bitmap("stage.jpg");
                // setting bullet
                if(input_file){
                    //read file
                    //change blood
                }else{
                    //
                }
                int i;
                for (i = 0; i < MAX_BULLET; i++) {
                    bullet[i].width = 30;
                    bullet[i].height = 60;
                    bullet[i].image_path = load_bitmap_at_size("bullet.png",bullet[i].width,bullet[i].height);
                    bullet[i].vx = 0;
                    bullet[i].vy = -3;
                    bullet[i].hidden = true;
                }

                //Initialize Timer
                timer  = al_create_timer(1.0/15.0);
                timer2  = al_create_timer(1.0);
                timer3  = al_create_timer(1.0/10.0);
                al_register_event_source(event_queue, al_get_timer_event_source(timer)) ;
                al_register_event_source(event_queue, al_get_timer_event_source(timer2)) ;
                al_register_event_source(event_queue, al_get_timer_event_source(timer3)) ;
                al_start_timer(timer);
                al_start_timer(timer2);
                al_start_timer(timer3);
                //draw blood initial
                blood_down_x = blood_top_x + blood_width;
                blood_down_y = blood_top_y + blood_height;
                blood_down_temp = blood_down_x;
            }
        }
    }
    // Second window(Main Game)
    else if(window == 2){
        // Change Image for animation
        al_draw_bitmap(background, 0,0, 0);
        if(ture) al_draw_bitmap(character1.image_path, character1.x, character1.y, 0);

        if(dir) al_draw_bitmap(character2.image_path, character2.x, character2.y, 0);
        else al_draw_bitmap(character3.image_path, character2.x, character2.y, 0);

        //blood
        //player

        if( blood_top_x <= blood_down_temp ){
            al_draw_rectangle(blood_top_x-0.5, blood_top_y-0.5, blood_down_x+1, blood_down_y+1, al_map_rgb(255, 255, 255), 1.0);
            al_draw_filled_rectangle(blood_top_x, blood_top_y, blood_down_temp, blood_down_y, al_map_rgb(255, 0, 0));
            blood_down_temp -= injury;
        }else{
            al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+220 , ALLEGRO_ALIGN_CENTRE, "Press R to RESATRT");
            al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+190 , ALLEGRO_ALIGN_CENTRE, "Press E to END");
            printf("OK!\n");
            ALLEGRO_EVENT event_r_e;
            al_wait_for_event(event_queue, &event_r_e);
            if(event_r_e.type == ALLEGRO_EVENT_KEY_DOWN){
                switch(event_r_e.keyboard.keycode){
                    case ALLEGRO_KEY_R:
                        blood_down_temp = blood_down_x;
                        window = 1;
                        return PLAY_AGAIN;
                        break;
                    case ALLEGRO_KEY_E:
                        return GAME_TERMINATE;
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        return GAME_TERMINATE;
                        break;
                    default:
                        al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+180 , ALLEGRO_ALIGN_CENTRE, "fuck you !");
                        al_flip_display();
                        al_rest(1.0);
                        return GAME_TERMINATE;
                        break;
                }
            }
            
            //return GAME_TERMINATE;
        }
        //enermy
        al_draw_rectangle(blood_top_x + blood_between_distance-0.5, blood_top_y-0.5, blood_down_x + blood_between_distance+1, blood_down_y+1, al_map_rgb(255, 255, 255), 1.0);
        al_draw_filled_rectangle(blood_top_x + blood_between_distance,
         blood_top_y ,
         blood_down_x + blood_between_distance,
         blood_down_y,
         al_map_rgb(255, 0, 0));

        //bullet
        int i;
        for(i = 0; i < MAX_BULLET; i++){
            if(!bullet[i].hidden) al_draw_bitmap(bullet[i].image_path, bullet[i].x, bullet[i].y, 0);
        }

        al_flip_display();
        al_clear_to_color(al_map_rgb(0,0,0));

        // Listening for new event
        if (!al_is_event_queue_empty(event_queue)) {
            error = process_event();
        }
    }
    return error;
}

void game_destroy() {
    // Make sure you destroy all things
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_timer(timer2);
    al_destroy_bitmap(image);
    al_destroy_sample(song);
}
ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h)
{
  ALLEGRO_BITMAP *resized_bmp, *loaded_bmp, *prev_target;

  // 1. create a temporary bitmap of size we want
  resized_bmp = al_create_bitmap(w, h);
  if (!resized_bmp) return NULL;

  // 2. load the bitmap at the original size
  loaded_bmp = al_load_bitmap(filename);
  if (!loaded_bmp)
  {
     al_destroy_bitmap(resized_bmp);
     return NULL;
  }

  // 3. set the target bitmap to the resized bmp
  prev_target = al_get_target_bitmap();
  al_set_target_bitmap(resized_bmp);

  // 4. copy the loaded bitmap to the resized bmp
  al_draw_scaled_bitmap(loaded_bmp,
     0, 0,                                // source origin
     al_get_bitmap_width(loaded_bmp),     // source width
     al_get_bitmap_height(loaded_bmp),    // source height
     0, 0,                                // target origin
     w, h,                                // target dimensions
     0                                    // flags
  );

  // 5. restore the previous target and clean up
  al_set_target_bitmap(prev_target);
  al_destroy_bitmap(loaded_bmp);

  return resized_bmp;
}
