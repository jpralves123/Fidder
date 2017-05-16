// PUB/SUB CLIENT MQTT PROTOCOL
// FIDDER - TECNOLOGIAS PET
// fidder.com.br

// **************************************************************************************
// Bibliotecas
#include <UIPEthernet.h>
#include <PubSubClient.h>

// **************************************************************************************
//Constantes
#define CLIENT_ID       "ArduinoMQTT"
#define USERNAME        "jpralves123"
#define PASSWORD        "8e0f3dca1eb5404fb9760b2cbae3e34e"
#define PUB_TOPIC       "jpralves123/f/temperature"
#define SUB_TOPIC       "jpralves123/f/control"
#define PUBLISH_DELAY   5000 //5 seconds
#define RELAY_PIN 13

// **************************************************************************************
// Endereço MAC da placa
uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};

// **************************************************************************************
// Global Variables and Objects
EthernetClient ethClient;
PubSubClient mqttClient;

long previousMillis;

// **************************************************************************************
void setup() {

  // Setup serial communication
  Serial.begin(9600);

  while(!Serial) {};

  Serial.println(F("ENC28j60 and Adafruit IO"));
  Serial.println();

  // setup ethernet communication using DHCP
  if(Ethernet.begin(mac) == 0) {

    Serial.println(F("Unable to configure Ethernet using DHCP"));
    for(;;);

  }

  Serial.println(F("Ethernet configured via DHCP"));
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
  Serial.println();

  // setup mqtt client
  mqttClient.setClient(ethClient);
  mqttClient.setServer("io.adafruit.com", 1883);
  mqttClient.setCallback(mqttCallback);
  Serial.println(F("MQTT client configured"));

  mqttConnect();

  Serial.println();
  Serial.println(F("Ready to receive and send messages!"));
  previousMillis = millis();

}

// **************************************************************************************
void loop() {

  // it's time to send new data?
  if(millis() - previousMillis > PUBLISH_DELAY) {
    sendData();
    previousMillis = millis();
  }

  mqttClient.loop();
}

// ***************************************************************************
void mqttConnect() {

  while(!mqttClient.connected()) {

    if(mqttClient.connect(CLIENT_ID, USERNAME, PASSWORD)) {

      Serial.println(F("MQTT client connected"));
      mqttClient.subscribe(SUB_TOPIC);
      Serial.println(F("Topic subscribed"));
    } else {
      Serial.println(F("Unable to connect, retry in 5 seconds"));
      delay(PUBLISH_DELAY);
    }
  }
}

// ***************************************************************************
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  if(strncmp((const char*)payload, "ON", 2) == 0) {
    Serial.println("ON message received, turning relay ON");
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    Serial.println("OFF message received, turning relay OFF");
    digitalWrite(RELAY_PIN, LOW);
  }
}
// ***************************************************************************
void sendData() {

  char msgBuffer[20];

  float t = 1885.01;
  Serial.print("Temperature: ");
  Serial.println(t);

  if(!mqttClient.connected()) mqttConnect();
  mqttClient.publish(PUB_TOPIC, dtostrf(t, 6, 2, msgBuffer));
}
