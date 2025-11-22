from locust import HttpUser, task, between
import random

# Зроби тут списки реальних слів, які точно є в корпусі.
# Поки залишимо базовий набір (можеш розширити).
WORDS = [
    "apple", "data", "network", "python", "server", "thread",
    "model", "search", "computer", "system", "algorithm",
    "parallel", "index", "performance", "memory"
]

class SearchWordUser(HttpUser):
    # тестуємо HTTP-proxy
    host = "http://127.0.0.1:3000"

    # майже без паузи між запитами -> високе навантаження
    wait_time = between(0.0, 0.05)

    @task(100)
    def search_word(self):
        w = random.choice(WORDS)
        self.client.get("/search", params={"q": w}, name="/search?q=word")

    @task(1)
    def hello(self):
        self.client.get("/hello")

    @task(1)
    def info(self):
        self.client.get("/info")
