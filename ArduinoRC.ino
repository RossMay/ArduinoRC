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

// Reciever Pins
#define CHANNEL_1 21
#define CHANNEL_2 20
#define CHANNEL_3 19
#define CHANNEL_4 18
#define CHANNEL_5 2
#define CHANNEL_6 3

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

// Directions
#define DIRECTION_STOP 0
#define DIRECTION_FORWARD 1
#define DIRECTION_REVERSE 2
#define DIRECTION_RIGHT 3
#define DIRECTION_LEFT 4

// Margin above and below mid throttle still considered idle
#define IDLE_MARGIN



