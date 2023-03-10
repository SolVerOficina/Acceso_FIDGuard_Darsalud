#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
String json;
StaticJsonDocument<500> doc;
JsonObject root;
JsonObject usuarios;
#include <SPI.h>
#include <MFRC522.h>
#include "SPIFFS.h"
 
#define SS_PIN 21
#define RST_PIN 22

int contador = 0;
int reseteo = 10;
float txValue = 0;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
//7C:9E:BD:45:52:8C
//ascensor 1: 0x84, 0xCC, 0xA8, 0x7A, 0xB6, 0x60
//ascensor 1 con 19 pisos: 84:CC:A8:7A:17:B0 - 0x84, 0xCC, 0xA8, 0x7A, 0x17, 0xB0
//ascensor 2: 84:CC:A8:7A:AD:C8 - 0x84, 0xCC, 0xA8, 0x7A, 0xAD, 0xC8
//ascensor 3: 84:CC:A8:7A:16:C8 - 0x84, 0xCC, 0xA8, 0x7A, 0x16, 0xC8 
//prueba: 7C:9E:BD:45:70:74 - 0x7C, 0x9E, 0xBD, 0x45, 0x70, 0x74

//rEPETIDOR lORA aNTENA rOJA 30:C6:F7:0D:D2:28 
//repetidor LORA ANTENA BLANCA 58:BF:25:99:BF:F8
//Darsalud 7C:9E:BD:ED:15:78 - 7C:9E:BD:F9:F4:5C
// AIRCALL bucaros 84:CC:A8:7A:01:EC


//Tarjeta prueba 7C:9E:BD:45:DA:58 7C:9E:BD:FA:17:B0

#define SERVICE_UUID           "ffe0" // UART service UUID a5f81d42-f76e-11ea-adc1-0242ac120002
#define CHARACTERISTIC_UUID_RX "ffe1"
#define CHARACTERISTIC_UUID_TX "ffe2"

uint8_t broadcastAddress[] = {0x84, 0xCC, 0xA8, 0x7A, 0x01, 0xEC};


esp_now_peer_info_t peerInfo; 

void create_user_json()
{ 
root = doc.to<JsonObject>();
usuarios = root["usuarios"].to<JsonObject>();

//-------ACA SE ASWIGNAN LOS UID CON LOS NOMBRES--------------------

  usuarios["edificio"]="DarSalud";
  usuarios["Direccion"]="kenedy";
  usuarios["51 de 4c 20"]="Martha Gutierrez";
  usuarios["01 be b3 40"]="Andrea Betancourt";
  usuarios["bb ce 59 01"]="Angie Mejia";
  usuarios["5b ca 96 e4"]="Daniel Baquero";
  usuarios["b1 eb eb 20"]="Estrella";
  usuarios["bb 40 a9 01"]="Usuario";
  usuarios["01 be b3 4d"]="Usuario";
  usuarios["56 0a 16 9b"]="Usuario";
  usuarios["bb 9c 3b 01"]="Usuario";
  usuarios["b1 6d 3f 20"]="Usuario";
  usuarios["bb fd 1b 00"]="Usuario";
  

serializeJsonPretty(doc, Serial);
serializeJsonPretty(doc, json);
}
  
// Structure example to send data
// Must match the receiver structure
typedef struct struct_message{
  String a;
 String  uid;
  String nombre;
} struct_message;

//Create a struct_message called myData
struct_message myData;

//Insert your SSID
//constexpr char WIFI_SSID[] = "THOT";
//
//int32_t getWiFiChannel(const char *ssid) {
//  if (int32_t n = WiFi.scanNetworks()) {
//      for (uint8_t i=0; i<n; i++) {
//          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
//              return WiFi.channel(i);
//          }
//      }
//  }
//  return 0;
//}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        Serial.println("***");
        Serial.print("Received Value: ");
        Serial.print(rxValue.length());
        Serial.println();
          for (int i = 0; i < rxValue.length(); i++) {
          Serial.println(rxValue[1]);
//           credentials[i] = rxValue[i];  
                         // Set values to send
         myData.a = rxValue[i];
         Serial.println(myData.a);

  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));     
    if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  ESP.restart();
  delay(2000);                   
        }
/*        String  temp = String(credentials);
          String ssid= s.separa(temp, ',',0);
          String pass= s.separa(temp, ',',1);
          Serial.println("El elemento 1 es: " + ssid);
          Serial.println("El elemento 2 es: " + pass);
          save_data(ssid,pass);*/
        Serial.println();
        // Do stuff based on the command received from the app
    
        if (rxValue.find("Send") != -1) {
   
        }

        Serial.println();
        Serial.println("***");
      }
    }
};

void succes(){
  Serial.println("succes");
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
}

void failure(){
  Serial.println("failure");
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  status == ESP_NOW_SEND_SUCCESS ? succes() : failure();
}

void setup() 
{
  Serial.begin(115200);   // Initiate a serial communication
  Serial.println("Usuarios inscritos:");
  create_user_json();
  SPI.begin();      // Initiate  SPI bus
  pinMode(2, OUTPUT);
  WiFi.mode(WIFI_STA);
  mfrc522.PCD_Init();   // Initiate MFRC522
  //Serial.println("Approximate your card to the reader...");
  //Serial.println();
  
 // Create the BLE Device
  BLEDevice::init("ACCESS SV"); // Give it a name
   // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                      
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify.");
 // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
   
  // register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // register first peer  
 
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void check_if_exist(String uid){
  Serial.print(uid);
  String name = usuarios[uid];
  Serial.print("Nombre asignado a este uid: ");
  Serial.println(name);
  if(name == "null"){
    Serial.println("usuario no existe");
  }else{
    Serial.println("USUARIO SI EXISTE");
    sent_data(uid,name);  
  }
}

void sent_data(String uid, String nombre){
  Serial.println(uid);
  String uuid = uid;
  myData.a = "K";
  myData.uid = uid;
  myData.nombre = nombre;
Serial.print(myData.uid);
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
 // delay(2000);
}

void loop() 
{
//--------------LEER LOS COMENTARIOS SIEMPRE PARA ENTENDER QUE HACE LA FUNCION! -------------
// Look for new cards
if ( ! mfrc522.PICC_IsNewCardPresent()) 
{
 // Serial.print("Contador: ");
 // Serial.println(contador);  
  
//--------------ESTA ES LA FUNCION QUE BUSCA EL NOMBRE DEPENDIENDO DEL UID QUE LLEGA-----------   
//check_if_exist("5B CA 96 E4");
 
 contador++;
//Serial.println("new card prtesent");
   if(contador > reseteo){ 
   Serial.println(" reseteando");
  ESP.restart();
   contador = 0;
   
  }
//  //Serial.print("contador");
//  Serial.println(contador);
  delay(1000);
    return;
  }
//  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    //Serial.println(" Access denied");
    return;
  }
  //Show UID on serial monitor
 // Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
   //  Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
   //  Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  Serial.println(content.substring(1));
  String lectura = content.substring(1);
 // content.toUpperCase(content.substring(1));
    check_if_exist(lectura);
    
    
// //LLAVE MAESTRA ----------------------------
//     if (lectura == "B1 6D 3F 20" ||
//         content.substring(1) == "01 BE B3 40" ||
//         content.substring(1) == "BB CE 59 01" ||
//         content.substring(1) == "5B CA 96 E4" ||
//         content.substring(1) == "B1 EB EB 20" ||
//        content.substring(1) == "BB 9C 3B 01" ) //change here the UID of the card/cards that you want to give access
//  {
////FUNCION QUE RECIBE EL STRING DEL BLE Y LO ENVIA PARA BUSCAR EL NOMBRE ASIGNADO --------
//    Serial.println();
//    delay(500);
//  }
} 
