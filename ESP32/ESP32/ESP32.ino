///////////////////////////////////////////////////////////////////////
// Universidad Del Valle De Guatemala
// IE2023: Electronica digital 2
// Autor: Samuel Tortola - 22094, Alan Gomez - 22115
// Proyecto: Proyecto 1: Red de sensores
// Hardware: ESP32
// Creado: 10/08/2024
///////////////////////////////////////////////////////////////////////
//Comunicacion con Adafruit IO
///////////////////////////////////////////////////////////////////////
#include <WiFi.h>  // Manejo de WiFi en el ESP32
#include <Wire.h>  // Comunicación I2C
#include "AdafruitIO_WiFi.h" // Adafruit IO con WiFi

// Variables de Adafruit IO
//#define IO_USERNAME "Usuario de Adafruit"
//#define IO_KEY  "Key de Adafruit"

// Variables para el manejo de WiFi
const char* ssid = "Nombre de red";               // SSID de la red WiFi
const char* password = "Contraseña de la red";       // Contraseña de la red WiFi

// Configura el cliente Adafruit IO WiFi
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, ssid, password);

// Definir feeds de Adafruit IO
AdafruitIO_Feed *rgbFeed = io.feed("rgb");          // Feed para el sensor RGB
AdafruitIO_Feed *rgbbFeed = io.feed("rgbb"); 
AdafruitIO_Feed *gasFeed = io.feed("gas");          // Feed para el sensor de gas
AdafruitIO_Feed *timeFeed = io.feed("time");        // Feed para el tiempo
AdafruitIO_Feed *ultrasonicFeed = io.feed("ultrasonic"); // Feed para el sensor ultrasónico

// Variable para datos recibidos a través de Serial
int incomingByte = 0;

// Funciones de manejo de eventos WiFi
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Connected to AP successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(ssid, password);
}

// Configuración inicial
void setup() {
  Serial.begin(9600);  // Inicia la comunicación serie

  // Registra funciones de callback para eventos WiFi
  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  // Conecta a la red WiFi
  WiFi.begin(ssid, password);

  // Conectar a Adafruit IO
  io.connect();

  Serial.println("Wait for WiFi...");
}

// Bucle principal
void loop() {
  // Ejecuta Adafruit IO
  io.run();

  // Lectura de datos desde Serial y envío a feeds
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
//    Serial.println(incomingByte);
  switch(incomingByte)
  {
    case 49:{//1 //Sistema apagado
      // Manejo de datos del primer sensor
        rgbFeed->save("#ff0000");  // Enviar datos del primer sensor a Adafruit IO
        rgbbFeed->save("Sistema Apagado");
        break;
    }
    case 56:{//8// Sistema encendido
      // Manejo de datos del primer sensor
        rgbFeed->save("#00ff11");  // Enviar datos del primer sensor a Adafruit IO
        rgbbFeed->save("Sistema Encendido");
        break;
    }
    case 51:{//3// Garra
      // Manejo de datos del primer sensor
        rgbFeed->save("#00f0ff");  // Enviar datos del primer sensor a Adafruit IO
        rgbbFeed->save("Modo Garra");
        break;
    }
    case 50:{//2// Caja Pequeña
      // Manejo de datos del primer sensor
        ultrasonicFeed->save("Caja Pequeña");  // Enviar datos del primer sensor a Adafruit IO
        break;
    }
    case 53:{//5// Caja Grande
      // Manejo de datos del primer sensor
        ultrasonicFeed->save("Caja Grande");  // Enviar datos del primer sensor a Adafruit IO
        break;
    }
    case 54:{//6// No hay gas
      // Manejo de datos del primer sensor
        gasFeed->save(6); // Enviar datos del primer sensor a Adafruit IO
        break;
    }
    case 55:{//7// Hay gas
      // Manejo de datos del primer sensor
        gasFeed->save(7); // Enviar datos del primer sensor a Adafruit IO
        break;
    }
  }

    // Aquí puedes agregar la lógica para enviar los datos a los feeds correspondientes
    // Ejemplo de envío de datos:
    // rgbFeed->save(incomingByte);    // Enviar datos al feed "rgb"
    // gasFeed->save(incomingByte);    // Enviar datos al feed "gas"
    // timeFeed->save(incomingByte);   // Enviar datos al feed "time"
    // ultrasonicFeed->save(incomingByte); // Enviar datos al feed "ultrasonic"
  }

  delay(10);  // Retraso para el bucle principal
}
