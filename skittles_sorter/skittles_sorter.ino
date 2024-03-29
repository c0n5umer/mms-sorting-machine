#include <AccelStepper.h>
#define ROUT 5
#define GOUT 6
#define BOUT 7
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

#define IN5 2
#define IN6 3
#define IN7 4
#define IN8 12

#define YELLOW 0
#define ORANGE 30
#define GREEN 60
#define PURPLE 90
#define RED 120

#define WORKMODE true
// if false stepper motors won't be used
// and serial will have detailed output

AccelStepper stepper(8, IN1, IN3, IN2, IN4);
AccelStepper stepperGiver(8, IN5, IN7, IN6, IN8);

int currentAngle = 0;

void setup()
{
  Serial.begin(9600);

  // set speed for calibration
  stepper.setMaxSpeed(300.0);
  stepper.setAcceleration(100.0);
  stepper.setSpeed(300);

  long minDiff = 1025;
  long index = 0;
  const int ROUNDS = 18;

  // find the hole
  for (int i = 0; i < ROUNDS; i++) {
    int light = getLightness(255, 255, 255, 300);
    int dark = getLightness(0, 0, 0, 300);
    int diff = dark - light;
    if (diff <= minDiff) {
      minDiff = diff;
      index = i;
    }
    Serial.println(diff);
    Serial.println(index);
    stepper.move(32);
    stepper.runToPosition();
  }

  // go back to hole
  long wayback = 32 * (index - ROUNDS - 1);
  stepper.move(wayback);

  Serial.println(wayback);

  // set speed for work
  stepper.runToPosition();
  stepper.setMaxSpeed(1024.0);
  stepper.setAcceleration(1024.0);

  stepperGiver.setMaxSpeed(1024.0);
  stepperGiver.setAcceleration(1024.0);
  delay(2000);
}

void moveRotator()
{
  // moves rotator one step
  stepper.move(522);
  stepper.runToPosition();
  stepper.move(90);
  stepper.runToPosition();
  stepper.move(-100);
  stepper.runToPosition();
}

void loop()
{
  String color = classify();
  Serial.println(color);

  if (WORKMODE)
  {
    if (color != "None") {
      setGiver(color);
    }
    moveRotator();
  }
}

String classify()
{
  // classifies color 
  // returns classified color
  int red = getLightness(255, 0, 0, 500);
  int green = getLightness(0, 255, 0, 500);
  int blue = getLightness(0, 0, 255, 500);
  // blue is not in use btw

  // output to IDE's `Serial Monitor`
  if (WORKMODE)
  {
    Serial.print(red);
    Serial.print(" ");
    Serial.print(green);
    Serial.print(" ");
    Serial.print(blue);
    Serial.print(" ");
  }

  // green or purple
  if (550 < red && red <= 620)
  {
    // green
    if (450 < green && green <= 525)
    {
      return "Green";
    }
    // purple
    if (525 < green && green <= 575)
    {
      return "Purple";
    }
    return "None";
  }

  // red
  if (475 < red && red <= 550)
  {
    if (525 < green && green <= 575)
    {
      return "Red";
    }
    return "None";
  }

  // yellow or orange
  if (419 < red && red <= 475)
  {
    // orange
    if (500 < green && green <= 560)
    {
      return "Orange";
    }
    // yellow
    if (430 < green && green <= 500)
    {
      return "Yellow";
    }
    return "None";
  }
  return "None";

}

long findMinAngle(int colorAngle)
{
  // calculates angle to rotate
  int angle = colorAngle - currentAngle;
  
  return (long)(angle * 11.37);
}

void setGiver(String color)
{
  // rotates gyver based on input color
  stepperGiver.enableOutputs();
  Serial.println("Im in rotator");
  if (color == "Yellow") {
    Serial.println("yellow angle");
    Serial.println(findMinAngle(YELLOW));
    
    stepperGiver.move(findMinAngle(YELLOW));    
    stepperGiver.runToPosition();

    currentAngle = YELLOW;
  }
  
  if (color == "Orange") {
    Serial.println("orange angle");
    Serial.println(findMinAngle(ORANGE));
    
    stepperGiver.move(findMinAngle(ORANGE));    
    stepperGiver.runToPosition();

    currentAngle = ORANGE;
  }
  
  if (color == "Green") {
    Serial.println("green angle");
    Serial.println(findMinAngle(GREEN));
    
    stepperGiver.move(findMinAngle(GREEN));    
    stepperGiver.runToPosition();

    currentAngle = GREEN;
  }

  if (color == "Purple") {
    Serial.println("purple angle");
    Serial.println(findMinAngle(PURPLE));
    
    stepperGiver.move(findMinAngle(PURPLE));    
    stepperGiver.runToPosition();

    currentAngle = PURPLE;
  }

  if (color == "Red") {
    Serial.println("red angle");
    Serial.println(findMinAngle(RED));
    
    stepperGiver.move(findMinAngle(RED));    
    stepperGiver.runToPosition();

    currentAngle = RED;
  }
  stepperGiver.disableOutputs();
  return;
}

void setColor(int r, int g, int b)
{
  // sets color to (r, g, b)
  analogWrite(ROUT, r);
  analogWrite(GOUT, g);
  analogWrite(BOUT, b);
}

int getLightness()
{
  // returns current lightness
  return analogRead(A3);
}

int getLightness(int r, int g, int b, int measureDelay)
{
  // args: r, g, b - ints from 0 to 255 desribing color
  // measureDelay - delay before measuring lightness
  // turns on RGB light (and off after work)
  // returns lighntess measured after delay
  setColor(r, g, b);
  delay(measureDelay);
  int lightness = getLightness();
  setColor(0, 0, 0);
  return lightness;
}