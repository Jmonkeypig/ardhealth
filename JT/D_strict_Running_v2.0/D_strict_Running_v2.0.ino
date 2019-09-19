#define ERR_LED 32
#define START_LED 52
#define END_LED 53
#define START_BTN 2
#define END_BTN 3


#define SYSTEM_STATUS_FLOAT 10
#define SYSTEM_STATUS_ERROR 20
#define SYSTEM_STATUS_READY 30
#define SYSTEM_STATUS_RUNNING 40
#define SYSTEM_STATUS_END 50

#define ESP_STATUS_FLOAT 10
#define ESP_STATUS_WIFI_NO_CONN 20
#define ESP_STATUS_WIFI_CONN 30
#define ESP_STATUS_ESP_NO_CONN 40
#define ESP_STATUS_ESP_CONN 50
#define ESP_STATUS_TCP_NO_CONN 60
#define ESP_STATUS_TCP_CONN 70
#define ESP_STATUS_OK 80

#define PLAY_TIMEOUT 29000

long int end_check_time = -1;
bool DEBUG = true;   //show more logs
int responseTime = 100; //communication timeout
String command = "";
String http = "";

//String wifi_ssid = "50dstrict2.4G";
//String wifi_passwd = "025155456";

String wifi_ssid = "NETGEAR_11ng";
String wifi_passwd = "";
//String wifi_ssid = "노준형의 iPhone";
//String wifi_passwd = "38299293";
//String target_server = "cloud.njcells.net";
//String target_server = "arduino.cc";
//String target_server = "10.10.20.65";
String target_server = "192.168.1.201";


int wifi_status = ESP_STATUS_WIFI_NO_CONN;
int tcp_status = ESP_STATUS_TCP_NO_CONN;
int esp_status = ESP_STATUS_FLOAT;
int system_status = SYSTEM_STATUS_FLOAT;


void setup() {


  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial1.begin(115200);
  while (!Serial1) {
    ;
  }
  Serial.println("");
  system_status = SYSTEM_STATUS_READY;

  Serial.println("Serial INIT OK");

  Serial.println("[GPIO INIT]");
  pinMode(START_LED, OUTPUT);
  pinMode(END_LED, OUTPUT);
  pinMode(ERR_LED, OUTPUT);
  pinMode(START_BTN, INPUT_PULLUP);
  pinMode(END_BTN, INPUT_PULLUP);
  Serial.println("DONE");

  digitalWrite (START_LED, HIGH);
  digitalWrite (END_LED, HIGH);
  digitalWrite (ERR_LED, HIGH);



  Serial.println("");
  Serial.println("[ESP INIT]");

  Serial.print("AT test : ");
  esp_status = ESP_STATUS_FLOAT;
  send_to_esp8266("AT", responseTime, DEBUG);
  if (esp_status == ESP_STATUS_OK) {
    Serial.println("TRUE");
  } else {
    system_status = SYSTEM_STATUS_ERROR;
    Serial.println("FALSE");
  }

  //  Serial.print("SET CWMODE : ");
  //  esp_status = ESP_STATUS_FLOAT;
  //  send_to_esp8266("AT+CWMODE=1",responseTime,DEBUG);
  //  if (esp_status == ESP_STATUS_OK) {
  //    Serial.println("TRUE");
  //  } else {
  //    system_status = SYSTEM_STATUS_ERROR;
  //    Serial.println("FALSE");
  //  }


  if (!connect_wifi()) {
    system_status = SYSTEM_STATUS_ERROR;

  }
  if (!connect_tcp()) {
    system_status = SYSTEM_STATUS_ERROR;

  }

  Serial.println("DONE");
  digitalWrite (START_LED, LOW);
  digitalWrite (END_LED, LOW);
  digitalWrite (ERR_LED, LOW);
  system_status = SYSTEM_STATUS_READY;


}

void loop() {

  if (system_status == SYSTEM_STATUS_READY) {
    pinMode(START_BTN, INPUT_PULLUP);
    int start_btn_status = digitalRead(START_BTN);
    delay(100);

    digitalWrite (START_LED, HIGH);
    digitalWrite (END_LED, HIGH);
    digitalWrite (ERR_LED, LOW);
    if (start_btn_status == LOW) {
      Serial.println("Start button pushed");
      digitalWrite (START_LED, LOW);

      http = "GET /?command=running_start HTTP/1.1\r\nHOST: " + target_server + "\r\nAccept: */*\r\n\r\n";
//      Serial.print(http);
      send_to_esp8266("AT+CIPSEND=" + String(http.length() + 2), responseTime, DEBUG);
      send_to_esp8266(http, responseTime, DEBUG);
      end_check_time = millis();

      delay(5000);
      Serial.print(http);Serial.print(http);
      system_status = SYSTEM_STATUS_RUNNING;
    }
  } else if (system_status == SYSTEM_STATUS_RUNNING) {
    pinMode(END_BTN, INPUT_PULLUP);

    int end_btn_status = digitalRead(END_BTN);
    delay(100);
    if (end_btn_status == LOW) {


      Serial.println("end button pushed");
      digitalWrite (END_LED, LOW);

      http = "GET /?command=running_finish HTTP/1.1\r\nHOST: " + target_server + "\r\nAccept: */*\r\n\r\n";
      Serial.print(http);
      send_to_esp8266("AT+CIPSEND=" + String(http.length() + 2), responseTime, DEBUG);
      send_to_esp8266(http, responseTime, DEBUG);


      system_status = SYSTEM_STATUS_END;
    }

  } else if (system_status == SYSTEM_STATUS_END) {
    delay(2000);
    system_status = SYSTEM_STATUS_READY;
  } else if (system_status == SYSTEM_STATUS_ERROR) {
    set_err_led();
  }
  if (end_check_time != -1) {
    if ((end_check_time + PLAY_TIMEOUT) < millis()) {
      Serial.print(end_check_time); Serial.print(" "); Serial.println(millis());
      end_check_time = -1;

      system_status = SYSTEM_STATUS_END;
    }
  }


  //  if (Serial1.available()) { f
  //    Serial.write(Serial1.read());
  //  }
  esp_monitor(DEBUG);
  err_check();
  if (Serial.available()) {
    Serial1.write(Serial.read());
  }
}

void esp_monitor(bool debug) {
  String response = "";

  while (Serial1.available())
  {
    // The esp has data so display its output to the serial window
    char c = Serial1.read(); // read the next character.
    Serial.println(response);

    if (c == 0x0D) {
    } else if (c == 0x0A) {
      if (debug) {
        Serial.println("m >>" + response);
      }
      if (response.equals("WIFI GOT IP")) {
        wifi_status = ESP_STATUS_WIFI_CONN;
      } else if (response.equals("IFI GOT IP")) {
        wifi_status = ESP_STATUS_WIFI_CONN;
      } else if (response.equals("FI GOT IP")) {
        wifi_status = ESP_STATUS_WIFI_CONN;
      } else if (response.equals("I GOT IP")) {
        wifi_status = ESP_STATUS_WIFI_CONN;
      } else if (response.equals("CONNECT")) {
        tcp_status = ESP_STATUS_TCP_CONN;
      } else if (response.equals("CLOSED")) {
        tcp_status = ESP_STATUS_TCP_NO_CONN;
      } else if (response.equals("LOSED")) {
        tcp_status = ESP_STATUS_TCP_NO_CONN;
      } else if (response.equals("OSED")) {
        tcp_status = ESP_STATUS_TCP_NO_CONN;
      } else if (response.equals("SED")) {
        tcp_status = ESP_STATUS_TCP_NO_CONN;
      } else if (response.equals("WIFI DISCONNECT")) {
        wifi_status = ESP_STATUS_WIFI_NO_CONN;
      } else if (response.equals("IFI DISCONNECT")) {
        wifi_status = ESP_STATUS_WIFI_NO_CONN;
      } else if (response.equals("FI DISCONNECT")) {
        wifi_status = ESP_STATUS_WIFI_NO_CONN;
      } else if (response.equals("I DISCONNECT")) {
        wifi_status = ESP_STATUS_WIFI_NO_CONN;
      }
      response = "";
    } else {
      response += c;
    }
  }
}


void err_check() {
  if (wifi_status == ESP_STATUS_WIFI_NO_CONN) {
    system_status = SYSTEM_STATUS_ERROR;
    set_err_led();
    if (connect_wifi()) {
      system_status = SYSTEM_STATUS_READY;
    }
  }
  if (tcp_status == ESP_STATUS_TCP_NO_CONN) {
    system_status = SYSTEM_STATUS_ERROR;
    set_err_led();
    if (connect_tcp()) {
      system_status = SYSTEM_STATUS_READY;
    }
  }
}


String send_to_esp8266(String command, const int timeout, boolean debug) {
  String response = "";
  Serial1.println(command); // send the read character to the esp8266
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (Serial1.available())
    {
      // The esp has data so display its output to the serial window
      char c = Serial1.read(); // read the next character.
      if (c == 0x0D) {
      } else if (c == 0x0A) {
        if (debug) {
          Serial.println(">>" + response);
        }
        if (response.equals("WIFI GOT IP")) {
          wifi_status = ESP_STATUS_WIFI_CONN;
        } else if (response.equals("OK")) {
          esp_status = ESP_STATUS_OK;
        } else if (response.equals("CONNECT")) {
          tcp_status = ESP_STATUS_TCP_CONN;
        } else if (response.equals("Connection: close")) {
          tcp_status = ESP_STATUS_TCP_NO_CONN;
        } else if (response.equals("CLOSED")) {
          tcp_status = ESP_STATUS_TCP_NO_CONN;
        } else if (response.equals("link is not valid")) {
          wifi_status = ESP_STATUS_WIFI_NO_CONN;
          tcp_status = ESP_STATUS_TCP_NO_CONN;
        } else if (response.equals("WIFI DISCONNECT")) {
          wifi_status = ESP_STATUS_WIFI_NO_CONN;
          tcp_status = ESP_STATUS_TCP_NO_CONN;
        }
        response = "";
      } else {
        response += c;
      }
    }
  }
  return response;
}

bool connect_tcp() {
  Serial.print("CONNECT TCP : ");
  esp_status = ESP_STATUS_FLOAT;
  tcp_status = ESP_STATUS_TCP_NO_CONN;
  send_to_esp8266("AT+CIPSTART=\"TCP\",\"" + target_server + "\",9514,7200", responseTime, DEBUG);
  if ((esp_status == ESP_STATUS_OK) && (tcp_status == ESP_STATUS_TCP_CONN)) {
    Serial.println("TRUE");
    return true;
  } else {
    Serial.println("FALSE");
    return false;
  }
}

bool connect_wifi() {
  Serial.print("CONNECT WIFI : ");
  esp_status = ESP_STATUS_FLOAT;
  wifi_status = ESP_STATUS_WIFI_NO_CONN;
  command = "";
  command += "AT+CWJAP=";
  command +=  "\"" + wifi_ssid + "\",\"" + wifi_passwd + "\"";
  send_to_esp8266(command, 10000, DEBUG);
  if ((esp_status == ESP_STATUS_OK) && (wifi_status == ESP_STATUS_WIFI_CONN)) {
    Serial.println("TRUE");
    return true;
  } else {
    Serial.println("FALSE");
    return false;

  }
}
void set_err_led() {
  digitalWrite (ERR_LED, HIGH);
  digitalWrite (START_LED, LOW);
  digitalWrite (END_LED, LOW);
}
