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

#define GAME_TERMINATE -1
#define Keynumber 10
#define Velocity 30
#define MAX_BULLET 20
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
ALLEGRO_KEYBOARD_STATE keyState ;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_TIMER *timer2 = NULL;
ALLEGRO_TIMER *timer3 = NULL;
ALLEGRO_SAMPLE *song=NULL;
ALLEGRO_FONT *font = NULL;

//Custom Definition
const char *title = "Final Project 107062115";
const float FPS = 60;
const int WIDTH = 400;
const int HEIGHT = 600;
const float MAX_COOLDOWN = 0.005;
//draw blood
float blood_top_x = 5.0; 
float blood_top_y = 10.0;
float blood_height = 10.0;
float blood_width = 100.0;
float blood_between_distance = 290.0;
float injury = 1.0;
float blood_down_x;
float blood_down_y;
float blood_down_temp;



double last_shoot_timestamp;


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

int imageWidth = 0;
int imageHeight = 0;
int draw = 0;
int done = 0;
int window = 1;
int injury_time = 0;

bool judge_next_window = false;
bool ture = true; //true: appear, false: disappear
bool next = false; //true: trigger
bool dir = true; //true: left, false: right
bool keys[Keynumber];
bool is_injury = true;


void show_err_msg(int msg);
void game_init();
void game_begin();
int process_event();
int game_run();
void game_destroy();
ALLEGRO_BITMAP *load_bitmap_at_size(const char * , int , int );


void show_err_msg(int msg) {
    fprintf(stderr, "unexpected msg: %d\n", msg);
    game_destroy();
    exit(9);
}