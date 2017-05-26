#include <Dhcp.h>
#include <Dns.h>
#include <ethernet_comp.h>
#include <UIPClient.h>
#include <UIPEthernet.h>
#include <UIPServer.h>
#include <UIPUdp.h>

// PUB/SUB CLIENT MQTT PROTOCOL
// FIDDER - PET TECHNOLOGIES
// fidder.com.br

//*********************************************************************
// Bibliotecas

//#include <UIPEthernet.h> /*https://github.com/ntruchsess/arduino_uip */
#include <PubSubClient.h> /*http://pubsubclient.knolleary.net/ */

//*********************************************************************
// Constantes

#define CLIENT_ID       "app1"
#define USERNAME        NULL
#define PASSWORD        NULL
#define PUB_TOPIC       "###########"
#define SUB_TOPIC       "/fidder"
#define PUBLISH_DELAY   5000 //5 seconds
#define RELAY_PIN 13

//*********************************************************************
// Configurações do Dispositivo

uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05}; // MAC da placa

//*********************************************************************
// Variáveis e Objetos Globais

EthernetClient ethClient;
PubSubClient mqttClient;

long previousMillis;

//*********************************************************************
// SETUP

void setup() {

  // Setup serial communication
  Serial.begin(9600);

  while(!Serial) {};

  Serial.println(F("MQTT IoT - FIDDER\n\n> Iniciando..."));
  Serial.println();
  
  // setup ethernet communication using DHCP
  if(Ethernet.begin(mac) == 0) {

    Serial.println(F("Configuração DHCP "));
    for(;;);

  }

  Serial.println(F("Configurações do Dispositivo na Rede"));
  Serial.print("IP.: ");
  Serial.println(Ethernet.localIP());
  Serial.println();

  // setup mqtt client
  mqttClient.setClient(ethClient);
  mqttClient.setServer("iot.eclipse.org", 1883);
  mqttClient.setCallback(mqttCallback);
  Serial.println(F("Cliente MQTT Configurado!"));

  mqttConnect();

  Serial.println();
  Serial.println(F("Pronto para recenber e enviar mensagens!"));
  previousMillis = millis();

}

//*********************************************************************
// LOOP

void loop() {

  // it's time to send new data?
  if(millis() - previousMillis > PUBLISH_DELAY) {
    sendData();
    previousMillis = millis();
  }

  mqttClient.loop();
}

//*********************************************************************
// FUNÇÃO AUXILIAR: MQTT CONNECT/SUBSCRIBE

void mqttConnect() {

  while(!mqttClient.connected()) {
    
    if(mqttClient.connect(CLIENT_ID, USERNAME, PASSWORD)) {

      Serial.println(F("> Cliente MQTT Conectado"));
      mqttClient.subscribe(SUB_TOPIC);
      Serial.println(F("> Subescrito no Tópico!"));

    } else {

      Serial.println(F("Conexão falhou! Tendando novamente em 5 segundos."));
      delay(5000);

    }
  }
}

//*********************************************************************
// FUNÇÃO AUXILIAR MQTT CALLBACK

  /*
     A função mqttCallback sempre é chamada quando o tópico ao qual  
  cliente se subescreveu sofreu alguma alteração.
  */

void mqttCallback(char* topic, byte* payload, unsigned int length) {

  Serial.println((const char*)payload); // Imprime o que leu do tópico

  /*
    // EXEMPLO DE MANIPULAÇÃO DO PAYLOAD
    if(strncmp((const char*)payload, "ON", 2) == 0) {
      Serial.println("ON message received, turning relay ON");
      digitalWrite(RELAY_PIN, HIGH);
    } else {
      Serial.println("OFF message received, turning relay OFF");
      digitalWrite(RELAY_PIN, LOW);
    }
  */
}

//*********************************************************************
// FUNÇÃO AUXILIAR MQTT PUBLISHER

  /*
     Função responsável por fazer publicações nos tópicos
  */

void sendData() {
/*
  char msgBuffer[20];
  
  float t = 1885.01;
  Serial.print("Temperature: ");
  Serial.println(t);
    
  if(!mqttClient.connected()) mqttConnect();
  mqttClient.publish(PUB_TOPIC, dtostrf(t, 6, 2, msgBuffer));
*/
}

