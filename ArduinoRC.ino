//                | |
//    G_   _      | |
//    _\\__\\____/   \___//__//_
//   / || ||Au          ( ) ||  \
//   |  _______        _______  |
//   | /   T   \      /   E   \ |
//   | | R + R |      | A + A | |
//   | \___T___/      \___E___/ |
//   |          ______          |
//    \________/      \________/
//

// CHANNEL_1 - Thro
// CHANNEL_2 - Aile
// CHANNEL_3 - Elev
// CHANNEL_4 - Rudd
// CHANNEL_5 - Gear
// CHANNEL_6 - Aux1

//---------Defines---------

// Reciever Pins
#define CHANNEL_1 21
#define CHANNEL_2 20
#define CHANNEL_3 19
#define CHANNEL_4 18
#define CHANNEL_5 2
#define CHANNEL_6 3

#define CHANNEL_1_INTERRUPT 2
#define CHANNEL_2_INTERRUPT 3
#define CHANNEL_3_INTERRUPT 4
#define CHANNEL_4_INTERRUPT 5
#define CHANNEL_5_INTERRUPT 0
#define CHANNEL_6_INTERRUPT 1

// Minimum, maximum and middle values for each channel
// Switches have an upper and lower value, so estimates work ok
#define CHANNEL_1_MIN 1087
#define CHANNEL_1_MID 1500
#define CHANNEL_1_MAX 1912

#define CHANNEL_2_MIN 1086
#define CHANNEL_2_MID 1494
#define CHANNEL_2_MAX 1898

#define CHANNEL_3_MIN 904
#define CHANNEL_3_MID 1192
#define CHANNEL_3_MAX 1482

#define CHANNEL_4_MIN 1107
#define CHANNEL_4_MID 1492
#define CHANNEL_4_MAX 1891

#define CHANNEL_5_MIN 1000
#define CHANNEL_5_MID 1500
#define CHANNEL_5_MAX 2000

#define CHANNEL_6_MIN 1000
#define CHANNEL_6_MID 1500
#define CHANNEL_6_MAX 2000

// Error margin for middle values. Ex. Don't turn unless stick is +- this value past the middle value
#define ERROR_MARGIN 25

// Motor direction pins
#define D_LEFT 12
#define D_RIGHT 7

// Define motor speed pins
#define M_LEFT 5
#define M_RIGHT 6

// Update flags
#define CHANNEL_1_FLAG 1
#define CHANNEL_2_FLAG 2
#define CHANNEL_3_FLAG 4
#define CHANNEL_4_FLAG 8
#define CHANNEL_5_FLAG 16
#define CHANNEL_6_FLAG 32

// Flags to set if a value has changed, such as gear, direction or switches
#define V_DIRECTION_FLAG 1
#define V_GEAR_FLAG 2
#define V_THROTTLE_FLAG 4
#define V_SWITCH_1_FLAG 8
#define V_SWITCH_2_FLAG 16

// Directions
#define DIRECTION_STOP 0
#define DIRECTION_FORWARD 1
#define DIRECTION_REVERSE 2
#define DIRECTION_RIGHT 3
#define DIRECTION_LEFT 4

// Margin above and below mid throttle still considered idle
#define IDLE_MARGIN 60

// Minimum and maximums for PWM values
#define PWM_MIN 0
#define PWM_MAX 255

//---------Globals---------

// Global flags variable. Bits get set per channel when an interrupt was called
volatile uint8_t channelFlagsGlobal;

// Global channel input values
volatile uint16_t channel1InGlobal;
volatile uint16_t channel2InGlobal;
volatile uint16_t channel3InGlobal;
volatile uint16_t channel4InGlobal;
volatile uint16_t channel5InGlobal;
volatile uint16_t channel6InGlobal;

// Global timing for each interrupt. Time since the last state change.
uint32_t channel1Start;
uint32_t channel2Start;
uint32_t channel3Start;
uint32_t channel4Start;
uint32_t channel5Start;
uint32_t channel6Start;

// Values
uint8_t currentThrottle = 0;
uint8_t currentDirection = DIRECTION_STOP;
uint8_t switch1 = false;
uint8_t switch2 = false;

// Flags set when a value above changes
uint8_t valueFlags;

void setup(){
  Serial.begin(9600); 
  Serial.print("Waiting for connection from receiver...");
  
  // Read one of the switch channels to look for a reading within its valid range.
  // If the receiver has not connected with the control yet, the value is likely 0
  // This will wait until the values even out and they connect before continuing
  uint16_t readWait = pulseIn(CHANNEL_5);
  while(readWait > CHANNEL_5_MAX || readWait < CHANNEL_5_MIN){
   readWait = pulseIn(CHANNEL_5);
  }  
  Serial.print("done!");
  
  // Read initial switch values
  switch1 = (pulseIn(CHANNEL_5) > CHANNEL_5_MID) ? true : false;
  switch2 = (pulseIn(CHANNEL_6) > CHANNEL_6_MID) ? true : false;
  
  // Set the two switch flags to show they were updated
  valueFlags |= V_SWITCH_1_FLAG + V_SWITCH_2_FLAG;
  
  // Set up the interrupts
  attachInterrupt(CHANNEL_1_INTERRUPT, processChannel1, CHANGE);
  attachInterrupt(CHANNEL_2_INTERRUPT, processChannel2, CHANGE);
  attachInterrupt(CHANNEL_3_INTERRUPT, processChannel3, CHANGE);
  attachInterrupt(CHANNEL_4_INTERRUPT, processChannel4, CHANGE);
  attachInterrupt(CHANNEL_5_INTERRUPT, processChannel5, CHANGE);
  attachInterrupt(CHANNEL_6_INTERRUPT, processChannel6, CHANGE);
  
  // Set the output pins for the two motor channels
  pinMode(M_LEFT,OUTPUT);
  pinMode(M_RIGHT,OUTPUT);
  
  // Set the output pins for the two motor directions
  pinMode(D_LEFT,OUTPUT);
  pinMode(D_RIGHT,OUTPUT);
}

void loop(){
  
}




