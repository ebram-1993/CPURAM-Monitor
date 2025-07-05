import requests
import time


def get_cpu_usage():
    try:
        response = requests.get("http://localhost:8080/cpu")
        return response.json().get("cpu_usage", None)
    except Exception as e:
        print(f"[CPU ERROR] {e}")
        return None

def get_memory_usage():
    try:
        response = requests.get("http://localhost:8080/memory")
        return response.json().get("mem_usage", None)
    except Exception as e:
        print(f"[MEMORY ERROR] {e}")
        return None

def main():
    print("📡 Monitoring system usage from http://localhost:8080 ...\n")
    while True:
        cpu = get_cpu_usage()
        mem = get_memory_usage()

        if cpu is not None and mem is not None:
            print(f" Memory Usage: {mem:.2f}% | ⚙️ CPU Usage: {cpu:.2f}%")
        else:
            print(" Failed to retrieve metrics. Is the C++ server running?")

        time.sleep(2)

if __name__ == "__main__":
    main()