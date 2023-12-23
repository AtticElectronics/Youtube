# 인공지능 스피커 제작과정

# 사용된 주요 하드웨어
ESP32-S3-R8N16
![esp32s3n16r8](https://github.com/AtticElectronics/Youtube/assets/128982852/f467125b-5dcb-438b-b2d7-b382d72f28b7)
INMP441 
![INMP441](https://github.com/AtticElectronics/Youtube/assets/128982852/4e40e425-29b0-4d2b-925c-9a9af591c272)
PCM5102A
![PCM5102A](https://github.com/AtticElectronics/Youtube/assets/128982852/e6b58a03-9460-4be1-9894-fad8d0460aea)
ILI9431 혹은 ILI9488(해상도: 480*320 픽셀)
![S32a4e5ca1ab44e8ea35c827b3bdfc4e1h](https://github.com/AtticElectronics/Youtube/assets/128982852/f1866273-4954-420c-b7e5-8ef1368c53fa)
PAM8403
![스크린샷 2023-12-24 오전 1 53 15](https://github.com/AtticElectronics/Youtube/assets/128982852/e948c773-80a6-4af7-b3a7-d9a77d5bcc99)

```
GPIO 결선

TFT display
  CS   : GPIO1
  RST  : GPIO2
  DC   : GPIO42
  MOSI : GPIO41
  CLK  : GPIO40
버튼
  GPIO9
I2S 0버스
  CLK : GPIO18
  SD  : GPIO17
  WS  : GPIO16
I2S 1버스
  CLK : GPIO8
  SD  : GPIO19
  WS  : GPIO20
```
# 케이스


# 펌웨어

