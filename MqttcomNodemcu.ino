
#include <ESP8266WiFi.h>  //essa biblioteca jÃ¡ vem com a IDE. Portanto, nÃ£o Ã© preciso baixar nenhuma biblioteca adicional

#include <PubSubClient.h> //Importa biblioteca MQTT



#define SSID_REDE     "node-red"  //coloque aqui o nome da rede que se deseja conectar
#define SENHA_REDE    "semsenha"  //coloque aqui a senha da rede que se deseja conectar

#define MQTT_SERVER "iotsaeg.nc2.iff.edu.br"
#define MQTT_USER   "saeg"
#define MQTT_PASS   "semsenha"
#define PORTA   1883


#define ledTop1 D7 // Led digital 13
int staLed1 = LOW;

//constantes e variÃ¡veis globais
WiFiClient wifiClient;


#define TOPICOLAMP1 "/quarto/lampada/lampada1"
int staTop1 = LOW;

//constantes e variÃ¡veis globais
PubSubClient MQTTClient(wifiClient);


//prototypes

void connectaClienteMQTT(void);
void iniciaMQTT(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
String mensagem(byte* payload, unsigned int length);
void trataTopico(char* topic, String msg);


void trataTopico(char* topic,String msg);

//prototypes
boolean conectaWiFi(void);


void setup() {
  Serial.begin(115200);
  pinMode(ledTop1, OUTPUT);
  digitalWrite(ledTop1, LOW);
  delay(10);
  if (conectaWiFi()){
     iniciaMQTT();
     
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED){
      if (!MQTTClient.connected()) {
        connectaClienteMQTT();
      }
     
      MQTTClient.loop(); 
  }else{
     if (conectaWiFi()){
     iniciaMQTT();
     }   
      
  }
}

//FunÃ§Ã£o: faz a conexÃ£o WiFI
//ParÃ¢metros: nenhum
//Retorno: nenhum
boolean conectaWiFi(void)
{
    
    wifiClient.stop();
    
    delay(500);
    Serial.println("Conectando-se Ã  rede WiFi...");
    Serial.println();  
    
    WiFi.begin(SSID_REDE, SENHA_REDE);
    
    int contDelay = 0;
    while ((WiFi.status() != WL_CONNECTED) && (contDelay < 60) ) 
    {
        delay(1000);
        Serial.print(".");
        contDelay++;
        
    }

    if(WiFi.status() != WL_CONNECTED){
       Serial.println("");
       Serial.println("WiFi nÃ£o connectado");
       return false;
    }
      
    Serial.println("");
    Serial.println("WiFi connectado com sucesso!");  
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());

    
    return true;
}


// 
//FunÃ§Ã£o: Trata o valor do Topico
//ParÃ¢metros: nenhum
//Retorno: nenhum
void trataTopico(char* topic,String msg){

  
    if (strcmp(topic,TOPICOLAMP1)==0){
      if(msg.equals("1")){
        staLed1 = HIGH;
      }
      if(msg.equals("0")){
        staLed1 = LOW;
      }
    }

   
    

    Serial.print("Led Topico 1:");
    Serial.println(staLed1);

    
    digitalWrite(ledTop1, staLed1);
   
  
}

// 
//FunÃ§Ã£o: conectando ao servidor por MQTT
//ParÃ¢metros: nenhum
//Retorno: nenhum
void connectaClienteMQTT(void) {
  // Espera atÃ© estar conectado ao servidor
  while (!MQTTClient.connected()) {
    Serial.println("Tentando MQTT connection...");
    
    // Tentativa de conexÃ£o
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if( MQTTClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS )) {
      Serial.println("connected: "+clientId);
      MQTTClient.subscribe(TOPICOLAMP1);
     
    } else {
      Serial.print("failed, rc=");
      Serial.print(MQTTClient.state());
      Serial.println(" try again in 5 seconds");
      // Espera 2 segundo e tenta novamente
      delay(5000);
    }
  }
}

//FunÃ§Ã£o: inicializa parÃ¢metros de conexÃ£o clienteMQTT(endereÃ§o do 
//        broker, porta e seta funÃ§Ã£o de callback)
//ParÃ¢metros: nenhum
//Retorno: nenhum
void iniciaMQTT(void){
  MQTTClient.setServer(MQTT_SERVER, PORTA);
  MQTTClient.setCallback(mqtt_callback); 
}

String mensagem(byte* payload, unsigned int length){

  String msg;
 
  //obtem a string do payload recebido
  for(int i = 0; i < length; i++) 
  {
     char c = (char)payload[i];
     msg += c;
  }
  return msg;
}


//FunÃ§Ã£o: funÃ§Ã£o de callback 
//        esta funÃ§Ã£o Ã© chamada toda vez que uma informaÃ§Ã£o de 
//        um dos tÃ³picos subescritos chega)
//ParÃ¢metros: nenhum
//Retorno: nenhum
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{     
    String msg = mensagem(payload,length);
    
    trataTopico(topic,msg);
    
}
