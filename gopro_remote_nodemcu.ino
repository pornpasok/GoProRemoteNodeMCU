//WiFi Remote for GoPro HERO4 NodeCMU v 1.0 #gopro #goproremote
#include <ESP8266WiFi.h>
const int btn_mode_pin = D1;
const int btn_trigger_pin = D2;
int btn_mode;
int btn_trigger;
const char* ssid = "TonGoPro";
const char* password = "****";
const char* host = "10.5.5.9";

void setup() {
  digitalWrite(BUILTIN_LED, HIGH);
  Serial.begin(9600);
  delay(10);
  pinMode(btn_mode_pin, INPUT_PULLUP);
  pinMode(btn_trigger_pin, INPUT_PULLUP);
  pinMode(BUILTIN_LED, OUTPUT);
  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected"); 
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

char* gp_mode_url[]={
"/gp/gpControl/command/mode?p=0", 
"/gp/gpControl/command/mode?p=1",
"/gp/gpControl/command/mode?p=2"
};

char* gp_mode[]={
"Video", 
"Photo",
"MultiShot"
};

char* gp_trigger_url[]={
"/gp/gpControl/command/shutter?p=0", 
"/gp/gpControl/command/shutter?p=1"
};

int i = 0;
int tm = 3; // Total Mode
int m = 0; // Default Video Mode
int t = 0;

void gp_fetch_url(char* gp_url) {
  Serial.print("connecting to ");
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
  return;
  }
  // We now create a URI for the request
  String url = gp_url;
  //String url = "/gp/gpControl/status"; 
  Serial.print("Requesting URL: ");
  Serial.println(url);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" + 
  "Connection: close\r\n\r\n");
  delay(10);
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
  Serial.print(line);
  }
  Serial.println();
  Serial.println("closing connection");
  // LED Blink
  digitalWrite(BUILTIN_LED, LOW);
  delay(500);
}

// the loop function runs over and over again forever
void loop() {
  //buttonState1 = digitalRead(buttonPin1);
  //buttonState2 = digitalRead(buttonPin2);
  btn_mode = digitalRead(btn_mode_pin);
  btn_trigger = digitalRead(btn_trigger_pin);
  Serial.print("(");
  Serial.print(btn_mode);
  Serial.print(", ");
  Serial.print(btn_trigger);
  Serial.println(")");
  Serial.println(t);
  /*
  if (btn_mode == 0) {
    digitalWrite(BUILTIN_LED, LOW);
    delay(100);
  }
  else {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(100);
  }
  */
  if (btn_mode == 0 and btn_trigger == 0 and t == 0) {
    //#print(gp_fetch_url(gp_trigger_url[0])) # Stop Video Mode
    Serial.println("Exit");
    //break();
  }
  else if (btn_mode == 0 and btn_trigger != 0 and t == 0) {
    i = i+1;
    m = i%tm;
    Serial.println(i);
    Serial.print("Mode : ");
    Serial.println(m);
    Serial.println(gp_mode[m]);
    Serial.println(gp_mode_url[m]);
    gp_fetch_url(gp_mode_url[m]);
  }
  else if (btn_trigger == 0) {
    // Photo & MultiShot Mode
    if (m != 0) {
      Serial.println(gp_trigger_url[1]);
      gp_fetch_url(gp_trigger_url[1]);
    }
    else {
      t = t+1;
      Serial.println(t);
      if (t%2 == 1) {
        Serial.println(gp_trigger_url[1]);
        gp_fetch_url(gp_trigger_url[1]); 
      }
      else {
        Serial.println(gp_trigger_url[0]);
        gp_fetch_url(gp_trigger_url[0]);
        t = 0; // Reset Trigger Video Mode
      }
    }
  }
  if (t%2 == 1) {
    // LED Status Video Recording
    digitalWrite(BUILTIN_LED, LOW);
    Serial.println("Recording ...");
    delay(500);
  }

  // LED Stop
  digitalWrite(BUILTIN_LED, HIGH);
  delay(200);
  /*
  digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because 
  // it is acive low on the ESP-01)
  delay(1000); // Wait for a second
  digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
  delay(2000); // Wait for two seconds (to demonstrate the active low LED)
  */
}
