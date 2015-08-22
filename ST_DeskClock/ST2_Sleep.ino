//*******************************************************************************************************************
//                                                          Enter Sleep Mode
//*******************************************************************************************************************

void GoToSleep()
{
      //  SLEEP_MODE_EXT_STANDBY

   // PORTB = (PORTB & B10000000);                           // Clear ROWs and De-select Demux chip
    UltraPowerDown(false);

    attachInterrupt(0, MinuteUP, FALLING);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);                                        //   was   (SLEEP_MODE_EXT_STANDBY);
    cli();
    //      if (some_condition)
    //      {
    sleep_enable();
    //       sleep_bod_disable();
    sei();
    sleep_cpu();
//    _NOP();
//    _NOP();
    sleep_disable();
    //      }
    sei();

    detachInterrupt(0);

    UltraPowerDown(true);

    blinkFlag = false;                                                         // Incase sleep started during time set
//    MODEOVERRIDEFLAG = false;
    next_state_flag = false;

    next_state_requested = false;
    next_sub_state_requested = false;

    SetTimeFlag = false;
//    SetDigit = 4;

    state = 0;
    sub_state = 0;
    just_woke_up_flag_1 = true;

//    check_alarm();
 //   if(alarm)
//    {
//      alarm = false;
//      enable_alarm_1(false);
//      state = 90;
//    }

//    displayString("Wake");
//    delay(1000);
//    clear_clock_screen();

    sleep_timer = millis();


}

//*******************************************************************************************************************
//                                                         MAX power Savings
//*******************************************************************************************************************
void UltraPowerDown(boolean onoff)
{
  if(onoff)                                                    // True = full power mode = ON
  {
//  pinMode(4, OUTPUT);                                      // DeMux A
//  pinMode(5, OUTPUT);                                      // DeMux B
//  pinMode(6, OUTPUT);                                      // DeMux C
//  pinMode(7, OUTPUT);                                      // DeMux D

   power_timer1_enable();                                       // Used for LED matrix refresh
   power_timer0_enable();                                       // Seems required (for delay ?)
   power_timer2_enable();                                       // Seems required for tone (crashes without)

   power_twi_enable();
  }
  else                                                        // False is LOW Power mode
  {
    // LED MATRIX
    //
    // Port C: C0 to C3 set to high. Columns 17 to 20 of LED matrix - Cathode connection
    PORTC = (PORTC & B11110000) | B00001111;

    // Port B: Unselect the MUX chip
    PORTB = (1<<PORTB7);
    // Port B: Set all the ROWs to high: High on both cathode and annode = no current ?
    PORTB = PORTB | B01111111;     // Could be PORTB =B11111111;

 // pinMode(4, INPUT);                                      // DeMux A // Set these to inputs to lower current on mux inputs
 // pinMode(5, INPUT);                                      // DeMux B
 // pinMode(6, INPUT);                                      // DeMux C
//  pinMode(7, INPUT);                                      // DeMux D

    PORTD = (PORTD & B00001111) | B11110000 ;                     // SET all address pins


    // Other Peripherals
    //
   power_timer1_disable();                                       // Used for LED matrix refresh
   power_timer0_disable();                                       // Seems required (for delay ?)
   power_timer2_disable();                                       // Seems required for tone (crashes without)

   power_twi_disable();

   PORTC = PORTC | 1 << PORTC4;                                  // Set SDA and SCL to high so they do not pull current from external pull-up
   PORTC = PORTC | 1 << PORTC5;

  DDRC = DDRC | 1 << PORTC4;                                      // Make SDA and SCL inputs = lower current
  DDRC = DDRC | 1 << PORTC5;                                      //
  }

}

//*******************************************************************************************************************
//                          WAKE-UP - Called by Interrupt 0 on Digital pin 2
//*******************************************************************************************************************

void MinuteUP()
{
  minute_up = true;
}

/*
void MinuteDOWN()
{

}
*/

void ResetSleepCount()
{
      sleep_timer = millis();
}
