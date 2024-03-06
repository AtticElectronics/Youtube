#include <TFT_eSPI.h>
#include <SPI.h>

#include <SimgSprite.h>


#include <ScrollElement.h>
#include <ScrollContainer.h>
#include <fonts/NotoSans.h>
#include <TextToSprite.h>

#include <WiFi.h>
#include <WiFiClient.h>

#include <I2SAudioReceiver.h>
#include <I2SAudioSender.h>

#include <Button.h>

#define VERTICAL_STEP 16
#define BG_COLOR 12712
#define START_Y 210

Button testbutton(9);

WiFiClient client;
I2SAudioSender sendVoice;
I2SAudioReceiver recvVoice;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite **sprites;
ScrollContainer container(&tft);

void pressedButton();
void holdButton();
void releasedButton();

void setup(void)
{
    Serial.begin(115200);
  
    WiFi.begin("ssid", "pw");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
    }

    recvVoice.setWifiClient(client);
    recvVoice.setServerAddr("192.168.0.200", 33819); // String ip, int port

    sendVoice.setWifiClient(client);
    sendVoice.setServerAddr("192.168.0.200", 33823); // String ip, int port
    sendVoice.setI2sBus(1);                          // 0 or 1
    sendVoice.setAudioQulity(16000, 16, 1);          // int sample_rate, int sample_size, int channels(only 1 tested)
    recvVoice.setI2sPin(18, 17, 16);         // int sck, int sd, int ws
    sendVoice.setDmaBuf(1024, 6);                    // int len(only 1024 tested), int count
    sendVoice.i2sBegin();

    recvVoice.setI2sBus(0);                  // 0 or 1
    recvVoice.setAudioQuality(16000, 16, 1); // int sample_rate, int sample_size(only 16), int channels(only 1)
    sendVoice.setI2sPin(8, 19, 20);                  // int sck, int sd, int ws
    recvVoice.setDmaBuf(1024, 6);            // int len(only 1024 tested), int count
    recvVoice.i2sBegin();

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(BG_COLOR);     // 배경색상 흰색 지정
    tft.loadFont(NotoSansBold15); // NotoSansd에 정의됨
    tft.setTextColor(TFT_WHITE, BG_COLOR);

    sprites = new TFT_eSprite *[6];

    SimgSprite simg(&tft);
    sprites[0] = simg.load("/mainlogo.simg");
    sprites[1] = simg.load("/maintext.simg");
    sprites[2] = simg.load("/rec1.simg");
    sprites[3] = simg.load("/rec2.simg");
    sprites[4] = simg.load("/chatlogo1.simg");
    sprites[5] = simg.load("/chatlogo2.simg");

    ScrollElement *mainlogo = new ScrollElement(184, 51, sprites[0]->width(), sprites[0]->height(), sprites[0]);
    ScrollElement *maintext = new ScrollElement(47, 163, sprites[1]->width(), sprites[1]->height(), sprites[1]);

    container.addElement(mainlogo);
    container.addElement(maintext);
    container.drawElements();

    footerUI(0);
}
void loop()
{
    int state = testbutton.checkState();
    if (state == 2)
    {
        sendVoice.writeData();
    }
    else if (state == 1)
    {
        pressedButton();
        sendVoice.openFile();
    }
    else if (state == 3)
    {
        String whisper = sendVoice.closeFile();

        tft.fillRect(82, START_Y, 85, 20, BG_COLOR);
        footerUI(2);
        TextToSprite *ttsprites = new TextToSprite(&tft, whisper, 16 * 17, 1);
        ttsprites->setBackgroundColor(BG_COLOR);
        while (true)
        {
            TFT_eSprite *sprite = ttsprites->getNextSprite(82, START_Y);
            if (sprite != nullptr)
            {
                container.addElement(new ScrollElement(82, START_Y, sprite->width(), sprite->height(), sprite, 1));
                container.updateAndDraw(VERTICAL_STEP);
            }
            else
            {
                ESP_LOGD("TextToSprite", "sprite is null");
                delete ttsprites; // TextToSprite 객체 메모리 해제
                break;
            }
        }

        // 한칸 위로 올리고
        container.addElement(new ScrollElement(47, START_Y + 16, sprites[4]->width(), sprites[4]->height(), sprites[4]));
        container.updateAndDraw(VERTICAL_STEP * 3);

        String gptmsg = recvVoice.startSteam();
        TextToSprite *ttsprites2 = new TextToSprite(&tft, gptmsg, 16 * 17, 1);
        ttsprites2->setBackgroundColor(BG_COLOR);

        while (true)
        {
            TFT_eSprite *sprite = ttsprites2->getNextSprite(82, START_Y);
            if (sprite != nullptr)
            {
                container.addElement(new ScrollElement(82, START_Y, sprite->width(), sprite->height(), sprite, 1));
                container.updateAndDraw(VERTICAL_STEP);
            }
            else
            {
                ESP_LOGD("TextToSprite", "sprite is null");
                delete ttsprites2; // TextToSprite 객체 메모리 해제
                break;
            }
        }

        int err = recvVoice.playStreamData();
    }
}

void pressedButton()
{
    footerUI(1);
    container.addElement(new ScrollElement(47, START_Y + 16, sprites[5]->width(), sprites[5]->height(), sprites[5]));
    container.updateAndDraw(VERTICAL_STEP * 3);
    tft.drawString("listening...", 82, START_Y);
}

void releasedButton()
{
}

// 하단 UI 변경
void footerUI(int state)
{
    if (state == 0)
    {
        tft.drawRoundRect(50, 257, 326, 42, 10, TFT_WHITE);
        tft.drawString("Press the button for voice recognition", 69, 272, 0);
        sprites[2]->pushSprite(380, 256);
    }
    else if (state == 1)
    {
        tft.fillRect(69, 267, 289, 20, BG_COLOR);                     // 하단 문자영역
        tft.drawString("Voice recognition in progress", 100, 272, 1); // 하단 문자영역
        sprites[3]->pushSprite(380, 256);
    }
    else if (state == 2)
    {
        tft.fillRect(69, 267, 289, 20, BG_COLOR);                             // 하단 문자영역
        tft.drawString("Press the button for voice recognition", 69, 272, 1); // 하단 문자영역
        sprites[2]->pushSprite(380, 256);
    }
    else if (state == 3)
    {
    }
}
