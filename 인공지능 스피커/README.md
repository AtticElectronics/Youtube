# 인공지능 스피커 제작과정
```
사용된 주요 하드웨어
  ESP32-S3-R8N16
  PCM5102APWR
  PAM8403
  ILI9431 혹은 ILI9488
 
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
![Schematic_aispeaker_2023-12-24](https://github.com/AtticElectronics/Youtube/assets/128982852/76720b4a-a85d-42c1-8d0d-843c70360c8e)
