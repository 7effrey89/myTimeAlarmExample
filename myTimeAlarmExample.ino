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

#include <TimeLib.h>
#include <TimeAlarms.h>

AlarmId myAlarms[5];
AlarmId id;
AlarmId currentAlarmId;
AlarmId testAlarm;
String incomingSerialText;
String prevIncomingSerialText;

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
}

AlarmId createAlarm(String alarmArrayString){
  int wod = alarmArrayString.substring(0, 1).toInt(); //måske bruge byte som datatype for at spare plads
  int hh = alarmArrayString.substring(1, 3).toInt();
  int mm = alarmArrayString.substring(3, 5).toInt();
  int freq = alarmArrayString.substring(5, 6).toInt();
  int state = alarmArrayString.substring(6, 7).toInt();
  
  Serial.println("Create Alarm with these settings: ");
  Serial.println(wod);
  Serial.println(hh);
  Serial.println(mm);
  Serial.println(freq);
  Serial.println(state);
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

  Serial.print("Alarm read: ");
  Serial.print("day: ");
  Serial.print(day(timeValue)); 
  Serial.print("-");
  Serial.print(month(timeValue));
  Serial.print("-");
  Serial.print(year(timeValue));
  Serial.print(" ");
  Serial.print(hour(timeValue));
  printDigits(minute(timeValue));
  printDigits(second(timeValue));
  Serial.println();
  
  dtAlarmPeriod_t alarmType = Alarm.readType(alarmID);
  Serial.print("alarm type: ");
  Serial.println(alarmType);
   
  Serial.print("time_t read: ");
  Serial.println(timeValue);
   
  return timeValue;
}

// functions to be called when an alarm triggers:
void WeeklyAlarm() {
  Serial.println("Every week day");
}

void Repeats() {
  Serial.println("Every day alarm");
}

void test() {
  Serial.println("test");
}

void OnceOnly() {
  Serial.println("This timer only triggers once, stop the 2 second timer");
  // use Alarm.free() to disable a timer and recycle its memory.
  Alarm.free(id);
  // optional, but safest to "forget" the ID after memory recycled
  id = dtINVALID_ALARM_ID;
  // you can also use Alarm.disable() to turn the timer off, but keep
  // it in memory, to turn back on later with Alarm.enable().
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
}

void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

String printDigitsAsString(int digits) {
  String number;
  if (digits < 10)
    number = "0";
  return number + String(digits);
}

void listAllAlarms() {
  //Prints all the alarms in array
  for (int i=0; i<sizeof(myAlarms); i++) {
    Serial.print("Alarm");
    Serial.print(i);
    readAlarm(myAlarms[i]);
  }
}
void listAllAlarmsThatAreSet() {
  //Works. overvej at sortere baseret på t_time værdi
  //Prints all the alarms in array
  for (int i=0; i<sizeof(myAlarms); i++) {
    if (Alarm.readType(myAlarms[i])!=0) {
      //if alarmtype== dtNotAllocated (or t_time ==-1) then alarm is not set
      Serial.print("Alarm");
      Serial.print(i);
      readAlarm(myAlarms[i]);
    }
  }
}
void assignRandomAlarmID(){
  
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
      Serial.println("b");
      currentAlarmId = Alarm.getTriggeredAlarmId();
      readAlarm(currentAlarmId);
      break;
    case 'd':
      Serial.println("c - disable current alarm");
      disableAlarm(currentAlarmId);
      readAlarm(currentAlarmId);
      break;
    case 'e': //works
     Serial.println("e - hardcoded alarm");
     testAlarm = Alarm.alarmOnce(17,29,30,test);
     readAlarm(testAlarm);
     break;
    case 'f': //Works
     Serial.println("f - hardcoded timer");
     Alarm.timerOnce(2,test); 
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
     listAllAlarmsThatAreSet();
  }
 }
}
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

