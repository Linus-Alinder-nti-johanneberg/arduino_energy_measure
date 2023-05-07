/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "xxxxxxxxxxxxxxxxxx"
#define BLYNK_TEMPLATE_NAME "xxxxxxxxxxxxxxxxxx"
#define BLYNK_AUTH_TOKEN "xxxxxxxxxxxxxxxxxx"
#define BLYNK_PRINT Serial

#include <SoftwareSerial.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <EmonLib.h>
EnergyMonitor emon;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "xxxxxxx";
char pass[] = "xxxxxxxx";

SoftwareSerial Serial1(6, 7);

BlynkTimer timer;
ESP8266 wifi(&Serial1);
double irms;
double power;
unsigned long previousMillis = 0;
const long interval = 1000; // interval (milliseconds)

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();
  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  Serial.println("Blynk Connected");
}
void setup()
{
  // Current: input pin, calibration.
  emon.current(1, 62);

  // Initialize serial for ESP module
  Serial1.begin(9600);
  delay(10);
  // Debug console
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass);
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
  timer.setInterval(1000L, myIrms);
  timer.setInterval(1000L, myPower);
  timer.setInterval(1000L, notification);
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

void myIrms()
{
  // This function describes what will happen with each timer tick
  // e.g. writing sensor value to datastream V5
  measure();
  Blynk.virtualWrite(V4, irms);
}
void myPower()
{
  measure();
  Blynk.virtualWrite(V5, power);
}
void measure()
{
  irms = emon.calcIrms(1480); // Calculate Irms only
  power = irms * 230.0;
  Serial.print(power); // Apparent power
  Serial.print(" ");
  Serial.println(irms); // Irms
  delay(1000);
}

void notification()
{ // timer without delay
  unsigned long currentMillis = millis();
  measure();

  if (currentMillis - previousMillis >= interval)
  {
    if (power > 100)
    {
      previousMillis = currentMillis;
      Serial.println("notis");
      Blynk.logEvent("lampa");
    }
  }
}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
