// constants won't change. They're used here to set pin numbers:
#include<Arduino.h>

#define button 14   // the number of the pushbutton pin
#define led 13      // the number of the LED pin
#define poten_pin 15 // the number of pin in which potentiometer is connected


unsigned long debounceDelay = 200;    // the debounce time; increase if the output flickers

//Variables used to define the led blinking
uint32_t voltage = 0;
float Time = 0;
uint8_t Frequency;

void InterruptCall();// Funtion decleration;Interrupt Service Routine

//codes that executes only once
void setup() 
{

  Serial.print(115200);// one of the baudrate for teensey 3.2 with minimum error
  pinMode(button, INPUT_PULLUP);// defining the input pin;Input_pull up pulls down the pin to +5v, giving a value '1' even without pressing a switch
  pinMode(led, OUTPUT);
  attachInterrupt(button, InterruptCall, FALLING);// Calling the external interrupt; To be active only for falling edge(HIGH TO LOW)
}

void loop()
{
    static uint32_t ledstart_time = 0;// Time in millis starts when led is on and off
    uint32_t runtime = millis();//Time for microcontroller on entering the loop
    uint32_t plotter_time = 0;// Time for the serial plotter to be activated
    uint32_t plotter_lasttime = 0;// To store the last time after the first plot 

    //Analog value of the potentiometer
    voltage =analogRead(poten_pin);
    
    // Mapping the values to a range(1,255)
    Frequency = map(voltage, 1, 1023, 1, 255);

    //Conversion of Frequency to Time period
    // 10000 is choosen to easily distinguish the flicker of led at frequency of 255
    Time = (1.0/Frequency) * 10000.0;
  
    //Code for blink for a time period = Time   
    for(ledstart_time = millis() ;runtime - ledstart_time <=(Time/2);)
    {
      digitalWrite(led, HIGH);//Led is ON
      runtime = millis();
    
    }
    for(ledstart_time= millis() ;runtime - ledstart_time <=(Time/2);)
    {
       digitalWrite(led, LOW);//Led is OFF
       runtime= millis();
            
    }
    
    // Code for the serial plotter; Starts only when, keep on pressing the switch; Plots the frequency
    plotter_time = millis();
    if(!digitalRead(button)&& (plotter_time - plotter_lasttime)>= (100*debounceDelay))
    {  
       Serial.println(Frequency);// Serial print frequency
       plotter_lasttime = plotter_time;
    }
         
}

//Function decleration; Interrupt Service Routine
void InterruptCall()
{  
   
   static unsigned long previous_presstime = 0;// Time when switch is pressed last time
   unsigned long current_presstime = millis();// Time that counts when switch is pressed
   
   // Code to avoid debouncing  
   if((current_presstime - previous_presstime)>= debounceDelay ) 
   { 
        Serial.print(" Blink Frequency :  ");
        Serial.print(Frequency);// Displaying the current frequency
        Serial.println("Hz");
        previous_presstime = current_presstime;
   }
 
     
}