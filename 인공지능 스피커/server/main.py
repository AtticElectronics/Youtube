from esp32_tcp import *
import datetime
import whisper
from simple_gpt import *
from text_to_wav import *

class YoutubeAIspeakerAudioSender(ESPTCPAudioSend):
    def __init__(self, client_socket, client_address, work_list, lock):
        super().__init__(client_socket, client_address)
        self.work_list = work_list
        self.lock = lock
        self.GPT = JarvisMemoryManager()
        self.converter = GoogleTextToWavConverter()

    def start(self):
        try:
            signal, validData = self.recv_exact21024()
            if signal == self.SIGNAL_MAC:
                mac_str = ''.join('{:02X}'.format(b) for b in validData)
                filename = f'tts_{mac_str}.wav'

                print("전체",self.work_list)
                text_from_user = self.update_work_list(validData)
                print("해당입력문장",text_from_user[1])

                self.GPT.add_msg(text_from_user[1])
                rid = self.GPT.get_run_id("jarvis_3.5")

                result = self.GPT.wait_run(rid)
                #result = "하이 방가방가"
                answer_bytes = result.encode()
                self.send_21024(len(answer_bytes), answer_bytes)

                if self.converter.convert_to_wav(result, filename):
                    print("음성파일 생성 성공")
                else:
                    print("음성파일 생성 실패")

                signal, validData = self.recv_exact21024()
                self.wav_file = wave.open(filename, 'rb')
                while True:
                    frames_to_send = self.wav_file.readframes(self.FRAME_SIZE)
                    if not frames_to_send:
                        self.send_21024(self.SIGNAL_END, b'')
                        print("전송종료")
                        break
                    self.send_21024(len(frames_to_send), frames_to_send)
        except Exception as e:
            print(f"Error: {e}")
        finally:
            self.wav_file.close()
            self.close()

    def update_work_list(self, macaddress, text=None):

        current_time = datetime.datetime.now()

        with self.lock:
            # 2분 이상 지난 항목 제거
            self.work_list[:] = [item for item in self.work_list if (current_time - item[2]).seconds <= 120]

            if text is not None:
                # 텍스트가 주어진 경우: 덮어쓰기 또는 추가
                for item in self.work_list:
                    if item[0] == macaddress:
                        item[1] = text
                        item[2] = current_time
                        return
                self.work_list.append([macaddress, text, current_time])
            else:
                # 텍스트가 주어지지 않은 경우: 해당 인덱스 찾아서 pop 및 반환
                for i, item in enumerate(self.work_list):
                    if item[0] == macaddress:
                        return self.work_list.pop(i)
            return None  # 맥주소가 리스트에 없는 경우

class YoutubeAIspeakerAudioReciver(ESP32TCPAudioRecv):
    def __init__(self, client_socket, client_address, work_list, lock):
        super().__init__(client_socket, client_address)
        self.work_list = work_list
        self.lock = lock
        self.model = whisper.load_model("base")

    def start(self):
        try:
            macaddress = ""
            while True:
                signal, validData = self.recv_exact21024()
                if signal == self.SIGNAL_MAC:
                    self.mkWaveFile(validData)
                    macaddress = validData
                if signal == self.SIGNAL_END:
                    self.wav_file.close()
                    break;
                if 0 < signal <= 1024:
                    amplified_data = self.amplify_audio_data(validData, 10)
                    self.wav_file.writeframes(amplified_data)

            # 위스퍼 문자열생성
            result = self.model.transcribe("F412FACB1214.wav")
            print(result["text"])

            print(self.work_list)
            self.update_work_list(macaddress,result["text"])
            self.send_21024(len(result["text"].encode()),result["text"].encode())


        except Exception as e:
            print(f"Error: {e}")
        finally:
            self.close()

    def update_work_list(self, macaddress, text=None):

        current_time = datetime.datetime.now()

        with self.lock:
            # 2분 이상 지난 항목 제거
            self.work_list[:] = [item for item in self.work_list if (current_time - item[2]).seconds <= 120]

            if text is not None:
                # 텍스트가 주어진 경우: 덮어쓰기 또는 추가
                for item in self.work_list:
                    if item[0] == macaddress:
                        item[1] = text
                        item[2] = current_time
                        return
                self.work_list.append([macaddress, text, current_time])
            else:
                # 텍스트가 주어지지 않은 경우: 해당 인덱스 찾아서 pop 및 반환
                for i, item in enumerate(self.work_list):
                    if item[0] == macaddress:
                        return self.work_list.pop(i)
            return None  # 맥주소가 리스트에 없는 경우

class ESPTCPServer:

    def __init__(self, host, port, audio_class, work_list, lock):
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind((host, port))
        self.server_socket.listen(5)
        self.audio_class = audio_class
        self.work_list = work_list
        self.lock = lock

    def handle_client(self, client_socket, client_address):
        audio_instance = self.audio_class(client_socket, client_address, self.work_list, self.lock)
        audio_instance.start()

    def start(self):
        while True:
            client_socket, client_address = self.server_socket.accept()
            client_thread = threading.Thread(target=self.handle_client, args=(client_socket, client_address))
            client_thread.start()

if __name__ == "__main__":
    work_list = []
    list_lock = threading.Lock()

    server1 = ESPTCPServer('0.0.0.0', 33819, YoutubeAIspeakerAudioSender, work_list, list_lock)
    server1_thread = threading.Thread(target=server1.start)
    server1_thread.start()

    server2 = ESPTCPServer('0.0.0.0', 33823, YoutubeAIspeakerAudioReciver, work_list, list_lock)
    server2_thread = threading.Thread(target=server2.start)
    server2_thread.start()
