from gevent import monkey
monkey.patch_all()

from locust import User, task, between, events
import random
import socket
import time

TCP_HOST = "127.0.0.1"
TCP_PORT = 8080

WORDS = [
    "apple", "data", "network", "python", "server", "thread",
    "model", "search", "computer", "system", "algorithm",
    "parallel", "index", "performance", "memory"
]

def recv_line(sock, buf: bytes):
    """
    Читає 1 лінію до '\n' і повертає (line, new_buf).
    """
    while b"\n" not in buf:
        chunk = sock.recv(4096)
        if not chunk:
            line = buf
            buf = b""
            return line.decode("utf-8", errors="ignore").strip("\r"), buf
        buf += chunk

    line, buf = buf.split(b"\n", 1)
    return line.decode("utf-8", errors="ignore").strip("\r"), buf


def send_command_and_read_all(cmd: str, timeout_s: float = 5.0) -> str:
    with socket.create_connection((TCP_HOST, TCP_PORT), timeout=timeout_s) as sock:
        sock.settimeout(timeout_s)
        sock.sendall((cmd + "\n").encode("utf-8"))

        lines = []
        buf = b""


        while True:
            line, buf = recv_line(sock, buf)
            if line == "":
                return ""
            if line.startswith("WAIT") or line.startswith("QUEUE"):
                continue
            lines.append(line)
            break


        while True:
            line, buf = recv_line(sock, buf)
            if line == "":
                break
            if line.startswith("WAIT") or line.startswith("QUEUE"):
                continue
            lines.append(line)

        return "\n".join(lines)


class CppSearchUser(User):
    wait_time = between(0.0, 0.05)

    def _measure(self, name: str, command: str):
        start = time.perf_counter()
        exc = None
        resp_text = ""
        try:
            resp_text = send_command_and_read_all(command)
        except Exception as e:
            exc = e

        elapsed_ms = (time.perf_counter() - start) * 1000

        events.request.fire(
            request_type="TCP",
            name=name,
            response_time=elapsed_ms,
            response_length=len(resp_text),
            exception=exc,
        )

    @task(100)
    def search_word(self):
        w = random.choice(WORDS)
        self._measure("SEARCH", f"SEARCH {w}")

    @task(1)
    def hello(self):
        self._measure("HELLO", "HELLO")

    @task(1)
    def info(self):
        self._measure("INFO", "INFO")
