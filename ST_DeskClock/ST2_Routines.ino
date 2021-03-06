//***********************************************************************************
//                                                 Helper - State incrementor
//***********************************************************************************
// Increments the Main state to the next state up to MAX_STATE
// If you add more States increase MAX_STATE to match.

void NextState()
{
   state += 1;
   sub_state = 0;
   next_state_requested = false;

   if (state > MAX_STATE) {
      state = 0;
      sub_state = 0;
   }
}

//***********************************************************************************
//                                                             Beep the piezo
//***********************************************************************************
// NOTE: the piezo uses the same pin as the SET Button input
// This routine switches the pin to an output during the beeping process
// (There is a current limited resistor on the board to prevent over current if the
// Set Button is pressed at the same time as the pin is driving the piezo.)

void beepsound(int freq, int frequency_length)
{
   // freq was 4000
   // frequency_length was 100
   pinMode(SET_BUTTON, OUTPUT);
   tone(SET_BUTTON, freq, frequency_length);
   delay(frequency_length);
   noTone(SET_BUTTON);
   digitalWrite(SET_BUTTON, HIGH);

#if ARDUINO >= 101
   pinMode(SET_BUTTON, INPUT_PULLUP);
   // digitalWrite(SET_BUTTON, HIGH);
#else
   pinMode(SET_BUTTON, INPUT);
   // digitalWrite(SET_BUTTON, HIGH);
#endif
}

//*******************************************************************************************************************
//                                                       Display Time Routine
//*******************************************************************************************************************
void DisplayTimeSub()
{

  if(next_state_requested)
  {
    sub_state =99;
  }

  if(next_sub_state_requested)
  {
    sub_state = sub_state +1;
    if(sub_state > 4)
    {
      sub_state =1;
    }
    next_sub_state_requested = false;
  }

  UpdateTime = UpdateTime + 1;
  if(UpdateTime > 10000)
  {
    check_time();
    check_date();
    UpdateTime = 0;
  }


  switch (sub_state)
  {
  case 0:                // Start Display Time
    sub_state = 1;
    blink_on_flag = true;
    blink_flag = false;
    blinkMin = false;
    blinkHour = false;

    check_time();
    check_date();

    if(!just_woke_up_flag_2)
    {
    displayString("Time");
    }
    else
    {
      just_woke_up_flag_2 = false;
    }
    delay(250);
    clear_clock_screen();
    break;

  case 1:              // Time

//    check_time();
    writeTime(hour_tens_place, hour_ones_place, MinTens, MinOnes);
    break;

  case 2:              // Day
 //   check_date();
    displayStringDay(Days -1);
    break;

  case 3:              // Month
 //   check_date();
    displayMonth(MonthCode-1);
    break;

  case 4:              // Date
 //   check_date();
    displayDate();
    delay(100);
    break;

  case 5:              // Year

    break;

  case 99:              // Exit Display Time

    NextState();
    clear_clock_screen();

    break;
  }
}

//*******************************************************************************************************************
//                                                       SET Time Routine
//*******************************************************************************************************************
void setTimeSub()
{


  switch (sub_state)
  {
  case 0:                // Start SET Time
    displayString("Set?");
    sub_state = 1;
    next_sub_state_requested = false;


    break;

  case 1:                                                          // Ask user if they want to set time
    if(next_sub_state_requested)
    {
      sub_state = 2;
      next_sub_state_requested = false;
      blink_flag = true;
    }

    if(next_state_requested)
    {
      sub_state =99;
      next_state_requested = false;
    }

    break;

  case 2:                                                          // Minute + one
    //    displayString("----");
    blinkMin = true;
    writeTime(hour_tens_place, hour_ones_place, MinTens, MinOnes);
    if(next_sub_state_requested)
    {
      set_time_new(1);
      next_sub_state_requested = false;
    }

    if(next_state_requested)
    {
      blinkMin = false;
      sub_state =3;
      next_state_requested = false;
    }
    break;

  case 3:                                                          // Hours + one
    blinkHour = true;
    writeTime(hour_tens_place, hour_ones_place, MinTens, MinOnes);
    if(next_sub_state_requested)
    {
      set_time_new(2);
      next_sub_state_requested = false;
    }

    if(next_state_requested)
    {
      blinkHour = false;
      sub_state = 4;
      next_state_requested = false;
    }

    break;

  case 4:                                                          // Day + one
    displayStringDay(Days -1);
    if(next_sub_state_requested)
    {
      set_time_new(3);
      next_sub_state_requested = false;
    }

    if(next_state_requested)
    {
      sub_state = 5;
      next_state_requested = false;
    }

    break;

  case 5:                                                          //  Month + one
    displayMonth(MonthCode-1);
    if(next_sub_state_requested)
    {
      set_time_new(4);
      next_sub_state_requested = false;
    }

    if(next_state_requested)
    {
      sub_state = 6;
      next_state_requested = false;
    }
    break;

  case 6:                                                          //  Date + one
    displayDate();
    if(next_sub_state_requested)
    {
      set_time_new(5);
      next_sub_state_requested = false;
    }
    if(next_state_requested)
    {
      sub_state = 8;
      next_state_requested = false;
    }
    break;

    /*
  case 7:                                                          //  Year + one ** NOTE: not used
     displayString("Year");

     if(next_sub_state_requested)
     {
     set_time_new(6);
     next_sub_state_requested = false;
     }

     if(next_state_requested)
     {
     sub_state =8;
     next_state_requested = false;
     }
     break;
     */

  case 8:
    new_time_format = TH_Not24_flag;                                // Pre-set before toggle
    sub_state = 9;
    break;


  case 9:                                                          // Select 12 or 24 hour clock

    if(new_time_format)
    {
      displayString("12 h");
    }
    else
    {
      displayString("24 h");
    }

    if(next_sub_state_requested)
    {
      new_time_format = !new_time_format;
      next_sub_state_requested = false;
      twelve_twenty_four_convert();
      A_TH_Not24_flag = new_time_format;
    }


    if(next_state_requested)
    {
      sub_state = 99;
      next_state_requested = false;
    }

    break;

  case 99:              // Exit Set Time
    blink_flag = false;
    NextState();
    clear_clock_screen();

    break;
  }
}

//*******************************************************************************************************************
//                                                       SET Alarm Routine
//*******************************************************************************************************************
void setAlarmSub()
{


  switch (sub_state)
  {
  case 0:                // Start SET Alarm
    displayString("ALM?");
    sub_state = 1;
    next_sub_state_requested = false;


    break;

  case 1:                                                          // Ask user if they want to set Alarm
    if(next_sub_state_requested)
    {
      //      displayString("A ON");
      //      delay(250);
      //      alarm_is_on = true;
      sub_state = 2;
      next_sub_state_requested = false;
      blink_flag = true;
    }

    if(next_state_requested)
    {
      //      displayString("AOFF");
      //      delay(500);
      //      enable_alarm_1(false);
      //      alarm_is_on = false;
      sub_state = 99;
      next_state_requested = false;
    }

    break;

  case 2:
    blinkMin = true;
    writeTime(alarm_hour_tens_place, alarm_hour_ones_place, alarm_minute_tens_place, alarm_minute_ones_place);
    if(next_sub_state_requested)
    {
      set_alarm(1);
      next_sub_state_requested = false;
    }

    if(next_state_requested)
    {
      blinkMin = false;
      sub_state = 3;
      next_state_requested = false;
    }
    break;

  case 3:
    blinkHour = true;
    writeTime(alarm_hour_tens_place, alarm_hour_ones_place, alarm_minute_tens_place, alarm_minute_ones_place);
    if(next_sub_state_requested)
    {
      set_alarm(2);
      next_sub_state_requested = false;
    }

    if(next_state_requested)
    {
      blinkMin = false;
      sub_state = 4;
      next_state_requested = false;
      //      enable_alarm_1(true);
    }
    break;

  case 4:
    if(alarm_is_on)
    {
//      displayString("A ON");
      displayGraphic(1,0,5);
      displayGraphic(2,5,5);
      displayGraphic(0,10,4);
      displayGraphic(3,14,5);
    }
    else
    {
//      displayString("AOFF");
      displayGraphic(1,0,5);
      displayGraphic(2,5,5);
      displayGraphic(0,10,4);
      displayGraphic(4,14,5);
    }

    if(next_sub_state_requested)
    {
      alarm_is_on = !alarm_is_on;
      next_sub_state_requested = false;
    }


    if(next_state_requested)
    {
      blinkMin = false;
      sub_state = 99;
      next_state_requested = false;
      if(alarm_is_on)
      {
        enable_alarm_1(true);
      }
      else
      {
        enable_alarm_1(false);
      }
    }

    break;

  case 99:              // Exit Set Alarm
    blink_flag = false;
    NextState();
    clear_clock_screen();

    break;
  }
}

//*******************************************************************************************************************
//                                                       Stop Watch
//*******************************************************************************************************************
void StopWatch()
{

  switch (sub_state)
  {
  case 0:                                                          // Stop Watch Set-up



    old_time = 0;
    current_time = 0;
    total_time = 0;
    stop_watch_digit_4 = 0;
    stop_watch_digit_3 = 0;
    stop_watch_digit_2 = 0;
    stop_watch_digit_1 = 0;

    blink_on_flag = true;
    blink_flag = false;
    blinkMin = false;
    blinkHour = false;

    sub_state = 1;
    next_sub_state_requested = false;
    displayString("Stop");
    delay(500);

    clear_clock_screen();
    writeTime(stop_watch_digit_4, stop_watch_digit_3, stop_watch_digit_2, stop_watch_digit_1);

    break;

  case 1:                                                          // Waiting for "Start" button to be pressed
    writeTime(stop_watch_digit_4, stop_watch_digit_3, stop_watch_digit_2, stop_watch_digit_1);

    if(next_sub_state_requested)
    {
      sub_state = 2;
      next_sub_state_requested = false;
      sleep_enabled = false;
      current_milliseconds = millis();
      sleep_timer = current_milliseconds;                                  // Using Long sleep_timer variable for timing not sleep
    }

    if(next_state_requested)
    {
      sub_state =99;
      next_state_requested = false;
    }

    break;

  case 2:                                                          // Stop Watch Running

 //   receive(RTC_DS1337,RTC_SEC);
 //   current_time =data_received_on_i2c & B00001111;
 current_milliseconds = millis();
    if((current_milliseconds - sleep_timer) >= 1000)
    {
 //     old_time = current_time;
       sleep_timer = current_milliseconds;
      total_time = total_time + 1;
      if(total_time > 5999)                                     // Over 99 minutes can "not" be displayed (60seconds x 99 = 5940)
      {
        total_time = 0;
      }
    }

    // Convert Total Time to digits
    stop_watch_minutes = total_time / 60;
    stop_watch_seconds = total_time % 60;

    stop_watch_digit_4 = stop_watch_minutes / 10;
    stop_watch_digit_3 = stop_watch_minutes % 10;
    stop_watch_digit_2 = stop_watch_seconds / 10;
    stop_watch_digit_1 = stop_watch_seconds % 10;

    writeTime(stop_watch_digit_4, stop_watch_digit_3, stop_watch_digit_2, stop_watch_digit_1);

    if(next_sub_state_requested)
    {
      sub_state = 1;
      next_sub_state_requested = false;
      sleep_enabled = true;
    }

    if(next_state_requested)
    {
      sub_state = 99;
      next_state_requested = false;
    }
    break;

/*
// Not using this RTC version since it seems too power hungery due to constant I2C polling
  case 2:                                                          // Stop Watch Running

    receive(RTC_DS1337,RTC_SEC);
    current_time =data_received_on_i2c & B00001111;
    if(current_time != old_time)
    {
      old_time = current_time;
      total_time = total_time + 1;
      if(total_time > 5940)                                     // Over 99 minutes can "not" be displayed (60seconds x 99 = 5940)
      {
        total_time = 0;
      }
    }

    // Convert Total Time to digits
    stop_watch_minutes = total_time / 60;
    stop_watch_seconds = total_time % 60;

    stop_watch_digit_4 = stop_watch_minutes / 10;
    stop_watch_digit_3 = stop_watch_minutes % 10;
    stop_watch_digit_2 = stop_watch_seconds / 10;
    stop_watch_digit_1 = stop_watch_seconds % 10;

    writeTime(stop_watch_digit_4, stop_watch_digit_3, stop_watch_digit_2, stop_watch_digit_1);

    if(next_sub_state_requested)
    {
      sub_state = 1;
      next_sub_state_requested = false;
      sleep_enabled = true;
    }

    if(next_state_requested)
    {
      sub_state =99;
      next_state_requested = false;
    }
    break;
*/
  case 99:              // Exit Stop Watch Function

    NextState();
    clear_clock_screen();

    sleep_enabled = true;

    break;
  }
}

//*******************************************************************************************************************
//                                                    Display Serial Data
//*******************************************************************************************************************

void DisplaySerialData()
{
  switch (sub_state)
  {

  case 0:                                                          // Display Set-up

    ResetScrollMessage();


    next_sub_state_requested = false;
    next_state_requested = false;
    option_mode_flag = false;
    displayString("Text");
    delay(250);

    GETFROMEEPROM();
    if(IncomingMessIndex == 0 || IncomingMessIndex > 27)
    {
      MessagePointer = 0;
      sub_state = 3;
      char Str2[] = "SpikenzieLabs";
      for(unsigned int i = 0; i <= sizeof(Str2); i++)                            // Show default Scrolling message
      {
        IncomingMessage[i] = Str2[i];
        IncomingMessIndex = i;
      }

    }
    else
    {
      MessagePointer= 0;
      sub_state = 3;
      sleep_enabled = true;
    }

    break;

  case 1:
    if(next_sub_state_requested)
    {
      ResetScrollMessage();

      sub_state = 2;
      next_sub_state_requested = false;
      power_usart0_enable();
      Serial.begin(57600);
      sleep_enabled = false;
    }

    if(next_state_requested)
    {
      sub_state = 99;
      next_state_requested = false;
    }
    break;

    //    break;

  case 2:                                                          // Receive Serial

    clear_clock_screen();
    // LED MATRIX
    //
    // Port C: C0 to C3 set to high. Columns 17 to 20 of LED matrix - Cathode connection
    PORTC = (PORTC & B11110000) | B00001111;
    // Port B: Unselect the MUX chip
    PORTB = (1<<PORTB7);
    // Port B: Set all the ROWs to high: High on both cathode and annode = no current ?
    PORTB = PORTB | B01111111;     // Could be PORTB =B11111111;
    Timer1.detachInterrupt();

shortloop:

    if(Serial.available() > 0)
    {
      MessageRead = Serial.read();

      if(IncomingMessIndex < 24)
      {
        IncomingMessage[IncomingMessIndex] = MessageRead;
        //  IncomingMessIndex = IncomingMessIndex + 1;
        IncomingMessIndex++;
        //   IncomingMessage[IncomingMessIndex] = '\0';
        Serial.print(MessageRead);
      }
    }

    button_value = !digitalRead(SET_BUTTON);
    if(!button_value)
    {
      goto shortloop;
    }
    else

    {
      //Timer1.attachInterrupt(LEDupdateTWO);

      while(button_value)
      {
        button_value = !digitalRead(SET_BUTTON);
      }
      delay(100);
    }

    if(IncomingMessIndex == 0)
    {
      sub_state = 99;
    }
    else
    {
      sub_state = 3;
      FILLEEPROM();
      option_mode_flag = false;


    }
    sleep_timer = millis();
    sleep_enabled = true;
    Serial.end();
    power_usart0_disable();

    break;

  case 3:

    for(int i = 0; i <= IncomingMessIndex - 1; i++)
    {
      received_incoming_message = IncomingMessage[i] - ASCII_OFFSET;
      for(int y = 0; y < 5; y++)
      {
        Message[MessagePointer] = LETTERS[received_incoming_message][y];
        MessagePointer = MessagePointer +1;
      }

      Message[MessagePointer] = 0;                                          // One space between words
      MessagePointer = MessagePointer +1;
      IncomingMax = MessagePointer;
    }


    for(int i = 0; i < 20; i++)                                             // 20 spaces between phrases
    {
      Message[MessagePointer] = 0;
      MessagePointer = MessagePointer +1;
      IncomingMax = MessagePointer;
    }

    sub_state = 4;
    scroll_loops = 3;
    sleep_enabled = true;


    break;

    // ==================================================================================================

  case 4:

    scroll_counter += 1;
    if(scroll_counter > scroll_speed)
    {

      if(scroll_loops > 0)
      {
        sleep_timer = millis();
      }

      IncomingIndex = StartWindow;
      for(int i = 0; i < 20; i++)
      {
   for (int bit = 0, mask = 1 ; bit < 7 ; bit++, mask <<= 1)
   {
      uint8_t bright = Message[IncomingIndex] & mask;
      led_draw(i, bit, bright ? 0xFF : 0);
   }
        IncomingIndex = IncomingIndex + 1;
        if(IncomingIndex>IncomingMax)
        {
          IncomingIndex = 0;                            // Rolled over end of message
    //      scroll_loops = scroll_loops - 1;                  // Used to extend number of messages to scroll before sleep mode reqctivated
        }
      }
      StartWindow = StartWindow + 1;
      if(StartWindow>IncomingMax)
      {
        StartWindow = 0;
        scroll_loops = scroll_loops - 1;                  // Used to extend number of messages to scroll before sleep mode reqctivated
      }

      scroll_counter= 0;
    }

    if(next_sub_state_requested)
    {
      scroll_speed = scroll_speed + 50;
      if(scroll_speed>400)
      {
        scroll_speed = 100;
      }
      scroll_counter= 0;
      next_sub_state_requested = false;
    }

    if(next_state_requested)
    {
      sub_state = 99;
      next_state_requested = false;
    }

    if(option_mode_flag)
    {
      sub_state = 1;
      displayString("NEW?");
      delay(250);
    }
    break;

  case 99:              // Exit Stop Watch Function

    Serial.end();
    power_usart0_disable();

    NextState();
    clear_clock_screen();

    sleep_enabled = true;

    break;
  }
}

void ResetScrollMessage()
{

  IncomingIndex = 0;
  IncomingMax = 0;
  MessagePointer = 0;
  IncomingMessIndex =0;
  StartWindow = 0;
  IncomingLoaded = 0;
  scroll_counter = 0;

  for(int i = 0; i < 275; i++)
  {
    Message[i] = 0;
  }

  for(int i = 0; i < 24; i++)
  {
    IncomingMessage[i] = 0;
  }
}

//*******************************************************************************************************************
//                                                    Graphic Animation
//*******************************************************************************************************************
void graphican()
{

  int temp = 0;
  //  int rand = 0;
  switch (sub_state)
  {

  case 0:
    sub_state = 1;
    scroll_counter = 0;
    scroll_speed = 200;
    //  sound_effect = false;
    y = 3;
    target = 1;
    targdist = 0;
    displayString("Worm");
    delay(250);
    break;

  case 1:

    if(scroll_counter > scroll_speed)
    {

      c += 1;
      if(c > 19)
      {
        c = 0;
      }
      // --


      if(next_sub_state_requested)
      {
        target = target +1;
        if(target> 3)
        {
          target = 0;
          // target_direction = false;
          targdist = 0;

        }
        next_sub_state_requested = false;
      }


      if(target_direction)
      {                                                        // Going up

        if(targdist == target)
        {
          target_direction = !target_direction;
          targdist = - target;
          if(sound_effect)
          {
            beepsound(4000,10);
          }

        }
        else
        {
          targdist = targdist +1;
          y = 3 - targdist;
        }
      }
      else
      {                                                       // Going Down

        if(targdist == target)
        {
          target_direction = !target_direction;
          targdist = - target;
          if(sound_effect)
          {
            beepsound(5000,10);
          }
        }
        else
        {
          targdist = targdist +1;
          y = 3 + targdist;
        }
      }


      bitSet(temp, y);


      // --
      led_draw_col(c, temp, 0xFF);
      if((c-worm_length)<0)
      {
        led_draw_col(19-((worm_length-1)-c), 0, 0);
      }
      else
      {
        led_draw_col(c-worm_length, 0, 0);
      }
      scroll_counter = 0;
    }



    if(next_state_requested)
    {
      sub_state = 99;
      next_state_requested = false;
    }

    if(option_mode_flag)
    {
      sound_effect = !sound_effect;
    }

    scroll_counter += 1;
    break;



  case 99:              // Exit Graphic Function

    NextState();
    clear_clock_screen();

    break;

  }
}

// LED tester
void lamptest()
{
  int lamptestspeed = 250;
  clear_clock_screen();
  button_value = !digitalRead(SET_BUTTON);
  if(button_value)
  {
    do
    {
      //    clear_clock_screen();
      for(int i = 0; i < 20; i++)
      {
        for(int y = 0; y < 8; y++)
        {
     led_draw(i, y, 0xFF);
          delay(lamptestspeed / 10);
          button_value = !digitalRead(SET_BUTTON);
          if(button_value)
          {
            lamptestspeed = lamptestspeed -1;
            if(lamptestspeed== 0)
            {
              lamptestspeed = 250;
            }
          }
        }

        button_value = !digitalRead(MODE_BUTTON);
        if(button_value)
        {
          break;
        }

        delay(lamptestspeed);
   led_draw_col(i, 0, 0);
        delay(lamptestspeed / 5);
      }
      button_value = !digitalRead(MODE_BUTTON);
    }
    while(!button_value);
    }
  }

  //*******************************************************************************************************************
  //                                                                                         Read Message from EEPROM
  //*******************************************************************************************************************
  void GETFROMEEPROM()
  {
    IncomingMessIndex = EEPROM.read(0);

    for(int EEPadd=1; EEPadd < 26; EEPadd++)
    {
      IncomingMessage[EEPadd-1] = EEPROM.read(EEPadd);
    }
  }

//*******************************************************************************************************************
//                                                                                   Save changed Message to EEPROM
//*******************************************************************************************************************
void FILLEEPROM()                                                      // Normally only run once if EEPROM is clear
{
  EEPROM.write(0, IncomingMessIndex);                                  // Holds the message lenght

  for(int EEPadd=1; EEPadd < IncomingMessIndex+1; EEPadd++)
  {

    EEPROM.write(EEPadd, IncomingMessage[EEPadd - 1]);
  }
  // EEPROM.write(25, 1);                                      // 1 is just a number we selected to show EEPROM was writen
}


/*
 * Output 4 Characters to Display
 */
void displayString(const char outText[])
{
   for (int i = 0 ; i < 4 ; i++)
      draw_char(5*i+1, outText[i]);
}


/*
 * Output Custom Graphic to Display
 */
void displayGraphic(int index, int pos, int len)
{
   for (int y = 0 ; y < len ; y++)
      led_draw_col(pos++, GRAPHIC[index][y], 0xFF);
}


static void
draw_small_digit(
   uint8_t column,
   unsigned digit,
   unsigned blinking
)
{
   for (unsigned i = 0 ; i < 4 ; i++)
   {
      led_draw_col(
         column+i,
         LETTERS[digit+DIGITAL_OFFSET][i+1],
         blink_on_flag && blinking ? 0 : 0xFF
      );
   }
}


void
draw_char(
   unsigned col,
   const char c
)
{
   for (int y = 0; y < 5; y++)
      led_draw_col(col++, LETTERS[c - ASCII_OFFSET][y], 0xFF);
}


/*
 * Display Month Text.
 */
void displayMonth(int code)
{
   if (!blink_on_flag)
   {
      clear_clock_screen();
      return;
   }

   draw_char(2, months[code][0]);
   draw_char(8, months[code][1]);
   draw_char(14, months[code][2]);
}


/*
 * Display Day Text
 */
void displayStringDay(int day)
{
   if(!blink_on_flag)
   {
      clear_clock_screen();
      return;
   }

   draw_char(2, days[day][0]);
   draw_char(8, days[day][1]);
   draw_char(14, days[day][2]);
}


/*
 * Display Date.
 */
void displayDate()
{
   if (!blink_on_flag)
   {
      clear_clock_screen();
      return;
   }

   draw_char( 5, DateTens + '0');
   draw_char(11, DateOnes + '0');
}


/*
 * Clear LED screen
 */
void clear_clock_screen()
{
   for (int i = 0; i < SCREEN_WIDTH; i++) {
      led_draw_col(i, 0, 0);
   }
}


/**
 * Display the four digit time with small characters.
 *
 *
 * Fills diplay with formated time
 * Depending on postion of "1"s spacing is adjusted beween it and next digit
 * Blinks if it settng mode
 * displays AM/PM dot and Alarm on dot
 */
void
writeTime(
   uint8_t dig1,
   uint8_t dig2,
   uint8_t dig3,
   uint8_t dig4
)
{
   draw_small_digit( 2, dig1, blinkHour);
   draw_small_digit( 6, dig2, blinkHour);

   // the slowly flashing " : "
   static uint16_t bright = 0;
   uint8_t b = bright++ / 8;
   if (b >= 128)
      b = 0xFF - b;
   led_draw(10, 2, 2*b);
   led_draw(10, 4, 2*b);

   draw_small_digit(12, dig3, blinkMin);
   draw_small_digit(16, dig4, blinkMin);

   am_pm_alarm_dots = 0;

   // Alarm dot (top left) Do not display while setting alarm
   if (alarm_is_on && (state == 1))
      bitSet(am_pm_alarm_dots,6);

   // AM / PM dot (bottom left) (Display or Set Time)
   if(PM_NotAM_flag && (state == 1 || state == 2) && TH_Not24_flag)
      bitSet(am_pm_alarm_dots,0);

   // AM / PM dot (bottom left) (Set Alarm Time)
   if(A_PM_NotAM_flag && (state == 3) && TH_Not24_flag)
      bitSet(am_pm_alarm_dots,0);
}
