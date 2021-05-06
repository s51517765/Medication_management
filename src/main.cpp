#include <WiFi.h>
#include <M5Stack.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "auth.h"

// M5Stac Basic Disp size 320 x 240
const int LineBtn0 = 35;
const int LineBtn1 = 130;
const int LineBtn2 = 225;
const int LineProgress = 295;
String respJSON;
DynamicJsonDocument resp(255);

bool wifiConnect()
{
  //ssid, password is writtern in "auth.h"
  int tryCount = 0;
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); // Wi-Fi接続
  Serial.print("wifi connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    // Wi-Fi 接続待ち
    delay(500);
    Serial.printf(".");
    tryCount += 1;
    if (tryCount > 30)
    {
      Serial.println("NG");
      M5.Lcd.setCursor(0, LineProgress);
      M5.Lcd.setTextColor(RED);
      M5.Lcd.println("NG");
      delay(1000);
      return false;
    };
  }
  Serial.println("OK");
  return true;
}

void showBattery()
{
  int BatteryLv = M5.Power.getBatteryLevel();

  if (BatteryLv < 40)
    M5.Lcd.setTextColor(RED);
  else
    M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print(BatteryLv);
  M5.Lcd.print("[");
  for (int i = 0; i < 5; i++)
  {
    if (i * 20 < BatteryLv)
      M5.Lcd.print("#");
    else
      M5.Lcd.print(" ");
  }
  M5.Lcd.println("]");
}

void initDisp()
{
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setCursor(0, LineBtn0);
  M5.Lcd.println("Vitamin");
  M5.Lcd.setCursor(0, LineBtn1);
  M5.Lcd.println("Allergies");
  M5.Lcd.setCursor(0, LineBtn2);
  M5.Lcd.println("Loxoprofen");
}
void getJson()
{
  deserializeJson(resp, respJSON);
  const char *data0 = resp["past"]["0"];
  const char *data1 = resp["past"]["1"];
  const char *data2 = resp["past"]["2"];
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, LineBtn0 + 30);
  M5.Lcd.println(data0);
  M5.Lcd.setCursor(0, LineBtn1 + 30);
  M5.Lcd.println(data1);
  M5.Lcd.setCursor(0, LineBtn2 + 30);
  M5.Lcd.println(data2);
  M5.Lcd.setTextSize(3);
}

void getTime()
{
  const long gmtOffset_sec = 3600 * 9;     //UTC + 9
  const int daylightOffset_sec = 3600 * 0; //Summer Timeなし
  const char *ntpServer = "pool.ntp.org";
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%m %d %A %Y %H:%M:%S");
  M5.Lcd.setCursor(0, LineProgress);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.print(&timeinfo, "%m/%d %a %H:%M");
  M5.Lcd.setTextSize(3);
  return;
}

boolean getRequest(String url)
{
  //HTTPClient code start
  HTTPClient http;

  boolean isSuccess = false;

  http.begin(url); //HTTP

  Serial.print("[HTTP GET] begin...\n");
  int httpCode = http.GET();

  if (httpCode > 0)
  {
    Serial.printf("[HTTP GET] Return... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      // 200
      Serial.println("[HTTP GET] Success!!");
      respJSON = http.getString();
      Serial.println(respJSON);

      isSuccess = true;
    }
  }
  else
  {
    Serial.printf("[HTTP POST] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();

  return isSuccess;
}

bool postRequest(int num, char *json)
{
  if (num >= 0)
  {
    M5.Lcd.clear(BLACK);
    initDisp();
    showBattery();
    getJson();
    M5.Lcd.setCursor(0, LineProgress);
    M5.Lcd.setTextColor(CYAN);
    M5.Lcd.println("Sending...");
  }
  else
  {
    M5.Lcd.setCursor(0, LineProgress);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.println("Getting...");
  }
  //HTTPClient code start
  HTTPClient http;

  boolean isSuccess = false;
  if (num == 0)
    sprintf(json, "{\"id\": \"%s\" , \"temp\": \"%s\" }", "0", "temp value"); //"temp" is not used, reserved for future.
  if (num == 1)
    sprintf(json, "{\"id\": \"%s\" , \"temp\": \"%s\" }", "1", "temp value");
  if (num == 2)
    sprintf(json, "{\"id\": \"%s\" , \"temp\": \"%s\" }", "2", "temp value");
  Serial.println(json);

  //published_url is writtern in "auth.h"
  Serial.print("[HTTP POST] begin...\n");
  http.begin(published_url); //HTTP

  // Locationをとるためにこれを書かないといけない
  const char *headerNames[] = {"Location"};
  http.collectHeaders(headerNames, sizeof(headerNames) / sizeof(headerNames[0]));

  Serial.print("[HTTP POST] ...\n");
  int httpCode = http.POST(json);

  // httpCode will be negative on error
  if (httpCode > 0)
  {
    Serial.printf("[HTTP POST] Return... code: %d\n", httpCode);

    Serial.println("Allow");
    Serial.println(http.header("Allow"));

    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      // 200
      Serial.println("[HTTP] Success!!");
      String payload = http.getString();
      Serial.println(payload);

      isSuccess = true;
    }
    else if (httpCode == HTTP_CODE_FOUND)
    {
      // 302 … ページからreturnが戻った場合はリダイレクトとなりこのエラーコードとなる
      String payload = http.getString();
      Serial.println(payload);

      // ヘッダのLocation（リダイレクト先URL）を取り出す
      Serial.println("Location");
      Serial.println(http.header("Location"));

      // リダイレクト先にGetリクエスト
      isSuccess = getRequest(http.header("Location"));
    }
    else
    {
      Serial.printf("[HTTP POST] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();

    return isSuccess;
  }
}

void setup()
{
  char json[100];
  sprintf(json, "{\"id\": \"%s\" , \"temp\": \"%s\" }", "inquery", "temp value"); // Inqueryモード

  M5.begin(true, false, true);
  Wire.begin();
  M5.Power.begin();
  bool bWifi = wifiConnect();
  WiFi.setSleep(false);
  postRequest(-1, json); //初期化時データ取得のみ
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setRotation(0); //90°回転
  showBattery();
  initDisp();
  getJson();
  getTime();
  if (!bWifi)
  {
    M5.Lcd.setCursor(0, LineProgress);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("wifi connect NG");
  }
}

unsigned long timePast = 0;
unsigned long timeCount = 0;
void loop()
{
  int flag = -1;
  M5.update();
  //make JSON
  char json[100];
  sprintf(json, "{\"id\": \"%s\" , \"temp\": \"%s\" }", "inquery", "temp value"); // Inqueryモード

  // お薬を飲んだ情報を送る
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200))
  {
    if (WiFi.status() != WL_CONNECTED)
      wifiConnect();
    Serial.println("BtnA\n");
    flag = postRequest(0, json);
  }
  else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200))
  {
    if (WiFi.status() != WL_CONNECTED)
      wifiConnect();
    Serial.println("BtnB\n");
    flag = postRequest(1, json);
  }
  else if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(1000, 200))
  {
    if (WiFi.status() != WL_CONNECTED)
      wifiConnect();
    Serial.println("BtnC\n");
    flag = postRequest(2, json);
  }

  //画面更新
  if (flag == 1)
  {
    M5.Lcd.clear(BLACK);
    initDisp();
    showBattery();
    getJson();
    getTime();
  }
  else if (flag == -1 && micros() - timePast > 60 * 1000 * 1000)
  {
    M5.Lcd.clear(BLACK);
    initDisp();
    showBattery();
    getJson();
    getTime();
    timePast = micros();
    if (timeCount++ > 60)
    {
      postRequest(-1, json);
      timeCount = 0;
    }
    Serial.println("timeCount " + String(timeCount));
  }
  else if (flag == 0)
  {
    M5.Lcd.setCursor(0, LineProgress);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("REPORT FAILED");
  }
  delay(100);
}
