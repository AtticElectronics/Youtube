from abc import ABC, abstractmethod
import wave
import struct
import socket
import threading

class ESP32TCP21024(ABC):
    SIGNAL_NONE = 3000
    SIGNAL_END = 3001
    SIGNAL_MAC = 3006

    __TOTAL__SIZE = 1026
    __PACKET__SIZE = 1024
    __HEADER__SIZE = 2


    def __init__(self, client_socket, client_address, openCallback=None, closeCallback=None):
        self.client_socket = client_socket
        self.client_address = client_address
        self.openCallback = openCallback
        self.closeCallback = closeCallback

    def send_21024(self, header, data):
        # header가 int 타입인지 확인
        if not isinstance(header, int):
            raise TypeError("header must be an integer")

        # data가 bytes 타입인지 확인
        if not isinstance(data, bytes):
            raise TypeError("data must be bytes")

        header_bytes = header.to_bytes(2, 'little')
        padding = b'\x00' * (self.__PACKET__SIZE - len(data))
        packet = header_bytes + data + padding
        self.client_socket.sendall(packet)

    def recv_exact21024(self):
        data = bytearray()
        while len(data) < self.__TOTAL__SIZE:
            packet = self.client_socket.recv(self.__TOTAL__SIZE - len(data))
            if not packet:
                return None
            data.extend(packet)
        signal_data = int.from_bytes(data[:self.__HEADER__SIZE], 'little')
        if signal_data >= self.SIGNAL_NONE:
            validData = data[self.__HEADER__SIZE:self.__HEADER__SIZE + signal_data-self.SIGNAL_NONE]
        else:
            validData = data[self.__HEADER__SIZE:self.__HEADER__SIZE + signal_data]
        return signal_data, validData

    @abstractmethod
    def start(self):
        pass

    def close(self):
        print(f"Disconnected from {self.client_address}")
        self.client_socket.close()
        self.OnClosedSocket()

    def OnOpenedSocket(self):
        if self.openCallback:
            self.openCallback()

    def OnClosedSocket(self):
        if self.closeCallback:
            self.closeCallback()

class ESP32TCPAudioRecv(ESP32TCP21024):
    wav_file = None
    def amplify_audio_data(self, data, k):
        amplified_data = bytearray()
        for i in range(0, len(data), 2):
            sample = struct.unpack('<h', data[i:i + 2])[0]
            amplified_sample = int(sample * k)
            amplified_sample = max(-32768, min(32767, amplified_sample))
            amplified_data.extend(struct.pack('<h', amplified_sample))
        return amplified_data

    def mkWaveFile(self, mac_address):
        mac_str = ''.join('{:02X}'.format(b) for b in mac_address)
        filename = f'{mac_str}.wav'
        self.wav_file = wave.open(filename, 'wb')
        self.wav_file.setnchannels(1)
        self.wav_file.setsampwidth(2)
        self.wav_file.setframerate(16000)

    def start(self):
        try:
            while True:
                signal, validData = self.recv_exact21024()
                if signal == self.SIGNAL_MAC:
                    self.mkWaveFile(validData)
                if signal == self.SIGNAL_END:
                    self.wav_file.close()
                    break;
                if 0 < signal <= 1024:
                    amplified_data = self.amplify_audio_data(validData, 10)
                    self.wav_file.writeframes(amplified_data)

            msg = "bye, esp32."
            self.send_21024(len(msg.encode()),msg.encode())

        except Exception as e:
            print(f"Error: {e}")
        finally:
            self.close()



class ESPTCPAudioSend(ESP32TCP21024):
    FRAME_SIZE = 512
    wav_file = None

    def start(self):
        try:
            signal, validData = self.recv_exact21024()
            if signal == self.SIGNAL_MAC:
                mac_str = ''.join('{:02X}'.format(b) for b in validData)
                filename = f'{mac_str}.wav'

                msg = f'hi, esp32. {mac_str}.wav will send!'
                msg_bytes = msg.encode()
                self.send_21024(len(msg_bytes), msg_bytes)  # 시작알림

                print(filename)
                signal, validData = self.recv_exact21024()
                self.wav_file = wave.open(filename, 'rb')
                while True:
                    frames_to_send = self.wav_file.readframes(self.FRAME_SIZE)
                    if not frames_to_send:
                        self.send_21024(self.SIGNAL_END, b'') # 종료알림
                        break
                    self.send_21024(len(frames_to_send), frames_to_send) # 스트리밍
        except Exception as e:
            print(f"Error: {e}")
        finally:
            self.wav_file.close()
            self.close()



class ESPTCPServer:
    def __init__(self, host, port, audio_class):
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind((host, port))
        self.server_socket.listen(5)
        self.audio_class = audio_class  # Audio 처리 클래스 저장

    def handle_client(self, client_socket, client_address):
        audio_instance = self.audio_class(client_socket, client_address)
        audio_instance.start()

    def start(self):
        while True:
            client_socket, client_address = self.server_socket.accept()
            client_thread = threading.Thread(target=self.handle_client, args=(client_socket, client_address))
            client_thread.start()


if __name__ == "__main__":
    # 두 번째 서버 인스턴스 생성 및 스레드 시작
    server2 = ESPTCPServer('0.0.0.0', 33823, ESP32TCPAudioRecv)
    server2_thread = threading.Thread(target=server2.start)
    server2_thread.start()

    # 첫 번째 서버 인스턴스 생성 및 스레드 시작
    server1 = ESPTCPServer('0.0.0.0', 33819, ESPTCPAudioSend)
    server1_thread = threading.Thread(target=server1.start)
    server1_thread.start()
