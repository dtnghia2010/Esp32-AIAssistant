#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Wifi";
const char* password = "khongchodau";
const char* Gemini_Token = "";
const char* Gemini_Max_Tokens = "100";
String res = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while(!Serial)
    ;
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}



void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("");
  Serial.println("Ask your Question: ");
  while(!Serial.available())
    ;
  while(Serial.available()) {
    char add = Serial.read();
    res = res + add;
    delay(1);
  }
  int len = res.length();
  res = res.substring(0, (len-1));
  res = "\"" + res + "\"";
  Serial.println("");
  Serial.println("Asking your question: ");
  Serial.println(res);

  HTTPClient https;
  if(https.begin("https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + (String)Gemini_Token)) {
    https.addHeader("Content-Type", "application/json");
    String payload = String("{\"contents\": [{\"parts\":[{\"text\":" + res + "}]}],\"generationConfig\": {\"maxOutputTokens\": " + (String)Gemini_Max_Tokens + "}}");
    
    int httpCode = https.POST(payload);

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = https.getString();
      DynamicJsonDocument doc(1024);

      deserializeJson(doc, payload);
      String Answer = doc["candidates"][0]["content"]["parts"][0]["text"];
      Answer.trim();
      String filteredAnswer = "";
      for (size_t i = 0; i < Answer.length(); i++) {
        char c = Answer[i];
        if(isalnum(c) || isspace(c)) {
          filteredAnswer += c;
        } else {
          filteredAnswer += ' ';
        }
      }
      Answer = filteredAnswer;

      Serial.println("");
      Serial.println("Here is your Answer: ");
      Serial.println("");
      Serial.println(Answer);
    
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }
    https.end();
  } else {
    Serial.printf("[HTPS] Unable to connect\n");
  }
  res = "";
}
