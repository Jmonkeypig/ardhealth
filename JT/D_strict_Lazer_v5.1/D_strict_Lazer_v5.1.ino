#define PLAYER_1P

#define ERR_LED 32
#define START_LED 53
#define START_BTN 24

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

//#define PLAY_TIMEOUT 5000
#define PLAY_TIMEOUT 35000

bool DEBUG = true;   //show more logs
int responseTime = 1500; //communication timeout
int btn_wifi_send_response_time = 100;
String command = "";
String http = "";
//String wifi_ssid = "nani_time";
//String wifi_passwd = "dstrict91919";
String wifi_ssid = "NETGEAR_11ng";
String wifi_passwd = "";

String target_server = "192.168.1.202";
//String target_server = "10.10.20.65";

int wifi_status = ESP_STATUS_WIFI_NO_CONN;
int tcp_status = ESP_STATUS_TCP_NO_CONN;
int esp_status = ESP_STATUS_FLOAT;
int system_status = SYSTEM_STATUS_FLOAT;
int end_btn_status = false;

#define BUTTON_GEN_MAX 5
#define BUTTON_GEN_MIN 3
#define BUTTON_NUM 20
int button_list[BUTTON_NUM] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                               14, 15, 16, 17,
                               20, 21, 22, 23
                              };
int led_list[BUTTON_NUM] = {33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
                            43, 44, 45, 46, 47, 48, 49, 50, 51, 52
                           };
int target_button[BUTTON_GEN_MAX] = { -1, -1, -1, -1, -1,};
int last_pushed_button = -1;

long int end_check_time = -1;


void setup() {
  randomSeed ( analogRead ( 0 ) );

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
  for ( int i = 0; i < BUTTON_NUM; i++) {
    pinMode(led_list[i], OUTPUT);
    pinMode(button_list[i], INPUT_PULLUP);
    digitalWrite (led_list[i], HIGH);
  }

  Serial.println("DONE");
  pinMode(START_BTN, INPUT_PULLUP);
  pinMode(START_LED, OUTPUT);
  pinMode(ERR_LED, OUTPUT);
  digitalWrite (START_LED, HIGH);
  digitalWrite (ERR_LED, HIGH);

  Serial.println("");
  Serial.println("[ESP INIT]");

  Serial.print("AT test : ");
  esp_status = ESP_STATUS_FLOAT;
  send_to_esp8266("AT+CWMODE=1", responseTime, DEBUG);
  if (esp_status == ESP_STATUS_OK) {
    Serial.println("TRUE");
  } else {
    system_status = SYSTEM_STATUS_ERROR;
    Serial.println("FALSE");
  }

  Serial.print("SET CWMODE : ");
  esp_status = ESP_STATUS_FLOAT;
  send_to_esp8266("AT+CWMODE=1", responseTime, DEBUG);
  if (esp_status == ESP_STATUS_OK) {
    Serial.println("TRUE");
  } else {
    system_status = SYSTEM_STATUS_ERROR;
    Serial.println("FALSE");
  }

  if (!connect_wifi()) {
    system_status = SYSTEM_STATUS_ERROR;

  }
  if (!connect_tcp()) {
    system_status = SYSTEM_STATUS_ERROR;

  }

  Serial.println("STSTEM READY");
  digitalWrite (START_LED, LOW);
  digitalWrite (ERR_LED, LOW);

  for (int i = 0; i < 20; i++) {
    digitalWrite (led_list[i], LOW);
  }

  system_status = SYSTEM_STATUS_READY;
  Serial.println("DONE");
}

void loop() {
  if (system_status == SYSTEM_STATUS_READY) {
    pinMode(START_BTN, INPUT_PULLUP);
    int start_btn_status = digitalRead(START_BTN);
    delay(100);

    digitalWrite (START_LED, HIGH);
    digitalWrite (ERR_LED, LOW);

    if (start_btn_status == LOW) {
      Serial.println("Start target_button pushed");
      digitalWrite (START_LED, LOW);
      end_btn_status = false;

#ifdef PLAYER_1P
      http = "GET /?command=spider_start_1p HTTP/1.1\r\nHOST: " + target_server + "\r\nAccept: */*\r\n\r\n";

#endif
#ifdef PLAYER_2P
      http = "GET /?command=spider_start_2p HTTP/1.1\r\nHOST: " + target_server + "\r\nAccept: */*\r\n\r\n";

#endif
      //      http = "GET /?command=spider_start_2p HTTP/1.1\r\nHOST: " + target_server + "\r\nAccept: */*\r\n\r\n";
      send_to_esp8266("AT+CIPSEND=" + String(http.length() + 2), btn_wifi_send_response_time, DEBUG);
      send_to_esp8266(http, btn_wifi_send_response_time, DEBUG);


      system_status = SYSTEM_STATUS_RUNNING;
      end_check_time = millis();
      delay(4000);
    }
  } else if (system_status == SYSTEM_STATUS_RUNNING) {
    target_button_generate();
    target_button_check();
    if (end_btn_status == true) {
      Serial.print("start time:");
      Serial.print(end_check_time);
      Serial.print(" " );
      Serial.println(PLAY_TIMEOUT);
      Serial.println(millis());
      Serial.println("end target_button pushed");
#ifdef PLAYER_1P
      http = "GET /?command=spider_finish_1p HTTP/1.1\r\nHOST: " + target_server + "\r\nAccept: */*\r\n\r\n";
#endif
#ifdef PLAYER_2P
      http = "GET /?command=spider_finish_2p HTTP/1.1\r\nHOST: " + target_server + "\r\nAccept: */*\r\n\r\n";
#endif

      //      http = "GET /?command=spider_finish_2p HTTP/1.1\r\nHOST: " + target_server + "\r\nAccept: */*\r\n\r\n";

      //      Serial.print(http);
      send_to_esp8266("AT+CIPSEND=" + String(http.length() + 2), btn_wifi_send_response_time, DEBUG);
      send_to_esp8266(http, btn_wifi_send_response_time, DEBUG);

      system_status = SYSTEM_STATUS_END;
    }

  } else if (system_status == SYSTEM_STATUS_END) {
    for ( int i = 0; i < 5; i++) {
      target_button[i] = -1;
    }

    for ( int i = 0; i < 20; i++) {
      digitalWrite (led_list[i], LOW);
    }

    end_btn_status = false;

    delay(5000);
    system_status = SYSTEM_STATUS_READY;
  } else if (system_status == SYSTEM_STATUS_ERROR) {
    set_err_led();
  }

  if ((end_check_time + PLAY_TIMEOUT) < millis()) {
    end_btn_status = true;
  }

  if (Serial1.available()) {
    Serial.write(Serial1.read());
  }
  esp_monitor(DEBUG);
  err_check();
  if (Serial.available()) {
    Serial1.write(Serial.read());
  }
}



void target_button_generate() {
  int check_btn_alive = BUTTON_GEN_MAX;
  for (int i = 0; i < BUTTON_GEN_MAX; i++) {
    if ( target_button[i] == -1) {
      check_btn_alive --;
    }
  }
  if (check_btn_alive >= BUTTON_GEN_MIN) { // if target_button is less than 3, generate for 5 btn;
    return;
  } else {
    Serial.println("BTN GEN");
    for (int i = 0; i < 5; i++) {
      if (target_button[i] == -1) {
        int new_btn = -1;
        while (true) {
          bool is_dup = false;
          new_btn = random(20);
          for ( int j = 0; j < 5; j++) {
            if (target_button[j] == button_list[new_btn]) {
              is_dup = true;
              break;
            }
            if (last_pushed_button == button_list[new_btn]) {
              is_dup = true;
              break;
            }
          }
          if (!is_dup) {
            break;
          }
        }

        target_button[i] = button_list[new_btn];
        digitalWrite(led_list[new_btn], HIGH);
      }
      Serial.print(target_button[i]);
      Serial.print(" ");
    }
    Serial.println("");
    delay(100);
  }
}

void target_button_check() {
  for ( int i = 0; i < BUTTON_NUM; i++) {
    if (digitalRead(button_list[i]) == LOW) {
      for (int j = 0; j < BUTTON_GEN_MAX ; j++) {
        if (target_button[j] == button_list[i]) { // check is on target_button
          last_pushed_button = button_list[i];
          Serial.print("BTN PUSHED ");
          Serial.println(button_list[i]);
#ifdef PLAYER_1P
          http = "GET /?command=1p HTTP/1.1\r\nHOST: " + target_server + "\r\nAccept: */*\r\n\r\n";
#endif

#ifdef PLAYER_2P
          http = "GET /?command=2p HTTP/1.1\r\nHOST: " + target_server + "\r\nAccept: */*\r\n\r\n";
#endif


          //            http = "GET /?command=2p HTTP/1.1\r\nHOST: " + target_server + "\r\nAccept: */*\r\n\r\n";
          //          Serial.print(http);
          send_to_esp8266("AT+CIPSEND=" + String(http.length() + 2), btn_wifi_send_response_time, DEBUG);
          send_to_esp8266(http, btn_wifi_send_response_time, DEBUG);
          target_button[j] = -1;
          digitalWrite(led_list[i], LOW);
        }
      }
    }
  }
}

void esp_monitor(bool debug) {
  String response = "";

  while (Serial1.available())
  {
    // The esp has data so display its output to the serial window
    char c = Serial1.read(); // read the next character.
    //    Serial.println(response);

    if (c == 0x0D) {
    } else if (c == 0x0A) {
      if (debug) {
        //        Serial.println("m >>"+response);
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
          //          Serial.println(">>"+response);
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
}
