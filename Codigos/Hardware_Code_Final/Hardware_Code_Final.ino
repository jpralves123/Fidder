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
#define PUB_REQ_TOPIC   "/req_topic"
#define PUB_ACK_TOPIC   "/ack_topic"
#define SUB_TOPIC       "/feed_topic"
#define PUBLISH_DELAY   5000 //5 seconds
#define RELAY_PIN 7
#define potPin 0

//*********************************************************************
// Configurações do Dispositivo
//uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05}; // MAC da placa
//uint8_t mac[6] = {0x78,0x56,0x5f,0x1c,0x89,0xab}; // MAC da placa
uint8_t mac[6] = {0x08,0x2e,0x5f,0x1c,0x89,0x32}; // MAC da placa
//uint8_t mac[6] = {0xC4,0x85,0x08,0x9e,0x8d,0x29}; // MAC da placa

int sinc = 0;

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

  Serial.println(F("MQTT IoT - MONITORAMENTO E CONTROLE - SISTEMAS DISTRIBUÍDOS\n\n> Iniciando..."));
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
  Serial.println(F("\nPronto para receber e enviar mensagens!"));
  previousMillis = millis();

  pinMode(RELAY_PIN, OUTPUT); 
  digitalWrite(RELAY_PIN, HIGH);

}

//*********************************************************************
// LOOP
void loop() {

  // it's time to send new data?
  if(millis() - previousMillis > PUBLISH_DELAY) {
    //sendData();
    previousMillis = millis();
  }

  mqttClient.loop();
}

//*********************************************************************
// FUNÇÃO AUXILIAR: MQTT CONNECT/SUBSCRIBE
void mqttConnect() {

  while(!mqttClient.connected()) {
    
    if(mqttClient.connect(CLIENT_ID, USERNAME, PASSWORD)) {

      //Serial.println(F("> Cliente MQTT Conectado"));
      mqttClient.subscribe(SUB_TOPIC);
      //Serial.println(F("> Subescrito no Tópico!"));

    } else {

      Serial.println(F("Conexão falhou! Tendando novamente em 5 segundos."));
      delay(5000);

    }
  }
}

//*********************************************************************
// FUNÇÃO AUXILIAR MQTT CALLBACK
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  String payloadS = payload;

      // Imprime o que leu do tópico
      Serial.print("\nMensagem Recebida no tópico"); 
      Serial.println(SUB_TOPIC); // Imprime o que leu do tópico
      Serial.println(payloadS); // Imprime o que leu do tópico  
      Serial.println();
  

    if (payloadS.substring(0,4)== "FEED") {  

      // Faz o cálculo do tempo de rotação com base no valor recebido 
      Serial.print("Tempo de rotação: ");
      Serial.print((payloadS.substring(5)).toInt()*10); // Imprime o tempo selecionado
      Serial.println(" ms");
  
      //Ativa o motor
      Serial.println("Ativando motor...");
      digitalWrite(RELAY_PIN, LOW);
      delay((payloadS.substring(5)).toInt()*10);
  
      //Desativa o motor
      Serial.println("Desativando motor...");
      digitalWrite(RELAY_PIN, HIGH);
  
      //Envia ACK de confirmação
      //mqttClient.publish("/fidder_h", "ACK");
      sendData();
      Serial.println("ACK Enviado...");
  
    } else if (payloadS.substring(0,10)== "REQ-FEEDER") {

        Serial.println("\nSolicitação de estado do reservatório recebida...");

        String stringAux;
        int valPot = 0; //Variável que armazena o valor da leitura do potenciômetro 
        
        char msgBuffer[20]; 
    
        // Limpa o Buffer
        memset(msgBuffer, 0, sizeof(msgBuffer));
          
        // Pega valores dos sensores  
        valPot =  analogRead(potPin);  
      
        //Converte o valor lido
        valPot = valPot*0.489;
      
        // Imprime na Serial
        Serial.print("Quantidade de Ração no Reservatório: ");
        Serial.println(valPot);   
      
        // Verifica Conexão
        if(!mqttClient.connected()) 
          mqttConnect();
      
        // Monta String
        stringAux = "INFO-FEEDER-";
        stringAux.concat(valPot); 
      
        // Publica
        stringAux.toCharArray(msgBuffer, 20); 
        mqttClient.publish(PUB_REQ_TOPIC, msgBuffer);
    
        // Limpa o Buffer
        memset(msgBuffer, 0, sizeof(msgBuffer));
      
    }

    mqttClient.disconnect();
    delay(300);
    mqttConnect();

    Serial.println();
    Serial.println(F("\nPronto para receber e enviar mensagens!"));

}



//*********************************************************************
// FUNÇÃO AUXILIAR MQTT PUBLISHER

void sendData() {

    mqttClient.publish(PUB_ACK_TOPIC, "ACK");

}
