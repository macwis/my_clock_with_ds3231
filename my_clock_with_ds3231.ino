#line 1 "my_clock_with_ds3231.c.ino"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>

#define DHTPIN 7                                   
#define DHTTYPE DHT22                      
#define fan 4

#include "Arduino.h"
void setup ();
void temp();
void loop ();
#line 16
int maxHum = 60;
int maxTemp = 40;

const int buttonRedPin = 1;
const int buttonGreenPin = 8;
const int buttonYellowPin = 10;
const int buttonBluePin = 6;

DHT dht(DHTPIN, DHTTYPE);

// initialize the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// coursor position tracking
int x = 0;

// Real-time clock initialization
RTC_DS3231 rtc;

char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
char monthsOfTheYear[12][4] = {"Jan", "Feb", "Mar", "Apr",
                               "May", "Jun", "Jul", "Aug",
                               "Sep", "Oct", "Nov", "Dec"};


int current_temp = 0;
int current_temp_dec = 0;
int current_humi = 0;
int state = 0;
DateTime now;

void setup () {  
  #ifndef ESP8266
    while (!Serial);                           
  #endif

  pinMode(buttonRedPin, INPUT);
  pinMode(buttonYellowPin, INPUT);
  pinMode(buttonBluePin, INPUT);
  pinMode(buttonGreenPin, INPUT);

  Serial.begin(9600);
  dht.begin();

  delay(3000);                            

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  /*
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");                                                        
    rtc.adjust(DateTime(__DATE__, __TIME__));                                                 
  }
  
  // Let's set complication date and time
  rtc.adjust(DateTime(__DATE__, __TIME__));
  */
  
  lcd.begin(16, 2);
  setup_big_fonts();
  lcd.clear();
  
  now = rtc.now();
}


void temp() {                                                   
                                                                               
  float h = dht.readHumidity();
  current_humi = round(h);
                                
  float t = dht.readTemperature();
  current_temp = t;
  current_temp_dec = round(t*10) % 10;
  
                                                             
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  if(h > maxHum || t > maxTemp) {
      digitalWrite(fan, HIGH);
  } else {
     digitalWrite(fan, LOW); 
  }
  
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" *C ");

}

void readButtonPress() {
  // TODO
  int buttonRedState, buttonBlueState, buttonYellowState, buttonGreenState;
  buttonRedState = digitalRead(buttonRedPin);
  buttonBlueState = digitalRead(buttonBluePin);
  buttonYellowState = digitalRead(buttonYellowPin);
  buttonGreenState = digitalRead(buttonGreenPin);
  
  if (buttonRedState == LOW) {
    Serial.println("Red pressed");
  }
  if (buttonBlueState == LOW) {
    Serial.println("Blue pressed");
  }
  if (buttonYellowState == LOW) {
    Serial.println("Yellow pressed");
  }
  if (buttonGreenState == LOW) {
    Serial.println("Green pressed");
  }
}

void big_fonts() {
  x = 0;
  lcd.clear();
  digitsndots(now.hour(), now.minute());
}

void full_status() {
  lcd.clear();
  int hour = now.hour();
  if (hour < 10) {
    lcd.print("0");
  }
  lcd.print(hour, DEC);
  lcd.print(':');
  
  int minute = now.minute();
  if (minute < 10) {
    lcd.print("0");
  }
  lcd.print(minute, DEC);
  lcd.print(' ');
  lcd.print(current_temp);
  lcd.print('.');
  lcd.print(current_temp_dec);
  lcd.print((char)223);
  lcd.print('C');
  lcd.print(' ');
  lcd.print(current_humi);
  lcd.print((char)37);

  lcd.setCursor(0, 1);

  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print(' ');
  lcd.print(now.day());
  lcd.print(' ');
  lcd.print(monthsOfTheYear[now.month()-1]);
  lcd.print(' ');
  lcd.print(now.year(), DEC);
}

void loop () {
    now = rtc.now();
    big_fonts();
    delay(15000);
    temp();
    full_status();
    readButtonPress();
    delay(7000);
}
