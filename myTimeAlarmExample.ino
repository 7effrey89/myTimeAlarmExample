/*
 * TimeAlarmExample.pde
 *
 * This example calls alarm functions at 8:30 am and at 5:45 pm (17:45)
 * and simulates turning lights on at night and off in the morning
 * A weekly timer is set for Saturdays at 8:30:30
 *
 * A timer is called every 15 seconds
 * Another timer is called once only after 10 seconds
 *
 * At startup the time is set to Jan 1 2011  8:29 am
 */

// Questions?  Ask them here:
// http://forum.arduino.cc/index.php?topic=66054.0

//#ifdef dtNBR_ALARMS //if the macro dtNBR_ALARMS is defined 
#pragma push_macro("dtNBR_ALARMS")
#define dtNBR_ALARMS 8//redefine it with the new value

#include <TimeLib.h>
#include <TimeAlarms.h>

#pragma pop_macro("dtNBR_ALARMS")
//#endif 

#define SNOOZE_TIME_MIN 8
AlarmId myAlarms[5];
AlarmId snoozeAlarmID;

AlarmId id;
AlarmId currentAlarmId;
AlarmId testAlarm;
String incomingSerialText;

byte SNOOZE_IS_ACTIVE = 0;
byte ALARM_SOUND_IS_ACTIVE = 0;


void setup() {
  Serial.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor

  /*
   * Alarm Format
   * WeekOfDay: Monday 1, Tuesday 2, Wednesday 3, Thursday 4, Friday 5, Sataurday 6, Sunday 7;
   * Hour: between 01-23;
   * Min: between 01-59;
   * Alarm Frequency: Once 1, Daily 2, Weekly 3;
   * Alarm State: On 1, Off 0;
  */
  
  //Define array of alarms
  //AlarmId myAlarms[] = {alarmID1, alarmID2, alarmID3, alarmID4, alarmID5, alarmID6};
  
  //setTime(hour(), minute(), second(), month(), day(), year());
  setTime(17,29,0,1,1,11); // set time to Saturday 8:29:00am Jan 1 2011

  // create the alarms, to trigger at specific times
  /*Alarm.alarmRepeat(8,30,0, MorningAlarm);  // 8:30am every day
  Alarm.alarmRepeat(17,45,0,EveningAlarm);  // 5:45pm every day
  Alarm.alarmRepeat(dowSaturday,8,30,30,WeeklyAlarm);  // 8:30:30 every Saturday

  // create timers, to trigger relative to when they're created
  Alarm.timerRepeat(15, Repeats);           // timer for every 15 seconds
  id = Alarm.timerRepeat(2, Repeats2);      // timer for every 2 seconds
  Alarm.timerOnce(10, OnceOnly);            // called once after 10 seconds
  */

}

void loop() {
  digitalClockDisplay();
  Alarm.delay(1000); // wait one second between clock display
  Serial.println("jeffrey");
  handleSerial(); //get serial command
  alarmSound();
}

//Alarm method classes
AlarmId createAlarm(String alarmArrayString){
  int wod = alarmArrayString.substring(0, 1).toInt(); //måske bruge byte som datatype for at spare plads
  int hh = alarmArrayString.substring(1, 3).toInt();
  int mm = alarmArrayString.substring(3, 5).toInt();
  int freq = alarmArrayString.substring(5, 6).toInt();
  int state = alarmArrayString.substring(6, 7).toInt();
  
  Serial.println("Create Alarm with these settings: ");
  Serial.print("Week of day: ");
  Serial.print(wod);
  Serial.print(" Time: ");
  Serial.print(hh);
  Serial.print(":");
  Serial.print(mm);
  Serial.print(" Frequency: ");
  Serial.print(freq);
  Serial.print(" On/Off: ");
  Serial.print(state);
  switch (freq) {
    case 1: 
      Serial.println("Once Set: ");
      return Alarm.alarmOnce(hh,mm,00,OnceOnly);
    case 2:
      Serial.println("Daily Set: ");
      return Alarm.alarmRepeat(hh,mm,00,Repeats); 
    case 3:
      Serial.println("Weekly Set: ");
      return Alarm.alarmRepeat(getDow(wod),hh,mm,00,WeeklyAlarm);
  }
}

void disableAlarm(AlarmId alarmID) {
  Alarm.disable(alarmID);
}

long readAlarm(AlarmId alarmID) {
  time_t timeValue = Alarm.read(alarmID);
  dtAlarmPeriod_t alarmType = Alarm.readType(alarmID);

  Serial.print("Alarm read: ");
  printDigits(day(timeValue)); 
  Serial.print("-");
  printDigits(month(timeValue));
  Serial.print("-");
  Serial.print(year(timeValue));
  Serial.print(" ");
  printDigits(hour(timeValue));
  Serial.print(":");
  printDigits(minute(timeValue));
  Serial.print(":");
  printDigits(second(timeValue));
  Serial.println();
  
  Serial.print("alarm type: ");
  Serial.println(alarmType);
   
  Serial.print("time_t read: ");
  Serial.println(timeValue);
   
  return timeValue;
}

void listAllAlarms() {
  //Method returns all the alarms in array - not used
  for (int i=0; i<sizeof(myAlarms); i++) {
    serialPrintOfAlarmID(i);
    readAlarm(myAlarms[i]);
  }
}

void listSetAlarms() {
  //Method returns a list of all set alarms
  for (int i=0; i<sizeof(myAlarms); i++) {
    //only read alarms that are set (alarmtype== dtNotAllocated (enum value 0) or t_time ==-1)
    if (Alarm.readType(myAlarms[i])!=0) {
      serialPrintOfAlarmID(i);
      readAlarm(myAlarms[i]);
    }
  }
}

AlarmId assignRandomAlarmID(){
  //Method returns first free AlarmID
  for (int i=0; i<sizeof(myAlarms); i++) {
    //if alarm is not set for the particular
    if (Alarm.readType(myAlarms[i])==0) {
      serialPrintOfAlarmID(i);
      readAlarm(myAlarms[i]);
      return myAlarms[i];
    }
    Serial.println("No more space for new alarm.");
  }
}

// functions to be called when an alarm triggers:
void OnceOnly() {
  Serial.println("only once alarm");
  ALARM_SOUND_IS_ACTIVE = 1;
  Alarm.free(Alarm.getTriggeredAlarmId());
}

void Repeats() {
  Serial.println("Every day alarm");
  ALARM_SOUND_IS_ACTIVE = 1;
}

void WeeklyAlarm() {
  Serial.println("Every week day");
  ALARM_SOUND_IS_ACTIVE = 1;
}

void startSnooze(){
  //Method only available when Alarm or snooze is due and sounding. Method triggered via snooze button
  if (ALARM_SOUND_IS_ACTIVE == 1) {
    SNOOZE_IS_ACTIVE = 1;
    ALARM_SOUND_IS_ACTIVE = 0;
    snoozeAlarmID = Alarm.timerOnce(SNOOZE_TIME_MIN*1000*60, startAlarmSound);
  }
}

void stopAlarm(){
  //Stop everything.  Method triggered via alarm-off button
  SNOOZE_IS_ACTIVE = 0;
  ALARM_SOUND_IS_ACTIVE = 0;
  Alarm.free(snoozeAlarmID);
}

void startAlarmSound(){
  //Triggered by global variable when Alarm/Snooze alarm is due
  ALARM_SOUND_IS_ACTIVE = 1;
}

void alarmSound() {
  //triggered by global variable
  while (ALARM_SOUND_IS_ACTIVE==1) {
    Serial.println("Beep Beep");
  }
}



//Utility classes
void digitalClockDisplay() {
  //Method displays digital clock time
  printDigits(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
}

void printDigits(int digits) {
  Serial.print(printDigitsAsString(digits));
}

String printDigitsAsString(int digits) {
  String number;
  if (digits < 10)
    number = "0";
  return number + String(digits);
}

void serialPrintOfAlarmID(int i) {
  Serial.print("Alarm[");
  Serial.print(i);
  Serial.print("] : ");
}

void getCurrentAlarm() {
        currentAlarmId = Alarm.getTriggeredAlarmId();
      readAlarm(currentAlarmId);
}


void handleSerial() {
 while (Serial.available() > 0) {
   char incomingCharacter = Serial.read();
//   String incomingCharacterAsString = Serial.readString();
   int minutter = minute()+1;
   String timeSring = "6" +  printDigitsAsString(hour()) + printDigitsAsString(minutter) + "11";
   String timeSringA = "6" +  printDigitsAsString(hour()) + printDigitsAsString(minutter) + "21";
   String timeSringC = "6" +  printDigitsAsString(hour()) + printDigitsAsString(minutter) + "31";
//   Serial.println(incomingCharacterAsString); //læser ikke første bogstav hvis Serial.read() er til
//   if (incomingCharacterAsString=="hej") {
//      incomingSerialText = timeSring; //"7235911";
//      id = createAlarm(incomingSerialText);
//      readAlarm(id);
//   }
   switch (incomingCharacter) {
     case 'a': //alarm once
      Serial.println(timeSring);
      incomingSerialText = timeSring; //"7235911";
      myAlarms[0] = createAlarm(incomingSerialText);
      readAlarm(myAlarms[0]);
      break;
     case 'b': //alarm everyday //work
      Serial.println(timeSringA);
      incomingSerialText = timeSringA; //"7235921";
      myAlarms[1] = createAlarm(incomingSerialText);
      readAlarm(myAlarms[1]);
      break;
     case 'c': //alarm everyday //work
      Serial.println(timeSringC);
      incomingSerialText = timeSringA; //"7235931";
      myAlarms[2] = createAlarm(incomingSerialText);
      readAlarm(myAlarms[2]);
      break;
     case 'z':
      Serial.println("z");
      currentAlarmId = Alarm.getTriggeredAlarmId();
      readAlarm(currentAlarmId);
      break;
     case 'w':
      Serial.println("w");
      Alarm.timerOnce(1, getCurrentAlarm);
      break;
    case 'd':
      Serial.println("c - disable current alarm");
      disableAlarm(currentAlarmId);
      readAlarm(currentAlarmId);
      break;
    case 'g': //works
     Serial.println("g - hardcoded alarm");
     testAlarm = Alarm.alarmRepeat(17,29,30,Repeats);
     readAlarm(testAlarm);
     break;
    case 'h': 
     Serial.println("h - read alarm");
     //alarmID1 = Alarm.timerOnce(2, OnceOnly);      //7-2-2106 6:28:15 = means free
     myAlarms[1] = Alarm.timerOnce(4, OnceOnly);
     myAlarms[2] = Alarm.timerOnce(6, OnceOnly);
     myAlarms[3] = Alarm.timerOnce(8, OnceOnly);
     myAlarms[4] = Alarm.timerOnce(10, OnceOnly);
     
     incomingSerialText = timeSring; //"7235911";
     myAlarms[5]= createAlarm(incomingSerialText);

     Alarm.disable(myAlarms[4]); //forbliver passiv: 1-1-1970 0:00:08
     Alarm.free(myAlarms[5]);    //Slettet: 7-2-2106 6:28:15 = means free

     break;
   case 'q': 
     Serial.println("q - read alarm");
     myAlarms[0] = Alarm.timerOnce(5, OnceOnly);
     break;
    case 'i': //works
     listAllAlarms();
     Serial.println("now print all that are set:");
     listSetAlarms();
  }
 }
}
//enums classes
timeDayOfWeek_t getDow(int dayNo) {
  switch (dayNo) {
        case 1:
            return dowMonday;
        case 2:
            return dowTuesday;
        case 3:
            return dowWednesday;
        case 4:
            return dowThursday;
        case 5:
            return dowFriday;
        case 6:
            return dowSaturday;
        case 7:
            return dowSunday;
    }
}

