/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "xxxxxxxxxxxxxxxxxx"
#define BLYNK_TEMPLATE_NAME "xxxxxxxxxxxxxxxxxx"
#define BLYNK_AUTH_TOKEN "xxxxxxxxxxxxxxxxxx"
#define BLYNK_PRINT Serial

// Include required libraries
#include <SoftwareSerial.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <EmonLib.h>

// Create an EnergyMonitor object to measure energy
EnergyMonitor emon;

// Define variables for WiFi credentials
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
    // Configure EnergyMonitor object with input pin and household calibration factor
    emon.current(1, 62);

    // Initialize serial communication with ESP8266 module
    Serial1.begin(9600);
    delay(10);

    // Initialize serial communication with serialmonitor for debugging
    Serial.begin(115200);

    // Connect to the Blynk server using WiFi credentials
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

    Blynk.virtualWrite(V2, millis() / 1000);
}

// This function sends the measured Irms to Virtual Pin 4
void myIrms()
{
    // Call the measure function
    measure();
    // Send the value to Blynk server
    Blynk.virtualWrite(V4, irms);
}
void measure()
{
    irms = emon.calcIrms(1480); // Calculate Irms
    power = irms * 230.0;
    Serial.print(power);  // Print Apparent power to the Serial monitor
    Serial.print(" ");    // Print space
    Serial.println(irms); // Print Irms to the Serial monitor
    delay(1000);          // Delay for 1 second
}

void notification() // Timer function without delay
{
    unsigned long currentMillis = millis(); // Get current time in milliseconds
    measure();                              // Call the measure function
    if (currentMillis - previousMillis >= interval)
    { // Check if the time interval has passed
        if (power > 100)
        {                                   // Check if the power is greater than 100
            previousMillis = currentMillis; // Store the current time as previous time
            Serial.println("notis");        // Print "notis" to the Serial monitor
            Blynk.logEvent("lampa");        // Log an event in the Blynk app
        }
    }
}

void loop()
{
Blynk.run(); // Run the Blynk application
timer.run(); // Run the timer
                // Check other examples on how to communicate with Blynk. Remember
                // to avoid delay() function!
}