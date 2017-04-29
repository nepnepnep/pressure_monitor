#include <LiquidCrystal.h> 

LiquidCrystal lcd(4, 5, 9, 11, 7, 13);
#define PRESSURE_SENSOR_PIN    A5
#define LEVEL_TRIGGER_ON_PIN   A3
#define LEVEL_TRIGGER_OFF_PIN  A2

float VCC                   = 5;
float VALUE_LOW_ON          = 5;
float VALUE_HIGH_OFF        = 7;

int   flag                  = 0;

void setup() 
{
    lcd.begin(16, 2);
    pinMode(LEVEL_TRIGGER_ON_PIN, OUTPUT); 
    pinMode(LEVEL_TRIGGER_OFF_PIN, OUTPUT); 
    //Напечатаем пороги давления
    lcd.setCursor(0,0);
    lcd.print("MIN:");
    lcd.setCursor(4,0);
    lcd.print(VALUE_LOW_ON);
    lcd.setCursor(7,0);
    lcd.print("  MAX:");
    lcd.setCursor(13,0);
    lcd.print(VALUE_HIGH_OFF);
    lcd.setCursor(0,1);
    lcd.print("Pressure:");
}
 
void loop() 
{
  int val = analogRead(PRESSURE_SENSOR_PIN);
  float pressure = 3*(val*VCC/1023)-1.5;
  //если в баке вакуум - считать давление нулевым.
  if (pressure < 0)
  {
    pressure=0;
  }
  //напечатать в перво   с
  lcd.setCursor(11,1);
  lcd.print(pressure);
  //Включаем компрессор при проверке флага и порога давления
  if (pressure < VALUE_LOW_ON && flag==0 )
  {
    digitalWrite(LEVEL_TRIGGER_ON_PIN, HIGH);
    delay(1000);
    digitalWrite(LEVEL_TRIGGER_ON_PIN,LOW);
    flag = 1;
  }
  //Выключаем компрессор при преодолении порога. В текущей реализации не предусмотрено преодоление порога в автоматическом режиме. Постоянно будет разпывать реле при привышении давления.
  else if (pressure > VALUE_HIGH_OFF)
 {
   digitalWrite(LEVEL_TRIGGER_OFF_PIN, HIGH);
   delay(10000);
   flag = 0;
 } 
 //обновление флага на случай ручного выключения. Анализ дельте дельте давления, если давление не растет - обновить флаг.
  else if(pressure > VALUE_LOW_ON && pressure < VALUE_HIGH_OFF && flag == 1)
  {
   val = analogRead(PRESSURE_SENSOR_PIN);
   float  P1 = 3*(val*VCC/1023)-1.5;
   delay (1000);
   val = analogRead(PRESSURE_SENSOR_PIN);
   float  P2 = 3*(val*VCC/1023)-1.5;
   if (P1 >= P2 )
   {
     flag = 0;
   }
   
  }
}
