#  Inverted Index Search Service
Практична частина курсової роботи з дисципліни **«Паралельні обчислення»**.

Реалізовано:
- побудова інвертованого індексу з текстових файлів;
- паралельна побудова індексу (**ThreadPool + ConcurrentMap**);
- TCP-сервер пошуку;
- два клієнти (C++ CLI та веб-клієнт через HTTP-proxy);
- навантажувальне тестування (**Locust**).

---

## 1. Вимоги / середовище
- **Windows 10/11**
- **CLion** або будь-яке IDE з CMake
- **MinGW-w64 / g++** (C++17)
- **Node.js 18+**
- **Python 3.9+** + **Locust**

---

##  2. Збірка проєкту (CMake)

### Через CLion
1. Відкрити теку `coursework/`
2. Дочекатися генерації CMake
3. Зібрати таргет `coursework`

### Через консоль
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
cmake --build .

---

##  3. Запуск TCP-сервера
Параметри у `src/main.cpp`:
- **N** — кількість потоків для побудови індексу  
- **port** — TCP порт  
- **serverWorkers** — кількість потоків у серверному пулі

Запуск:
.\cmake-build-debug\coursework.exe

Очікуваний лог:
Loaded XXXX documents
[Index N=8] Build time: ...
[Server] Listening on port 8080

---

##  4. TCP протокол

Формат:
<COMMAND> [args...]\n

### Команди
HELLO  
→ `HELLO_OK`

INFO  
→
INFO_OK  
documents: <count>  
unique_words: <count>

SEARCH <word>  
→
DOCS <k>  
<file_path>\t<snippet>

SEARCH_PHRASE <text>  
→
DOCS <k>  
<file_path>

---

##  5. C++ CLI клієнт
src/client/Client.cpp

Збірка:
g++ src/client/Client.cpp -o Client.exe -lws2_32

Команди:
HELLO  
INFO  
SEARCH apple  
SEARCH_PHRASE some text here  
exit

---

Запуск:
locust -f locustfile.py --headless -u 100 -r 10 -t 1m
