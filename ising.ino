/*
    LOL Ising Model for the Arduino Micro-Controller
    Copyright (C) 2013 John Ericksen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/.
*/

#include <Charliplexing.h>

#define RESEEDRATE 100000
#define SCREENX 14
#define SCREENY 9
#define SCALEX 4
#define SCALEY 4

#define SIZEX 56 // SCREENX * SCALEX
#define SIZEY 36 // SCREENY * SCALEY

//using bitwise operations so SCALEX (4) is incorpoated into byte
byte worldArray[(SIZEX / 4) * SIZEY];
long density = 50;
int loopvar = 0;
double shadesScale = (SCALEX * SCALEY) * 1.0 / SHADES;
 
void setup() {
  LedSign::Init(GRAYSCALE);
  randomSeed(analogRead(0));
  //set up random world
  seedWorld();
}

void loop() {
  
  // Pick a random location to calculte a probability.
  int x = random(0, SIZEX);
  int y = random(0, SIZEY);
  
  double neighbors = 0;
  
  neighbors += map0neg(world(wrap((x + 1), SIZEX), y));
  neighbors += map0neg(world(wrap((x - 1), SIZEX), y));
  neighbors += map0neg(world(x, wrap((y + 1), SIZEY)));
  neighbors += map0neg(world(x, wrap((y - 1), SIZEY)));
  
  double prob = exp(neighbors) / (exp(-neighbors) + exp(neighbors));
  
  // ...and use that probability to update.
  if(prob > (random(1000) / 1000.0)){
    setWorld(x, y, 1);
  }
  else{
    setWorld(x, y, 0);
  }
  
  // Update the related pixel  
  update(x,y);
  
  // Block to reseed at the given rate
  loopvar++;
  if (loopvar > RESEEDRATE){
    seedWorld();
    loopvar = 0;
  }
}

// Update the given pixel location.
void update(int x, int y){
  
  int scaledx = x / SCALEX;
  int scaledy = y / SCALEY;
  
  int xoffset = SCALEX * scaledx;
  int yoffset = SCALEY * scaledy;
  // Sum the local pixels around the scaled location.
  int total = 0;
  for(int i = 0; i < SCALEX; i++){
    for(int j = 0; j < SCALEY; j++){
      total += world(xoffset + i, yoffset + j);
    }
  }
 
 // Set the pixel location to the shade value.
 LedSign::Set(scaledx, scaledy, total / shadesScale); 
}

// Maps 1 -> 1 and 0 -> -1
int map0neg(int input){
  if(input == 0){
    return -1;
  }
  return 1;
}

// Wrap the input around the m (modulus) value turning the array into a Torus.
int wrap(int input, int m){
  return (input + m) % m;
}

//Re-seeds based off of RESEEDRATE
void seedWorld(){
  for (int i = 0; i < SIZEX; i++) {
    for (int j = 0; j < SIZEY; j++) {
      if (random(100) < density) {
        setWorld(i, j, 1);
      }
      else {
        setWorld(i, j, 0);
      }
    }
  }
  
  // Update the entire scren.
  for (int i = 0; i < SCREENX; i++) {
    for (int j = 0; j < SCREENY; j++) {
      update(i * SCALEX,j * SCALEY);
    }
  }
}

// Set a bit at the given location.
byte setWorld(int x, int y, byte input){
  
  int xRef = x / 4;
  int xBit = x - (4 * (x / 4));
  
  bitWrite(worldArray[(xRef * SIZEY) + y], xBit, input);
}

// Get a bit from the given location.
byte world(int x, int y){
  
  int xRef = x / 4;
  int xBit = x - (4 * (x / 4));
  
  return bitRead(worldArray[(xRef * SIZEY) + y], xBit);
}
