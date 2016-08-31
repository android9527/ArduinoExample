#include <Time.h>



#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>


/*
 Yeelink sensor client 
 by zcb
 2012.08.27
 */
#include <EtherCard.h>
#include <dht.h>
#include <LiquidCrystal.h>
#include <Time.h>
 
#define IN 
#define OUT
 
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
 
#define DHT_PIN A0
#define DSM_PIN_10 A4
#define DSM_PIN_25 A2
 
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
////传感器个数及对应传感器ID
#define N 4 
const uint16_t PostingInterval = 5000;
// 发送到服务器时间间隔,N大了可以适当缩短,
//每个传感器发送时间间隔为PostingInterval*N
 
// const uint16_t SensorID[N] = {
//  393394}; // replace your sensor ID
 
const uint8_t MyMac[6] = { 
  0x74,0x69,0x69,0x20,0x07,0xAA };
 
////////////////////////////////////////////////////////////
 
// for yeelink api
// for yeelink api
#define APIKEY         "b9857bd2b879d64c5d25a1be3f195ab0" // 这里用您的yeelink API key替换
#define DEVICEID       350523 // 这就是您yeelink网站上的设备号，请参见Yeelink平台手册获取
#define SENSORID       393394 // 这个更换成您的传感器号码，如何取得请参阅Yeelinkreplace your sensor ID

// char APIKey[] PROGMEM = ""; // replace your yeelink api key here
// char DeviceID[] PROGMEM = "350523"; // replace your device ID
 
 
// 固定的网关
#define website "api.yeelink.net"
// char website[] PROGMEM = "api.yeelink.net";
 
 
uint32_t lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
 
//上传数据的字符串变量及字符串长度变量
char sensorData[N][30];
uint8_t dataLength[N];
 
uint8_t Ethernet::buffer[700];
Stash stash;
 
//ASM2302
DHT sensor = DHT();
 
//轮流向服务器发送相应传感器变量 0 <= i <N,
uint8_t i=0;//
 
//time
boolean timeNeedSet = true;
uint16_t timeSetTimer = 0;
#define TIME_SET_TIMER 4000 //about TIME_SET_TIMER*5s
 
 
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
 
  // start serial port:
  // start the Ethernet connection:
  if (ether.begin(sizeof Ethernet::buffer, MyMac) == 0) 
    Serial.println( "Failed to access Ethernet controller");
 
  Serial.println("Setting up DHCP");
  if (!ether.dhcpSetup())
    Serial.println( "DHCP failed");
 
  ether.printIp("My IP: ", ether.myip);
  ether.printIp("Netmask: ", ether.mymask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);
  ether.dnsLookup(website);
  sensor.attach(DHT_PIN);
 
  pinMode(DSM_PIN_10, INPUT);
 
}
 
void loop() {
  if (!ether.dhcpValid() && !ether.dhcpSetup())
Serial.println("DHCP failed");
  ether.packetLoop(ether.packetReceive());
  if( millis() < lastConnectionTime || millis() - lastConnectionTime > PostingInterval){
    Serial.println(i);
    get_Send_String(i);
    send_Data(i);
    i++;
    if (i == N) i=0;  
  }
  else dsmStatistics();
}
 
void send_Data(uint8_t j) {
  // Create a Post for yeelink server,
  // and send request saving sessionID
  Serial.println(sensorData[j]);
  Serial.println(SensorID[j]);
  Serial.println();
  Stash::prepare(PSTR("POST /v1.0/device/$F/sensor/$D/datapoints HTTP/1.1" "\r\n" 
    "Host: api.yeelink.net" "\r\n" 
    "U-ApiKey: $F" "\r\n" 
    "Content-Length: $D" "\r\n"
    "Content-Type: application/x-www-form-urlencoded" "\r\n" "\r\n"                       
    "$S" "\r\n"),
  DeviceID,SensorID[i],APIKey,dataLength[i],sensorData[i]);
 
  if(timeNeedSet){
    setupTime();      
  }
  else{
    ether.tcpSend();
    displayClock();
  }
  lastConnectionTime = millis();  
}
 
void switchtoEthercard(){
  word len;
  do{
    len = ether.packetReceive();
    ether.packetLoop(len);
  }
  while(len != 0);
}

#define REQEST_TIMEOUT 10000
 
void setupTime(){
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("Update time...");
  uint8_t session_id;
  session_id = ether.tcpSend();
  uint32_t requestTimer = millis() + REQEST_TIMEOUT;
  while(millis() < requestTimer){
    ether.packetLoop(ether.packetReceive());
    char bufx[21];
    const char* reply = ether.tcpReply(session_id);
    if(reply !=0){
      for(uint8_t I=0;I<21;I++){
        bufx[I]=reply[50+I];       
      }
      tmElements_t time1;
      timeconvert(bufx,&time1);
      time_t second = makeTime(time1);        
      setTime(second);
      adjustTime(8*60*60);
      lcd.print("OK");
      timeNeedSet=false;
      break;
    }
  }
}
void displayClock(){
  // digital clock display of the time
  timeSetTimer++;
  if (timeSetTimer>TIME_SET_TIMER) {
    timeSetTimer=0;
    timeNeedSet=true;
  }
  lcd.setCursor(0,1);
  lcd.print(year());
  lcd.print("-");
  printDigits(month());
  lcd.print("-");
  printDigits(day());
  lcd.print(" ");
  printDigits(hour());
  lcd.print(":");
  printDigits(minute());
}
void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10)
    lcd.print("0");
  lcd.print(digits);
}

//比较周数，成功返回0-6的数，错误返回7
//p代表周数，取周数前3个字母，如Mon代表周1，以此类推
//改动周几不影响返回的时间值，可以通过改动日期的日数来达到修改时间
 
//比较月份，成功返回0-11的数，错误返回12
//P 为月份的前三个字母，如Feb代表二月，以此类推
int monthcmp( IN char *p)
{
  char *month[]={
    "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"            };
 
  int i;
  for(i=0;i<12;i++){
    if(strcmp(p,month[i])==0)
      break;
  }
  if(i==12)
  {
    return i;
  }
  return i;
}
//将字串格式的时间转换为结构体,返回距离1970年1月1日0：0：0的秒数，当字符串格式错误或超值时返回0
//BUF 为类似Tue May 15 14:46:02 2007格式的，p为时间结构体
void timeconvert(IN char *buf,OUT tmElements_t *p)
{
  char cmonth[4];
  int16_t td,th,tm,ts,ty;
  sscanf(buf,"%d %s %d %d:%d:%d",&td,cmonth,&ty,&th,&tm,&ts);
  //sscanf("2012 Aug " ,"%4d %s",&ty,cmonth);
  p->Year = ty - 1970;
  p->Month = monthcmp(cmonth) + 1;
  p->Day = td;
  p->Hour = th;
  p->Minute = tm;
  p->Second = ts;
}

//dsm501统计变量
uint32_t dsmStatTimes10 = 0; //DSM
uint32_t sumTimeofLow10 = 0;
uint32_t dsmStatTimes25 = 0;
uint32_t sumTimeofLow25 = 0;
uint8_t switch1025 = 0;
//递推滤波变量
#define FILT_N 10
#define FILT_I 2
uint8_t filtI[FILT_I]={
  0,0};
uint16_t filtDsm[FILT_I][FILT_N];
uint32_t filtSum[FILT_I] = {
  0,0};
 
void dsmStatistics(){
  if(switch1025){
    dsmStatTimes10 ++;
    sumTimeofLow10 += !digitalRead(DSM_PIN_10);
    switch1025 = 0;
  }
  else{
    dsmStatTimes25 ++;
    sumTimeofLow25 += !digitalRead(DSM_PIN_25);
    switch1025 = 1;
  }
}
 
uint16_t filteringDsm(uint8_t l,uint16_t _dsmData){
  filtSum[l] =filtSum[l]- filtDsm[l][filtI[l]];
  filtSum[l] =filtSum[l]+ _dsmData;
  filtDsm[l][filtI[l]] = _dsmData;
  filtI[l]=filtI[l]+1;
  if (filtI[l] >= FILT_N) filtI[l] = 0;
  return filtSum[l]/FILT_N;
}

float dsmTemp25;
 
void get_Send_String(uint8_t k){ 
  switch(k){
  case 0: //DSM501
    {
      float dsmTemp0 = (sumTimeofLow10*100.0)/dsmStatTimes10;
      float dsmTemp10 = 0.1776*pow(dsmTemp0,3) - 0.24*pow(dsmTemp0,2) + 94.003*dsmTemp0;
      lcd.setCursor(12,0);
      lcd.print(dsmTemp10);
    dataLength[k] = sprintf(sensorData[k],"{\"value\":%d}",filteringDsm(k,int(dsmTemp10))); 
      sumTimeofLow10=0;
      dsmStatTimes10=0;
      dsmTemp0 = (sumTimeofLow25*100.0)/dsmStatTimes25;
      dsmTemp25 = 0.1776*pow(dsmTemp0,3) - 0.24*pow(dsmTemp0,2) + 94.003*dsmTemp0;
      dsmTemp25 = dsmTemp10-dsmTemp25;
      sumTimeofLow25=0;
      dsmStatTimes25=0;
      switchtoEthercard();
      break;  
    }
  case 1://pm25
    {
      dataLength[k] = sprintf(sensorData[k],"{\"value\":%d}",filteringDsm(k,int(dsmTemp25)));
      break;
    }
  case 2://Asm2302 temperature
    {
      sensor.update();
      uint16_t tempData = (sensor.getTemperatureInt() << 8) | sensor.getTemperatureFrac(); 
      lcd.setCursor(0,0);
      lcd.print(tempData/10);
      lcd.print(".");
      lcd.print(tempData%10);
      lcd.print("C");
      format_String(k,tempData);
      break;
    }
  case 3://Asm2302 humidity
    {
      sensor.update();
      uint16_t humiData = (sensor.getHumidityInt() << 8) | sensor.getHumidityFrac();
      lcd.setCursor(6,0);
      lcd.print(humiData/10);
      lcd.print(".");
      lcd.print(humiData%10);
      lcd.print("%");
      format_String(k,humiData);
      break;
    }
 
  }
}
 
void format_String(uint8_t l,uint16_t sensor_Data){
  uint16_t Tc_100 = sensor_Data;
  uint8_t whole, fract;
  whole = Tc_100/10 ;  // separate off the whole and fractional portions
  fract = Tc_100 % 10;
  dataLength[l] = sprintf(sensorData[l],"{\"value\":%d.%d}",whole,fract); 
}

