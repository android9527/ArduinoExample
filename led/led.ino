/*
 作者:极客工坊
 时间：2012年5月18日
 IDE版本号：1.0
 发表地址：www.geek-workshop.com
 程序说明： 
 使一个Led亮一秒，灭一秒，如此往复。
 */
 
void setup() {                
  // 初始化数字引脚，使其为输出状态。
  // 大部分Arduino控制板上，数字13号引脚都有一颗Led。
  pinMode(13, OUTPUT);     
}
 
void loop() {
  digitalWrite(13, HIGH);   // 使Led亮
  delay(1000);              // 持续1秒钟
  digitalWrite(13, LOW);    // 使Led灭
  delay(1000);              // 持续1秒钟。
}

