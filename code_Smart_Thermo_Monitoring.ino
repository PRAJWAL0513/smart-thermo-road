#include<LiquidCrystal.h>

#define heaterPin D13
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
float val, voltage, temp;
String ssid     = "Simulator Wifi";  	// SSID to connect to
String password = ""; 	// Virtual WiFi has no password 
String host     = "api.thingspeak.com"; 	// ThingSpeak API
const int httpPort = 80;
String url = "<api_key>";	// API Key

void setupESP8266() {
  Serial.begin(115200);
  Serial.println("AT");
  lcd.begin(16, 2); 
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);

  if (Serial.find("OK")) {
    Serial.println("ESP8266 OK!!!");
  } else {
    Serial.println("ESP8266 Not Responding!");
  }

  Serial.println("Connecting to WiFi...");
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  
  long timeout = millis();
  while (!Serial.find("OK") && millis() - timeout < 10000) {
    delay(500);  // Wait for WiFi connection
  }

  Serial.println("Connected to WiFi!!!");
  
  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(1000);

  if (Serial.find("OK")) {
    Serial.println("ESP8266 Connected to Server!!!");
  } else {
    Serial.println("Failed to Connect to Server!!!");
  }
}

void sendData() {
  val = analogRead(A0);
  voltage = val * (5.0 / 1023.0);  // Proper conversion for accuracy
  temp = (voltage - 0.5) * 100.0;

  String httpPacket = "GET " + url + String(temp) + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
  int length = httpPacket.length();

  Serial.print("AT+CIPSEND=");
  Serial.println(length);
  delay(1000); 

  if (!Serial.find(">")) {
    Serial.println("Failed to initiate send");
    return;
  }

  Serial.print(httpPacket);
  delay(1000);

  if (Serial.find("SEND OK")) {
    Serial.println("Data sent to ThingSpeak successfully!");
  } else {
    Serial.println("Failed to send data!");
  }
}

void setup() {
  pinMode(A0, INPUT);
  setupESP8266();
}

void loop() {
  
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("\xB0"); // shows degree symbol
  	Serial.print("C \n");
  	
  	lcd.setCursor(0,0);          
  	lcd.print(" Temparature is "); 
 	 lcd.setCursor(5,1);           
  	lcd.print(temp);
  	lcd.print('0');
  
  	int x= temp;
  	int thrshold= 15;
  
  	if (x< thrshold )
  	{
   		digitalWrite(LED_BUILTIN, HIGH);
    	delay(5000);		// 5 Second delay ( Heater on duration )
    	digitalWrite(LED_BUILTIN, LOW);
  	}

 	sendData();
	delay(500); // Increase delay to avoid rate-limiting issues with ThingSpeak
}
