//Kelompok 3 Embedded System
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "esp_sleep.h"

//define pin
#define PIR_PIN 14
#define DHT_PIN 4
#define DHT_TYPE DHT22
#define POT_PIN 34
#define IN1 25
#define IN2 26
#define ENA 27
#define SDA_PIN 21
#define SCL_PIN 22
#define Power_DHT 19
#define Power_pot 18

//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

//DHT
DHT dht(DHT_PIN, DHT_TYPE);

//PWM
#define PWM_CHANNEL 0
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8

//timing
#define UPDATE_INTERVAL 300
#define SLEEP_TIMEOUT 60000
#define WAKTU_VAL 3000

//FSM
enum State
{
  StateSleep,
  StateValidasi,
  ActiveAutoState,
  ActivemanualState
};

State currentState;

//Timer
unsigned long lastMotion = 0;
unsigned long lastUpdate = 0;
unsigned long valStart = 0;
int valCount = 0;
int valPirPrev = LOW;

//Fungsi fan off
void fanOff()
{
  ledcWrite(PWM_CHANNEL, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

//Fungsi fan speed
void setFanSpeed(int pwm)
{
  pwm = constrain(pwm, 0, 255);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ledcWrite(PWM_CHANNEL, pwm);
}

//fungsi auto PWM berdasarkan suhu
int getAutoPWM(float temp)
{
  if (temp <= 22) return 90;
  if (temp <= 23) return 110;
  if (temp <= 24) return 120;
  if (temp <= 25) return 155;
  if (temp <= 26) return 185;
  if (temp <= 27) return 205;
  if (temp <= 28) return 215;
  if (temp <= 29) return 235;
  return 255;
}

//Fungsi deep sleep
void enterDeepSleep()
{
  fanOff();
  digitalWrite(Power_DHT, LOW);
  digitalWrite(Power_pot, LOW);
  lcd.clear();
  lcd.noBacklight();
  delay(200);
  esp_sleep_enable_ext1_wakeup(
    (1ULL << PIR_PIN),
    ESP_EXT1_WAKEUP_ANY_HIGH
  );
  esp_deep_sleep_start();
}

//Fungsi masuk validation state
void enterValidation()
{
  valCount = 0;
  valPirPrev = LOW;
  currentState = StateValidasi;
  lcd.backlight();
  lcd.clear();
  delay(1000);
  valStart = millis();
}

void setup()
{
  Serial.begin(115200);
  pinMode(Power_DHT, OUTPUT);
  pinMode(Power_pot, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  digitalWrite(Power_DHT, HIGH);
  digitalWrite(Power_pot, HIGH);
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SMART FAN KEL 3");
  lcd.setCursor(0, 1);
  lcd.print("BOOTING...");
  dht.begin();
  analogReadResolution(12);
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENA, PWM_CHANNEL);
  fanOff();
  delay(1500);

  esp_sleep_wakeup_cause_t wakeup =
    esp_sleep_get_wakeup_cause();

  if (wakeup == ESP_SLEEP_WAKEUP_EXT1)
  {
    enterValidation();
  }
  else
  {
    currentState = StateSleep;
  }
}

void loop()
{
  switch (currentState)
  {

    //Sleep State
    case StateSleep:
    {
      enterDeepSleep();
      break;
    }

    //Validation State
    case StateValidasi:
    {
      unsigned long now = millis();
      unsigned long elapsed = now - valStart;
      int pirVal = digitalRead(PIR_PIN);

      if (pirVal == HIGH && valPirPrev == LOW)
      {
        valCount++;
      }
      valPirPrev = pirVal;

      int sisaDetik = (int)((WAKTU_VAL - elapsed) / 1000) + 1;
      if (sisaDetik < 1) sisaDetik = 1;
      const char* pirStr = (pirVal == HIGH) ? "H" : "L";
      char l1[17];
      snprintf(l1, 17, "Valid St 3s, P=%s ", pirStr);
      lcd.setCursor(0, 0);
      lcd.print(l1);

      char l2[17];
      snprintf(l2, 17, "Countdown=%d Dtk ", sisaDetik);
      lcd.setCursor(0, 1);
      lcd.print(l2);

      if (elapsed >= WAKTU_VAL)
      {
        lcd.clear();

        if (valCount >= 1)
        {
          lcd.setCursor(0, 0);
          lcd.print("Validasi Berhasil");
          lcd.setCursor(0, 1);
          lcd.print("Aktifkan Sistem");
          delay(1000);
          lcd.clear();
          lastMotion = millis();
          lastUpdate = 0;

          int pot = analogRead(POT_PIN);

          if (pot <= 50)
          {
            currentState = ActiveAutoState;
          }
          else
          {
            currentState = ActivemanualState;
          }
        }
        else
        {
          lcd.setCursor(0, 0);
          lcd.print("Validasi Gagal");
          lcd.setCursor(0, 1);
          lcd.print("Kembali Sleep...");
          delay(1000);
          currentState = StateSleep;
        }
      }

      break;
    }

    //Auto State
    case ActiveAutoState:
    {
      if (millis() - lastUpdate >= UPDATE_INTERVAL)
      {
        lastUpdate = millis();

        int pir = digitalRead(PIR_PIN);

        if (pir == HIGH)
        {
          lastMotion = millis();
        }

        if (millis() - lastMotion >= SLEEP_TIMEOUT)
        {
          currentState = StateSleep;
          break;
        }

        int pot = analogRead(POT_PIN);

        if (pot > 50)
        {
          currentState = ActivemanualState;
          break;
        }

        float temp = dht.readTemperature();
        if (isnan(temp)) temp = 25;

        int pwm = getAutoPWM(temp);
        setFanSpeed(pwm);
        const char* pirStr = (pir == HIGH) ? "H" : "L";
        char l1[17], l2[17];
        snprintf(l1, 17, "Auto, %4.1fC, P:%s ", temp, pirStr);
        snprintf(l2, 17, "PWM:%-3d,POT:%-4d", pwm, pot);

        lcd.setCursor(0, 0);
        lcd.print(l1);
        lcd.setCursor(0, 1);
        lcd.print(l2);
      }

      break;
    }

    //Manual State
    case ActivemanualState:
    {
      if (millis() - lastUpdate >= UPDATE_INTERVAL)
      {
        lastUpdate = millis();
        int pir = digitalRead(PIR_PIN);

        if (pir == HIGH)
        {
          lastMotion = millis();
        }

        if (millis() - lastMotion >= SLEEP_TIMEOUT)
        {
          currentState = StateSleep;
          break;
        }

        int pot = analogRead(POT_PIN);

        if (pot <= 50)
        {
          currentState = ActiveAutoState;
          break;
        }

        int pwm = map(pot, 0, 4095, 0, 255);

        float temp = dht.readTemperature();
        if (isnan(temp)) temp = 25;

        setFanSpeed(pwm);
        const char* pirStr = (pir == HIGH) ? "H" : "L";
        char l1[17], l2[17];
        snprintf(l1, 17, "Man, %4.1fC, P:%s ", temp, pirStr);
        snprintf(l2, 17, "PWM:%-3d,POT:%-4d", pwm, pot);

        lcd.setCursor(0, 0);
        lcd.print(l1);
        lcd.setCursor(0, 1);
        lcd.print(l2);
      }

      break;
    }
  }
}