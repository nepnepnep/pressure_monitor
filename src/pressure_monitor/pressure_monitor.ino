#include <LiquidCrystal.h> 

#define PRESSURE_SENSOR_PIN    A5
#define LEVEL_TRIGGER_ON_PIN   A3
#define LEVEL_TRIGGER_OFF_PIN  A2


LiquidCrystal lcd(4, 5, 9, 11, 7, 13);

const float VCC                   = 5;
const float VALUE_LOW_ON          = 5;
const float VALUE_HIGH_OFF        = 7;

static bool flag;

unsigned long on_time;
unsigned long off_time;

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
  lcd.print("P=");

  flag = false;
  on_time = 0;
  off_time = millis();
  delay(10000);
}

void engine_on()
{
  digitalWrite(LEVEL_TRIGGER_ON_PIN, HIGH);
  delay(200);
  digitalWrite(LEVEL_TRIGGER_ON_PIN, LOW);
  if (!flag) {
    on_time = millis();
    flag = true;
  }
}

void engine_off()
{
  digitalWrite(LEVEL_TRIGGER_OFF_PIN, HIGH);
  delay(200);
  digitalWrite(LEVEL_TRIGGER_OFF_PIN, LOW);
  if (flag) {
    off_time = millis();
    flag = false;
  }
}

int is_engine_on()
{
  return flag;
}

int engine_on_time_s()
{
  if (is_engine_on())
    return (int)((millis() - on_time)/1000);
  else
    return 0;
}

int engine_off_time_s()
{
  if (!is_engine_on())
    return (int)((millis() - off_time)/1000);
  else
    return 0;
}

float read_pressure()
{
  int val = analogRead(PRESSURE_SENSOR_PIN);

  return 3*(val*VCC/1023)-1.5;
}

void check_engine()
{
  float  P1 = read_pressure();
  delay (1000);
  float  P2 = read_pressure();
  if (P1 >= P2 )
  {
    //engine_off();
    flag = false;
  }
}

void loop()
{
  float pressure = read_pressure();
  //если в баке вакуум - считать давление нулевым.
  if (pressure < 0)
  {
    pressure = 0;
  }
  //напечатать в перво   с
  lcd.setCursor(2,1);
  lcd.print(pressure);

  if (is_engine_on()) {
    lcd.setCursor(6,1);
    lcd.print("Ton=");
    lcd.setCursor(10,1);
    lcd.print(engine_on_time_s());

  } else {
    lcd.setCursor(6,1);
    lcd.print("Toff=");
    lcd.setCursor(11,1);
    lcd.print(engine_off_time_s());
  }

  //Включаем компрессор при проверке флага и порога давления
  if (pressure < VALUE_LOW_ON && !is_engine_on())
  {
    engine_on();
  }
  //Выключаем компрессор при преодолении порога. В текущей реализации не предусмотрено преодоление порога в автоматическом режиме. Постоянно будет разпывать реле при привышении давления.
  else if (pressure > VALUE_HIGH_OFF)
  {
    engine_off();
  }
  //обновление флага на случай ручного выключения. Анализ дельте дельте давления, если давление не растет - обновить флаг.
  else if(pressure > VALUE_LOW_ON && pressure < VALUE_HIGH_OFF && is_engine_on())
  {
    check_engine();
  }
}

