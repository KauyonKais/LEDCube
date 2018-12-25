#include <TimerOne.h>

//GLOBAL VARIABLES
#define MAX_PX 128
uint16_t pixels_to_draw[MAX_PX];
uint8_t px_iter = 0;
static const uint8_t x_axis[] {25, 24, 23, 22, 21, 20, 19, 18};//
static const uint8_t y_axis[] {17, 45, 44, 43, 42, 41, 40, 39};//latch
static const uint8_t z_axis[] {27, 0, 1, 2, 3, 4, 5, 6};

/* ############################################################## */
//SETUP
void setup() {
  pinMode(26, INPUT); //don't use, ever. EVER.
  set_pins();
  wipe_pins();
  //Timer
  Timer1.initialize(100000);
  Timer1.attachInterrupt(cube_anim);//ADD STUFF
  //randomSeed(55684987657);
}
/* ############################################################## */
//LOOP
void loop() {
  draw_pixels(pixels_to_draw);
}
/* ############################################################## */
//ANIMATIONS

//Cube
uint8_t cc = 2;
uint8_t cube_iter = 1;
void cube_anim() {
  
  if(cc > 6 || cc < 1)
    cube_iter = -cube_iter;
  cc += cube_iter;
  
  clearPixels();
  uint16_t ta = px(cc, cc, cc);
  uint16_t tb = px(0, cc, cc);
  uint16_t tc = px(0, 0, cc);
  uint16_t td = px(cc, 0, cc);

  uint16_t ba = px(cc, cc, 0);
  uint16_t bb = px(0, cc, 0);
  uint16_t bc = px(0, 0, 0);
  uint16_t bd = px(cc, 0, 0);

  plotLine(ta, tb);
  plotLine(tb, tc);
  plotLine(tc, td);
  plotLine(td, ta);

  plotLine(ba, ta);
  plotLine(bb, tb);
  plotLine(bc, tc);
  plotLine(bd, td);

  plotLine(ba, bb);
  plotLine(bb, bc);
  plotLine(bc, bd);
  plotLine(bd, ba);
}

/* ############################################################## */
//plot line using Bresenham
void plotLine(int16_t start, int16_t end) {
  int8_t x0 = px_x(start);
  int8_t y0 = px_y(start);
  int8_t z0 = px_z(start);

  int8_t x1 = px_x(end);
  int8_t y1 = px_y(end);
  int8_t z1 = px_z(end);

  int8_t *a0, *a1, *b0, *b1, *c0, *c1;

  if (abs(y1 - y0) > abs(x1 - x0)) {
    if (abs(y1 - y0) > abs(z1 - z0)) {
      //a=y
      a0 = &y0;
      a1 = &y1;
      b0 = &x0;
      b1 = &x1;
      c0 = &z0;
      c1 = &z1;
    } else {
      //a=z
      a0 = &z0;
      a1 = &z1;
      b0 = &x0;
      b1 = &x1;
      c0 = &y0;
      c1 = &y1;
    }
  } else {
    if (abs(x1 - x0) > abs(z1 - z0)) {
      //a=x
      a0 = &x0;
      a1 = &x1;
      b0 = &y0;
      b1 = &y1;
      c0 = &z0;
      c1 = &z1;
    } else {
      //a=z
      a0 = &z0;
      a1 = &z1;
      b0 = &x0;
      b1 = &x1;
      c0 = &y0;
      c1 = &y1;
    }
  }


  int8_t da = *a1 - *a0;
  if (da < 0) {
    da = -da;
    int8_t t = *a0;
    *a0 = *a1;
    *a1 = t;
    t = *b0;
    *b0 = *b1;
    *b1 = t;
    t = *c0;
    *c0 = *c1;
    *c1 = t;
  }
  int8_t db = *b1 - *b0;
  int8_t dc = *c1 - *c0;
  int8_t bi = 1;
  int8_t ci = 1;
  if (db < 0) {
    db = -db;
    bi = -1;
  }
  if (dc < 0) {
    dc = -dc;
    ci = -1;
  }

  int8_t Db = (2 * db) - da;
  int8_t Dc = (2 * dc) - da;
  for (; *a0 <= *a1; (*a0)++) {
    addPixel(px(x0, y0, z0));
    if (Db > 0) {
      *b0 += bi;
      Db = Db - 2 * da;
    }
    if (Dc > 0) {
      *c0 += ci;
      Dc = Dc - 2 * da;
    }
    Db = Db + 2 * db;
    Dc = Dc + 2 * dc;
  }
}
/* ############################################################## */
//data structure for pixels
void addPixel(uint16_t pixel) {
  if (px_iter < MAX_PX) {
    pixels_to_draw[px_iter] = pixel;
    px_iter++;
  }
}

void clearPixels() {
  px_iter = 0;
}
/* ############################################################## */
//draw pixels
inline void draw_pixels(uint16_t pixels[]) {
  uint16_t px = 0;
  for (uint8_t i = 0; i < px_iter; i++) {
    px = pixels[i];
    draw_px(px >> 8 & 0xF, px >> 4 & 0xF, px & 0xF);
    delt_px(px >> 8 & 0xF, px >> 4 & 0xF, px & 0xF);
  }
}
inline void set_px(uint8_t x, uint8_t y, uint8_t z, bool on) {
  digitalWrite(x_axis[x], on);
  digitalWrite(z_axis[z], on);
  digitalWrite(y_axis[y], HIGH);
  digitalWrite(y_axis[y], LOW);
}
inline void draw_px(uint8_t x, uint8_t y, uint8_t z) {
  set_px(x, y, z, true);
}
inline void delt_px(uint8_t x, uint8_t y, uint8_t z) {
  set_px(x, y, z, false);
}
inline uint16_t px(uint8_t x, uint8_t y, uint8_t z) {
  return x << 8 | y << 4 | z;
}
inline uint8_t px_x(uint16_t px) {
  return px >> 8 & 0xF;
}
inline uint8_t px_y(uint16_t px) {
  return px >> 4 & 0xF;
}
inline uint8_t px_z(uint16_t px) {
  return px  & 0xF;
}

/* ############################################################## */
//handle pins (for setup)
inline void wipe_pins() {
  //set shiftregister
  //x axis
  digitalWrite(25, LOW);
  digitalWrite(24, LOW);
  digitalWrite(23, LOW);
  digitalWrite(22, LOW);
  digitalWrite(21, LOW);
  digitalWrite(20, LOW);
  digitalWrite(19, LOW);
  digitalWrite(18, LOW);
  //pull latch
  //y axis
  digitalWrite(39, HIGH);
  digitalWrite(40, HIGH);
  digitalWrite(41, HIGH);
  digitalWrite(42, HIGH);
  digitalWrite(43, HIGH);
  digitalWrite(44, HIGH);
  digitalWrite(45, HIGH);
  digitalWrite(17, HIGH);
  digitalWrite(39, LOW);
  digitalWrite(40, LOW);
  digitalWrite(41, LOW);
  digitalWrite(42, LOW);
  digitalWrite(43, LOW);
  digitalWrite(44, LOW);
  digitalWrite(45, LOW);
  digitalWrite(17, LOW);
  //set switch U9 to high
  //z axis
  digitalWrite(27, LOW);
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
}
inline void set_pins() {
  //P00-07 for shift register data
  pinMode(25, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(18, OUTPUT);
  //P20 for shift latch U1
  pinMode(39, OUTPUT);
  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);
  pinMode(42, OUTPUT);
  pinMode(43, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(17, OUTPUT);
  //P10-17 for switch U9
  pinMode(27, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}
