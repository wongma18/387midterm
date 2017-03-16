#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

// initialize GPS
const int RXPin = 51;
const int TXPin = 53;

int GPSBaud = 4800;

TinyGPSPlus gps;

SoftwareSerial gpsSerial(RXPin, TXPin);


// initialize the lcd
LiquidCrystal lcd(33, 31, 29, 27, 25, 23);


// initialize the keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {34, 32, 30, 28}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 24, 22}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


static double latitude = 0.0;
static double longitude = 0.0;
double skmph = 0.0;
static double dir = 0.0;
double altm = 0.0;
static double lat1 = 0;
static double lng1 = 0;

int mode = 0;

void setup() {
  // start hardware serial
  Serial.begin(9600);

  // start software serial
  gpsSerial.begin(GPSBaud);

  // start lcd
  lcd.begin(16, 2);
}


void loop() {
  while (gpsSerial.available() > 0)
  {
    if (gps.encode(gpsSerial.read()))
    {


      latitude = gps.location.lat();
      Serial.print("Lat: ");
      Serial.print(latitude, 4);
      Serial.print(", ");
      Serial.print("Lng: ");
      longitude = gps.location.lng();
      Serial.print(longitude, 4);
      Serial.print(", ");
      Serial.print("Alt: ");
      altm = gps.altitude.meters();
      Serial.print(altm);
      Serial.print(", ");
      Serial.print("Crs: ");
      dir = gps.course.deg();
      Serial.print(dir);
      Serial.print(", ");
      Serial.print("Kmph: ");
      skmph = gps.speed.kmph();
      Serial.print(skmph);
      Serial.print(", ");
      Serial.println();
      Serial.print(mode);
      Serial.println();

      // look for key pressed
      char key = keypad.getKey();
      if (key != NO_KEY && key == '#')
      {
        mode = mode + 1;
        if (mode >= 6)
          mode = 0;
        lcd.clear();
        //mode = key;
      }
      else if (key != NO_KEY && key == '*')
      {
        mode = mode - 1;
        if (mode <= 0)
          mode = 5;
        lcd.clear();
        //mode = key;
      }

      switch (mode)
      {
        case 0 :
          mode0();
          break;
        case 1 :
          mode1();
          break;
        case 2 :
          mode2();
          break;
        case 3 :
          mode3();
          break;
        case 4 :
          mode4();
          break;
        case 5 :
          mode5();
          break;
        default :
          mode0();
          break;
      }

      lcd.setCursor(15, 0);
      if (key != NO_KEY) {
        lcd.print(key);
      }
    }
  }
} // end loop

// Default
void mode0()
{
  lcd.setCursor(0, 0);
  lcd.print("GPS READY");
}

// clock
void mode1()
{
  // Date
  lcd.setCursor(0, 0);
  lcd.print(F("Date: "));
  if (gps.date.isValid())
  {
    lcd.print(gps.date.month());
    lcd.print(F("/"));
    lcd.print(gps.date.day());
    lcd.print(F("/"));
    lcd.print(gps.date.year());
  }
  else
  {
    lcd.print(F("INVALID"));
  }

  // Time
  lcd.setCursor(0, 1);
  lcd.print("UT: ");
  lcd.setCursor(4, 1);
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) lcd.print(F("0"));
    lcd.print(gps.time.hour());
    lcd.print(F(":"));
    if (gps.time.minute() < 10) lcd.print(F("0"));
    lcd.print(gps.time.minute());
    lcd.print(F(":"));
    if (gps.time.second() < 10) lcd.print(F("0"));
    lcd.print(gps.time.second());
  }
  else
  {
    lcd.print(F("INVALID"));
  }
} // end clock

// position
void mode2()
{
  lcd.setCursor(0, 0);
  if (gps.location.isValid())
  {
    lcd.print("LAT: ");
    lcd.print(gps.location.lat(), 6);

    lcd.setCursor(0, 1);
    lcd.print("LNG: ");
    lcd.print(gps.location.lng(), 6);
  }
  else
  {
    lcd.print(F("INVALID"));
  }
} // end position

// bearing
void mode3()
{
  lcd.setCursor(0, 0);
  if (gps.speed.isValid())
  {
    lcd.print("SPEED: ");
    lcd.print(gps.speed.mps(), 1);
    lcd.print(" MPS");
  }
  else
  {
    lcd.print("INVALID");
  }

  lcd.setCursor(0, 1);
  if (gps.course.isValid())
  {
    lcd.print("HDG: ");
    lcd.print(gps.course.deg(), 2);
    lcd.print(" DEG");
  }
  else
  {
    lcd.print("INVALID");
  }
} // end bearing

// enter coordinates
// This is not yet functional.
void mode4()
{
  lat1 = enterCord();
  lng1 = enterCord();
  lcd.setCursor(0, 0);
  lcd.print("lat: ");
  lcd.print(lat1);
  lcd.setCursor(0, 1);
  lcd.print("lng: ");
  lcd.print(lng1);
  
} // end enter coordinates

// navigate
void mode5()
{
  double angle = gps.course.deg() - (TinyGPSPlus::courseTo(gps.location.lat(),
    gps.location.lng(), lat1, lng1));
  double distTo = TinyGPSPlus::distanceBetween(gps.location.lat(),
    gps.location.lng(), lat1, lng1);
  Serial.println(angle);

  lcd.setCursor(0, 0);
  if (angle >= -15 && angle <= 15)
  {
    lcd.print("STRAIGHT"); 
  }
  else if (angle < -15)
  {
    lcd.print("RIGHT   ");
  }
  else if ( angle > 15)
  {
    lcd.print("LEFT    ");
  }
  else
  {
    lcd.print("ERROR   ");
  }

  lcd.setCursor(9, 0);
  lcd.print(TinyGPSPlus::courseTo(gps.location.lat(),
    gps.location.lng(), lat1, lng1));
    
  lcd.setCursor(0, 1);
  lcd.print(distTo);
}

// enter coordinate
// This is for mode4() and does not yet work
float enterCord()
{
  
  char key = keypad.getKey();
  int c = 0;
  int sign;
  int d1, d2, d3, d4, d5, d6, d7;
  float coord;
  
  switch (c)
  {
    case '0' :
      if (key != NO_KEY)
        sign = key;
        c++;
      break;
    case '1' :
      if (key != NO_KEY)
        d1 = key;
        c++;
      break;
    case '2' :
      if (key != NO_KEY)
        d2 = key;
        c++;
      break;
    case '3' :
      if (key != NO_KEY)
        d3 = key;
        c++;
        break;
    case '4' :
      if (key != NO_KEY)
        d4 = key;
        c++;
        break;
    case '5' :
      if (key != NO_KEY)
        d5 = key;
        c++;
      break;
    case '6' :
      if (key != NO_KEY)
        d6 = key;
        c++;
      break;
    case '7' :
    if (key != NO_KEY)
        d7 = key;
        c++;
      break;
    default :
      break;
  }

  coord = (100 * d1) + (10 * d2) + d3 + (0.1 * d4) + (0.01 * d5) + (0.001 * d6) + (0.0001 * d7);
  if (sign)
  {
    coord = coord * -1;
  }

  return coord;
} // end enter coordinates
