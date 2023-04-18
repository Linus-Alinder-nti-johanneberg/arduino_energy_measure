/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPLi2ZCp4JV"
#define BLYNK_TEMPLATE_NAME "Quickstart Device"
#define BLYNK_AUTH_TOKEN "Ws9HJ2uqnVAvPRPnxu9W64o1qHoApLv4"
#define BLYNK_PRINT Serial

// Include required libraries
#include <SoftwareSerial.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <EmonLib.h>

// Create an EnergyMonitor object to measure energy
EnergyMonitor emon;

// Define variables for WiFi credentials
// Set password to "" for open networks.
char ssid[] = "NTIG Guest";
char pass[] = "TeknikPassion";

// Create a SoftwareSerial object to communicate with the ESP8266 module, digital pin 6 = RX, digital pin 7 = TX
SoftwareSerial Serial1(6, 7);
// Create a BlynkTimer object to schedule tasks
BlynkTimer timer;
// Create an ESP8266 object to handle WiFi communication
ESP8266 wifi(&Serial1);
// Define variables for energy measurement
double irms;
double power;
// Define variables for time-based notification
unsigned long previousMillis = 0;
const long interval = 10000; // interval (milliseconds)

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
    Serial.println(F("Blynk Connected"));
}
void setup()
{
    // Configure EnergyMonitor object with input pin and household calibration factor
    emon.current(1, 62);
    // Initialize serial communication with ESP8266 module
    Serial1.begin(9600);
    delay(10);
    // Initialize serial communication with serialmonitor for debugging
    Serial.begin(115200);
    // Connect to the Blynk server using WiFi credentials
    Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass);

    // Setup a functions to be called every second
    timer.setInterval(1000L, myTimerEvent);
    timer.setInterval(1000L, measure);
    timer.setInterval(1000L, myIrms);
    timer.setInterval(1000L, myPower);
    timer.setInterval(2000L, myNotification);
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
    // You can send any value at any time.
    // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V2, millis() / 1000);
}

// function to send the value of "irms" to Blynk server
void myIrms()
{
    Blynk.virtualWrite(V4, irms);
}

// function to send the value of "power" to Blynk server
void myPower()
{
    Blynk.virtualWrite(V5, power);
}

// finction to measure power and irms
void measure()
{
    irms = emon.calcIrms(1480); // Calculate Irms
    if (irms < 1.0)
    { // if the irms is low, this function makes it act like the device is tunned of
        irms = 0;
    }
    power = irms * 230.0;
    // Print Apparent power to the Serial monitor
    Serial.print(power);
    Serial.print(" ");
    // Print Irms to the Serial monitor
    Serial.println(irms);
}

void myNotification()
{ // Timer function without delay to send Notifications to a mobilephone
    // static int i = 0;
    // if (i = 10) {
    //   Serial.println(F("Notification"));		   // Irms
    //   Blynk.logEvent(F("lampa_pa"), F("Stäng av lampan!"));
    // }
    // i++;

    // Get current time in milliseconds
    unsigned long currentMillis = millis();
    // Call the measure function
    measure();
    // Check if the time interval has passed
    if (currentMillis - previousMillis >= interval)
    {
        // Check if the power is greater than 1
        if (power < 1)
        {
            // Store the current time as previous time
            previousMillis = currentMillis;
            // Print "Notification" to the Serial monitor
            Serial.println(F("Notification"));
            // Log the event "lampa_pa" in the Blynk app
            Blynk.logEvent(F("lampa_pa"), F("Glöm inte Stänga av lampan!"));
        }
    }
}

void loop()
{
    Blynk.run(); // Run the Blynk application
    timer.run(); // Run the timer
    // You can inject your own code or combine it with other sketches.
    // Check other examples on how to communicate with Blynk. Remember
    // to avoid delay() function!
}
