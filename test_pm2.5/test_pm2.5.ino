nt measurePin = 0; // 连接模拟口0

int ledPower = 2; // 连接数字口2

int samplingTime = 280;

int deltaTime = 40;

int sleepTime = 9680;

float voMeasured = 0;

float calcVoltage = 0;

float dustDensity = 0;

void setup（）{

Serial.begin（9600）;

pinMode（ledPower，OUTPUT）;

}

void loop（）{

digitalWrite（ledPower，LOW）; //开启内部LED

delayMicroseconds（samplingTime）; // 开启LED后的280us的等待时间

voMeasured = analogRead（measurePin）; // 读取模拟值

delayMicroseconds（deltaTime）; // 40us等待时间

digitalWrite（ledPower，HIGH）; // 关闭LED

delayMicroseconds（sleepTime）;

// 0 - 5V mapped to 0 - 1023 integer values

// recover voltage

calcVoltage = voMeasured * （5.0 / 1024.0）; //将模拟值转换为电压值

dustDensity = 0.17 * calcVoltage - 0.1; //将电压值转换为粉尘密度输出单位

Serial.print（“Raw Signal Value （0-1023）： ”）;

Serial.print（voMeasured）;

Serial.print（“ - Voltage： ”）;

Serial.print（calcVoltage）;

Serial.print（“ - Dust Density： ”）;

Serial.println（dustDensity）; // 输出单位： 毫克/立方米

delay（1000）;

}
