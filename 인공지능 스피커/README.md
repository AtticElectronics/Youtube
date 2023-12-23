# 인공지능 스피커 제작과정

# 사용된 주요 하드웨어

ESP32-S3-R8N16
INMP441 
PCM5102A
ILI9431 혹은 ILI9488(해상도: 480*320 픽셀)
PAM8403
![Group 26](https://github.com/AtticElectronics/Youtube/assets/128982852/52b3849c-c727-455a-9a2d-ecd4cb4bcaa0)

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

