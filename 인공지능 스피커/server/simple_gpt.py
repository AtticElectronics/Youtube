import time
import os
from dotenv import load_dotenv
from openai import OpenAI

class JarvisMemoryManager:
    def __init__(self):
        load_dotenv()
        self.api_key = os.environ['OPENAI_API_KEY']
        self.client = OpenAI(api_key=self.api_key)
        self.memory_id = self.load_memory()

    def create_new_memory(self):
        thread = self.client.beta.threads.create()
        with open('memory.txt', 'w') as file:
            file.write(thread.id)
        self.memory_id = thread.id

    def load_memory(self):
        # memory.txt 파일이 존재하는지 확인
        if not os.path.exists('memory.txt'):
            # 파일이 없으면 새 메모리 생성
            self.create_new_memory()
        # 파일에서 메모리 ID 읽기
        with open('memory.txt', 'r') as file:
            return file.read()

    def add_msg(self, msg):
        message = self.client.beta.threads.messages.create(
            thread_id=self.memory_id,
            role="user",
            content=msg
        )
        print(message)

    def get_run_id(self, ai_name):
        if ai_name == "jarvis_4":
            run = self.client.beta.threads.runs.create(
                thread_id=self.memory_id,
                assistant_id=os.environ['AI_ID4']
            )
        elif ai_name == "jarvis_3.5":
            run = self.client.beta.threads.runs.create(
                thread_id=self.memory_id,
                assistant_id=os.environ['AI_ID35']
            )
        elif ai_name =="Terminal_AI":
            run = self.client.beta.threads.runs.create(
                thread_id=self.memory_id,
                assistant_id=os.environ['TERMINAL']
            )
        else:
            return None
        return run.id

    def wait_run(self, rid):
        TIMEOUT = 20
        INACTIVE_STATUSES = ['queued', 'in_progress', 'cancelling', 'requires_action']
        answer = "20초동안 응답이 없습니다."
        for i in range(TIMEOUT):
            run = self.client.beta.threads.runs.retrieve(thread_id=self.memory_id, run_id=rid)
            if run.status in INACTIVE_STATUSES:
                pass
            elif run.status == 'completed':
                messages = self.client.beta.threads.messages.list(thread_id=self.memory_id)
                answer = messages.data[0].content[0].text.value
                break
            elif run.status in ['failed', 'cancelled', 'expired']:
                answer = f"응답이 {run.status}되었습니다."
                break
            time.sleep(1)
        return answer

if __name__ == "__main__":
    manager = JarvisMemoryManager()
    manager.add_msg("내 이름이 뭐라고?")
    rid = manager.get_run_id("jarvis_3.5")
    print(manager.wait_run(rid))

    manager.add_msg("내 이름은 김철수야")
    rid = manager.get_run_id("jarvis_3.5")
    print(manager.wait_run(rid))

    manager.add_msg("내 이름이 뭐라고?")
    rid = manager.get_run_id("jarvis_3.5")
    print(manager.wait_run(rid))

    manager.create_new_memory()

    manager.add_msg("내 이름이 뭐라고?")
    rid = manager.get_run_id("jarvis_3.5")
    print(manager.wait_run(rid))
