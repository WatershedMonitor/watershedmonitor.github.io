#include <FuGPS.h>

int vOutPin = 16;

int rxPin = 36;
int txPin = 33;
FuGPS gps(Serial2);
bool gpsAlive = false;

void setup()
{
  // Start serial for debugging
  Serial.begin(115200);
  Serial.println("GPS begin");

  // Enable VOUT
  pinMode(vOutPin, OUTPUT);
  digitalWrite(vOutPin, LOW);

  // Start serial 2 to get data from GPS module
  Serial2.begin(9600, SERIAL_8N1, rxPin, txPin);
}

void loop()
{
  // Read and parse data from GPS until we have a whole message
  if (gps.read())
  {
    // Just parsed a message from the GPS
    gpsAlive = true;

    // Fix quality
    Serial.printf("Quality: %d\n", gps.Quality);

    // Number of satellites visble
    Serial.printf("Satellites: %d\n", gps.Satellites);

    Serial.printf("Date UTC (MM/DD/YY): %02d/%02d/%02d\n", gps.Months, gps.Days, gps.Years);
    Serial.printf("Time UTC (HH:MM:SS): %02d:%02d:%02d\n", gps.Hours, gps.Minutes, gps.Seconds);

    // Check whether location is known
    if (gps.hasFix() == true)
    {
      Serial.printf("Latitude: %.6f\n", gps.Latitude);
      Serial.printf("Longitude: %.6f\n", gps.Longitude);
      Serial.printf("Altitude (above sea level): %.2f\n", gps.Altitude);
      Serial.printf("Accuracy (HDOP): %.2f\n", gps.Accuracy);

      Serial.println("https://www.google.com/maps/search/?api=1&query=" + String(gps.Latitude, 6) + "," + String(gps.Longitude, 6));
    }

    Serial.println();
    Serial.println("----------------------------------------------------------------------------------------");
    Serial.println();
  }

  // Default is 10 seconds
  if (gps.isAlive() == false)
  {
    if (gpsAlive == true)
    {
      gpsAlive = false;
      Serial.println("GPS module not responding with valid data.");
      Serial.println("Check wiring or restart.");
    }
  }
}
