#include <esp_now.h>
#include <WiFi.h>
#include "CircularQueue.h"
#include <Adafruit_NeoPixel.h>

#define SAMPLE_LEN 4
#define K 30
// 송신할 데이터의 구조체 정의
Adafruit_NeoPixel pixels(1, 21, NEO_GRB + NEO_KHZ800);

esp_now_peer_info_t peerInfo;
typedef struct struct_message
{
    char msg[32];
    int channel;
} struct_message;

CircularQueue q1(SAMPLE_LEN);
CircularQueue q2(SAMPLE_LEN);
CircularQueue q3(SAMPLE_LEN);
CircularQueue q4(SAMPLE_LEN);
CircularQueue q5(SAMPLE_LEN);
// 송신할 데이터
struct_message myData;
int channel = 0;

// 브로드캐스트 주소
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup()
{
    Serial.begin(115200);

    // 시작 시 빨간색으로 설정
    pixels.begin();
    pixels.show();

    // GPIO 13,12,11,10 내부풀업으로 설정
    pinMode(13, INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);
    pinMode(11, INPUT_PULLUP);
    pinMode(10, INPUT_PULLUP);
    channel = digitalRead(13) * 8 + digitalRead(12) * 4 + digitalRead(11) * 2 + digitalRead(10) * 1;

    myData.channel = channel;
    // 구조체에 "toggle" 문자열 복사
    strcpy(myData.msg, "toggle");

    // Wi-Fi를 초기화합니다. ESP-NOW 사용을 위해 Wi-Fi 모드를 설정합니다.
    WiFi.mode(WIFI_STA);
    // ESP-NOW 초기화
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    // 피어 (수신기)를 추가합니다. 여기서는 브로드캐스트 주소를 사용합니다.
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // 피어 추가
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
        return;
    }

    // GPIO 1,2,3,4,5 핀을 아날로그 입력으로 설정
    pinMode(1, INPUT);
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
}

void loop()
{

    // 아날로그 입력값을 10개의 평균을 저장
    int a1 = 0;
    int a2 = 0;
    int a3 = 0;
    int a4 = 0;
    int a5 = 0;
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

    // 시리얼 플로터로 확인하기 위해 출력
    // Serial.print(2000);
    // Serial.print(',');
    // Serial.print(a1);
    // Serial.print(',');
    // Serial.print(a2);
    // Serial.print(',');
    // Serial.print(a3);
    // Serial.print(',');
    // Serial.print(a4);
    // Serial.print(',');
    // Serial.print(a5);
    // Serial.print(',');
    // Serial.println(1000);

    if (q1.hasIncreaseDecreaseEdge(K) || q2.hasIncreaseDecreaseEdge(K) || q3.hasIncreaseDecreaseEdge(K) || q4.hasIncreaseDecreaseEdge(K) || q5.hasIncreaseDecreaseEdge(K))
    {
        // 큐에 증가/감소 패턴이 있을 때
        pixels.setPixelColor(0, pixels.Color(0, 100, 0)); // 첫 번째 LED를 녹색으로 변경
        pixels.show();
        // 큐 초기화
        q1.clear();
        q2.clear();
        q3.clear();
        q4.clear();
        q5.clear();
        // 데이터 송신
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
        if (result == ESP_OK)
        {
            Serial.println("Sent with success");
        }
        else
        {
            Serial.println("Error sending the data");
        }

        pixels.setPixelColor(0, pixels.Color(0, 0, 0)); // 첫 번째 LED를 빨간색으로 설정
        pixels.show();
    }
}
