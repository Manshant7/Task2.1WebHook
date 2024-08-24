#include <WiFiNINA.h>      // We're including the WiFiNINA library, which will help us connect to WiFi.
#include <ThingSpeak.h>    // This library is essential for sending data to ThingSpeak.
#include <DHT.h>           // We're using the DHT library to read temperature and humidity data.

#define DHTPIN 3           // We're connecting our DHT11 sensor to pin 3.
#define DHTTYPE DHT11      // We're using the DHT11 sensor type for this project.

DHT dht(DHTPIN, DHTTYPE);  // Here, we create a DHT object so we can interact with our sensor.

#define SECRET_SSID "manshantdhillon"    // This is the name of our WiFi network .
#define SECRET_PASS "2444666668888888"   // This is our WiFi password .

#define SECRET_CH_ID 2729147             // The unique channel ID for our ThingSpeak channel.
#define SECRET_WRITE_APIKEY "MYC8CT80NBAMRJ8W"  // The API key that allows us to send data to ThingSpeak.

char ssid[] = SECRET_SSID;  // We store the SSID in a char array to use it in the WiFi connection.
char pass[] = SECRET_PASS;  // Similarly, we store the password in a char array.

WiFiClient client;  // We create a WiFiClient object, which we'll use to connect to the internet.

unsigned long channelID = SECRET_CH_ID;  // Storing our ThingSpeak channel ID.
const char* apiKey = SECRET_WRITE_APIKEY; // Storing our ThingSpeak write API key.

void setup() {
  Serial.begin(9600);  // Start the Serial Monitor so we can see what's happening.

  // We wait for the Serial connection to establish before doing anything else.
  while (!Serial) {
    ; 
  }

  dht.begin();  // Initialize the DHT sensor so we can start reading data from it.

  // Let's connect to our WiFi network.
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, pass);  // We start the connection process here.

  // This loop keeps checking until we're connected to the WiFi.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");  //  We're connected to the WiFi.

  ThingSpeak.begin(client);  // Initialize ThingSpeak so we can send data to it.
}

void loop() {
  delay(5000);  // We wait for 5 seconds between each reading to avoid flooding the server with too much data.
  
  // Now let's read the humidity and temperature from our sensor.
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // If we get NaN (Not a Number) values, it means something went wrong with the sensor reading.
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;  // We exit the loop if the reading failed, no point in sending bad data.
  }

  //  This will print the humidity and temperature to the Serial Monitor.
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  // Now we set the fields in ThingSpeak to the humidity and temperature we just read.
  ThingSpeak.setField(1, humidity);
  ThingSpeak.setField(2, temperature);

  // We attempt to send the data to ThingSpeak and check the response.
  int responseCode = ThingSpeak.writeFields(channelID, apiKey);
  if (responseCode == 200) {  
    Serial.println("Channel update successful.");
  } else {  // If we get any other response, something went wrong.
    Serial.print("Problem updating channel. HTTP error code ");
    Serial.println(responseCode);
  }
}
