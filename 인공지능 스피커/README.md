# 인공지능 스피커 제작과정

# 사용된 주요 하드웨어

![Group 26](https://github.com/AtticElectronics/Youtube/assets/128982852/52b3849c-c727-455a-9a2d-ecd4cb4bcaa0)
1. ESP32-S3-R8N16
2. INMP441 
3. PCM5102A
4. ILI9431 혹은 ILI9488(해상도: 480*320 픽셀)
5. PAM8403
 


# STEP 1. 하드웨어 결선
## 아래처럼 결선한다.
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
I2S 0버스 (INMP441)
  CLK : GPIO18
  SD  : GPIO17
  WS  : GPIO16
I2S 1버스 (PCM5102A)
  CLK : GPIO8
  SD  : GPIO19
  WS  : GPIO20
```

# STEP 2. 케이스 제작 (필요시)
```
해당 영상의 3mf 파일 업로드 
```

# STEP 3. 펌웨어 업로드
1. ESP32 LittleFS로 simg파일을 업로드한다.
2. zip 파일을 다운받아 아두이노 라이브러리로 설치
3. 예제폴더의 FW/FW.ino를 빌드후 업로드한다.

# STEP 4. 서버코드
1. 관련 패키지를 설치한다.
2. server/main.py를 실행한다. 

# STEP 5. PUSH BUTTON & ENJOY 



