int LCD1602_RS=12;   
    int LCD1602_RW=11;   
    int LCD1602_EN=10;   
    int DB[] = { 6, 7, 8, 9};
    char logtxt[]="Local Time";
    char *sec[60]={"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39","40","41","42","43","44","45","46","47","48","49","50","51","52","53","54","55","56","57","58","59"};

    
    void LCD_Command_Write(int command)
    {
    int i,temp;
    digitalWrite( LCD1602_RS,LOW);
    digitalWrite( LCD1602_RW,LOW);
    digitalWrite( LCD1602_EN,LOW);
     
    temp=command & 0xf0;
    for (i=DB[0]; i <= 9; i++)
    {
       digitalWrite(i,temp & 0x80);
       temp <<= 1;
    }
     
    digitalWrite( LCD1602_EN,HIGH);
    delayMicroseconds(1);
    digitalWrite( LCD1602_EN,LOW);
     
    temp=(command & 0x0f)<<4;
    for (i=DB[0]; i <= 9; i++)
    {
       digitalWrite(i,temp & 0x80);
       temp <<= 1;
    }
     
    digitalWrite( LCD1602_EN,HIGH);
    delayMicroseconds(1);
    digitalWrite( LCD1602_EN,LOW);
    }
     
    void LCD_Data_Write(int dat)
    {
    int i=0,temp;
    digitalWrite( LCD1602_RS,HIGH);
    digitalWrite( LCD1602_RW,LOW);
    digitalWrite( LCD1602_EN,LOW);
     
    temp=dat & 0xf0;
    for (i=DB[0]; i <= 9; i++)
    {
       digitalWrite(i,temp & 0x80);
       temp <<= 1;
    }
     
    digitalWrite( LCD1602_EN,HIGH);
    delayMicroseconds(1);
    digitalWrite( LCD1602_EN,LOW);
     
    temp=(dat & 0x0f)<<4;
    for (i=DB[0]; i <= 9; i++)
    {
       digitalWrite(i,temp & 0x80);
       temp <<= 1;
    }
     
    digitalWrite( LCD1602_EN,HIGH);
    delayMicroseconds(1);
    digitalWrite( LCD1602_EN,LOW);
    }
     
    void LCD_SET_XY( int x, int y )
    {
      int address;
      if (y ==0)    address = 0x80 + x;
      else          address = 0xC0 + x;
      LCD_Command_Write(address);
    }
     
    void LCD_Write_Char( int x,int y,int dat)
    {
      LCD_SET_XY( x, y );
      LCD_Data_Write(dat);
    }
     
    void LCD_Write_String(int X,int Y,char *s)
    {
        LCD_SET_XY( X, Y );    //设置地址
        while (*s)             //写字符串
        {
          LCD_Data_Write(*s);   
          s ++;
        }
    }
     
    void setup (void)
    {
      int i = 0;
      for (i=6; i <= 12; i++)
       {
         pinMode(i,OUTPUT);
       }
      delay(100);
      LCD_Command_Write(0x28);//4线 2行 5x7
      delay(50);
      LCD_Command_Write(0x06);
      delay(50);
      LCD_Command_Write(0x0c);
      delay(50);
      LCD_Command_Write(0x80);
      delay(50);
      LCD_Command_Write(0x01);
      delay(50);
     
    }
     
    void loop (void)
    {


       for(int index=0,minindex=59,hourindex=12;index<60;index++)   //这里来设定现在时间，目前设定为12：59：00
       {
         LCD_Command_Write(0x01);
         delay(50);
         LCD_Write_String(3,0,logtxt);
         LCD_Write_String(3,1,sec[hourindex]);
         LCD_Write_Char(6,1,0x3a);//显示：
         LCD_Write_String(7,1,sec[minindex]);
         LCD_Write_Char(10,1,0x3a);
         LCD_Write_String(11,1,sec[index]);

         delay(950);
         if (index==59)
         {
           index=-1;
           minindex++;
           if (minindex==60)
           {
             minindex=0;
             hourindex++;
             if (hourindex==24)
             {
               hourindex=0;
             }
           }

         }
       }
     
    }
