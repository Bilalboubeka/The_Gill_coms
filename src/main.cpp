#include <WiFi.h>
#include <WebSocketsServer.h>
#include <FS.h>
#include <ESP32Servo.h>



IPAddress local_IP(4,4,4,100);
IPAddress gateway(4,4,4,100);
IPAddress subnet(255,255,255,0);


void connectToWiFi();
void startUDPServer();
void receiveUDPPackets();
void sendResponseToApp(IPAddress deviceIP);

const char* ssid = "GG";
const char* password = "0556411291";




float temperature = 25.0;
float batteryVoltage = 3.7;


unsigned long lastUDPPacketTime = 0;
unsigned long udpTimeout = 5000;  
const unsigned int udpPort = 4210;  


WiFiUDP udp;
WebSocketsServer webSocket = WebSocketsServer(81);


char incomingPacket[255];
String receivedMessage;

void setup() {

   setCpuFrequencyMhz(240);
  Serial.begin(115200);
  connectToWiFi();
  Serial.println(getCpuFrequencyMhz());
  
 
  
  connectToWiFi();

  startUDPServer();

  

}

void loop() {
  
  receiveUDPPackets();
 


    // Only call sendResponseToApp if a UDP packet was received
  if (lastUDPPacketTime > 0) { // Check if any UDP packet was received
  //  IPAddress senderIP = udp.remoteIP(); // Get the IP address of the sender
  //  sendResponseToApp(senderIP); // Send response to the app
  }


  
  // Send temperature and battery status every 5 seconds if no UDP packet is received
  if (millis() - lastUDPPacketTime > udpTimeout) {
    //sendTemperatureAndBattery();
  }


}


void connectToWiFi() {
 WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}






void startUDPServer() {
  udp.begin(udpPort);
  Serial.printf("UDP server started at IP: %s, port: %d\n", WiFi.localIP().toString().c_str(), udpPort);
}


void receiveUDPPackets() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    lastUDPPacketTime = millis();  // Reset the timeout
    int len = udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;
    receivedMessage = String(incomingPacket);
    Serial.printf("Received UDP packet: %s\n", incomingPacket);
    
    IPAddress senderIP = udp.remoteIP();
    Serial.printf("Received UDP packet from IP: %s\n", senderIP.toString().c_str());
    
   
  }
}

void sendResponseToApp(IPAddress deviceIP) {
  udp.beginPacket(deviceIP, udpPort); // Use the IP from the sender
  
  // Convert the message to a char array
  String message = "Temperature: " + String(temperature) + ", Battery: " + String(batteryVoltage);
  udp.write((const uint8_t*)message.c_str(), message.length()); // Explicitly cast to uint8_t*
  
  udp.endPacket();
}

void sendIpAddress() {
  String message = "ESP32:" + WiFi.localIP().toString();  
  udp.beginPacket(WiFi.localIP(), udpPort);
  udp.write(message.c_str());
  udp.endPacket();
}