#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 16, 2);

// WIFI
const char* SSID = "FIAP-IBM"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "Challenge@23!"; // Senha da rede WI-FI que deseja se conectar

// MQTT
const char* BROKER_MQTT = "46.17.108.113"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT

//definindo pinagem sensor de distancia
#define BUZZ_PIN 25
const int trigPin = 5;
const int echoPin = 18;
const int PIN_TRIGGER =  4;
const int PIN_ECHO = 2;

//definindo id mqtt
#define ID_MQTT  "fiware_112"

//declarando variável
float distancia;

#define SOUND_SPEED 0.034

long duration;
float distanceCm;
float distanceCm2;

//Tópicos
#define TOPICO_PUBLISH   "/TEF/lamp112/attrs/d"
#define TOPICO_PUBLISH_2   "/TEF/lamp112/attrs/d2"

//pinagem
#define GLED_PIN 12
#define YLED_PIN 13
#define RLED_PIN 23

WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
char EstadoSaida = '0';  //variável que armazena o estado atual da saída
int D4 = 2;

void setup() {
  // put your setup code here, to run once:
  LCD.init();
  LCD.setCursor(0,0);
  LCD.print("Funciona");
  Serial.begin(115200);
  pinMode(GLED_PIN, OUTPUT);
  pinMode(YLED_PIN, OUTPUT);
  pinMode(RLED_PIN, OUTPUT);
  digitalWrite(GLED_PIN, HIGH);
  digitalWrite(YLED_PIN, HIGH);
  digitalWrite(RLED_PIN, HIGH);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(PIN_TRIGGER, OUTPUT); // Sets the trigPin as an Output
  pinMode(PIN_ECHO, INPUT); // Sets the echoPin as an Input
  initWiFi();
  initMQTT();
}

void loop() {
  // put your main code here, to run repeatedly:
  VerificaConexoesWiFIEMQTT();
  verDist();
  verDist2();
  tone(BUZZ_PIN, 2000);
  delay(1000);
  noTone(BUZZ_PIN);
  delay(2000);
  MQTT.loop();
}

void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
     
    reconectWiFi();
}

void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}

void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            //MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
     
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
    
    Serial.print("- Mensagem recebida: ");
    Serial.println(msg);
    
    //toma ação dependendo da string recebida:
    //verifica se deve colocar nivel alto de tensão na saída D0:
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    if (msg.equals("lamp112@on|"))
    {
        digitalWrite(D4, HIGH);
        EstadoSaida = '1';
    }
 
    //verifica se deve colocar nivel alto de tensão na saída D0:
    if (msg.equals("lamp112@off|"))
    {
        digitalWrite(D4, LOW);
        EstadoSaida = '0';
    }
}

void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
     
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void verDist(){
  // put your main code here, to run repeatedly:
  char msgBuffer[4];
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  dtostrf(distanceCm, 4, 2, msgBuffer);
  MQTT.publish(TOPICO_PUBLISH,msgBuffer);
  //colocando nova mensagem
  if (50 < distanciaCm && distanciaCm < 100){
    LCD.clear();
    digitalWrite(YLED_PIN, HIGH);
    MQTT.publish(TOPICO_PUBLISH,"Meia distancia");
  }
  else if (distanceCm < 50){
    LCD.clear();
    LCD.setCursor(0,0);
    LCD.print("Você esta perto de");
    LCD.setCursor(0,1);
    LCD.print("sensor 1");
    digitalWrite(RLED_PIN, HIGH);
    MQTT.publish(TOPICO_PUBLISH,"Próximo");
  }
}

void verDist2(){
  // put your main code here, to run repeatedly:
  char msgBuffer[4];
  // Clears the trigPin
  digitalWrite(PIN_TRIGGER, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(PIN_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIGGER, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(PIN_ECHO, HIGH);
  
  // Calculate the distance
  distanceCm2 = duration * SOUND_SPEED/2;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance sensor 2 (cm): ");
  Serial.println(distanceCm2);
  dtostrf(distanceCm2, 4, 2, msgBuffer);
  MQTT.publish(TOPICO_PUBLISH_2,msgBuffer);
  //mandando mensagem nova
  if (50 < distanceCm2 && distanceCm2 < 100){
    LCD.clear();
    digitalWrite(YLED_PIN, HIGH);
    MQTT.publish(TOPICO_PUBLISH2,"Meia distancia");
  }
  else if(distanceCm2 < 50){
    LCD.clear();
    LCD.setCursor(0,0);
    LCD.print("Voce esta perto de");
    LCD.setCursor(0,1);
    LCD.print("sensor 2");
    digitalWrite(RLED_PIN, HIGH);
    MQTT.publish(TOPICO_PUBLISH2,"Próximo");
  }
}
