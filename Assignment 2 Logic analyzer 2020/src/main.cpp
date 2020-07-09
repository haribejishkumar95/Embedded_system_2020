/* NAME: Harikrishnan Bejishkumar
 * STUDENT ID: 2193231
 * ASSIGNMENT: ASSIGNMENT 2 LOGIC ANALYZER - CORONA VERSION 2020
 * 
 * This program is created using Adafruit and SPI libraries with Teensey 3.2 microcontroller. This creates an Oscilioscope, Function generator (that has option to 
 * select between RAMP, SINE and TRIANGLE wave) and a Logic Analyzer. 
 * Start button - Used to move to selection screen and used to select the required mode.
 * Right button - Used to move through the modes in the selection screen
 * Left Button - Used to move back to welcome screen from any state.
*/
//Libraries Included
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//defining pins used
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define dacpin A14 // DAC pin
#define adcpin A1 // Analog pin
#define startbuttonpin 14 // Pin for Start button
#define rightbuttonpin 16 // Pin for Right button
#define leftbuttonpin 20 // Pin for Left button
#define potenpin 23// Pin for potentiometer

// Declaration for an SSD1306 display connected to I2C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

uint8_t startbuttonstate = 0;// State that changes throughout the program
uint8_t startbuttonpress = 0;// press count for start button
uint8_t previous_startbuttonpresscount = 0;
uint8_t current_startbuttonpresscount = 0;
uint8_t rightbuttonstate = 0;
uint8_t rightbuttonpress = 0;
uint8_t previous_rightbuttonpresscount = 0;
uint8_t current_rightbuttonpresscount = 0;
uint8_t leftbuttonstate = 0;
uint8_t leftbuttonpress = 0;
uint8_t previous_leftbuttonpresscount = 0;
uint8_t current_leftbuttonpresscount = 0;
uint32_t debounce_delay = 200;// delay to encounter bouncing

float pot_value; // value from the potentiometer

void startbutton();// debouncing algorithm for Start button
void rightbutton();// debouncing algorithm for right button
void leftbutton();// debouncing algorithm for left button
void welcomescreen();// displaying welcome screen
void selectionscreen();//displaying the screen for mode selection
void oscilioscope();//function to show oscillioscope 

uint8_t selection(uint8_t y);// function to select modes
uint8_t logic_analyzer(float data);//functin to show logic analyzer
float display_voltage(float voltage);// function to display voltage on OLED
float triangle_generator(uint8_t function);//function to display triangular wave
float sine_generator(uint8_t function);//function to display sine wave
float ramp_generator(uint8_t function);//function to display ramp function 

//Union to store the floating DAC_Input for logic analyzer
union serial_frame{
  int i;
  float f;
}frame;

void setup() {
  analogWriteResolution(8);// 8 bit resolution for analogWrite
  analogReadResolution(8);// 8 bit resolution for analogRead
  
  Serial.begin(9600);
  
   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){  // Address 0x3D for 128x64
    Serial.println(" Display failed");
    for(;;); //Don't proceed loop, forever
  }
  //defining the input pins, activating the input pull-ups
  pinMode(startbuttonpin, INPUT_PULLUP);
  pinMode(rightbuttonpin, INPUT_PULLUP);
  pinMode(leftbuttonpin, INPUT_PULLUP);
  pinMode(potenpin, INPUT_PULLUP);

  //Calling ISR routines
  attachInterrupt(startbuttonpin, startbutton, FALLING);
  attachInterrupt(rightbuttonpin, rightbutton, FALLING);
  attachInterrupt(leftbuttonpin, leftbutton, FALLING);

}

void loop() {
  static uint8_t dac_in = 0;//Input to DAC pin
  static uint8_t y_pos = 0;//Updating the position of the selection cursor
  frame.f = (float)dac_in; // storing the floating value to the float variable of union through type-casting
  
  //At the Initial state( i.e if nothing is pressed) or if left button is pressed
  if(startbuttonpress == 0){
    welcomescreen();
    rightbuttonstate = 0;
    previous_leftbuttonpresscount = 0;
    leftbuttonpress = 0; 
  }
  // If start button is pressed
  if(startbuttonpress > 0){
    //moves to selection screen
    if(digitalRead(startbuttonpin)){
      selectionscreen();
      display.clearDisplay();
    }
    //if no other button is pressed
    if(rightbuttonstate == 0){
      y_pos = 0;//position of the selection cursor is against oscillioscope
      if(!digitalRead(startbuttonpin)  and y_pos == 0 and startbuttonpress > 1){
        oscilioscope(); //if start button is kept pressed it shows the wave form wrt variation in potentiometer 
      }    
    }
    //Next time right button press will make the selection cursor end up in Function Generator mode, When pressed again(rightbutton press = 2)
    if(rightbuttonstate == 2){
      y_pos = 20;//position of the selection cursor is against ramp generator
      if(!digitalRead(startbuttonpin)  and y_pos == 20){
        ramp_generator(dac_in); //if start button is kept pressed it shows the ramp function
        
      }
    }
    //Next right button press
    if(rightbuttonstate == 3){
      y_pos = 30;//position of the selection cursor is against triangle generator
      if(!digitalRead(startbuttonpin)  and y_pos == 30){
        triangle_generator(dac_in);//if start button is kept pressed it shows the triangle function
      }
    }
    //Next right button press
    if(rightbuttonstate == 4){
      y_pos = 40;//position of the selection cursor is against sine generator
      if(!digitalRead(startbuttonpin)  and y_pos == 40){
        sine_generator(dac_in);//if start button is kept pressed it shows the sine function
      }
    }
    //Next right button press
    if(rightbuttonstate == 5){
      y_pos = 50;
      if(!digitalRead(startbuttonpin)  and y_pos == 50){
        logic_analyzer(frame.f);//position of the selection cursor is against logic analyzer  
      }
    }
    //Next right button press will take the cursor against the initial Oscilioscope mode 
    if(rightbuttonstate > 5){
      y_pos = 0;
      rightbuttonstate = 0;
      startbuttonstate = 0;
    }
    
  }
  //At any stage, the welcome screen can be shown on pressing left button
  if((previous_leftbuttonpresscount != leftbuttonpress)){
     
     welcomescreen();
     rightbuttonstate = 0;
     previous_leftbuttonpresscount = 0;
     leftbuttonpress = 0;
     startbuttonpress = 0;
     
  }
  
  dac_in++;//Incrementing the input to DAC
}

//Function for ramp generator
 float ramp_generator(uint8_t function){
    static uint8_t x = 0;
    display.drawFastVLine(0,0, SCREEN_HEIGHT , WHITE);
    display.drawFastHLine(0,63, SCREEN_WIDTH , WHITE);
    analogWrite(dacpin, function);
    uint32_t ADC_rawvalue = analogRead(adcpin);
    float ADC_voltage = (float)map(ADC_rawvalue, 0, 255, 0, 3300)/1000;
    Serial.println(ADC_voltage);
    display_voltage(ADC_voltage);
    Serial.flush();
    float pixel = (float)map(ADC_voltage, 0, 3.3, 63, 40);
    display.setCursor(0,63);
    display.setTextSize(1);
    display.drawPixel(x , pixel, WHITE);
    display.display();
    if(pixel == 40){
    for(float z = pixel;z <= 63;z++){
      display.drawLine(x, 58, x, 63, WHITE);
      }  
   }
     x++;

    if(x > SCREEN_WIDTH - 8){
        x = 0;
        display.clearDisplay();
      }
   
}

//Function for sine generator
float sine_generator(uint8_t function){
    static uint8_t x = 0;
    display.drawFastVLine(0,0, SCREEN_HEIGHT , WHITE);
    display.drawFastHLine(0,63, SCREEN_WIDTH , WHITE);
    uint32_t sineInput = sin((double)function/10)* 127 + 127;
    analogWrite(dacpin, sineInput);
    uint32_t ADC_rawvalue = analogRead(adcpin);
    float ADC_voltage = (float)map(ADC_rawvalue, 0, 255, 0, 3300)/1000.0;
    Serial.println(ADC_voltage);
    display_voltage(ADC_voltage);
    Serial.flush();
    float pixel = (float)map(ADC_voltage, 0, 3.3, 63, 45);
    display.setCursor(0,63);
    display.setTextSize(1);
    display.drawPixel(x , pixel, WHITE);
    display.display();
    x++;
    if(x > SCREEN_WIDTH - 8){
        x = 0;
        display.clearDisplay();
      }
    
    
}

//Function for triangle generator
float triangle_generator(uint8_t function){
    display.drawFastVLine(0,0, SCREEN_HEIGHT , WHITE);
    display.drawFastHLine(0,63, SCREEN_WIDTH , WHITE);
    static uint8_t x = 0;
    uint8_t top;
    uint8_t descent = 0;
    if(function <= 255 && top == 0){
      top = 0;
      analogWrite(dacpin, function);
      uint32_t ADC_rawvalue = analogRead(adcpin);
      float ADC_voltage = (float)map(ADC_rawvalue, 0, 255, 0, 3300)/1000;
      Serial.println(ADC_voltage);
      display_voltage(ADC_voltage);
      float pixel = (float)map(ADC_voltage, 0, 3.3, 63, 40);
      display.setCursor(0,63);
      display.setTextSize(1);
      display.drawPixel(x , pixel, WHITE);
      display.display();
        x++;
        if(x > 120){
        display.clearDisplay();
        x = 0;
        
      }
     
      if(function == 255){
      top = 1; 
      descent = function;
    }
   } 
    
    while(top == 1){
      display.drawFastVLine(0,0, SCREEN_HEIGHT , WHITE);
      display.drawFastHLine(0,63, SCREEN_WIDTH , WHITE); 
      analogWrite(dacpin, descent);
      uint32_t ADC_rawvalue = analogRead(adcpin);
      float ADC_voltage = (float)map(ADC_rawvalue, 0, 255, 0, 3300)/1000;
      Serial.println(ADC_voltage);
      display_voltage(ADC_voltage);
      float pixel = (float)map(ADC_voltage, 0, 3.3, 63, 40);
      display.setCursor(0,63);
      display.setTextSize(1);
      display.drawPixel(x , pixel, WHITE);
      display.display();
        descent --;
        x++;
 
      if(x > SCREEN_WIDTH - 8){
        x = 0;
        display.clearDisplay();
      }
      if (descent == 0){
        top = 0;
      }
       
    }
      
    
}

//Function for logic analyzer
uint8_t logic_analyzer(float data){
    display.clearDisplay();
    Serial.println(" Bit pattern is: ");
    Serial.println(frame.i, BIN);
    display.setCursor(28,28);
    display.println(frame.i,BIN);
    display.setTextSize(1);
    display.display();
    
    
}
//Function to show WELCOME SCREEN
void welcomescreen(){

   display.clearDisplay();
   display.setTextSize(2.5);
   display.setTextColor(BLACK,WHITE);
   display.setCursor(17,28);
   display.println("WELCOME");
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(13,45);
   display.println("OSCI+FUNC+LOG.AN");
   display.display();

}

//Function to show SELECTION SCREEN
void selectionscreen(){
   static uint8_t y = 0 ;
   display.clearDisplay();
   display.setTextSize(1.5);
   display.setTextColor(WHITE);
   display.setCursor(0,0);
   display.println("* OSCILIOSCOPE");
   display.setTextSize(1.5);
   display.setTextColor(WHITE);
   display.setCursor(0,10);
   display.println("* WAVE GENERATOR");
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(10,20);
   display.println("- RAMP");
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(10,30);
   display.println("- TRIANGLE");
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(10,40);
   display.println("- SINE");
   display.setTextSize(1.5);
   display.setTextColor(WHITE);
   display.setCursor(0,50);
   display.println("*LOGIC ANALYSER");
   selection(y);
   if(rightbuttonpress !=0 ){ 
     rightbuttonpress = 0;
     y = y+10;
     if(y == 60){
      y = 0;
     }
  }
   display.display();
}

//Function to show SELECTION CURSOR
uint8_t selection(uint8_t y){

       display.setTextSize(0.5);
       display.setTextColor(WHITE);
       display.setCursor(110,y);
       display.println("<-");// SELECION CURSOR
       return y;
}

//Function to show Oscilioscope
void oscilioscope(){
  static uint8_t x = 0;
  display.drawFastVLine(0,0, SCREEN_HEIGHT , WHITE);
  display.drawFastHLine(0,63, SCREEN_WIDTH , WHITE);
  pot_value = analogRead(potenpin);
  float voltage = map(pot_value, 0, 60, 0, 3.3);
  float pixel = map(voltage, 0, 3.3, 63, 10);
  display.setCursor(0,63);
  display.setTextSize(1);
  display.drawPixel(x , pixel, WHITE);
  display.display();
  Serial.println(voltage);
  display_voltage(voltage);
  x++;
  if(x > SCREEN_WIDTH - 8){
    x = 0;
    display.clearDisplay();
  }
  
}

//Function to display voltage
float display_voltage(float voltage){
  display.setCursor(100,0);
  display.setTextSize(1);
  display.print(voltage);
  
}

//Debouncing algorithm for Startbutton
void startbutton(){
  static uint32_t previous_startbuttonpress = 0;
  uint32_t current_startbuttonpress = millis();
   previous_startbuttonpresscount = startbuttonpress;
   if((current_startbuttonpress - previous_startbuttonpress)>= debounce_delay){
  
   previous_startbuttonpress = current_startbuttonpress;
   startbuttonpress++;
   startbuttonstate++;
   previous_leftbuttonpresscount = leftbuttonpress;
   previous_rightbuttonpresscount = rightbuttonpress;
   
   }
   current_startbuttonpresscount = startbuttonpress;
}

//Debouncing algorithm for Startbutton
void rightbutton(){
  static uint32_t previous_rightbuttonpress = 0;
  uint32_t current_rightbuttonpress = millis();
   previous_rightbuttonpresscount = rightbuttonpress;
   if((current_rightbuttonpress - previous_rightbuttonpress)>= debounce_delay){
  
   previous_rightbuttonpress = current_rightbuttonpress;
   rightbuttonpress++;
   rightbuttonstate++;
   previous_leftbuttonpresscount = leftbuttonpress;
   previous_startbuttonpresscount = startbuttonpress;
   
   }
   current_rightbuttonpresscount = rightbuttonpress;
}

//Debouncing algorithm for Startbutton
void leftbutton(){
  static uint32_t previous_leftbuttonpress = 0;
  uint32_t current_leftbuttonpress = millis();
    previous_leftbuttonpresscount = leftbuttonpress;
   if((current_leftbuttonpress - previous_leftbuttonpress)>= debounce_delay){
  
   previous_leftbuttonpress = current_leftbuttonpress;
   leftbuttonpress++;
   leftbuttonstate++;
   previous_rightbuttonpresscount = rightbuttonpress;
   previous_startbuttonpresscount = startbuttonpress;
   
   }
   current_leftbuttonpresscount = leftbuttonpress;
}
