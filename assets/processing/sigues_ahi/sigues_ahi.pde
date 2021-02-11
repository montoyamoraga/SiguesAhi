////////
// about
////////

// sigues ahi?
// a project by aaron montoya-moraga
// available at 
// github.com/montoyamoraga/sigues-ahi

// sigues_ahi.pde
// this is a Processing sketch for graphic identity
// started in september 2020
// modified in Februar 2021
// v0.0.2

///////
// code
///////

// global variable for storing font
PFont myFont;

void setup() {

  size(1280, 640);

  // load font from data/folder
  // 8bit operator by Grandoplex Productions,
  // available at 1001freefonts.com
  // public domain, GPl, OFL
  myFont = createFont("8bitOperatorPlus8-Regular.ttf", 32);
  
  // set the font for text
  textFont(myFont);

  // text options
  // align to center
  textAlign(CENTER);
  // text size
  textSize(128);

  // drawing options
  // no border
  noStroke();
  // green fill
  fill(0, 255, 0);

  // set background color black
  background(0);

  // draw text at the center of canvas
  text("sigues ahí?", width/2, height/2);

  // save resulting file
  save("output.jpg");
}
