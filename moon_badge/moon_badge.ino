#include <Arduboy2.h>

Arduboy2 arduboy;

extern const uint8_t PROGMEM moon_sprite[];
extern const uint8_t PROGMEM beacon_sprite[];

// function definitions
void update_beacon_step();
void reset_auto_animation_timer();
void clear_screen();
void print_text();

constexpr uint8_t auto_steps_max = 100;
constexpr uint8_t auto_offset_min = 0;
constexpr uint8_t auto_offset_max = auto_steps_max - 1;
constexpr uint8_t auto_half_width = WIDTH / 2;
constexpr uint8_t auto_half_height = HEIGHT / 2;
const float pi_rate = 32 * 3.1415633f;
const float pi_step = pi_rate/32;
const float pi_reset = 24 * pi_rate;
uint8_t auto_offset = 0;
float auto_step = 0.0f;
double phantom_cursor = 0;
const int auto_animation_frames_start = 60;
int frames_since_last_input = 0;


const int beacon_frames_cycle = 16; // frames until beacon animation completion
const int beacon_frames_animation_delay = 70; // frames between each beacon animation
const int num_beacon_frames = 5;
int beacon_step = 0; // iteration of beacon animation
int beacon_frame = 0;
int beacon_frame_interval;
int beacon_wait_counter;

int cursor_x = 0, cursor_y = 0;


void setup() {
  beacon_frame_interval = beacon_frames_cycle/num_beacon_frames;
  beacon_wait_counter = beacon_frames_animation_delay;
  
  arduboy.boot(); // boot raw arduboy hardware
  arduboy.display(); // blank the display (sBuffer is global, so cleared automatically)

  // flashlight(); // light the RGB LED and screen if UP button is being held.

  // check for and handle buttons held during start up for system control
  arduboy.systemButtons();
  arduboy.audio.begin();

  // bootLogo();

  // waitNoButtons(); // wait for all buttons to be released

  arduboy.setFrameRate(60);
  arduboy.clear();

  clear_screen();
}

void loop() {

  // limit to 60fps
  if (!arduboy.nextFrame()) {
    return;
  }

  // calculate automatic cursor positioning
  if ( frames_since_last_input >= auto_animation_frames_start ) {
    auto_position_cursor();
  } else {
    frames_since_last_input++;
  }

  // handle controls
  if ( arduboy.pressed(UP_BUTTON) ) {
    reset_auto_animation_timer ();
    --cursor_y;
  }

  if ( arduboy.pressed(DOWN_BUTTON) ) {
    reset_auto_animation_timer ();
    ++cursor_y;
  }

  if ( arduboy.pressed(RIGHT_BUTTON) ) {
    reset_auto_animation_timer ();
    ++cursor_x;
  }

  if ( arduboy.pressed(LEFT_BUTTON) ) {
    reset_auto_animation_timer ();
    --cursor_x;
  }
  

  // clear screen before display
  clear_screen();

  print_text();
  
  Sprites::drawSelfMasked(cursor_x, cursor_y, moon_sprite, 0);
  Sprites::drawSelfMasked( (cursor_x+80), (cursor_y+60), beacon_sprite, beacon_frame  );

  arduboy.display();
  update_beacon_step();
}

void update_beacon_step() {

  if( beacon_frame < num_beacon_frames ) {
    if(arduboy.everyXFrames(beacon_frame_interval)) {
      beacon_frame++;
    }
  } else if (beacon_wait_counter > 0) {
    beacon_wait_counter--; // wait
    return;
  } else  {// reset after waiting
    beacon_wait_counter = beacon_frames_animation_delay;
    beacon_frame = 0;
  }
}

void reset_auto_animation_timer () {
  frames_since_last_input = 0;
  phantom_cursor = (float)cursor_y;
  auto_step;
}

void auto_position_cursor() {
  
  auto_step += pi_step;

  double yval = (-1 * sin( (auto_step / pi_rate) ) );

  // imperceptively reset the counter
  if( auto_step > pi_reset && yval < 0.1 ){
    auto_step = 0;
  }

  phantom_cursor += yval;
    
  cursor_y = (int)phantom_cursor;

}

void print_text(){

  // I just want to put some text to make it seem personal
  arduboy.setCursor(0, 0);
  arduboy.print("50 Years!");
  
}
