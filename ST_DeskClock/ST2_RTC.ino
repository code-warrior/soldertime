/** *********************************************************************************
 *
 *  Check Time
 *
 * ********************************************************************************/
void check_time()
{
   receive(RTC_DS1337,RTC_SEC);
   SecOnes = data_received_on_i2c & B00001111;

   SecTens = data_received_on_i2c & B01110000;
   SecTens = SecTens >> 4;

   receive(RTC_DS1337,RTC_MIN);
   MinOnes = data_received_on_i2c & B00001111;

   MinTens = data_received_on_i2c & B01110000;
   MinTens = MinTens >> 4;

   receive(RTC_DS1337,RTC_HOUR);
   HourOnes = data_received_on_i2c & B00001111;

   TH_Not24_flag = bitRead(data_received_on_i2c, 6); // False on RTC when 24 mode selected
   PM_NotAM_flag = bitRead(data_received_on_i2c, 5);

   if (true == TH_Not24_flag) {
      hour_tens_place = data_received_on_i2c & B00010000;
      hour_tens_place = hour_tens_place >> 4;
   } else {
      hour_tens_place = data_received_on_i2c & B00110000;
      hour_tens_place = hour_tens_place >> 4;
   }
}

/** *********************************************************************************
 *
 *  Check Date
 *
 * ********************************************************************************/
void check_date()
{
   int temp = 0;

   receive(RTC_DS1337, RTC_DAY);
   Days = data_received_on_i2c & B00000111;

   receive(RTC_DS1337, RTC_MONTH);
   MonthCode = data_received_on_i2c & B00001111;

   temp = (data_received_on_i2c & B00010000) >> 4;

   if (temp) {
      MonthCode += 10; // Convert BCD month into integer month
   }

   receive(RTC_DS1337, RTC_DATE);
   DateOnes = data_received_on_i2c & B00001111;
   DateTens = (data_received_on_i2c & B00110000) >> 4;
}

/** *********************************************************************************
 *
 *  Set Time – New
 *
 * both min digits or both hour digits (advance one at a time)
 *
 * ********************************************************************************/
void set_time_new(uint8_t setselect)
{
   uint8_t temp = 0;

   switch (setselect) {
   case 1:
      MinOnes += 1;

      if (MinOnes > 9) {
         MinOnes = 0;

         MinTens += 1;

         if (MinTens > 5) {
            MinTens = 0;
         }

         // temp = (MinTens << 4) + MinOnes;
         // transmit(RTC_DS1337,RTC_MIN,temp);
      }

      temp = (MinTens << 4) + MinOnes;

      transmit(RTC_DS1337, RTC_MIN, temp);

      break;

   case 2:
      HourOnes += 1;

      // 12 hours mode increment
      if (TH_Not24_flag) {
         if(HourOnes > 9) {
            HourOnes = 0;
            hour_tens_place = 1;
         }

      if ((2 == HourOnes) &&  (1 == hour_tens_place)) {
         PM_NotAM_flag = !PM_NotAM_flag;
      }

      if ((HourOnes > 2) &&  (1 == hour_tens_place)) {
         // PM_NotAM_flag = !PM_NotAM_flag;
         hour_tens_place = 0;
         HourOnes = 1;
      }
   } else { // 24 hours mode increment - S
      if ((HourOnes > 9) && (hour_tens_place < 2)) {
         HourOnes = 0;
         hour_tens_place += 1;
      }

      if ((2 == hour_tens_place) && (4 == HourOnes)) {
         HourOnes = 0;
         hour_tens_place = 0;
      }
    }

   // 24 hours mode increment - E

   temp = (hour_tens_place << 4) + HourOnes;

   if (TH_Not24_flag) {
      bitWrite(temp, 5, PM_NotAM_flag);
   }

   bitWrite(temp, 6, TH_Not24_flag);

   transmit(RTC_DS1337, RTC_HOUR, temp);

   break;

   case 3:
      Days += 1;

      if (Days > 7) {
         Days = 1;
      }

      temp = Days & B00000111;

      transmit(RTC_DS1337, RTC_DAY, temp);

      break;

   case 4:
      temp = 0;

      MonthCode += 1;

      if (MonthCode > 12) {
         MonthCode = 1;
      }

      if (MonthCode > 9) {
         temp = MonthCode - 10;

         // MonthCode = MonthCode & B00001111;

         bitSet(temp, 4); // Convert int to BCD
      } else {
         temp = MonthCode & B00001111;
      }

      transmit(RTC_DS1337, RTC_MONTH, temp);

      break;

  case 5: // Date

      //   receive(RTC_DS1337,RTC_DATE);
      //   DateOnes = data_received_on_i2c & B00001111;
      //   DateTens = (data_received_on_i2c & B00110000) >> 4;

      DateOnes = DateOnes + 1;

      if ((3 == DateTens) && (DateOnes > 1)) {
         DateOnes = 1;
         DateTens = 0;
      } else {
         if (DateOnes > 9) {
            DateOnes = 0;
            DateTens += 1;
         }
       }

      temp = (DateOnes & B00001111) | ((DateTens << 4) & B00110000);
      transmit(RTC_DS1337, RTC_DATE, temp);

      break;

   case 6: // year
      break;
   }
}

/** *********************************************************************************
 *
 *  12:00 — Start Time
 *
 *  THIS FUNCTION MIGHT NOT BE BEING USED IN THIS PROJECT.
 *  — Roy Vanegas, 14 August 2015
 *
 * ********************************************************************************/
void set_start_time()
{
   uint8_t temp = 0;
   hour_tens_place = 1;
   HourOnes = 2;
   MinTens = 0;
   MinOnes = 0;
   temp = (hour_tens_place << 4) + HourOnes;
   bitWrite(temp, 5, PM_NotAM_flag);
   bitWrite(temp, 6, TH_Not24_flag);
   transmit(RTC_DS1337, RTC_HOUR, temp);
   temp = (MinTens << 4) + MinOnes;
   transmit(RTC_DS1337, RTC_MIN, temp);
}

/** *********************************************************************************
 *
 *  Set Alarm
 *
 * ********************************************************************************/
void set_alarm_time() // Just for testing set to 12:01 PM
{
   uint8_t temp = 0;
   hour_tens_place = 1;
   HourOnes = 2;
   MinTens = 0;
   MinOnes = 1;
   temp = (hour_tens_place << 4) + HourOnes;
   bitWrite(temp, 5, A_PM_NotAM_flag);
   bitWrite(temp, 6, A_TH_Not24_flag);
   transmit(RTC_DS1337,RTC_ALARM1HOUR,temp);
   temp = (MinTens << 4) + MinOnes;
   transmit(RTC_DS1337, RTC_ALARM1MIN, temp);
}

/** *********************************************************************************
 *
 *  Check Alarm
 *
 * ********************************************************************************/
void check_alarm()
{
   uint8_t temp = 0;
   receive(RTC_DS1337, RTC_STATUS);
   alarm = bitRead(data_received_on_i2c, 0);

   if (alarm) {
      temp = data_received_on_i2c;

      bitClear(temp, 0);

      transmit(RTC_DS1337, RTC_STATUS, temp);
   }
}

/** *********************************************************************************
 *
 *  Enable Alarm
 *
 * ********************************************************************************/
void enable_alarm_1(boolean onoff) // Trigger on Hours & Minutes Match
{
   uint8_t temp = 0;

   // Adjust for Hours - Minutes Trigger -S
   receive(RTC_DS1337, RTC_ALARM1SEC);
   temp = data_received_on_i2c;
   bitClear(temp, 7);
   transmit(RTC_DS1337, RTC_ALARM1SEC, temp);
   receive(RTC_DS1337, RTC_ALARM1MIN);
   temp = data_received_on_i2c;
   bitClear(temp, 7);
   transmit(RTC_DS1337, RTC_ALARM1MIN, temp);
   receive(RTC_DS1337, RTC_ALARM1HOUR);
   temp = data_received_on_i2c;
   bitClear(temp, 7);
   transmit(RTC_DS1337, RTC_ALARM1HOUR, temp);
   receive(RTC_DS1337, RTC_ALARM1DATE);
   temp = data_received_on_i2c;
   bitSet(temp, 7);
   transmit(RTC_DS1337, RTC_ALARM1DATE, temp);

   // Adjust for Hours - Minutes Trigger -E
   receive(RTC_DS1337, RTC_CONT); // Enable Alarm Pin on RTC
   temp = data_received_on_i2c;

   if (onoff) {
      bitSet(temp, 0);
   } else {
      bitClear(temp, 0);
   }

   transmit(RTC_DS1337, RTC_CONT, temp);
   receive(RTC_DS1337, RTC_STATUS); // Clear Alarm RTC internal Alarm Flag
   temp = data_received_on_i2c;
   bitClear(temp, 0);
   transmit(RTC_DS1337, RTC_STATUS, temp);
}

/** *********************************************************************************
 *
 *  Set Alarm Time
 *
 *  both min digits or both hour digits (advance one at a time)
 *
 * ********************************************************************************/
void set_alarm(uint8_t setselect)
{
   uint8_t temp = 0;

   switch (setselect) {
   case 1:
      alarm_minute_ones_place += 1;

      if (alarm_minute_ones_place > 9) {
         alarm_minute_ones_place = 0;

         alarm_minute_tens_place += 1;

         if (alarm_minute_tens_place > 5) {
            alarm_minute_tens_place = 0;
         }
      }

      temp = (alarm_minute_tens_place << 4) + alarm_minute_ones_place;

      transmit(RTC_DS1337, RTC_ALARM1MIN, temp);

      break;

   case 2:
      alarm_hour_ones_place += 1;

      // -----------*
      if (A_TH_Not24_flag) { // 12 hours mode increment
         if (alarm_hour_ones_place > 9) {
            alarm_hour_ones_place = 0;
            alarm_hour_tens_place = 1;
         }

         if ((2 == alarm_hour_ones_place) && (1 == alarm_hour_tens_place)) {
            A_PM_NotAM_flag = !A_PM_NotAM_flag;
         }

         if ((alarm_hour_ones_place > 2) && (1 == alarm_hour_tens_place)) {
            // PM_NotAM_flag = !PM_NotAM_flag;
            alarm_hour_tens_place = 0;
            alarm_hour_ones_place = 1;
         }
      } else { // 24 hours mode increment - S
         if ((alarm_hour_ones_place > 9) && (alarm_hour_tens_place < 2)) {
            alarm_hour_ones_place = 0;
            alarm_hour_tens_place += 1;
         }

         if ((2 == alarm_hour_tens_place) && (4 == alarm_hour_ones_place)) {
            alarm_hour_ones_place = 0;
            alarm_hour_tens_place = 0;
         }
      }

// 24 hours mode increment - E
// -----------*

/*
      if (alarm_hour_ones_place > 9) {
         alarm_hour_ones_place = 0;
         alarm_hour_tens_place += 1;

         if ((alarm_hour_tens_place > 1) && (A_TH_Not24_flag)) {
            alarm_hour_tens_place = 0;
         } else {
            if (alarm_hour_tens_place > 2) {
               alarm_hour_tens_place = 0;
            }
         }
      }
*/

      temp = (alarm_hour_tens_place << 4) + alarm_hour_ones_place;

      if (A_TH_Not24_flag) {
         bitWrite(temp, 5, A_PM_NotAM_flag);
      }

      bitWrite(temp, 6, A_TH_Not24_flag);

      transmit(RTC_DS1337, RTC_ALARM1HOUR, temp);

      break;
   }
}

/** *********************************************************************************
 *
 *  Toggle Twelve- and Twenty-Four hour time
 *
 * ********************************************************************************/
void twelve_twenty_four_convert()
{
   int temphours = 0;
   int temp = 0;

   receive(RTC_DS1337, RTC_HOUR);

   HourOnes = data_received_on_i2c & B00001111;

   // TH_Not24_flag = bitRead(data_received_on_i2c, 6); // False on RTC when 24 mode selected
   // PM_NotAM_flag = bitRead(data_received_on_i2c, 5);

   if (TH_Not24_flag) {
      hour_tens_place = data_received_on_i2c & B00010000;
      hour_tens_place = hour_tens_place >> 4;
   } else {
      hour_tens_place = data_received_on_i2c & B00110000;
      hour_tens_place = hour_tens_place >> 4;
   }

   temphours = HourOnes + (hour_tens_place * 10);// 12 .... 1.2.3...12 or 0 ..1.2.3. ...23

   if (TH_Not24_flag != new_time_format) {
      // NewTimeFormat is same format as TH_Not24_flag where H is 12 and LOW is 24
      if (new_time_format) {
         // ---------------- 24 -> 12
         // Convert into 12 hour clock
         if (temphours >= 12) {
            PM_NotAM_flag = true; // it is in the PM

            if (temphours > 12) {
               /* Convert from 13:00 .... 23:00 to 1:00 ... 11:00
                  [Go from 23:59 / 13:00 to 12:00 to 1:00] ? */
               temphours -= 12;
            } else {
               temphours = temphours; // do nothing it is 12:00
            }
         } else {
            PM_NotAM_flag = false; // it is in the AM - No other conversion needed
         }

         if (0 == temphours) {
            temphours = 12;
         }
      } else { // ---------------- 12 -> 24 // Convert into 24 hour clock
         if ((false == PM_NotAM_flag) && (12 == temphours)) { // AM only check for 00 hours
            temphours = 0;
         }

         if (PM_NotAM_flag == true) { // PM conversion
            if (12 != temphours) { // Leave 12 as 12 in 24h time
               temphours += 12;
            }
         }
      }

      // Common finish conversion section
      TH_Not24_flag = new_time_format;

      hour_tens_place = temphours / 10;
      HourOnes = temphours % 10;

      // ---
      temp = (hour_tens_place << 4) + HourOnes;

      if (TH_Not24_flag) {
         bitWrite(temp, 5, PM_NotAM_flag);
      }

      bitWrite(temp, 6, TH_Not24_flag);
      transmit(RTC_DS1337, RTC_HOUR, temp);
   }
}
