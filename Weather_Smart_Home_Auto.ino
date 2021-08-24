#include <ArduinoHttpClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <WiFiManager.h>

U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
#define SUN 0
#define CLOUDS 1
#define HUMID 2
#define NIGHT 3
#define RAIN 4
#define ROOM_TEMP 5
#define ROOM_HUMID 6

const char *serverAddress1 = "blynk-cloud.com";  // server address
const char *serverAddress2 = "api.openweathermap.org";
char path[200];
String jsonBuffer, response;
int statusCode;

DHT dht(D6, DHT11);
float humidity, temp_f, temp_c;
unsigned long previousMillis = 0;
const long interval = 2000;

const int button_pin = D5;
WiFiManager wifiManager;

ICACHE_RAM_ATTR void rstButton()
{
  WiFi.disconnect(true);
  wifiManager.resetSettings();
  for(int i = 0; i < EEPROM.length(); i++)
  {
    EEPROM.write(i,0);
  }
  drawU8g2("WiFi Settings", "Reset!");

  ESP.reset();
}

void setup() 
{
  initSettings();
  drawU8g2("if not", "connected");
  delay(3000);
  drawU8g2("Connect To", "\'Dynamic WiFi\'");
  delay(3000);
  drawU8g2("And Goto", "192.168.4.1");
  delay(3000);
  drawU8g2("Waiting...", "For WiFi");
  wifiManager.autoConnect("Dynamic WiFi", "//add your own password");
  drawU8g2("Connection", "Established");
  
  delay(1000);
}

void loop() 
{
    blynk();
    openWeather();
    gettemperature();
    formatData();
}

void initSettings()
{
  Serial.begin(115200);
  pinMode(D6, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(button_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(button_pin), rstButton, RISING);
  u8g2.begin();  
  u8g2.enableUTF8Print();
  dht.begin();
  digitalWrite(LED_BUILTIN, HIGH);
}

void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval) {
  // save the last time you read the sensor
  previousMillis = currentMillis;
  // Reading temperature for humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  temp_c = dht.readTemperature(false); // Read temperature as Fahrenheit
  humidity = dht.readHumidity(); // Read humidity (percent)
  delay(500);
  Serial.println(humidity);
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temp_c)) {
  Serial.println("Failed to read from DHT sensor!");
  return;
  }
  }
}

void formatData()
{
  digitalWrite(LED_BUILTIN, LOW);
  DynamicJsonBuffer jsonBuff;
  JsonObject& root = jsonBuff.parseObject(jsonBuffer);

  int temp = round(root["main"]["temp"]);
  int hum = (int)root["main"]["humidity"];
  String description = root["weather"][0]["description"];
  String windSpeed = root["wind"]["speed"];
  String degree = root["wind"]["deg"];
  String lon = root["coord"]["lon"];
  String latt = root["coord"]["lat"];
  int feelsLike = round(root["main"]["feels_like"]);
  String finalStr = description + " ,feels like: " + feelsLike + "C";
  String finalStr1 = "Wind Speed: " + windSpeed + "Km/h" + " ,Degree: " + degree;
  String finalStr2 = "India ,lon: " + lon + " ,lat: " + latt; 
  char buff[80];
  char buff1[80];
  char buff2[80];
  finalStr.toCharArray(buff, 80);
  finalStr1.toCharArray(buff1, 80);
  finalStr2.toCharArray(buff2, 80);
  if(strcmp(root["sys"]["country"], "IN") == 0)
    drawFinalString(buff2, root["name"]);
  if(strcmp(root["weather"][0]["main"], "Rain") == 0) 
    draw(buff, RAIN, temp, "°C");
  else if(strcmp(root["weather"][0]["main"], "Sunny") == 0)
    draw(buff, SUN, temp, "°C");
  else if(strcmp(root["weather"][0]["main"], "Clouds") == 0 || strcmp(root["weather"][0]["main"], "Haze") == 0 || strcmp(root["weather"][0]["main"], "Mist") == 0)
    draw(buff, CLOUDS, temp, "°C"); 
  draw(buff1, HUMID, hum, "%");
  drawFinalString("Temperature and Humidity", "Room");
  draw("My room temperature", ROOM_TEMP, temp_c, "°C");
  draw("My room humidity", ROOM_HUMID, humidity, "%");
}

void drawU8g2(String string1, String string2)
{
  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_ncenB12_tf);
    u8g2.setCursor(0, 20);
    u8g2.println(string1);
    u8g2.setCursor(0, 48);
    u8g2.print(string2);
  } while(u8g2.nextPage());
}

void getPath(const char *string, char path1[200])
{
  WiFiClient wifi;
  HttpClient client = HttpClient(wifi, string, 80);

  client.get(path1);

  statusCode = client.responseStatusCode();
  response = client.responseBody();
}

void openWeather()
{
  getPath(serverAddress2, path);

  // read the status code and body of the response
  jsonBuffer = response;
  Serial.println(response);
}

void blynk()
{
  getPath(serverAddress1, "//your blynk auth key");

  // read the status code and body of the response
  response.remove(0,2);
  int responseLen = response.length();
  response.remove(responseLen-2,2);
  responseLen = response.length();

  char kPath[responseLen+1];
  response.toCharArray(kPath, responseLen+1);

  strcpy(path, kPath);
  Serial.println(path);
}

void drawWeatherSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol)
{
  switch(symbol)
  {
    case SUN:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 69);  
      break;
    case CLOUDS:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 65);  
      break;
    case HUMID:
      u8g2.setFont(u8g2_font_open_iconic_all_6x_t);
      u8g2.drawGlyph(x, y, 152);  
      break;     
    case NIGHT:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 66);
      break;
    case RAIN:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 67);  
      break;
    case ROOM_TEMP:
      u8g2.setFont(u8g2_font_open_iconic_all_6x_t);
      u8g2.drawGlyph(x, y, 184);  
      break;
    case ROOM_HUMID:
      u8g2.setFont(u8g2_font_open_iconic_all_6x_t);
      u8g2.drawGlyph(x, y, 152);  
      break;
  }
}

void drawStrings(String Strings)
{
  u8g2.setFont(u8g2_font_logisoso26_tf);
  u8g2.setCursor(0, 42);
  u8g2.print(Strings);
}

void drawWeatherDetails(uint8_t symbol, int Value, char sign[4])
{
  drawWeatherSymbol(0, 48, symbol);
  u8g2.setFont(u8g2_font_logisoso32_tf);
  u8g2.setCursor(48+3, 42);
  u8g2.print(Value);
  u8g2.print(sign);    // requires enableUTF8Print()
}

void drawScrollString(int16_t offset, const char *s)
{
  static char buf[36];  // should for screen with up to 256 pixel width 
  size_t len;
  size_t char_offset = 0;
  u8g2_uint_t dx = 0;
  size_t visible = 0;
  len = strlen(s);
  if ( offset < 0 )
  {
    char_offset = (-offset)/8;
    dx = offset + char_offset*8;
    if ( char_offset >= u8g2.getDisplayWidth()/8 )
      return;
    visible = u8g2.getDisplayWidth()/8-char_offset+1;
    strncpy(buf, s, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(char_offset*8-dx, 62, buf);
  }
  else
  {
    char_offset = offset / 8;
    if ( char_offset >= len )
      return; // nothing visible
    dx = offset - char_offset*8;
    visible = len - char_offset;
    if ( visible > u8g2.getDisplayWidth()/8+1 )
      visible = u8g2.getDisplayWidth()/8+1;
    strncpy(buf, s+char_offset, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(-dx, 62, buf);
  }
  
}

void drawFinalString(const char *s, String Strings)
{
  int16_t offset = -(int16_t)u8g2.getDisplayWidth();
  int16_t len = strlen(s);
  for(;;)
  {
    u8g2.firstPage();
    do {
      drawStrings(Strings);
      drawScrollString(offset, s);
    } while ( u8g2.nextPage() );
    delay(20);
    offset+=2;
    if ( offset > len*8+1 )
      break;
  }
}

void draw(const char *s, uint8_t symbol, int degree, char sign[4])
{
  int16_t offset = -(int16_t)u8g2.getDisplayWidth();
  int16_t len = strlen(s);
  for(;;)
  {
    u8g2.firstPage();
    do {
      drawWeatherDetails(symbol, degree, sign);
      drawScrollString(offset, s);
    } while ( u8g2.nextPage() );
    delay(20);
    offset+=2;
    if ( offset > len*8+1 )
      break;
  }
}
