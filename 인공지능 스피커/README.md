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
  GPIO9 (내부풀업, 즉,GND와 연결)
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
```
1. TFT_eSPI bodmer씨의 라이브러리를 철치
2. TFT_eSPI를 사용가능하게 설정한다. (예제 동작확인 후 계속진행)
3. 필자의 TFT_eSPI_Utilities 라이브러리 설치
4. 필자의 ESP32_Utilities 라이브러리 설치
5. LittleFS로 FW/data/*.simg파일들을 플래시에 저장한다.
6. FW/FW.ino를 빌드후 업로드한다.
```

# STEP 4. 서버코드
```
1. 관련 패키지를 설치한다.
2. .env 파일을 만든다.
3. server/main.py를 실행한다. 
```
### .env파일 형태
```cpp
OPENAI_API_KEY=yourkey
AI_ID4=인공지능키?아이디?코드?
AI_ID35=인공지능키?아이디?코드?
TERMINAL=인공지능키?아이디?코드?
```

# STEP 5. 즐기기
```
버튼을 눌러 테스트하며 놀다가 코드를 개선하거나 만족한다.
```


