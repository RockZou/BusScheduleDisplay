/* 
 * TimeSerial.pde
 * example code illustrating Time library set through serial port messages.
 *
 * Messages consist of the letter T followed by ten digit time (as seconds since Jan 1 1970)
 * you can send the text on the next line using Serial Monitor to set the clock to noon Jan 1 2010
 T1262347200  
 *
 * A Processing example sketch to automatically send the messages is inclided in the download
 */ 
 
#include <Time.h>  

#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 
#include <LiquidCrystal.h>

int theHours[]= 
                {
                 
                 //0,//test value
                 7,7,7,7,7,
                 8,8,8,8,8,8,8,8,
                 9,9,9,9,9,9,9,9,
                 10,10,10,10,10,
                 11,11,11,11,11,
                 12,12,12,12,12,
                 13,13,13,13,
                 14,14,14,14,14,
                 15,15,15,15,15,
                 16,16,16,16,16,16,
                 17,17,17,17,17,17,
                 18,18,18,18,
                 19,19,19,
                 20,20,20,
                 21,21,21,
                 22,22,22,22                 
                };
int CSEhours[]=
                {
                //0,//test value
                7,
                8,8,
                10,
                11,
                13,
                16,16,
                17,
                18
                };
int theMinutes[]=
                {
                  //2,//test value
                  7,27,37,47,54,
                  0,7,10,12,20,25,38,42,
                  1,7,16,22,29,40,42,52,
                  10,22,33,45,55,
                  10,23,32,40,55,
                  10,25,37,50,55,
                  10,25,40,55,
                  10,23,35,47,55,
                  10,25,36,47,57,
                  7,15,18,25,41,50,
                  2,15,22,27,42,58,
                  17,32,42,57,
                  7,26,42,
                  2,22,41,
                  1,20,40,
                  0,20,41,51
                 };
int CSEminutes[]=
                {
                  //3,//test value
                  42,
                  14,59,
                  4,
                  52,
                  8,
                  2,43,//16
                  37,
                  7
                };
int counter=0;
int DTCnumber;//number of buses running to DTC
int CSEcounter=0;
int CSEnumber;//number of buses running to CSE
int DTCbusDate=0;//when the last bus of the day has gone, bus date increment
int CSEbusDate=0;
int i=0;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()  
{
  Serial.begin(9600);
  pinMode(9,OUTPUT);
  lcd.begin(16, 2);
  //setSyncProvider( requestSync);  //set function to call when sync required
  setSyncProvider(0);
  Serial.write(TIME_REQUEST); 
  Serial.println("Waiting for sync message");
  DTCnumber=sizeof(theHours)/sizeof(int);
  CSEnumber=sizeof(CSEhours)/sizeof(int);
}

void loop()
{    
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print("                ");
 
  if(Serial.available() ) 
  {
    processSyncMessage();
  }
  
  analogWrite(9,20);
  analogWrite(10,0);
  
  digitalClockDisplay();
  if(timeStatus()!= timeNotSet)   
  {
    digitalWrite(13,timeStatus() == timeSet); // on if synced, off if needs refresh  
    digitalClockDisplay();
  }
  delay(1000);
}


void blicking()
{
  analogWrite(9,i);
  i=100-i;
}

void digitalClockDisplay()
{

  // digital clock display of the time
  lcd.setCursor(0, 0);
  lcd.print("Now:    ");
  printDigits(hour());
  lcd.print(':');
  printDigits(minute());
  Serial.print(" ");
  //lcd.print(" ");
  /*
  lcd.setCursor(0, 1);
  Serial.print(day());
  lcd.print(day());
  Serial.print(" ");
  lcd.print(" ");
  Serial.print(month());
  lcd.print(month());
  Serial.print(" ");
  lcd.print(" ");
  Serial.print(year()); 
  lcd.print(year());
  Serial.println(); 
  */
 
  
  int tillDTC;
  int tillCSE;
  
  if (DTCbusDate==0)//there are still buses remaining today
  {
    lcd.setCursor(0,1);
    tillDTC=(theHours[counter]*60+theMinutes[counter])-(hour()*60+minute());
    if (tillDTC<=0)
    counter++;
    
    if (counter>=DTCnumber)
    {
      counter=0;
      DTCbusDate++;
    }
  }//(DTCbusDate==0)
  
  
  if (CSEbusDate==0)
  {
    tillCSE=(CSEhours[CSEcounter]*60+CSEminutes[CSEcounter])-(hour()*60+minute());
    if (tillCSE<=0)
    CSEcounter++;
    
    if (CSEcounter>=CSEnumber)
    {
      CSEcounter=0;
      CSEbusDate++;
    }
  }
  
  if ((tillDTC==1)||(tillCSE==1))
  blicking();
  
  if (hour()==0&&minute()==0)
  {
    DTCbusDate=0;
    CSEbusDate=0;
  }
  
  lcd.setCursor(0,1);
  if ((millis()/1000)%10>3)
  printDTC();
  else
  printCSE();

}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if (digits < 10)
    lcd.print('0');
  //6if(digits < 10)
    //Serial.print('0');
  //Serial.print(digits);
  lcd.print(digits);
}

void printDTC()
{
  lcd.print("To DTC: ");
  printDigits(theHours[counter]);
  lcd.print(":");
  printDigits(theMinutes[counter]);
}

void printCSE()
{
  lcd.print("To CSE: ");
  printDigits(CSEhours[CSEcounter]);
  lcd.print(":");
  printDigits(CSEminutes[CSEcounter]);
}

void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of a header and ten ascii digits
    char c = Serial.read() ; 
    Serial.print(c);  
    if( c == TIME_HEADER ) 
    {       
      time_t pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++)
      {   
        c = Serial.read();          
        if( c >= '0' && c <= '9')
        {   
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
        }
      }   
      setTime(pctime);   // Sync Arduino clock to the time received on the serial port
    }
  }
}

/*
time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}
*/


