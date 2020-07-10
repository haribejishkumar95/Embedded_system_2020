## **MG7013 EMBEDDED SYSTEM**

# Assignment2

**Logic Analyser + Minimal Oscilloscope + Signal generator**

Submitted By: **Harikrishnan Bejishkumar**

Student ID: **2193231**

Date: **10/07/2020**

Submitted To: **Frank Beinersdorf**

**ABSTRACT**

The system developed through the project is a microcontroller-based entity which made use of OLED display to act as a combined system for displaying waveforms and bit patterns of the signal.

**AIM**

To implement a microcontroller –based system that provides the following functionality:

- One channel oscilloscope (0V - 3.3V) using the microcontroller ADCperipheral.
- One channel function generator using the microcontroller DAC pin with an option to select between a ramp, triangle, and sinsignal.
- One channel logic analyzer, that allows for decoding of a 9600 baud serialframe.
- For all the modes, the relevant signals should be streamed out in &quot;real-time&quot; via UART (-\&gt; USB) to a connected PC and visualized using a serial plotter and in parallel displayed on the OLED display.
- The control of the system must be possible through push buttons and via commands given through PC.
- The system should be implemented as a Finite state machine.

**PROJECT DESIGN**

The project design consists of Hardware and software model.

In Hardware model, the system is designed using a Teensey 3.2 microcontroller, which is the heart of the system. Teensey is a 72MHz Cortex M4 architectural, USB based microcontroller, used for many different projects. Teensey is powered via USB serial cable connected to the PC. This can be easily programmed via Arduino-IDE, with an add-on, Teenseyduino.

An OLED display, in the class of SSD1306 with an address of 0x3C, is interfaced with it to display the waveforms, being an oscilloscope. SSD1306 OLED used is a monochrome, single chip display driver which communicates to interfaces with I2C protocol (consists of 4 lines, SDA, SCL, VCC, GND). It is provided with a 3.3v from Teensey. It is a 128x64 segments display. The system also consists of 3 push buttons and a 10k potentiometer. The circuit diagram and the design of the system is shown in figure 1 and 2. The serial connection at a baud rate of 9600. The push buttons functions as an input to toggle between the screens or the modes. The potentiometer connected to the hardware, provides the variation in the signal for oscilloscope. The system used the A14/DAC pin and A1 pin to read adc out. The buttons are connected to pins 14, 16, 20. 10k Potentiometer uses 3.3v from Teensey and connects the potentiometer out pin to pin 23 of Teensey.

In Software model, the system is structured as a finite state machine. Visual studio code platform is used to program the system. The program used Wire libraries for the communication to and from between display and microcontroller. The states, actions and events are structured as user-defined functions. The different modes selection represents the states which changes because of actions which are represented by changing the variables (right button state and state button state). The events are represented by the button presses. The right button state and state button state change is due to the change in button presses. The finite state diagram (figure 3) shows the actual working of the robot.

The system in the ON state changes to Welcome screen, when powered. The welcome screen changes to selection screen, only if start button is pressed. The right button can be used to toggle through the modes for selection using the selection cursor. Start button can be kept pressing to show a certain mode. When start button is released, it moves to the selection screen. Left button can be used to move back to home screen. All the buttons are provided with sufficient debouncing delay of 200ms to avoid bouncing of mechanical contacts. The selection is basically done by updating the y pixel of the selection cursor. The Oscilloscope displays the wave corresponding to change in potentiometer voltage. The function generator mode has three wave generators sine, triangle, and ramp. The logical analyzer basically prints the bit pattern for the DAC Input. The wave pattern is also shown simultaneously on a serial plotter via UART on connected PC.

States are:

- Welcome screen
- Selection screen
- Oscilloscope mode
- Sine wave generator
- Triangle wave generator
- Ramp wave generator
- Logic analyzer

Actions are:

- Right button state
- Left button state
- Start button state

Events are:

- Right button pressed
- Start button pressed
- Left button pressed

**CALCULATION**

tr (rise time of SDA &amp; SCL signal) for fast mode = 300ns

VOL max (Low-level output voltage at 3mA) = 0.4V

Cb capacitive load of each bus line = 400pF

Thus, from equation for Pull up resistor RP

RP (min)== = 9.6KΩ

RP (max)== = 885.16Ω

Thus, value of RP of OLED is between 885.16 Ω and 9.6KΩ

**DISCUSSION**

- The first and foremost problem that I faced was creating an algorithm for the wave generators.
- The unfamiliarity with OLED display was the next situation that I had to face with. The learning process took a lot of time with OLED.
- I have used nested if for structuring an finite state machine.
- The slope for the ramp and triangle in OLED was to low, trying to make it, would make the code lengthier and more complex.
- The I2C communication requires pull up resistors, haven&#39;t used external pull up&#39;s because OLED SSD1306 has internal pull up resistors.

**RESULT**

The required output was obtained with the below mentioned rules:

- Implement Start screen, menu screen, and mode screen
- Design and implement as finite state machine
- Apply debouncing for buttons