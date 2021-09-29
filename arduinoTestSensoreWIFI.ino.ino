#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>

char ssid[] = "";     // your network SSID (name)
char pass[] = "";   // your network password

//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
boolean acceso = false;
char server[]=""; // indirizzo ip server locale
WiFiClient client;
int status = WL_IDLE_STATUS; 

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds

const unsigned long postingInterval = 10L*1000L; // delay between updates, in milliseconds

//necessario per NTP
String strURL = "";
unsigned int localPort = 8888;
const char timeServer[] = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

String starTime;
unsigned long memMillis;
unsigned long start;


void setup() {
  Serial.begin(9600);
  
  while (!Serial) {

    ; // wait for serial port to connect. Needed for native USB port only

  }
  if (WiFi.status() == WL_NO_SHIELD) {

    Serial.println("WiFi shield not present");

    // don't continue:

    while (true);

  }
  String fv = WiFi.firmwareVersion();

  if (fv != "1.1.0") {

    Serial.println("Please upgrade the firmware");

  }
  
  if ( status != WL_CONNECTED) {

    Serial.print("Attempting to connect to SSID: ");

    Serial.println(ssid);
    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:

    delay(10000);

  }
  printWifiStatus();
  starTime = getOra(); 
  memMillis = millis();
  start = millis(); 
}

void loop() {
  
  while (client.available()) {
      
      char c = client.read();
      Serial.write(c);
  }
  
  delay(1000);
  
  /*if ((millis() - start) >= 60000){
    starTime = getOra();
    delay(2000);
    memMillis = millis();
    start = millis();
    Serial.println("sono passati due minuti");
  }
  else{
    Serial.println("Ancora non sono passati due minuti");
  }
  */
  
  if (millis() - lastConnectionTime > postingInterval) {
    
    acceso=!acceso;
    
    unsigned long intervallo = millis() - memMillis;
    memMillis = millis();


    starTime = editDelay(starTime, intervallo);
    creaUrl(starTime);    
  }
  delay(4000);
  
}

void creaUrl(String sTime){  
  
  client.stop();
  if (client.connect(server, 80))
  {
    
    Serial.println("connected");
    
    
    strURL = "GET /dbManage.php?statoLed=";
    if (acceso){
      strURL += "'acceso'";
    }
    else{
      strURL += "'spento'";
    }
    strURL += "&timestamp='";
    strURL += sTime;
    strURL += "'";
    client.println(strURL);
    Serial.println(strURL);
    client.println("Host: ");
    client.println("Connection: close");
    client.println();
    
    lastConnectionTime = millis();

}

  else
{
    
    Serial.println("connection failed");
    
}
}

String getOra(){
  
  Udp.begin(localPort);  
  
  sendNTPpacket(timeServer); // send an NTP packet to a time server

  // wait to see if a reply is available
  delay(1000);
  if (Udp.parsePacket()) {
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    //Serial.print("Seconds since Jan 1 1900 = ");
    //Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    //Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    //Serial.println(epoch);


    // print the hour, minute and second:
    //Serial.println("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    unsigned long ore = ((epoch  % 86400L) / 3600)+2;// print the hour (86400 equals secs per day) +2 per utc+2
    String sOre = String(ore);
    
    sOre = sOre + ":";
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      sOre = sOre + '0';
    }
    
    unsigned long minu = (epoch  % 3600) / 60; // print the minute (3600 equals secs per minute)
    String sMinu = String(minu);
    sMinu = sMinu + ':';
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      sMinu = sMinu + '0';
    }
    unsigned long sec = (epoch % 60) /1; // print the second
    
    
    String sSec = String(sec);
    return(sOre + sMinu + sSec);
    
    
  }
}

// send an NTP request to the time server at the given address
void sendNTPpacket(const char * address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); // NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void printWifiStatus() {

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

String checkZero (int i){
  if( i / 10 == 0){
    String s = "0" + String(i);
    return s;
  }
  else{
    return String(i) ;
  }
  
}

String editDelay(String sOrario, long times){
  String sec = sOrario.substring(6,8);
    int secInt = sec.toInt();
    secInt = secInt + (times/1000);
    
    if (secInt >= 60){
        String min = sOrario.substring(3,5);
        int minInt = min.toInt();
        minInt = minInt + (secInt/60);
        if (minInt>=60){
          String ore = sOrario.substring(0,2);
          int oreInt = ore.toInt();
          oreInt = oreInt + (minInt / 60);
          if (oreInt > 23){
            return ("00:" + checkZero(minInt%60) + ":" + checkZero(secInt%60));
          }
          else{
            return (checkZero(oreInt) + ":" + checkZero(minInt%60) + ":" + checkZero(secInt%60));
          }
        }
        else{
          return (sOrario.substring(0,3) + checkZero(minInt) + ":" + checkZero(secInt%60));      
        }
             
    }
    else
    {
      return (sOrario.substring(0,6) + checkZero(secInt)); 
    }
}
