#include "LedControlMS.h"
#include <Wire.h> 
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleStream.h>
#include <Keypad.h>

#define BLYNK_PRINT SwSerial
#define MAX_LIMIT 500
#define HAPPY_LIMIT 30
#define POKER_LIMIT 15

SoftwareSerial SwSerial(10, 11); // RX, TX
LedControl lc=LedControl(12,11,10,1);
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);  // Set the LCD I2C address

const int trigPin = 8;
const int echoPin = 9;
const int buzzer = 13;

const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad
const byte keyPadSize=16; // number of chars on the keypad
const byte passSize=4; // The size of the password

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]=
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {7,6,5,4}; //Rows 0 to 3
byte colPins[numCols]= {3,2,A2,A3}; //Columns 0 to 3

float longtitiude = 0;
float latitiude =0;

//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

byte keyPadMap[keyPadSize]={'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'}; // Map number to char on the keypad
char realPassword[passSize]={0}; // The real password
byte userPasswordIndex=0;
boolean validPassword=true;
boolean failedLogin=true;

// Shapes
byte happy[8]={0x3c, 0x42, 0xa5, 0x81, 0xa5, 0x99, 0x42, 0x3c};
byte sad[8]={0x3c, 0x42, 0xa5, 0x81,0x99, 0xa5, 0x42, 0x3c};
byte poker[8]={0x3c, 0x42, 0xa5, 0x81,0x81, 0xbd, 0x42, 0x3c};

int lastDistance=-1;

char auth[] = "a29e4036404341f9ba30c7ac27e9fd44";

void setup()
{  
  SwSerial.begin(9600);
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 13 as an output 

  lc.shutdown(0,false);
  lc.setIntensity(0,8); // Set the brightness to a medium values
  lc.clearDisplay(0); // clear the display

  lcd.begin (16,2);
  lcd.clear ();

  Blynk.begin(Serial, auth);

  generatePassword();

  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print("Enter Password:");

  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
}

void loop()
{
  if(failedLogin)
  {
    if (checkPassword())
      failedLogin=false;
    return;
  }
  
  long duration, distance;

  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  
  distance = microsecondsToCentimeters(duration);

  boolean noSameRange = notSameRange(distance,lastDistance);

  if(distance<=3)
  {
    String msg = "Hi, Your SmartReverse device informed us that you made an accident. Location: Longtitude: ";
    msg+=  longtitiude ;
    msg+= "  , Latitude: ";
    msg+= latitiude; 
    Blynk.run(); 
    Blynk.email("Accident", msg);
    distance=2;
    Blynk.run(); 
  }
 
  if(distance <= MAX_LIMIT) // 500 cm
  {
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print(distance);
    lcd.print(" cm");
    tone(buzzer, 1000);
    delay(40);
    noTone(buzzer);

    if(noSameRange)
    {
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      if(distance > HAPPY_LIMIT) // 30 cm
      {
        lcd.print("Far away...");
        writeToMatrix(happy);
      }
      else if(distance > POKER_LIMIT) // 15cm
      {
        lcd.print("Getting close...");
        writeToMatrix(poker);
      }
      else // 0 to 15 cm
      {
        lcd.print("Stop !!!");
        writeToMatrix(sad);
      }
    }

    delay(distance*5-9);
    
  }
  else if(noSameRange) // more than 500 cm
  {
    lcd.clear();
    lc.clearDisplay(0);
  }

    Blynk.run();
    Blynk.virtualWrite(V0, distance);
    lastDistance = distance;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back.
  return microseconds / 29 / 2;
}                                

void writeToMatrix(byte shape[]) {
 for(int row=0;row<8;row++)
  lc.setRow(0,row,shape[row]);
}

boolean notSameRange(int dis0, int dis1)
{
  if(dis0 > 500 && dis1 > 500)
    return false;

  if(dis0 < 500 && dis1 < 500)
  {
    if(dis0 > 30 && dis1 > 30)
      return false;

    if(dis0<30&&dis1<30)
    {
      if(dis0>15&&dis1>15 || dis0<15&&dis1<15)
        return false;
      
    }
  }
  
  return true;
}

boolean checkPassword()
{
  char keypressed = myKeypad.getKey();
  if (keypressed != NO_KEY)
    {
      if(userPasswordIndex == 0)
      {
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);
      }
      if(realPassword[userPasswordIndex] != keypressed)
        validPassword=false;
      lcd.print(keypressed);
      ++userPasswordIndex;
    }
  if(userPasswordIndex == passSize) // Finish to enter password
  {
    if(validPassword)
      return true;

     tone(buzzer, 1000);
     delay(40);
     noTone(buzzer);
     lcd.setCursor(0,1);
     lcd.print("                ");
     lcd.setCursor(0,1);
     lcd.print("Wrong Password...");
     validPassword=true;
     userPasswordIndex=0;
  }
  return false;
}

void generatePassword()
{
  randomSeed(analogRead(0));
  for(int i=0; i<passSize; ++i)
    realPassword[i]=keyPadMap[random(0,keyPadSize)];
  Blynk.run();  
  Blynk.virtualWrite(V1, realPassword); // Send to Blink the password
}


BLYNK_WRITE(V2) 
{
  GpsParam gps(param);
  SwSerial.println(gps.getLat(), 7);
  SwSerial.println(gps.getLon(), 7);
  SwSerial.println(gps.getAltitude(), 2);
  SwSerial.println(gps.getSpeed(), 2);
  longtitiude = gps.getLat();
  latitiude = gps.getLon();
}
