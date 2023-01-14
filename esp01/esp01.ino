#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#define TXD 1 // GPIO1/TXD01

const char* ssid = "My_WiFi";
const char* password = "qsefthuk";
const char* host = "https://yandex.com/time/sync.json?geo=970"; 
String lastTime = "--";
double timestamp = 0;
long mls = 0;
int sec;
int minut;
int hor;
int temper;

WiFiServer server(80);  
StaticJsonDocument<512> doc;

void setup() {
  pinMode(TXD, OUTPUT);
  Serial.begin(9600);  
  Serial.println("");
  Serial.println("Start ESP");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Non Connecting to WiFi..");
  }
  else
  {
    // Иначе удалось подключиться отправляем сообщение
    // о подключении и выводим адрес IP
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    getData();
    server.begin();
  }   
}

void loop() {
  long t = millis();  
  if(t - mls > 1800000 || t < mls){
    mls = t;
    getData();
  }
   
  WiFiClient client2 = server.available();
  if (client2){
  while (client2.connected()){                           // Пока есть соединение с клиентом 
    if (client2.available()){                            // Если клиент активен 
       String request = client2.readStringUntil('\r');
       if (request.indexOf("alarm=ON") != -1) {
          Serial.println("alon");          
       }
       if (request.indexOf("alarm=OFF") != -1) {
          Serial.println("alof");          
       }
       if (request.indexOf("alarm=SET") != -1) { // alarm=SET:12345 
          String t = request.substring(10);
          int i = t.indexOf(" ");
          t = t.substring(0, i);
          Serial.println("alrm:" + t);          
       }
       client2.println("HTTP/1.1 200 OK");               // Стандартный заголовок HTTP 
       client2.println("Content-Type: text/html"); 
       client2.println("Connection: close");             // Соединение будет закрыто после завершения ответа
       client2.println("Refresh: 10");                   // Автоматическое обновление каждые 10 сек 
       client2.println();
       client2.println("<!DOCTYPE HTML>");               // Веб-страница создается с использованием HTML
       client2.println("<html>");                        // Открытие тега HTML 
       client2.println("<head>");
       client2.print("<title>ESP8266 TEMP</title>");     // Название страницы
       client2.println("</head>");
       client2.println("<body>");
       client2.println("<h1>ESP8266 - Time</h1>"); 
       client2.println("<h3>Time = ");
       client2.println("  " + toStr(hor) + ":" + toStr(minut) + ":" + toStr(sec));
       client2.println("<h3>Temperature = ");
       client2.println("  " + toStr(temper) + "</h3>");
       client2.println("</body>");
       client2.println("</html>");                       // Закрытие тега HTML 
       break;                                           // Выход                                
    }
  }  
    client2.stop();                                      // Закрытие соединения
  }
}

String toStr(int i){
  if(i < 10){
    return "0" + String(i);
  }
  return String(i);
}

void getData(){
  WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http; 
    client.connect(host, 443);
    http.begin(client, host);
    int httpCode = http.GET();   
    if (httpCode > 0) { 
      String payload = http.getString(); 
      int str_len = payload.length() + 1; 
      char chars[str_len];
      //Serial.println(payload);
      payload.toCharArray(chars, str_len); 
      DeserializationError error = deserializeJson(doc, chars);
      if(! error){ 
        double timestamp = doc["time"];  
        const char* nm = doc["clocks"]["970"]["name"];
        temper =  doc["clocks"]["970"]["weather"]["temp"];
        // Секунды
        double t = timestamp / 60000.0;
        sec = round((t - trunc(t)) * 60.0);
        // Минуты
        timestamp = trunc(t);
        t = timestamp  / 60.0;
        minut = round((t - trunc(t)) * 60.0);
        // Часы
        timestamp = trunc(t);
        t = timestamp / 24.0;
        hor = round((t - trunc(t)) * 24.0) + 3;
        if(hor > 23){
          hor -= 24;
        }
        int allsec = sec + minut * 60 +  hor * 60 * 60;        
        Serial.println("time:" + String(allsec));
        Serial.println("temp:" + String(temper));
      }     
    }
    http.end();   //Закрыть соединение
}
