/*
  Standalone Sketch to use with a Arduino UNO and a
  Sharp Optical Dust Sensor GP2Y1010AU0F
  I2C LCD 1602
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <dht11.h>
#include <Ethernet.h>
#include <SPI.h>

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 }; // RESERVED MAC ADDRESS
EthernetClient client;

dht11 DHT11;
 
#define DHT11PIN 2

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

float temperature = 0; // 温度数据
float humidity = 0;  // 湿度数据

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin(9600);
  setupPm();
  setupTempAndHumidity();

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP"); 
  }
  
}

void loop() {
  getPmData();
  delay (5000);

  // start get temperature and humidity
  getTempAndHumidityData();

  // start post data
  postData();
  
  delay (5000);
}

/**
 * 获取Pm数据setup
 */
void setupPm() {
  pinMode(ledPower, OUTPUT);
  lcd.init();
  // initialize the lcd
  lcd.noBacklight();
  //lcd.backlight();
  lcd.home();
}


/**
 * 获取温度湿度数据
 * DHT11 温湿度传感器
 */
void setupTempAndHumidity() {
  Serial.println("DHT11 TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);
  Serial.println();
}

void getPmData() {
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
}

double Fahrenheit(double celsius) 
{
        return 1.8 * celsius + 32;
}    //摄氏温度度转化为华氏温度
 
double Kelvin(double celsius)
{
        return celsius + 273.15;
}     //摄氏温度转化为开氏温度
 
// 露点（点在此温度时，空气饱和并产生露珠）
// 参考: [url=http://wahiduddin.net/calc/density_algorithms.htm]http://wahiduddin.net/calc/density_algorithms.htm[/url] 
double dewPoint(double celsius, double humidity)
{
        double A0= 373.15/(273.15 + celsius);
        double SUM = -7.90298 * (A0-1);
        SUM += 5.02808 * log10(A0);
        SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
        SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
        SUM += log10(1013.246);
        double VP = pow(10, SUM-3) * humidity;
        double T = log(VP/0.61078);   // temp var
        return (241.88 * T) / (17.558-T);
}
 
// 快速计算露点，速度是5倍dewPoint()
// 参考: [url=http://en.wikipedia.org/wiki/Dew_point]http://en.wikipedia.org/wiki/Dew_point[/url]
double dewPointFast(double celsius, double humidity)
{
        double a = 17.271;
        double b = 237.7;
        double temp = (a * celsius) / (b + celsius) + log(humidity/100);
        double Td = (b * temp) / (a - temp);
        return Td;
}

void getTempAndHumidityData() {
  Serial.println("\n");
 
  int chk = DHT11.read(DHT11PIN);
 
  Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
                Serial.println("OK"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.println("Checksum error"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.println("Time out error"); 
                break;
    default: 
                Serial.println("Unknown error"); 
                break;
  }
 
  Serial.print("Humidity (%): ");
  humidity = DHT11.humidity;
  Serial.println(humidity, 2);
 
  Serial.print("Temperature (oC): ");
  temperature = DHT11.temperature;
  Serial.println(temperature, 2);
 
  Serial.print("Temperature (oF): ");
  Serial.println(Fahrenheit(DHT11.temperature), 2);
 
  Serial.print("Temperature (K): ");
  Serial.println(Kelvin(DHT11.temperature), 2);
 
  Serial.print("Dew Point (oC): ");
  Serial.println(dewPoint(DHT11.temperature, DHT11.humidity));
 
  Serial.print("Dew PointFast (oC): ");
  Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));
  
  lcd.clear();
  
  lcd.print((String)humidity + " %");
  lcd.setCursor(0, 1);

  lcd.print((String)temperature + " oC");
}

void postData() {

  String data = "user_id=1&pm_value=" + (String)dustDensity + "&temperature=" + (String)temperature + "&humidity=" + (String)humidity + "&other=";
  Serial.print(data);

  if (client.connect("www.*****.com",8080)) { // REPLACE WITH YOUR SERVER ADDRESS
    client.println("POST /add.php HTTP/1.1"); 
    client.println("Host: *****.com"); // SERVER ADDRESS HERE TOO
    client.println("Content-Type: application/x-www-form-urlencoded"); 
    client.print("Content-Length: "); 
    client.println(data.length()); 
    client.println(); 
    client.print(data); 
  } 

  if (client.connected()) { 
    client.stop();  // DISCONNECT FROM THE SERVER
  }
  
}

