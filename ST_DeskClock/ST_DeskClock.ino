/**
 * Solder:Time Desk Clock
 * SpikenzieLabs.com
 *
 * By: MARK DEMERS, May 2013
 * Updated: Trammell Hudson <hudson@trmm.net>, Feb 2014
 * VERSION 1.1
 *
 * Brief:
 * Sketch used in the Solder:Time Desk Clock Kit. More info and instructions at
 * http://www.spikenzielabs.com/stdc
 *
 * Updated information at http://trmm.net/SolderTime_Desk_Clock
 *
 * LEGAL:
 * This code is provided as is. No guaranties or warranties are given in any form.
 * It is your responsibility to determine this codes suitability for your
 * application.
 *
 * Changes:
 * A. Modified LEDupdateTHREE() void used by ST:2 Watch to function with the new
 *    circuits in the Solder:Time Desk Clock.
 *
 * B. Modified port directions on some pins to deal with new circuits.
 *
 * C. Changed sleep mode into a “change back to display time” mode.
 */

#include <Wire.h>
#include <EEPROM.h>
#include <TimerOne.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// Overall width and height of the LED screen
#define SCREEN_WIDTH 20
#define SCREEN_HEIGHT 8

// Constants
// DS1337+ Address locations
#define RTC_DS1337     B01101000 // was B11010000
#define RTC_CONT       B00001110 //; Control
#define RTC_STATUS     B00001111 //; Status

// #define RTC_HSEC    B00000001 //; Hundredth of a secound
#define RTC_SEC        B00000000 //; Seconds
#define RTC_MIN        B00000001 //; Minuites
#define RTC_HOUR       B00000010 //; Hours
#define RTC_DAY        B00000011 //; Day
#define RTC_DATE       B00000100 //; Date
#define RTC_MONTH      B00000101 //; Month
#define RTC_YEAR       B00000110 //; Year
#define RTC_ALARM1SEC  B00000111 //; Seconds
#define RTC_ALARM1MIN  B00001000 //; Minuites
#define RTC_ALARM1HOUR B00001001 //; Hours
#define RTC_ALARM1DATE B00001010 //; Date

// Buttons
#define MODE_BUTTON    2
#define SET_BUTTON     3

#define MAX_STATE      6

// Offsets
#define DIGITAL_OFFSET 95   // was “int digitoffset = 95” w/a val of 16 at one point
#define ASCII_OFFSET   0x20 // Font offset

#define BLINK_TIME     500  // was 1000
#define SLEEP_LIMIT    6000

boolean target_direction = true;
boolean sound_effect = false;
boolean next_state_requested = false;
boolean next_state_flag = false;
boolean next_sub_state_requested = false;
boolean just_woke_up_flag_1 = false;
boolean just_woke_up_flag_2 = false;
boolean option_mode_flag = false;
boolean blink_flag = false;
boolean blink_on_flag = true;
boolean blinkHour = false;
boolean blinkMin = false;
boolean sleep_enabled = true;
boolean button_value = false;
boolean TH_Not24_flag = true;
boolean new_time_format = TH_Not24_flag;
boolean PM_NotAM_flag = false;
boolean A_TH_Not24_flag = true;
boolean A_PM_NotAM_flag = false;
boolean SetTimeFlag = false;
boolean alarm = false;
boolean alarm_is_on = false;
//boolean MODEOVERRIDEFLAG = false;

uint8_t received_incoming_message = 65;
uint8_t data_received_on_i2c = 0;

int c = 0;
int y = 3;
int target = 3;
int targdist = 0;
int worm_length = 15;
int state = 0;
int sub_state = 0;
int scroll_counter = 0;
int scroll_loops = 3;
int scroll_speed = 300;
int blinkCounter = 0;
int UpdateTime = 0;

// StopWatch
int old_time = 0;
int current_time = 0;
int total_time = 0;

int IncomingIndex = 0;
int IncomingMessIndex = 0;
int IncomingMax = 0;
int MessagePointer = 0;
int StartWindow = 0;
int IncomingLoaded = 0;

//char Str1[] = "Hi";
char IncomingMessage[24];
char MessageRead;
char days[7][4] =
   {
      "Sun","Mon","Tue","Wed","Thu","Fri","Sat"
   };
char months[12][4] =
   {
      "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
   };

// Time Variables
uint8_t hour_tens_place = 1;
uint8_t hour_ones_place = 2;
uint8_t MinTens = 0;
uint8_t MinOnes = 0;
uint8_t SecTens = 0;
uint8_t SecOnes = 0;
uint8_t Days = 1;
uint8_t DateOnes = 1;
uint8_t DateTens = 0;
uint8_t MonthOnes = 1;
uint8_t MonthTens = 1;
uint8_t YearsOnes = 2;
uint8_t YearsTens = 1;
uint8_t DayCode = 1;
uint8_t MonthCode = 1;
uint8_t am_pm_alarm_dots = 0;

// Alarm
uint8_t alarm_hour_tens_place = 1;
uint8_t alarm_hour_ones_place = 2;
uint8_t alarm_minute_tens_place = 0;
uint8_t alarm_minute_ones_place = 0;

// Some stopwatch variables
uint8_t stop_watch_digit_4 = 0;
uint8_t stop_watch_digit_3 = 0;
uint8_t stop_watch_digit_2 = 0;
uint8_t stop_watch_digit_1 = 0;
uint8_t stop_watch_minutes = 0;
uint8_t stop_watch_seconds = 0;

//uint8_t INBYTE;
uint8_t Message[275];

unsigned long sleep_timer;
unsigned long current_milliseconds;

volatile boolean minute_up = false;

extern const uint8_t LETTERS[][5];
extern const uint8_t GRAPHIC[][5];
extern const uint8_t LOGO[][20];
