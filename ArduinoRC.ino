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
//Accessory Pins
#define KEY_PIN 22
#define LED_PIN 16

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
#define D_LEFT 10
#define D_RIGHT 11

// Define motor speed pins
#define M_LEFT 8
#define M_RIGHT 9

// Update flags
#define CHANNEL_1_FLAG 1
#define CHANNEL_2_FLAG 2
#define CHANNEL_3_FLAG 4
#define CHANNEL_4_FLAG 8
#define CHANNEL_5_FLAG 16
#define CHANNEL_6_FLAG 32

// Flags to set if a value has changed, such as gear, direction or switches
#define V_SWITCH_1_FLAG 1
#define V_SWITCH_2_FLAG 2

// Directions
#define DIRECTION_STOP 0
#define DIRECTION_FORWARD 1
#define DIRECTION_REVERSE 2
#define DIRECTION_RIGHT 3
#define DIRECTION_LEFT 4

// Gears
#define GEAR_IDLE 0
#define GEAR_FULL 1

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
uint8_t curThrottle = 0;
uint8_t curGear = GEAR_IDLE;
uint8_t prevGear = GEAR_IDLE;
uint8_t curDirection = DIRECTION_STOP;
uint8_t prevDirection = DIRECTION_STOP;
uint8_t switch1 = 0;
uint8_t switch2 = 0;

// Flags set when a value above changes
uint8_t valueFlags;

void setup(){
  Serial.begin(9600); 

  // Channel 1 outputs 0 until a connection is present, keep checking until we get a nonzero value
  Serial.print("\nWaiting for connection from receiver...");
  uint16_t readWait = pulseIn(CHANNEL_1, HIGH);
  while(readWait == 0){
    readWait = pulseIn(CHANNEL_1, HIGH);
  } 
  Serial.println("done!");

  // Read initial switch values
  switch1 = (pulseIn(CHANNEL_5, HIGH) > CHANNEL_5_MID) ? true : false;
  switch2 = (pulseIn(CHANNEL_6, HIGH) > CHANNEL_6_MID) ? true : false;

  // Set the two switch flags to show they were updated
  bitSet(valueFlags,V_SWITCH_1_FLAG);
  bitSet(valueFlags,V_SWITCH_2_FLAG);
  
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
  
  // Set pin mode for accessories
  pinMode(KEY_PIN,INPUT_PULLUP);
  pinMode(LED_PIN,OUTPUT);
}

void loop(){
  // Local copies of the input values so they don't get changed by an interrupt mid read
  static uint16_t channel1In;
  static uint16_t channel2In;
  static uint16_t channel3In;
  static uint16_t channel4In;
  static uint16_t channel5In;
  static uint16_t channel6In;

  // Local copy of the global flags
  static uint8_t channelFlags;

  //If any of the channel values have changes, we update our local copy
  if(channelFlagsGlobal){
    noInterrupts();    
    channelFlags = channelFlagsGlobal;

    if(channelFlags & CHANNEL_1_FLAG){
      channel1In = channel1InGlobal; 
    }
    if(channelFlags & CHANNEL_2_FLAG){
      channel2In = channel2InGlobal; 
    }
    if(channelFlags & CHANNEL_3_FLAG){
      channel3In = channel3InGlobal; 
    }
    if(channelFlags & CHANNEL_4_FLAG){
      channel4In = channel4InGlobal; 
    }
    if(channelFlags & CHANNEL_5_FLAG){
      channel5In = channel5InGlobal; 
    }
    if(channelFlags & CHANNEL_6_FLAG){
      channel6In = channel6InGlobal; 
    }

    channelFlagsGlobal = 0;
    interrupts();    
  }

  // Process changed throttle
  if(channelFlags & CHANNEL_3_FLAG){
    // Constrain the value between the channel min and max
    channel3In = constrain(channel3In, CHANNEL_3_MIN, CHANNEL_3_MAX);

    if(channel3In > CHANNEL_3_MID){
      // Map the throttle from mid to max to pwm values 0-255
      curThrottle = map(channel3In,CHANNEL_3_MID,CHANNEL_3_MAX,PWM_MIN,PWM_MAX);
      // Input is more then mid, so we're going forward.
      curDirection = (curThrottle < IDLE_MARGIN)? DIRECTION_STOP : DIRECTION_FORWARD;
    }
    else{
      // Map the throttle from min to mid to pwm values 0-255
      curThrottle = map(channel3In,CHANNEL_3_MIN,CHANNEL_3_MID,PWM_MAX,PWM_MIN);
      // Input is less than mid, so we're in reverse
      curDirection = (curThrottle < IDLE_MARGIN)? DIRECTION_STOP : DIRECTION_REVERSE;
    }
    curGear = (curThrottle < IDLE_MARGIN) ? GEAR_IDLE : GEAR_FULL;
  }

  // Process changed steering, also process when throttle changes because it will overwrite a turn
  if((channelFlags & CHANNEL_2_FLAG) || (channelFlags & CHANNEL_3_FLAG)){
    // Left and right speed values
    uint8_t speedLeft = curThrottle;
    uint8_t speedRight = curThrottle;
    // Constrain the value to its min and max
    channel2In = constrain(channel2In,CHANNEL_2_MIN,CHANNEL_2_MAX);

    // If we're within the idle margin on the throttle
    if(curGear == GEAR_IDLE){
      // If the steering is greater than the mid point plus the error margin, we're going to spin right
      if(channel2In > (CHANNEL_2_MID + ERROR_MARGIN)){
        curDirection = DIRECTION_RIGHT;
        speedRight = speedLeft = map(channel2In,CHANNEL_2_MID,CHANNEL_2_MAX,PWM_MIN,PWM_MAX);
      }
      // If it is less than the mid point minus the error margin, spin left
      else if(channel2In < (CHANNEL_2_MID - ERROR_MARGIN)){
        curDirection = DIRECTION_LEFT;
        speedRight = speedLeft = map(channel2In,CHANNEL_2_MIN,CHANNEL_2_MID,PWM_MAX,PWM_MIN);
      }else{
        // If we're in idle with no steering, we're stopped.
        speedRight = speedLeft = PWM_MIN; 
      }
    }
    // Otherwise, we're moving
    else{
      if(channel2In > (CHANNEL_2_MID + ERROR_MARGIN)){
        speedLeft = map(channel2In,CHANNEL_2_MID,CHANNEL_2_MAX,curThrottle,PWM_MIN);
      }
      else if(channel2In < (CHANNEL_2_MID - ERROR_MARGIN)){
        speedRight = map(channel2In,CHANNEL_2_MIN,CHANNEL_2_MID,PWM_MIN,curThrottle);
      }
    }
    // Set the motors to their respective speeds
    if(digitalRead(KEY_PIN) == HIGH){
      analogWrite(M_LEFT,speedLeft);
      analogWrite(M_RIGHT,speedRight); 
      digitalWrite(LED_PIN,HIGH);
    }else{
      analogWrite(M_LEFT,0);
      analogWrite(M_RIGHT,0); 
      digitalWrite(LED_PIN,LOW);
      return;
    }
  }

  if((curDirection != prevDirection) || (curGear != prevGear)){
    prevGear = curGear;
    prevDirection = curDirection;

    digitalWrite(D_LEFT,LOW);
    digitalWrite(D_RIGHT,LOW);

    switch(curDirection){
      case DIRECTION_FORWARD:
        digitalWrite(D_LEFT,LOW);
        digitalWrite(D_RIGHT,HIGH);
        break;
      case DIRECTION_REVERSE:
        digitalWrite(D_LEFT,HIGH);
        digitalWrite(D_RIGHT,LOW);
        break;
      case DIRECTION_LEFT:
        digitalWrite(D_LEFT,LOW);
        digitalWrite(D_RIGHT,LOW);
        break;
      case DIRECTION_RIGHT:
        digitalWrite(D_LEFT,HIGH);
        digitalWrite(D_RIGHT,HIGH);
        break;
    }
  }
}

// Interrupts for each channel, called on a signal change
void processChannel1(){
  // If we went from LOW to HIGH, start the counter
  if(digitalRead(CHANNEL_1) == HIGH){
    channel1Start = micros();
  }
  else{
    //If we went from HIGH to LOW, calculate the time since the last HIGH then set the value and flag
    channel1InGlobal = (uint16_t)(micros() - channel1Start);
    channelFlagsGlobal |= CHANNEL_1_FLAG;
  }
}

void processChannel2(){
  if(digitalRead(CHANNEL_2) == HIGH){
    channel2Start = micros();
  }
  else{
    channel2InGlobal = (uint16_t)(micros() - channel2Start);
    channelFlagsGlobal |= CHANNEL_2_FLAG;
  }
}

void processChannel3(){
  if(digitalRead(CHANNEL_3) == HIGH){
    channel3Start = micros();
  }
  else{
    channel3InGlobal = (uint16_t)(micros() - channel3Start);
    channelFlagsGlobal |= CHANNEL_3_FLAG;
  }
}

void processChannel4(){
  if(digitalRead(CHANNEL_4) == HIGH){
    channel4Start = micros();
  }
  else{
    channel4InGlobal = (uint16_t)(micros() - channel4Start);
    channelFlagsGlobal |= CHANNEL_4_FLAG;
  }
}

void processChannel5(){
  if(digitalRead(CHANNEL_5) == HIGH){
    channel5Start = micros();
  }
  else{
    channel5InGlobal = (uint16_t)(micros() - channel5Start);
    channelFlagsGlobal |= CHANNEL_5_FLAG;
  }
}

void processChannel6(){
  if(digitalRead(CHANNEL_6) == HIGH){
    channel6Start = micros();
  }
  else{
    channel6InGlobal = (uint16_t)(micros() - channel6Start);
    channelFlagsGlobal |= CHANNEL_6_FLAG;
  }
}








