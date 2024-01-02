from abc import ABC, abstractmethod
from gtts import gTTS
from pydub import AudioSegment
import os
import wave
import time

class TextToWavConverter(ABC):
    @abstractmethod
    def convert_to_wav(self, text: str, output_filename: str) -> bool:
        pass

    def get_wav_info(self, wav_file_path: str) -> dict:
        try:
            with wave.open(wav_file_path, 'r') as wav:
                length = wav.getnframes() / wav.getframerate()
                channels = wav.getnchannels()
                sample_width = wav.getsampwidth()
                frame_rate = wav.getframerate()
                frames = wav.getnframes()

            return {
                "길이(초)": length,
                "채널 수": channels,
                "샘플 당 비트 수": sample_width * 8,
                "프레임률(샘플 레이트)": frame_rate,
                "프레임 수": frames
            }
        except Exception as e:
            print(f"WAV 파일 정보를 가져오는 중 오류 발생: {e}")
            return {}



class GoogleTextToWavConverter(TextToWavConverter):
    def convert_to_wav(self, text: str, wav_file_path: str) -> bool:
        try:
            dir_path, filename = os.path.split(wav_file_path)
            file_base_name, _ = os.path.splitext(filename)
            temp_mp3_path = os.path.join(dir_path, f"{file_base_name}_temp.mp3")

            # 기존 WAV 파일이 있다면 삭제하고, 완전히 삭제될 때까지 기다림
            while os.path.exists(wav_file_path):
                try:
                    os.remove(wav_file_path)
                    time.sleep(0.1)  # 잠시 대기 후 확인
                except PermissionError:
                    # 파일이 다른 프로세스에 의해 사용 중일 수 있음
                    continue

            # 텍스트를 MP3로 변환 후 임시 파일에 저장
            tts = gTTS(text, lang='ko')
            tts.save(temp_mp3_path)

            # MP3를 WAV로 변환
            audio = AudioSegment.from_mp3(temp_mp3_path)
            audio = audio.set_frame_rate(16000)
            audio.export(wav_file_path, format='wav')

            # 임시 MP3 파일 삭제
            os.remove(temp_mp3_path)

            # 파일이 완전히 생성되었는지 확인
            if os.path.exists(wav_file_path):
                return True
            else:
                return False
        except Exception as e:
            print(f"변환 중 오류 발생: {e}")
            return False



if __name__ == "__main__":
    converter = GoogleTextToWavConverter()
    text = """안녕하세요! 영어를 배우는 것은 흥미롭고 도전적일 수 있습니다. 여러분의 영어 학습 여정에 도움이 될 몇 가지 팁을 드리겠습니다."""
    if converter.convert_to_wav(text, "test.wav"):
        wav_info = converter.get_wav_info("test.wav")
        print(wav_info)