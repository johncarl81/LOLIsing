#include <Charliplexing.h>   

#define DELAY 0
#define RESEEDRATE 100000
#define SCREENX 14
#define SCREENY 9
#define SCALEX 4
#define SCALEY 6
#define SIZEX SCREENX * SCALEX
#define SIZEY SCREENY * SCALEY

byte worldArray[SCREENX * SIZEY];
long density = 50;
int loopvar = 0;
double shadesScale = (SCALEX * SCALEY) * 1.0 / SHADES;

 
void setup() {
  LedSign::Init(GRAYSCALE);
  randomSeed(analogRead(5));
  //Builds the world with an initial seed.
  Serial.begin(9600);
  seedWorld();
}

void loop() {
  
  int x = random(SIZEX);
  int y = random(SIZEY);
  
  double neighbors = 0;
  
  neighbors += map0neg(world(wrap((x + 1), SIZEX), y));
  neighbors += map0neg(world(wrap((x - 1), SIZEX), y));
  neighbors += map0neg(world(x, wrap((y + 1), SIZEY)));
  neighbors += map0neg(world(x, wrap((y - 1), SIZEY)));
  
  double prob = exp(neighbors) / (exp(-neighbors) + exp(neighbors));
  
  if(prob > (random(1000) / 1000.0)){
    setWorld(x, y, 1);
  }
  else{
    setWorld(x, y, 0);
  }
    
  update(x,y);
  
  //Counts and then checks for re-seeding
  //Otherwise the display will die out at some point
  loopvar++;
  if (loopvar > RESEEDRATE){
    seedWorld();
    loopvar = 0;
  }
  
  delay(DELAY);
}

void update(int x, int y){
  int scaledx = x / SCALEX;
  int scaledy = y / SCALEY;
  
 
  int total = 0;
  for(int i = 0; i < SCALEX; i++){
    for(int j = 0; j < SCALEY; j++){
      total += world(scaledx + i, scaledy + j);
    }
  }
 
  LedSign::Set(scaledx, scaledy, total / shadesScale); 
}

int map0neg(int input){
  if(input == 0){
    return -1;
  }
  return 1;
}

int wrap(int input, int m){
 if(input < 0){
  return m-1;
 }
 else{ 
   return input % m;
 }
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
  
  update();
}

void update(){
  for (int i = 0; i < SCREENX; i++) {
    for (int j = 0; j < SCREENY; j++) {
      update(i * SCALEX,j * SCALEY);
    }
  }
}

byte setWorld(int x, int y, byte input){
  
  int xRef = x / 4;
  int xBit = x - (4 * (x / 4));
  
  bitWrite(worldArray[(xRef * SIZEY) + y], xBit, input);
}

byte world(int x, int y){
  
  int xRef = x / 4;
  int xBit = x - (4 * (x / 4));
  
  return bitRead(worldArray[(xRef * SIZEY) + y], xBit);
}
