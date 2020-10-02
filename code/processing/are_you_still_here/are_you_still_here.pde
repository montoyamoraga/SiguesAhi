////////
// about
////////

// are you still here?
// a project by aaron montoya-moraga
// available at 
// github.com/montoyamoraga/are-you-still-here

// are_you_still_here.pde
// this is a Processing sketch for graphic identity
// started in september 2020
// modified in october 2020
// v0.0.1

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
  textSize(64);

  // drawing options
  // no border
  noStroke();
  // green fill
  fill(0, 255, 0);

  // set background color black
  background(0);

  // draw text at the center of canvas
  text("are you still here?", width/2, height/2);

  // save resulting file
  save("output.jpg");
}
