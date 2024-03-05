#include <Adafruit_NeoPixel.h>
#include "CircularQueue.h"
#include <esp_now.h>
#include <WiFi.h>

#define SAMPLE_LEN 5
#define K 30

typedef struct struct_message
{
    char msg[32];
    int channel;
} struct_message;

struct_message myData;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
bool light_toggle = false;

CircularQueue q1(SAMPLE_LEN), q2(SAMPLE_LEN), q3(SAMPLE_LEN), q4(SAMPLE_LEN), q5(SAMPLE_LEN);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, 6, NEO_GRB + NEO_KHZ800);
int channel = 0;

void (*loopFunction)();

void setupHardware();
void initializeNeoPixel();
void setup()
{
    Serial.begin(115200);
    delay(1000);

    setupHardware();
    initializeNeoPixel();

    channel = digitalRead(13) * 8 + digitalRead(12) * 4 + digitalRead(11) * 2 + digitalRead(10) * 1;
    // channel = 1; // For testing, override channel value

    toggleLight();
    if (channel != 0)
    {
        loopFunction = wirelessLoop;
        WiFi.mode(WIFI_STA);
        if (esp_now_init() != ESP_OK)
        {
            Serial.println("Error initializing ESP-NOW");
            return;
        }
        esp_now_register_recv_cb(OnDataRecv);
    }
    else
    {
        loopFunction = wireLoop;
    }
}

void setupHardware()
{
    pinMode(13, INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);
    pinMode(11, INPUT_PULLUP);
    pinMode(10, INPUT_PULLUP);

    pinMode(1, INPUT);
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);

    pinMode(7, OUTPUT);
}

void initializeNeoPixel()
{
    strip.begin();
    strip.show();
    showRainbow(3000); // 전체 무지개 표현을 3초간 진행
}

void loop()
{
    loopFunction();
}

void wireLoop()
{
    int a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0;
    for (int i = 0; i < 10; i++)
    {
        a1 += analogRead(1);
        a2 += analogRead(2);
        a3 += analogRead(3);
        a4 += analogRead(4);
        a5 += analogRead(5);
    }
    a1 /= 10;
    a2 /= 10;
    a3 /= 10;
    a4 /= 10;
    a5 /= 10;

    q1.enqueue(a1);
    q2.enqueue(a2);
    q3.enqueue(a3);
    q4.enqueue(a4);
    q5.enqueue(a5);

    if (q1.hasIncreaseDecreaseEdge(K) || q2.hasIncreaseDecreaseEdge(K) ||
        q3.hasIncreaseDecreaseEdge(K) || q4.hasIncreaseDecreaseEdge(K) ||
        q5.hasIncreaseDecreaseEdge(K))
    {
        Serial.println("Pattern Detected");
        toggleLight();
        q1.clear();
        q2.clear();
        q3.clear();
        q4.clear();
        q5.clear();
    }
}

void wirelessLoop()
{
    delay(1000);
}

void toggleLight()
{
    light_toggle = !light_toggle;
    digitalWrite(7, light_toggle ? HIGH : LOW); // GPIO 7번 핀 상태를 light_toggle 값에 따라 변경

    // light_toggle 값에 따라 LED 색상을 변경: false일 때 빨간색, true일 때 초록색
    uint32_t redColor = strip.Color(100, 0, 0);   // 빨간색
    uint32_t greenColor = strip.Color(0, 100, 0); // 초록색

    strip.setPixelColor(0, light_toggle ? greenColor : redColor);
    strip.setPixelColor(1, light_toggle ? greenColor : redColor);
    strip.show();
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Message: ");
    Serial.println(myData.msg);
    Serial.print("Number: ");
    Serial.println(myData.channel);
    if (myData.channel == channel && strcmp(myData.msg, "toggle") == 0)
    {
        toggleLight();
    }
}
// 무지개 색상을 표현하는 함수
void showRainbow(unsigned long wait)
{
    unsigned long startTime = millis();
    unsigned long currentTime;

    do
    {
        currentTime = millis();
        // 현재 시간에 따라 무지개 색상의 위치 계산
        float progress = float(currentTime - startTime) / wait;
        int rainbowColorPosition = int(progress * 256 * 5) % 256; // 256 색상을 5번 반복

        for (int i = 0; i < strip.numPixels(); i++)
        {
            // 픽셀별로 색상 설정
            strip.setPixelColor(i, Wheel(rainbowColorPosition));
        }
        strip.show();

        // 다음 색상으로 넘어가기 전에 약간 대기
        delay(20);
    } while (currentTime - startTime < wait);
}

// Wheel 함수: 색상 휠에서 색상을 선택 (0~255 범위의 색상)
uint32_t Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    else
    {
        WheelPos -= 170;
        return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}