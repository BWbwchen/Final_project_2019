#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define IWANT
#define GAME_TERMINATE -1
#define PLAY_AGAIN 666
#define Keynumber 10
#define Velocity 3
#define MAX_BULLET 10
#define MAX_ENEMY_BULLET 6
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SPACE 4


// ALLEGRO Variables
ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_BITMAP *image = NULL;
ALLEGRO_BITMAP *image2 = NULL;
ALLEGRO_BITMAP *image3 = NULL;
ALLEGRO_BITMAP *background = NULL;
ALLEGRO_BITMAP *start_menu = NULL;
ALLEGRO_BITMAP *store_graph = NULL;
ALLEGRO_BITMAP *win_graph = NULL;
ALLEGRO_BITMAP *setting = NULL;
ALLEGRO_KEYBOARD_STATE keyState ;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_TIMER *timer2 = NULL;
ALLEGRO_TIMER *timer3 = NULL;
ALLEGRO_SAMPLE *song=NULL;
ALLEGRO_SAMPLE *song_AGAIN=NULL;
ALLEGRO_FONT *font = NULL;

//Custom Definition
const char *title = "Final Project 107062115";
const float FPS = 60;
const int WIDTH = 400;
const int HEIGHT = 600;
const float MAX_ENEMY_COOLDOWN = 1.0;
//draw blood
float MAX_COOLDOWN = 0.2;
float blood_top_x = 5.0;
float blood_top_y = 10.0;
float blood_height = 10.0;
float blood_width = 100.0;
float blood_between_distance = 290.0;
float injury = 0;
float blood_down_x;
float blood_down_y;
float blood_down_temp;
float blood_down_enemy;
float injury_enemy = 0;




double last_shoot_timestamp;
double last_shoot_timestamp_ENEMY;



typedef struct character
{
    int height;//graph
    int width;//graph
    int x;
    int y;
    int vx;
    int vy;
    bool hidden;
    ALLEGRO_BITMAP *image_path;

}Character;

Character character1;//player
Character character2;//anermy
Character character3;//anermy
Character bullet[MAX_BULLET];
Character enemy_bullet[MAX_BULLET];

int imageWidth = 0;
int imageHeight = 0;
int draw = 0;
int done = 0;
int window = 1;
int injury_time = 0;
int score = 0;

bool judge_next_window = false;
bool ture = true; //true: appear, false: disappear
bool next = false; //true: trigger
bool dir = true; //true: left, false: right
bool keys[Keynumber];
bool is_injury = true;
bool input_file = false;
bool out_file = false;
bool store = false;
bool touch_enemy = false;
bool half_injury = false;
bool king = false;
bool action = false;

void show_err_msg(int msg);
void game_init();
void game_begin();
int process_event();
int game_run();
void game_destroy();
ALLEGRO_BITMAP *load_bitmap_at_size(const char * , int , int );
void load_bullet();
void reflection(int ,int ,int ,int ,int );
void play_music();
void input();
void output();
void initial_enemy_bullet();


void show_err_msg(int msg) {
    fprintf(stderr, "unexpected msg: %d\n", msg);
    game_destroy();
    exit(9);
}
