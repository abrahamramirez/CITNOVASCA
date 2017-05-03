#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "INFINITUME7EC5E";         // Nombre de la red a conectar
const char* password = "E3F7CFC6AA";          // Contraseña de la red a conectar
const char* url = "";
int timeout = 0;
String cmd = "";
String strUrl = "";
String payload = "";

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
    would try to act as both a client and an access-point and could cause
    network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Intentar conectar hasta agotar tiempo de espera
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");      // Caracter a imprimir mientras se realiza la conexión
    timeout++;
    if (timeout == 20) {
      break;
    }
  }
  if (timeout == 100)
    Serial.println("ERROR");
  else {
    Serial.print(WiFi.localIP().toString());
    Serial.println("OK");
  }
  delay(3000); 
}


void loop() {
  if (Serial.available() > 0) {
    cmd = Serial.readStringUntil('\r');             // Obtener comando AT
    if (cmd.equals("AT")) {
      Serial.println("OK");
    }
    // ---------------------------
    // Comando para obtener IP
    //
    else if (cmd.equals("AT+IP?")) {
      if (WiFi.status() == WL_CONNECTED) {
        Serial.print("     ");
        Serial.print(WiFi.localIP().toString());
        Serial.println("OK");
      }
      else {
        Serial.println("ERROR");
      }
    }
    // --------------------------------------------------
    // Comando para enviar petición GET y obtener JSON
    //
    else if (cmd.startsWith("AT+GETJ=")) {
      if (WiFi.status() == WL_CONNECTED) {
        int from = 0;
        String url = "";
        // Descomponer cadena para obtener Pass
        for(int i = 0; i <= cmd.length(); i++){
          if(cmd.charAt(i) == '='){
            from = i;
            break;  
          } 
        }
        url = cmd.substring(from + 1);
        HTTPClient http;  
        http.begin(url);                          // URL destino para enviar petición GET que retorne JSON  
        int httpCode = http.GET();                // Enviar petición                                                       
        if (httpCode > 0) {                       // Check the returning code
          payload = http.getString();             // Get the request response payload
          Serial.println(payload);                
        }
        else{
          Serial.println("ERROR");
        }
        http.end();                               // Close connection
      }
      else {
        Serial.println("ERROR");
      }
    }
  }

//  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
//    HTTPClient http;  //Declare an object of class HTTPClient
////    http.begin("http://jsonplaceholder.typicode.com/users/1");
//    http.begin("http://httpbin.org/get");  
//    int httpCode = http.GET();                                                                  //Send the request
//    if (httpCode > 0) { //Check the returning code
//      payload = http.getString();   //Get the request response payload
//      Serial.println(payload);                     //Print the response payload
//    }
//    http.end();   //Close connection
//  }
  
//  StaticJsonBuffer<500> JSONBuffer;   //Memory pool
//  JsonObject& parsed = JSONBuffer.parseObject(payload); //Parse message
// 
//  if (!parsed.success()) {   //Check for errors in parsing
//    Serial.println("Parsing failed");
//    delay(5000);
//    return;
//  }
//  const char * sensorType = parsed["origin"]; 
//  const char * sensorType2 = parsed["url"]; 
//  
//  Serial.println(sensorType);
//  Serial.println(sensorType2);
  
}



