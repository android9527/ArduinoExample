#include <ArduinoHttpClient.h>

/*
  Standalone Sketch to use with a Arduino UNO and a
  Sharp Optical Dust Sensor GP2Y1010AU0F
  I2C LCD 1602
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int measurePin = 0; //Connect dust sensor to Arduino A0 pin
int ledPower = 2;   //Connect 3 led driver pins of dust sensor to Arduino D2

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
int i = 0;

float voMeasured = 0;
float voMeasuredReal = 0;
float voMeasuredLast = 123.0;
float calcVoltage = 0;
float dustDensity = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin(9600);
  pinMode(ledPower, OUTPUT);
  lcd.init();
  // initialize the lcd
  lcd.noBacklight();
  //lcd.backlight();
  lcd.home();
}

void loop() {

  // 0 - 5V mapped to 0 - 1023 integer values
  //calcVoltage = voMeasured/5.0*5.5;
  //dustDensity = 0.17 * calcVoltage - 0.1;

  digitalWrite(ledPower, LOW); // power on the LED
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(measurePin); // read the dust value via pin 5 on the sensor
  voMeasuredReal = voMeasured;
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH); // turn the LED off

  //smooth noise
  voMeasured = voMeasuredLast + (voMeasured - voMeasuredLast) / 10.0;

  if (voMeasured < 150)
    dustDensity = 0.15 * (voMeasured - 130.0);
  else if (voMeasured < 160)
    dustDensity = 3 + 1.7 * (voMeasured - 150.0);
  else if (voMeasured < 220)
    dustDensity = 5 * (voMeasured - 150.0);
  else
    //0.6~3.6 = 123~737 map to 0~500 ug/m3
    dustDensity = voMeasured - 220.0;

  if (dustDensity < 0) dustDensity = 0.0;
  if (dustDensity > 600) dustDensity = 600.0;

  lcd.clear();
  String result = (String)voMeasuredReal + "  ug/m3";
  lcd.print(result);
  lcd.setCursor(0, 1);

  //lcd.print("Volt ");
  //lcd.print(voMeasuredReal);

  lcd.print("PM2.5 ");
  lcd.print(dustDensity);
  //lcd.print(" ");
  //lcd.print(voMeasuredReal);
  Serial.println("dustDensity = " + char(dustDensity));

  voMeasuredLast = voMeasured;
  delayMicroseconds(sleepTime);
  if (i <= 30)delay(1000);
  else delay (5000);
  i++;
  if (i > 1024) i = 31;
}
