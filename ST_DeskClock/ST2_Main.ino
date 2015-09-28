/** \file
 * Main loop for Solder:Time Desk Clock.
 *
 * Run continuously after bootup.
 */

// Test for Sleep
static void check_sleep_timer(void)
{
   current_milliseconds = millis();
   option_mode_flag = false;

   if (sleep_enabled && (current_milliseconds - sleep_timer) > SLEEP_LIMIT) {
      // New for ST Desk Clock - goto Time vs Sleep
      if (state == 1) {
         sub_state = 1;
         blink_on_flag = true;
         blink_flag = false;
         blinkMin = false;
         blinkHour = false;
      } else {
         state= 1; // was state= 99;
         sub_state = 0;
         clear_clock_screen();
      }

      sleep_timer = millis();
   }
}

// Test for Mode Button Press
static void check_mode_button(void)
{
   if (digitalRead(MODE_BUTTON)) {
      return;
   }

   if (alarm_is_on) {
      check_alarm();
   }

   if (alarm) {
      alarm = false;
      alarm_is_on = false;
      enable_alarm_1(false);
      state = 90;
      just_woke_up_flag_1 = false;
   } else {
      if (just_woke_up_flag_1) {
         // Used to supress "Time" text from showing when waking up.
         just_woke_up_flag_1 = false;
         just_woke_up_flag_2 = true;
      } else {
         next_state_requested = true;
         clear_clock_screen();
      }
   }

   while (1) {
      // check for simultaneous mode and set buttons
      if (!digitalRead(SET_BUTTON)) {
         option_mode_flag = true;
         next_state_requested = false;
         next_sub_state_requested = false;
         displayString("SPEC");
         delay(300);
      }

      // wait for them to stop holding the button
      if (!digitalRead(MODE_BUTTON)) {
         break;
      }
   }

   delay(100);
   sleep_timer = millis();
}

// Test for SET Button Press
static void check_set_button(void)
{
   if (digitalRead(SET_BUTTON)) {
      return;
   }

   // if the mode button is held down as well, do nothing
   if (option_mode_flag) {
      return;
   }

   next_sub_state_requested = true;
   clear_clock_screen();

   while (1) {
      // this is repeated from above; can be merged?
      if (!digitalRead(MODE_BUTTON)) {
         option_mode_flag = true;
         next_state_requested = false;
         next_sub_state_requested = false;
         displayString("SPEC");
         delay(300);
      }

      // wait for them to stop holding the button
      if (digitalRead(SET_BUTTON)) {
         break;
      }
   }

   delay(100);
   sleep_timer = millis();
}

// Running Blink counter
static void check_blink(void)
{
   if (!blink_flag) {
      // Not blinking, just leave the LEDs lit
      blink_on_flag = true;

      return;
   }

   blinkCounter++;

   if (blinkCounter >BLINK_TIME) { // was 150
      blink_on_flag = !blink_on_flag;
      blinkCounter = 0;
   }
}

void loop() {
   check_sleep_timer();
   check_mode_button();
   check_set_button();
   check_blink();

   // these should be moved into function pointers.
   switch (state) {
   case 0: // Set-Up
      state = 1;

      break;

   case 1: // Display Time
      DisplayTimeSub();

      break;

   case 2: // Set Time
      setTimeSub();

      break;

   case 3: // Config Alarm
      setAlarmSub();

      break;

   case 4: // Stop Watch
      StopWatch();

      break;

   case 5: // Serial Display
      DisplaySerialData();

      break;

   case 6: // Graphic Demo
      graphican();

      break;

   case 90: // Alarm Triggered
      blink_flag = true;
      displayString("Beep");

      if (blink_on_flag) {
         pinMode(SET_BUTTON, OUTPUT);
         tone(SET_BUTTON,4000) ;
         delay(100);
         noTone(SET_BUTTON);
         digitalWrite(SET_BUTTON, HIGH);
      }

#if ARDUINO >= 101
   pinMode(SET_BUTTON, INPUT_PULLUP);
#else
   pinMode(SET_BUTTON, INPUT);
#endif

      delay(250);

      if (next_sub_state_requested || next_state_requested) {
         state = 0;
         sub_state = 0;
         // next_state_flag = true;
         next_state_requested = false;
         next_sub_state_requested = false;
         blink_flag = false;
      }

      break;

   case 99: // Sleep
      displayString("Nite");
      delay(500);
      clear_clock_screen();
      GoToSleep();
      sleep_timer = millis();
      state = 0;
      sub_state = 0;

      break;
   }
}
