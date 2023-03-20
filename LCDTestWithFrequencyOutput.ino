#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include "MusicCode.h"
#include <avr/power.h>
//https://forum.arduino.cc/t/reduce-clockspeed-arduino-uno/583224/5

ThreeWire myWire(9,7,12); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

bool Hour24Format = false;bool BuzzerSound = true;int waitTime=500;bool playingMusic = false;bool BuzzerFrequencyControl = false;

unsigned int BuzzerFreqSave = 500;
int LeftB = A6;int RightB = A5;int EnterB = A4;int MenuB = A3;int ClearB = A2;int ButtonThreshold = 50;
int ClockClock=12;int DataClock=20;int ResetClock = 7;
int Buzzer = 6;
int ClockScreen = 4;int LoadScreen = 3;int DataInScreen = 2;int MysteryScreen = A7;int currentLineDisplay = 0;
bool OutputLED[64] = {false};

#define countof(a) (sizeof(a) / sizeof(a[0]))

char timestring[6];
char datestring[4];
bool GreatTen=false;//says if month greater than 10
void AssembleString(const RtcDateTime& dt){
  //creates time and date strings to use later for displaying
  if (dt.Hour()>12 && !Hour24Format){
    snprintf_P(timestring, countof(timestring),PSTR("%02u%02u"),dt.Hour()-12,dt.Minute());
  } else {
    snprintf_P(timestring, countof(timestring),PSTR("%02u%02u"),dt.Hour(),dt.Minute());
  } 
  int MonthSymbol=dt.Month();GreatTen=true;
  if(MonthSymbol>9){ MonthSymbol-=20;GreatTen=true; 
  } else {GreatTen=false;}  
  snprintf_P(datestring, countof(datestring),PSTR("%01u%02u"),MonthSymbol,dt.Day());
}

void SetDisplayOut(int Day, char StringDisplay[7], bool AM, char DateDisplay[4], int EpicOveride = false){
  for (int i=0;i<64;i++){OutputLED[i]=false;}
  OutputLED[Day]=true;OutputLED[10]=true;//Make the dot for hour/minute seperation
  if ((!AM) && (!Hour24Format)){OutputLED[8]=true;}//Make the dot for hour/minute seperation
  if (!BuzzerSound){OutputLED[11]=true;}//Make the dot for hour/minute seperation
  int CharLineStarter=12;
  for (int charDisplay=3;charDisplay>=0;charDisplay--){
    if((StringDisplay[charDisplay]=='0' || StringDisplay[charDisplay]=='O') && (charDisplay!=0 || EpicOveride)){
      OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;
    } else if(StringDisplay[charDisplay]=='1'){
      OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;
    } else if(StringDisplay[charDisplay]=='2'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;
    } else if(StringDisplay[charDisplay]=='3'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;OutputLED[CharLineStarter+3] = true;
    } else if(StringDisplay[charDisplay]=='4'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;
    } else if(StringDisplay[charDisplay]=='5'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+6] = true;
    } else if(StringDisplay[charDisplay]=='6'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+6] = true;
    } else if(StringDisplay[charDisplay]=='7'){
      OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;
    } else if(StringDisplay[charDisplay]=='8'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;
    } else if(StringDisplay[charDisplay]=='9'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;
    } else if(StringDisplay[charDisplay]=='V'){
      OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;
    } else if(StringDisplay[charDisplay]=='L'){
      OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;
    }
    CharLineStarter += 7; 
  }
  CharLineStarter=40;
  for (int charDisplay=2;charDisplay>=0;charDisplay--){
    if(DateDisplay[charDisplay]=='0' || DateDisplay[charDisplay]=='O'){
      OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;
    } else if(DateDisplay[charDisplay]=='1'){
      OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;
    } else if(DateDisplay[charDisplay]=='2'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;
    } else if(DateDisplay[charDisplay]=='3'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;OutputLED[CharLineStarter+3] = true;
    } else if(DateDisplay[charDisplay]=='4'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;
    } else if(DateDisplay[charDisplay]=='5'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+6] = true;
    } else if(DateDisplay[charDisplay]=='6'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+6] = true;
    } else if(DateDisplay[charDisplay]=='7'){
      OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;
    } else if(DateDisplay[charDisplay]=='8'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;
    } else if(DateDisplay[charDisplay]=='9'){
      OutputLED[CharLineStarter] = true;OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;OutputLED[CharLineStarter+6] = true;
    } else if(DateDisplay[charDisplay]=='V'){
      OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;OutputLED[CharLineStarter+4] = true;OutputLED[CharLineStarter+5] = true;
    } else if(DateDisplay[charDisplay]=='L'){
      OutputLED[CharLineStarter+1] = true;OutputLED[CharLineStarter+2] = true;OutputLED[CharLineStarter+3] = true;
    }
    CharLineStarter += 7; 
  }
}

void setup() {
  clock_prescale_set(clock_div_1);


  
  Rtc.Begin();
  Serial.begin(9600);
  //Rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
  //SET UP BUZZER
  pinMode(Buzzer, OUTPUT);  analogWrite(Buzzer, 0);
  PlayMusicStart();
  //SET UP SCREEN OUTPUT PINS
  pinMode(ClockScreen, OUTPUT);digitalWrite(ClockScreen, LOW);pinMode(LoadScreen, OUTPUT);digitalWrite(LoadScreen, LOW);
  pinMode(DataInScreen, OUTPUT);digitalWrite(DataInScreen, LOW);pinMode(MysteryScreen, OUTPUT);digitalWrite(MysteryScreen, HIGH);
  //SET UP SCREEN BUTTON PINS
  pinMode(LeftB, INPUT);pinMode(RightB, INPUT);pinMode(EnterB, INPUT);pinMode(MenuB, INPUT);pinMode(ClearB, INPUT);
  if (!Rtc.GetIsRunning()){Rtc.SetIsRunning(true);}
}

void SetManualTime(int year,int month,int day,int hh,int mm,int ss){
  char userTime[8];
  userTime[0] = hh / 10 + '0';userTime[1] = hh % 10 + '0';userTime[2] = ':';
  userTime[3] = mm / 10 + '0';userTime[4] = mm % 10 + '0';userTime[5] = ':';
  userTime[6] = ss / 10 + '0';userTime[7] = ss % 10 + '0';
  char userDate[12];
  if(month==1){userDate[0]='J';userDate[1]='a';userDate[2]='n';
  } else if(month==2){userDate[0]='F';userDate[1]='e';userDate[2]='b';
  } else if(month==3){userDate[0]='M';userDate[1]='a';userDate[2]='r';
  } else if(month==4){userDate[0]='A';userDate[1]='p';userDate[2]='r';
  } else if(month==5){userDate[0]='M';userDate[1]='a';userDate[2]='y';
  } else if(month==6){userDate[0]='J';userDate[1]='u';userDate[2]='n';
  } else if(month==7){userDate[0]='J';userDate[1]='u';userDate[2]='l';
  } else if(month==8){userDate[0]='A';userDate[1]='u';userDate[2]='g';
  } else if(month==9){userDate[0]='S';userDate[1]='e';userDate[2]='p';
  } else if(month==10){userDate[0]='O';userDate[1]='c';userDate[2]='t';
  } else if(month==11){userDate[0]='N';userDate[1]='o';userDate[2]='v';
  } else if(month==12){userDate[0]='D';userDate[1]='e';userDate[2]='c';
  }
  userDate[3] = ' ';userDate[4] = day / 10 + '0';userDate[5] = day % 10 + '0';
  userDate[6] = ' ';userDate[7] = '2';  userDate[8] = '0';userDate[9] = (year-2000) / 10 + '0';userDate[10] = year % 10 + '0';
  Rtc.SetDateTime(RtcDateTime(userDate, userTime));
}

void changeTime(const RtcDateTime& dt,int amount){
  int hh=dt.Hour();
  int mm=dt.Minute()+amount;
  if (mm<=0){mm=59;hh--;
  } else if (mm>=60){mm=0;hh++;}
  if (hh>24){hh=24;
  } else if (hh<0){hh=0;}
  SetManualTime(dt.Year(),dt.Month(),dt.Day(),hh,mm,0);
}

int prevFreq=0;
int HoldPressThreshhold=5;int HoldPresses=0;
bool MenuButtonOn=false;bool ClearButtonOn=false;bool EnterButtonOn=false;bool PressButton=false;
void ButtonFunctions(const RtcDateTime& dt){
  PressButton=false;
  if(analogRead(LeftB)>ButtonThreshold){
    if (HoldPresses==0 || HoldPresses >HoldPressThreshhold){
      PressButton=true;
      if (BuzzerFrequencyControl){
        if (analogRead(EnterB)>ButtonThreshold){
          BuzzerFreqSave-=1;
        } else {
          BuzzerFreqSave-=100;          
        }
      } else {
        changeTime(dt,-1);
      }
    }
    HoldPresses++;
  } else if(analogRead(RightB)>ButtonThreshold){
    if (HoldPresses==0 || HoldPresses >HoldPressThreshhold){
      PressButton=true;
      if (BuzzerFrequencyControl){
        if (analogRead(EnterB)>ButtonThreshold){
          BuzzerFreqSave+=1;
        } else {
          BuzzerFreqSave+=100;
        }
      } else {
        changeTime(dt,1);
      }
    }
    HoldPresses++;
  } else{
    HoldPresses=0;
  }
  if(analogRead(EnterB)>ButtonThreshold and !BuzzerFrequencyControl){
    if(!EnterButtonOn){
      EnterButtonOn=true;
      playingMusic=!playingMusic;
      analogWrite(Buzzer, 0);
      timeNextNote=0;thisNote=0;
    }
  } else {
    EnterButtonOn=false;
  }
  if(analogRead(MenuB)>ButtonThreshold){
    PressButton=true;
    if(!MenuButtonOn){
      noTone(Buzzer);
      prevFreq=0;
      MenuButtonOn=true;
      BuzzerFrequencyControl=!BuzzerFrequencyControl;
    }
  } else {
    MenuButtonOn=false;
  }
  if(analogRead(ClearB)>ButtonThreshold and !BuzzerFrequencyControl){
    PressButton=true;
    if(!ClearButtonOn){
      ClearButtonOn=true;
      BuzzerSound=!BuzzerSound;
    }
  } else {
    ClearButtonOn=false;
  }
  if (BuzzerFrequencyControl){
    if (prevFreq!=BuzzerFreqSave){
      tone(Buzzer, BuzzerFreqSave);
      delayMicroseconds(10000);    
      prevFreq=BuzzerFreqSave;
    }
  } else if (PressButton && BuzzerSound){
    if (!playingMusic){
      analogWrite(Buzzer, 1);
    }
  } else if (!playingMusic){
    analogWrite(Buzzer, 0);
  }
}

char stringFrequencyOut[5];
char stringFrequencyOutBig[2];
int weekDayMapper[8] = {-1,7,4,2,1,0,63,5};
void loop() {
  if(playingMusic && !BuzzerFrequencyControl){
    PlayMusicLoop(Buzzer);
  }
  currentLineDisplay++;
  if (currentLineDisplay%64==0){
    RtcDateTime now = Rtc.GetDateTime();
    AssembleString(now);
    if (BuzzerFrequencyControl){
      sprintf(stringFrequencyOut, "%04d",(BuzzerFreqSave)%10000);
      Serial.println(stringFrequencyOut);
      sprintf(stringFrequencyOutBig, "%02d", (BuzzerFreqSave)/10000);
      //Serial.println(stringFrequencyOut);
      SetDisplayOut(3,stringFrequencyOut,true,stringFrequencyOutBig,true);//weekDayMapper[now.DayOfWeek()]
    } else {
      SetDisplayOut(weekDayMapper[now.DayOfWeek()],timestring,now.Hour()<=12,datestring);//weekDayMapper[now.DayOfWeek()]
    }
    currentLineDisplay=0;
    ButtonFunctions(now);
    digitalWrite(LoadScreen, HIGH);
  }
  digitalWrite(DataInScreen, OutputLED[currentLineDisplay]);digitalWrite(ClockScreen, HIGH);
  delayMicroseconds(waitTime);
  digitalWrite(ClockScreen, LOW);digitalWrite(DataInScreen, LOW);digitalWrite(LoadScreen, LOW);
  delayMicroseconds(waitTime);
}