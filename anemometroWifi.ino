#include <SPI.h>
#include <WiFi.h>

char ssid[] = ""; // your network 2.4Ghz SSID (name)
char pass[] = ""; // your network password

char server[] = ""; // indirizzo ip server locale
WiFiClient client;
int status = WL_IDLE_STATUS;

unsigned long lastConnectionTime = 0; // last time you connected to the server, in milliseconds

const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds


//necessario per anemometro
float windspeed;
float C1 = 2.5;
float C2 = 0.62;
int getpulsepin = 2; //pin digitale di Arduino al quale collegare l'anemometro
float contTours = 0;
boolean stato = 0;
unsigned long startTours;

//necessario per funzione creaUrl
String strURL = "";

void setup()
{
  Serial.begin(9600);
  pinMode(getpulsepin, INPUT);

  while (!Serial)
  {

    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (WiFi.status() == WL_NO_SHIELD)
  {

    Serial.println("WiFi shield not present");

    // don't continue:

    while (true)
      ;
  }
  String fv = WiFi.firmwareVersion();

  if (fv != "1.1.0")
  {

    Serial.println("Please upgrade the firmware");
  }

  if (status != WL_CONNECTED)
  {

    Serial.print("Attempting to connect to SSID: ");

    Serial.println(ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:

    delay(10000);
  }

  printWifiStatus();

}

void loop()
{

  while (client.available())
  {
    char c = client.read();
    Serial.write(c);
  }

  delay(1000);

  if (millis() - lastConnectionTime > postingInterval)
  {

    contTours = 0;
    startTours = millis();

    while ((millis() - startTours) < 60000)
    {

      boolean pinval = digitalRead(getpulsepin); //lettura segnale pin anemometro

      if ((stato == 0) & (pinval == 1))
      {
        contTours++;                            // contatore giri compitui dall'anemometro
      }

      stato = pinval;
    }

    windspeed = (contTours / 60000) * C1 * C2 * 1000.0; //velocità del vento tarata sul contatore di giri e un intervallo di 1 minuto
    Serial.print(windspeed, 2);
    Serial.println(" Km/h");

    creaUrl(windspeed);
  }
}

void creaUrl(float wind)
{

  client.stop();
  if (client.connect(server, 80))
  {

    Serial.println("connected");

    strURL = "GET /backAnem.php?windspeed=";
    strURL += "'";
    strURL += wind;
    strURL += "Km/h'";
    client.println(strURL);
    Serial.println(strURL);

    lastConnectionTime = millis();
  }

  else
  {
    Serial.println("connection failed");
  }
}

void printWifiStatus()
{

  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");

  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");

  Serial.println(ip);

  // print the received signal strength:

  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI):");

  Serial.print(rssi);

  Serial.println(" dBm");
}
