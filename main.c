#include "initial.h"

int main(int argc, char *argv[]) {
    #ifdef IWANT
    freopen ("file.in", "r", stdin);
    #endif

    int msg = 0;

    game_init();
    game_begin();

    while (msg != GAME_TERMINATE) {
        msg = game_run();
        if (msg == GAME_TERMINATE){
            //printf("Game Over\n");
        }else if(msg == PLAY_AGAIN){
            al_destroy_sample(song);
            play_music();
            game_init();
            game_begin();
        }
    }

    #ifdef IWANT
    freopen ("file.in", "w", stdout);
    #endif

    if(out_file) output();
    
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

    //initial key state and rand
    memset (keys, false, Keynumber * sizeof(bool ));
    srand(time(NULL));
}

void game_begin() {
    // Load sound
    song = al_load_sample( "Birds_in_Flight.wav" );
    if (!song){
        printf( "Audio clip sample not loaded!\n" );
        show_err_msg(-6);
    }
    // Loop the song until the display closes
    al_play_sample(song, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
    al_clear_to_color(al_map_rgb(100,100,100));
    //draw start menu
    start_menu = al_load_bitmap( "start.jpg" );
    al_draw_bitmap(start_menu, 0, 0, 0);
    // Load and draw text
    font = al_load_ttf_font("pirulen.ttf",16,0);
    //al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+220 , ALLEGRO_ALIGN_CENTRE, "Press 'Enter' to start");
    //al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+200 , ALLEGRO_ALIGN_CENTRE, "Press 'f' to input the file");
    //al_draw_rectangle(WIDTH/2-150, 510, WIDTH/2+150, 550, al_map_rgb(255, 255, 255), 0);
    al_flip_display();
}

int process_event(){
    // Request the event
    ALLEGRO_EVENT event;
    al_wait_for_event(event_queue, &event);

    // Our setting for controlling animation
    if(event.timer.source == timer){
        if(character2.x < -150) dir = false;//character3
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
            case ALLEGRO_KEY_B://into the store
                window = 4;
                break;
            case ALLEGRO_KEY_P://into setting
                window = 6;
                al_destroy_sample(song);
                song = al_load_sample ( "Safety_Net.wav" );
                al_play_sample(song, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                break;
            // For Start Menu
            case ALLEGRO_KEY_ENTER:
                judge_next_window = true;
                action = true;
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

    //border limit
    if (character1.x <= 0)
        character1.x = 0;
    else if (character1.x + character1.width >= WIDTH)
        character1.x = WIDTH - character1.width;
    if (character1.y <= 0)
        character1.y = 0;
    else if (character1.y + character1.height >= HEIGHT)
        character1.y = HEIGHT - character1.height;

    //player's bullet
    int i;
    for (i = 0; i < MAX_BULLET; i++) {
        if (bullet[i].hidden)
            continue;
        bullet[i].x += bullet[i].vx;
        bullet[i].y += bullet[i].vy;
        if (bullet[i].y  <=  0){
            bullet[i].hidden = true;
        }
    }
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
        bullet[i].y = character1.y;
    }

    //enermy's bullet
    for (i = 0; i < MAX_ENEMY_BULLET; i++) {
        if (enemy_bullet[i].hidden)
            continue;
        reflection(enemy_bullet[i].x, enemy_bullet[i].y, enemy_bullet[i].height, enemy_bullet[i].width, i);
        enemy_bullet[i].x += enemy_bullet[i].vx;
        enemy_bullet[i].y += enemy_bullet[i].vy;
        if (enemy_bullet[i].y  <  0){
            enemy_bullet[i].hidden = true;
            enemy_bullet[i].x = 0;
            enemy_bullet[i].y = 0;
            enemy_bullet[i].vy = 5;
        }
    }
    double enemy_now = al_get_time();
    if ((enemy_now - last_shoot_timestamp_ENEMY >= MAX_ENEMY_COOLDOWN) && (action)) {
        for (i = 0; i < MAX_ENEMY_BULLET; i++) {
            if (enemy_bullet[i].hidden)
                break;
        }
        if (i == MAX_ENEMY_BULLET)
            return 0;
        last_shoot_timestamp_ENEMY = enemy_now;
        enemy_bullet[i].hidden = false;
        if(dir){
            enemy_bullet[i].x = character2.x;
            enemy_bullet[i].y = character2.y;
        }else{
            enemy_bullet[i].x = character2.x;
            enemy_bullet[i].y = character2.y;
        }
    }

    //player injury
    for(i = 0; i < MAX_ENEMY_BULLET; i++){
        if((!enemy_bullet[i].hidden) && (enemy_bullet[i].y+enemy_bullet[i].height >= character1.y)&&
        (enemy_bullet[i].y+enemy_bullet[i].height <= character1.y+character1.height)&&
        (enemy_bullet[i].x >= character1.x)&&
        (enemy_bullet[i].x+enemy_bullet[i].width <= character1.x+character1.width) ){
            enemy_bullet[i].hidden = true;
            if(king){
                injury = 0;
            }else{
                injury = 2.0 - (half_injury)*1.0;
            }   
        }
    }

    //touch the enemy would injury too
    if((character1.x > character2.x) && (character1.x < character2.x + character2.width)
        &&(character1.y > character2.y) && (character1.y < character2.y + character2.height) ){
        touch_enemy = true;
        injury = 0.5 - (king)*0.5;
    }else{
        touch_enemy = false;
    }

    //enemy injury
    for(i = 0; i < MAX_BULLET; i++){        
        if((!bullet[i].hidden) && (bullet[i].x > character2.x)&&
        (bullet[i].x < character2.x+character2.width)&&
        (bullet[i].y > character2.y)&&
        (bullet[i].y < character2.y+character2.height) ){
            bullet[i].hidden = true;
            injury_enemy = 2.0 + (king)*48.0;
            score++;
        }
    }

    //if my bullet hit other's bullet
    for (i = 0; i < MAX_BULLET; ++i){
        int j;
        for (j = 0; j < MAX_ENEMY_BULLET; ++j){
            if((!bullet[i].hidden) && (!enemy_bullet[j].hidden) && (bullet[i].x > enemy_bullet[j].x) &&
                (bullet[i].x < enemy_bullet[j].x + enemy_bullet[j].width) &&
                (bullet[i].y > enemy_bullet[j].y)&&
                (bullet[i].y < enemy_bullet[j].y + enemy_bullet[j].height)){
                enemy_bullet[j].hidden = true;
                bullet[i].hidden = true;
            }
        }
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
                character1.height = 50;
                character1.width = 50;
                // setting character2
                character2.x = WIDTH + 100;
                character2.y = HEIGHT / 2 - 280;
                character2.width = 100;
                character2.height = 96;
                //character3.x = -100;
                //character3.y = HEIGHT / 2 - 280;
                
                character1.image_path =load_bitmap_at_size("main_character.png",character1.height,character1.width);
                character2.image_path = al_load_bitmap("teemo_left.png");
                character3.image_path = al_load_bitmap("teemo_right.png");
                background = al_load_bitmap("stage.jpg");

                // setting bullet
                //load enemy's bullet
                int i;
                for(i = 0; i < MAX_ENEMY_BULLET; i++){
                    enemy_bullet[i].width = 30;
                    enemy_bullet[i].height = 30;
                    enemy_bullet[i].image_path = load_bitmap_at_size("enemy_bullet.png",enemy_bullet[i].width,enemy_bullet[i].height);
                    enemy_bullet[i].vx = rand()%7 - 3;
                    enemy_bullet[i].vy = 5;
                }
                //load player's bullet
                load_bullet();
                //Initialize Timer
                timer  = al_create_timer(1.0/20.0);
                timer2  = al_create_timer(1.0);
                timer3  = al_create_timer(1.0/20.0);
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
                blood_down_enemy = blood_down_x + blood_between_distance;
                if(input_file) input();
                
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
            //player's
        if( blood_top_x < blood_down_temp ){
            blood_down_temp -= injury;
            al_draw_rectangle(blood_top_x-0.5, blood_top_y-0.5, blood_down_x+1, blood_down_y+1, al_map_rgb(255, 255, 255), 1.0);
            al_draw_filled_rectangle(blood_top_x, blood_top_y, blood_down_temp, blood_down_y, al_map_rgb(255, 0, 0));
            injury = 0;
        }else{
            window = 3;//lose
        }
            //enermy
        if( blood_top_x + blood_between_distance < blood_down_enemy ){
            blood_down_enemy -= injury_enemy;
            al_draw_rectangle(blood_top_x + blood_between_distance-0.5, blood_top_y-0.5, blood_down_x + blood_between_distance+1, blood_down_y+1, al_map_rgb(255, 255, 255), 1.0);
            al_draw_filled_rectangle(blood_top_x + blood_between_distance,
                blood_top_y ,
                blood_down_enemy,
                blood_down_y,
                al_map_rgb(255, 0, 0));
            injury_enemy = 0;
        }else{
            //win
            window = 5;
        }
        
        //draw player's bullet
        int i;
        for(i = 0; i < MAX_BULLET; i++){
            if(!bullet[i].hidden) al_draw_bitmap(bullet[i].image_path, bullet[i].x, bullet[i].y, 0);
        }

        //draw enemy's bullet
        for(i = 0; i < MAX_ENEMY_BULLET; i++){
            if(!enemy_bullet[i].hidden){
                al_draw_bitmap(enemy_bullet[i].image_path, enemy_bullet[i].x, enemy_bullet[i].y, 0);
            } 
        }

        //draw notice if touch the enemy
        if(touch_enemy){
            al_draw_text(font, al_map_rgb(255,0,0), WIDTH/2, HEIGHT/2 , ALLEGRO_ALIGN_CENTRE, "touch the enemy injury !");    
        }

        //draw score
        al_draw_textf(font, al_map_rgb(255, 255, 255), WIDTH/2-50, 0, 0, "score: %d", score);

        al_flip_display();
        al_clear_to_color(al_map_rgb(0,0,0));

        // Listening for new event
        if (!al_is_event_queue_empty(event_queue)) {
            error = process_event();
        }
    }else if(window == 3){
        //lose
        al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+220 , ALLEGRO_ALIGN_CENTRE, "Press R to RESATRT");
        al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+190 , ALLEGRO_ALIGN_CENTRE, "Press E to END");
        al_flip_display();

        ALLEGRO_EVENT event_r_e;
        al_wait_for_event(event_queue, &event_r_e);
        if(event_r_e.type == ALLEGRO_EVENT_KEY_DOWN){
            switch(event_r_e.keyboard.keycode){
                case ALLEGRO_KEY_R://restart
                    blood_down_temp = blood_down_x;
                    blood_down_enemy = blood_down_x + blood_between_distance;
                    window = 1;
                    judge_next_window = false;
                    initial_enemy_bullet();
                    return PLAY_AGAIN;
                    break;
                case ALLEGRO_KEY_E://end
                    return GAME_TERMINATE;
                    break;
                case ALLEGRO_KEY_ESCAPE://end
                    return GAME_TERMINATE;
                    break;
                default://little trick
                    al_clear_to_color(al_map_rgb(0,0,0));
                    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+180 , ALLEGRO_ALIGN_CENTRE, "fuck you !");
                    al_flip_display();
                    al_rest(1.0);
                    return GAME_TERMINATE;
                    break;
            }
        }
    }else if(window == 4){
        //store
        store_graph = al_load_bitmap( "store.jpg" );
        al_draw_bitmap(store_graph, 0, 0, 0);
        al_draw_textf(font, al_map_rgb(255, 255, 0), WIDTH/2, 80, ALLEGRO_ALIGN_CENTRE, "score :%d", score);
        al_flip_display();

        ALLEGRO_EVENT event_r_e;
        al_wait_for_event(event_queue, &event_r_e);
        if(event_r_e.type == ALLEGRO_EVENT_KEY_DOWN){
            switch(event_r_e.keyboard.keycode){
                case ALLEGRO_KEY_1://increasing rate of shoot
                    if(score - 10 >= 0){
                        score -= 10;
                        MAX_COOLDOWN = 0.07;
                        al_clear_to_color(al_map_rgb(0,0,0));
                        al_draw_textf(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+180 , ALLEGRO_ALIGN_CENTRE, "rate of shot increase !");
                        al_flip_display();
                        al_rest(1.0);
                    }else{
                        al_clear_to_color(al_map_rgb(0,0,0));
                        al_draw_textf(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+180 , ALLEGRO_ALIGN_CENTRE, "Are you idiot ?");
                        al_draw_textf(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+195 , ALLEGRO_ALIGN_CENTRE, "not enough money !");
                        al_flip_display();
                        al_rest(1.0);
                    }
                    break;
                case ALLEGRO_KEY_2:
                    if(score - 50 >= 0){//half the injury
                        score -= 50;
                        half_injury = true;
                    }else{
                        al_clear_to_color(al_map_rgb(0,0,0));
                        al_draw_textf(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+180 , ALLEGRO_ALIGN_CENTRE, "Are you idiot ?");
                        al_draw_textf(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+195 , ALLEGRO_ALIGN_CENTRE, "not enough money !");
                        al_flip_display();
                        al_rest(1.0);
                    }
                    break;
                case ALLEGRO_KEY_TAB:
                    king = true;//king mode 
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    return GAME_TERMINATE;
                case ALLEGRO_KEY_BACKSPACE://back
                    al_draw_bitmap(start_menu, 0, 0, 0);
                    al_flip_display();
                    window = 1;
                    break;
                default:
                    al_clear_to_color(al_map_rgb(0,0,0));
                    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+180 , ALLEGRO_ALIGN_CENTRE, "Are you idiot ?");
                    al_flip_display();
                    al_rest(1.0);
                    break;
            }
        }
    }else if(window == 5){
        //win 
        win_graph = al_load_bitmap( "win.jpg" );
        al_draw_bitmap(win_graph, 0, 0, 0);
        al_draw_textf(font,al_map_rgb(255, 255, 255), WIDTH/2-20, HEIGHT-150, ALLEGRO_ALIGN_CENTRE, "Press R to next");
        al_flip_display();

        ALLEGRO_EVENT event_r_e;
        al_wait_for_event(event_queue, &event_r_e);
        if(event_r_e.type == ALLEGRO_EVENT_KEY_DOWN){
            switch(event_r_e.keyboard.keycode){
                case ALLEGRO_KEY_R://restart
                    blood_down_temp = blood_down_x;
                    blood_down_enemy = blood_down_x + blood_between_distance;
                    window = 1;
                    judge_next_window = false;
                    action = false;
                    initial_enemy_bullet();
                    return PLAY_AGAIN;
                    break;
                default:
                    return GAME_TERMINATE;
                    break;
            }
        }
    }else if(window == 6){
        //setting
        setting = al_load_bitmap("setting.jpg");
        al_draw_bitmap(setting, 0, 0, 0);
        al_flip_display();

        ALLEGRO_EVENT event_r_e;
        al_wait_for_event(event_queue, &event_r_e);
        if(event_r_e.type == ALLEGRO_EVENT_KEY_DOWN){
            switch(event_r_e.keyboard.keycode){
                case ALLEGRO_KEY_1://switch song
                    al_destroy_sample(song);
                    song = al_load_sample ( "Sunshine.wav" );
                    al_play_sample(song, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                    break;
                case ALLEGRO_KEY_2://switch song
                    al_destroy_sample(song);
                    song = al_load_sample ( "Perrywinkle.wav" );
                    al_play_sample(song, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                    break;
                case ALLEGRO_KEY_3://switch song
                    al_destroy_sample(song);
                    song = al_load_sample ( "Laugh_and_Cry.wav" );
                    al_play_sample(song, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                    break;
                case ALLEGRO_KEY_I://input file
                    input_file = true;
                    input();
                    al_draw_textf(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+195 , ALLEGRO_ALIGN_CENTRE, "input ok!");
                    al_flip_display();
                    al_rest(1.0);
                    break;
                case ALLEGRO_KEY_S://save file
                    out_file = true;
                    al_draw_textf(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+195 , ALLEGRO_ALIGN_CENTRE, "save result !");
                    al_flip_display();
                    al_rest(1.0);
                    break;
                case ALLEGRO_KEY_BACKSPACE://back to start menu
                    window = 1;
                    al_draw_bitmap(start_menu, 0, 0, 0);
                    al_flip_display();
                    break;
                default:
                    al_clear_to_color(al_map_rgb(0,0,0));
                    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+180 , ALLEGRO_ALIGN_CENTRE, "fuck you !");
                    al_flip_display();
                    al_rest(1.0);
                    break;
            }
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
    al_destroy_bitmap(start_menu);
    al_destroy_bitmap(win_graph);
    al_destroy_bitmap(store_graph);
    al_destroy_bitmap(setting);
    al_destroy_sample(song);
    al_destroy_sample(song_AGAIN);
}
ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h){
  ALLEGRO_BITMAP *resized_bmp, *loaded_bmp, *prev_target;

  resized_bmp = al_create_bitmap(w, h);
  if (!resized_bmp) return NULL;

  loaded_bmp = al_load_bitmap(filename);
  if (!loaded_bmp)
  {
     al_destroy_bitmap(resized_bmp);
     return NULL;
  }

  prev_target = al_get_target_bitmap();
  al_set_target_bitmap(resized_bmp);

  al_draw_scaled_bitmap(loaded_bmp, 0, 0, al_get_bitmap_width(loaded_bmp), al_get_bitmap_height(loaded_bmp), 0, 0, w, h, 0);

  al_set_target_bitmap(prev_target);
  al_destroy_bitmap(loaded_bmp);

  return resized_bmp;
}

void load_bullet(){
    int i;
    for (i = 0; i < MAX_BULLET; i++) {
        bullet[i].width = 30;
        bullet[i].height = 30;
        switch(i){
            case 0:
                bullet[i].image_path = load_bitmap_at_size("ball_1.png",bullet[i].width,bullet[i].height);
                break;
            case 1:
                bullet[i].image_path = load_bitmap_at_size("ball_2.png",bullet[i].width,bullet[i].height);
                break;
            case 2:
                bullet[i].image_path = load_bitmap_at_size("ball_3.png",bullet[i].width,bullet[i].height);
                break;
            case 3:
                bullet[i].image_path = load_bitmap_at_size("ball_4.png",bullet[i].width,bullet[i].height);
                break;
            case 4:
                bullet[i].image_path = load_bitmap_at_size("ball_5.png",bullet[i].width,bullet[i].height);
                break;
            case 5:
                bullet[i].image_path = load_bitmap_at_size("ball_6.png",bullet[i].width,bullet[i].height);
                break;
            case 6:
                bullet[i].image_path = load_bitmap_at_size("ball_7.png",bullet[i].width,bullet[i].height);
                break;
            case 7:
                bullet[i].image_path = load_bitmap_at_size("ball_8.png",bullet[i].width,bullet[i].height);
                break;
            case 8:
                bullet[i].image_path = load_bitmap_at_size("ball_9.png",bullet[i].width,bullet[i].height);
                break;
            case 9:
                bullet[i].image_path = load_bitmap_at_size("ball_10.png",bullet[i].width,bullet[i].height);
                break;
            case 10:
                bullet[i].image_path = load_bitmap_at_size("ball_11.png",bullet[i].width,bullet[i].height);
                break;
            case 11:
                bullet[i].image_path = load_bitmap_at_size("ball_12.png",bullet[i].width,bullet[i].height);
                break;
            case 12:
                bullet[i].image_path = load_bitmap_at_size("ball_13.png",bullet[i].width,bullet[i].height);
                break;
            case 13:
                bullet[i].image_path = load_bitmap_at_size("ball_14.png",bullet[i].width,bullet[i].height);
                break;
            case 14:
                bullet[i].image_path = load_bitmap_at_size("ball_15.png",bullet[i].width,bullet[i].height);
                break;
            case 15:
                bullet[i].image_path = load_bitmap_at_size("ball_white.png",bullet[i].width,bullet[i].height);
                break;
        }
        bullet[i].vx = 0;
        bullet[i].vy = -7;
        bullet[i].hidden = true;
    }
}
void reflection(int x, int y, int height, int width, int index){
    if(x < 0){
        enemy_bullet[index].x = 0;
        enemy_bullet[index].vx = -enemy_bullet[index].vx;
    }else if(y < 0){
        enemy_bullet[index].y = 0;
        enemy_bullet[index].vy = -enemy_bullet[index].vy;
    }else if(x + width >= WIDTH){
        enemy_bullet[index].x = WIDTH - width;
        enemy_bullet[index].vx = -enemy_bullet[index].vx;
    }else if(y + height >= HEIGHT){
        enemy_bullet[index].y = HEIGHT - height;
        enemy_bullet[index].vy = -enemy_bullet[index].vy;
    }
}
void play_music(){
    song_AGAIN = al_load_sample( "Mysteries.wav" );
    if (!song){
        printf( "Audio clip sample not loaded!\n" );
        show_err_msg(-6);
    }
    // Loop the song until the display closes
    al_play_sample(song_AGAIN, 0.5, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
}
void input(){
    scanf("%f", &blood_down_temp);
    scanf("%f", &blood_down_enemy);
    scanf("%f", &injury_enemy);
    scanf("%d", &score);
}
void output(){
    printf("%f\n", blood_down_temp);
    printf("%f\n", blood_down_enemy);
    printf("%f\n", injury_enemy);
    printf("%d\n", score);
}
void initial_enemy_bullet(){
    int i;
    for(i = 0; i < MAX_ENEMY_BULLET; i++){
        enemy_bullet[i].hidden = true;
        enemy_bullet[i].vx = rand()%7 - 3;
        enemy_bullet[i].vy = 5;
        enemy_bullet[i].x = 0;
        enemy_bullet[i].y = 0;
    }
}