
#include <string.h>
#include <Adafruit_NeoPixel.h>

#define PIN 11   // input pin Neopixel is attached to
#define NUMPIXELS      30 // number of neopixels in strip

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 0; // timing delay in milliseconds
int machine_rythm = 20;
  
int color[3] = {0,0,0};
int r,g,b;

int button_state[6] = {0,0,0,0,0,0};

int previous_state[6] = {0,0,0,0,0,0};

int button_ref[4][6] =  { {0,0,0,1,0,0},
               {1,0,0,1,0,0},
               {1,0,0,1,1,0},
             {1,1,0,1,1,0} };

int button_switch[5] = {2,3,4,5,6};

int step = 0;

bool new_input = false;
int step_phase_1 = 0;
int step_phase_2 = 0;
int r_led = 10;
int y_led = 9;
int g_led = 8;

void setup()
{
  Serial.begin(9600);
  pinMode(button_switch[0], INPUT);
  pinMode(button_switch[1], INPUT);
  pinMode(button_switch[2], INPUT);
  pinMode(button_switch[3], INPUT);
  pinMode(button_switch[4], INPUT);
  pinMode(r_led, OUTPUT);
  pinMode(y_led, OUTPUT);
  pinMode(g_led, OUTPUT);
  pixels.begin();
  pixels.setBrightness(50);
  pixels.show(); 
}


/*
Get switch's state for the first part of the game
*/
int *get_current_state(int *state)
{
    for (int i = 0; i < 5; i++)
    {
      state[i] = digitalRead(button_switch[i]);
      /*
      if (digitalRead(button_switch[i]) == HIGH)
      {
        state[i] = 1;
      }
      else 
      {
        state[i] = 0;
      }
      */
      
      Serial.print("state button");
      Serial.print(i);
      Serial.print(" = ");
      Serial.println(state[i]);
      
    }
  return (state);
}

/*
When gets switch's state, compare with the reference state and return if it's good or not 
*/

bool compare_state(int *first, int *sec)
{
  int i =0;
  
    while (i < 6)
    {
    if (first[i] != sec[i])
          return (false);
    i++;
    }
  return (true);
}

void print_state(int *state)
{
  for (int i =0; i < 5 ; i++)
    {
      Serial.print(state[i]);
    }
    Serial.println("");
}


/*
Main function of the first part :
Get the current switch's state and check if it's the good sequence then active led as feedback to the user :
[red] = bad sequence, need to restart 
[yellow] = unfinished good sequence 
[green] = Good sequence, end of the first part
*/
void part_1(void)
{
 get_current_state(button_state);
  if ( !compare_state(button_state, previous_state))
      {
        if (compare_state(button_state,button_ref[step_phase_1]))
            {
              step_phase_1++;
            }
      else 
        {
            step_phase_1 = 0;
        }
      }
  print_state(button_state);
  digitalWrite(r_led, LOW);
  digitalWrite(y_led, LOW);
  digitalWrite(g_led, LOW);
  
  if ( step_phase_1 == 0)
      digitalWrite(r_led, HIGH);
  else if (step_phase_1 == 4)
  {
      digitalWrite(g_led, HIGH);
      step = 1;
      delay(1000);
      digitalWrite(g_led, LOW);
      wake_up();
    
  }
   else 
      digitalWrite(y_led, HIGH);
  Serial.println(step_phase_1);
  memcpy (previous_state, button_state, sizeof(button_state));
}



void moving_dot()
{
  int dlay;
  int static current_led = 0;
  if (current_led % NUMPIXELS == 0)
    pixels.setPixelColor(NUMPIXELS - 1, 0,0,0);
  else 
    pixels.setPixelColor(current_led % NUMPIXELS - 1, 0,0,0);
   
  pixels.setPixelColor(current_led % NUMPIXELS, r + color[0] ,g + color[1] ,b + color[2]);
   current_led++;
  pixels.show();
  dlay = machine_rythm - delayval;
  if (dlay <= 50)
    dlay = 50;
  Serial.println(dlay);
  delay(dlay);
  
}

void wake_up(void)
{
  for (int i = 0 ; i < 255; i++)
  {
    pixels.setPixelColor(13, i,0,0);
    pixels.show();
    delay(20);
  }
  pixels.setPixelColor(13, 0,0,0);
  for (int i = 0 ; i < 4; i++)
  {
    pixels.setPixelColor(13, 255,0,0);
    pixels.show();
    delay(500);
    pixels.setPixelColor(13, 0,0,0);
    pixels.show();
    delay(500);
  }
  
}

void compute_color(void)
{
 delayval = map(analogRead(0), 0, 1024, -200,800 );
 color[0] = map(analogRead(1), 0, 1024, -255, 255);
 color[1] = map(analogRead(2), 0, 1024, -255, 255);
 color[2] = map(analogRead(3), 0, 1024, -255, 255);
 for (int i = 0; i < 3; i++)
 {
  color[i] = max(color[i],0);
  color[i] = min(color[i],255);
 }
}

void print_color(void)
{
    Serial.println("*********");
  Serial.print("r = ");
    Serial.println(color[0]);
    Serial.print("g = ");
    Serial.println(color[1]);
    Serial.print("b = ");
    Serial.println(color[2]);

    Serial.println(step_phase_2);
  Serial.println("*********");
  }

void end_animation(void)
{
  digitalWrite(g_led, LOW);
  digitalWrite(r_led, LOW);
  digitalWrite(y_led, LOW);
  for (int i = 0; i < NUMPIXELS ; i++)
  {
    pixels.setPixelColor(i, 0,0,0);
  }  
  pixels.show();
  delay(1000);
  for (int i = 0; i < 4 ; i++)
  {
    digitalWrite(g_led, HIGH);
    delay(500);
    digitalWrite(g_led, LOW);
    delay(500);
  }  
  digitalWrite(g_led, HIGH);
}

void part_2(void)
{
 int static end_c = 0;
 compute_color();
 
 print_color();
 if (step_phase_2 == 0)
 {
  if ( (abs(color[0] - color[1]) < 30) && (color[0] > 80) && ( color [2] < 100) && ( (machine_rythm - delayval) > 200))
  {
   step_phase_2 = 1;
   digitalWrite(r_led, HIGH);
  }
 }
  else if (step_phase_2 == 1)
  {
    if ( (color[0] > 140 && color[1] < 100 && color[2] < 100 ) && ( ((machine_rythm - delayval) > 200) && ((machine_rythm - delayval) < 350)))
    {
     step_phase_2 = 3;
     digitalWrite(y_led, HIGH);
    }
  }
  else if (step_phase_2 == 3)
  {
    if ( (color[0] < 100 && color[1] < 100 && color[2] > 150 ) && ( ((machine_rythm - delayval) > 50) && ((machine_rythm - delayval) < 200)))
    {
     step_phase_2 = 4;
     digitalWrite(g_led, HIGH);
    }
  }
  else if (step_phase_2 == 4)
  {
    end_c++;
    Serial.print("end : ");
    Serial.println(end_c);
    if (end_c > 60)
    {
     delay(200);
     end_animation();
     step = 2;
    }
     
  }

 
 moving_dot();
}

void part_3(void)
{
  int static dlay = 200;
  int static current_led = 0;
  
  if (current_led % NUMPIXELS == 0)
    pixels.setPixelColor(NUMPIXELS - 1, 0,0,0);
  else 
    pixels.setPixelColor(current_led % NUMPIXELS - 1, 0,0,0);
   
  pixels.setPixelColor(current_led % NUMPIXELS, 255 ,255 , 255);
   current_led++;
  pixels.show();
  dlay--;
  if (dlay <= 30)
    dlay = 30;
  delay(dlay);
  
}


void loop()
{
  
 if (step == 0)
 {
   part_1();
 }
 else if (step == 1)
 {
  part_2();
 }
 else if (step == 2)
 {
  part_3();
 }
  
}
