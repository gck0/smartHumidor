    /* 
       smartHumidor: ESP8266 with DHT22 and OLed display
                     supports Sparkfun and ThingSpeak for sending data

       by Francesco Alfieri

       based on:
       Arduino core for ESP8266 WiFi chip by ESP8266 Community
       Unified Sensor, DHT-sensor-library, Si7021 library by Adafruit 
       OLED Driver for ESP8266 by Daniel Eichhorn and Fabrice Weinberg
       
       tested on:
       ESPDuino (ESP-13 Module) by DoIt
       D1 R2 & Mini (ESP-12F) by WeMos
       
       supposed to work on other ESP8266-based boards
       you just have to check the pinout and change pins accordingly

       CHANGELOG:
       [0.3] fan control
       [0.2] some fixes
       [0.1] first version

    */

    #include <ESP8266WiFi.h>
    #include <DHT.h>
    #include <SSD1306.h> 
   
    // PARAMETERS

    // Sensors parameters
    const int INTERVAL = 60000;       // [ms] delay between readings
    //dht22
    const int DHTpin[] =    {1,2,4};  // add/remove sensors by pin
    const float offsetT[] = {0,0,0};  // temperature offsets
    const float offsetH[] = {5,5,1};  // humidity offsets

    // Fan parameters
    //fan pin
    const int fanPin = 1;
    boolean fanAlwaysON = false;  // fan always on?
    //fan triggering
    const int fanTemp = 23;       // [°C] min temperature to start spinning
    const int fanHumi = 68;       // [%]  min humidity to start spinning
    
    // Router credentials
    const char* wifissid = "";
    const char* wifipass = "";

    // Sparkfun account
    const String sparkPubKey = "";
    const String sparkPrvKey = "";

    // ThingSpeak account
    const String tsWriteKey = "";

    // OLED Data
    //SSD1306  display(0x3c, 1, 3);   // ADDRESS, SDA, SDC
    SSD1306 oled(0x3C, 13, 12);

    // PROGRAM CODE

    // Sensors data
    const int maxSensors = (sizeof(DHTpin)/sizeof(int));
    DHT *myDHT[maxSensors];
    float humi[maxSensors];
    float temp[maxSensors];
    // averages
    float avgTemp;
    float avgHumi;

    // START
    void setup()
    {
      Serial.begin(115200);
      Serial.flush();
      displayInit();
      connectToRouter();
      // initialize fan pin
      pinMode(fanPin, OUTPUT);
      // initialize sensors
      const int DHTTYPE = 22;
      for (int i=0;i<maxSensors;i++)
      {
        myDHT[i] = new DHT(DHTpin[i], DHTTYPE);
        myDHT[i]->begin();
      }
    }
     
    void loop()
    {
      // average reset
      avgTemp = 0;
      avgHumi = 0;
      // retrieves data from all sensors
      for(int i=0;i<maxSensors;i++)
      {
        temp[i] = myDHT[i]->readTemperature(false);
        humi[i] = myDHT[i]->readHumidity();
        // offsets
        temp[i] += offsetT[i];
        humi[i] += offsetH[i];

        avgTemp += temp[i];
        avgHumi += humi[i];
        
        // prints sensor data to serial
        Serial.print("T" + String(i) + ": " + String(temp[i]));
        Serial.print("\t");
        Serial.println("H" + String(i) + ": " + String(humi[i]));
      }
      avgTemp=round(avgTemp/maxSensors);
      avgHumi=round(avgHumi/maxSensors);
      Serial.println("avgT: " + String(avgTemp));
      Serial.println("avgH: " + String(avgHumi));

      fanControl();
      displayValues();
      sendToSparkfun();
      sendToThingspeak();
      delayBar();
      //delay(INTERVAL);  //simple delay: comment if using displayBar()
     }


    // FAN FUNCTIONS

    void fanControl()
    {
      if(fanAlwaysON)
        digitalWrite(fanPin, HIGH);
      else
      {
        if((avgTemp>=fanTemp)||(avgHumi>=fanHumi))
          digitalWrite(fanPin, HIGH);
         else
          digitalWrite(fanPin, LOW);
      }
     }

    // OLED DISPLAY FUNCTIONS
    
    // initialize display
    void displayInit()
    {
      String displaystring = "Starting";
      oled.init();
      oled.clear();
      oled.flipScreenVertically();
      oled.setFont(ArialMT_Plain_16);  // size choice 10/16/24
      oled.drawString(0,0, displaystring);
      oled.display(); 
    }
 
    // shows data on oled display
    // values are rounded to integers
    void displayValues()
    {
      oled.clear();
      oled.setFont(ArialMT_Plain_10);
      oled.setTextAlignment(TEXT_ALIGN_LEFT);
      String displaystring;
      displaystring += "°C: ";
      for(int i=0;i<maxSensors;i++)
      {
        displaystring += String(round(temp[i])) + "  ";
      }
      displaystring += '\n';
      displaystring += "%H: ";
      for(int i=0;i<maxSensors;i++)
      {
        displaystring += String(round(humi[i])) + "  ";
      }
      //displaystring += "\n";
      oled.drawString(0,0, displaystring);
      oled.display();
    }

    // shows a simple progress bar to keep track of interval between sensors readings
    void delayBar()
    {
      for(int i=0;i<=100;i+=10)
      {
        delay(INTERVAL/10);
        oled.drawProgressBar(0, 52, 120, 10, i);
        oled.display();
      }
    }


    // IOT FUNCTIONS

    // connects to router
    void connectToRouter()
    {
      
      WiFi.mode(WIFI_STA);  // Station mode (disables default AP+STA)
      Serial.print("\n\rConnecting to router..");
      WiFi.begin(wifissid, wifipass);
      // wait for connection
      while (WiFi.status() != WL_CONNECTED)
      {
        Serial.print(".");
        delay(5000);
      }
      Serial.print("connected to ");
      Serial.println(wifissid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      delay(1000);
    }

    
    // Sends data to Sparkfun
        void sendToSparkfun()
    {  
      const char* host = "data.sparkfun.com";
      Serial.print("connecting to ");
      Serial.println(host);
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort))
      {     
        Serial.println("connection failed");
        return;
      }
      // URI for the request
      String url = "/input/" + sparkPubKey + "?private_key=" + sparkPrvKey;
      // fields
      for(int i=0;i<maxSensors;i++)
      {
        //fields name are t0,h0,t1,h1...
        url += "&t" + String(i) + "=" + temp[i];
        url += "&h" + String(i) + "=" + humi[i];
      }
      //url += "&fieldvalue=" + value;    // add custom fields/values
      // display URI in serial monitor
      Serial.print("Requesting URL: "+ url + '\n');
      // HTTP GET/POST to send the request to the server
      client.print("GET " + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
      delay(10);    
      // print server response to serial
      while(client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }
      Serial.println("closing connection");
    }


    // Sends data to ThingSpeak
    void sendToThingspeak()
    {  
      const char* host = "api.thingspeak.com";

      Serial.print("connecting to ");
      Serial.println(host);
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort))
      {     
        Serial.println("connection failed");
        return;
      }
      // URI for the request
      String url = "/update/?key=" + tsWriteKey;
      // fields
      int field=0;
      for(int i=0;i<maxSensors;i++)
      {
        url += "&field" + String(field+1) + "=" + temp[i];
        url += "&field" + String(field+2) + "=" + humi[i];
        field+2;
      }
      //url += "&fieldvalue=" + value;    // to add custom fields/values
      // display URI in serial monitor
      Serial.print("Requesting URL: "+ url + '\n');
      // HTTP GET/POST to send the request to the server
      client.print("GET " + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
      delay(10);    
      // print server response to serial
      while(client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }
      Serial.println("closing connection");
    }
