import keyboard
import requests
import queue
import threading


# =====================================================
# ESP32 Configuration
# =====================================================

ESP32_IP = "10.164.92.49"
ESP32_PORT = 80


# =====================================================
# Command Queue
# =====================================================

command_queue = queue.Queue()


# =====================================================
# Send Command To ESP32
# =====================================================

def send_command(command):

    url = f"http://{ESP32_IP}:{ESP32_PORT}/action"

    params = {"cmd": command}

    try:

        response = requests.get(
            url,
            params=params,
            timeout=1
        )

        if response.status_code == 200:
            print(f"\nMoMo → {command}")

        else:
            print(
                f"\nESP32 error: "
                f"{response.status_code} "
                f"{response.text}"
            )

    except requests.RequestException:
        print(f"\nMoMo → {command} (connection failed)")


# =====================================================
# Send Custom Pomodoro Timer
# =====================================================

def set_pomodoro_timer():

    print("\n========== CUSTOM POMODORO ==========")

    try:

        focus = int(input("Focus time (minutes): "))
        break_time = int(input("Break time (minutes): "))

        if focus <= 0 or break_time <= 0:
            print("❌ Enter values greater than 0.")
            return

        url = f"http://{ESP32_IP}:{ESP32_PORT}/pomodoro"

        response = requests.get(
            url,
            params={
                "focus": focus,
                "break": break_time
            },
            timeout=2
        )

        if response.status_code == 200:
            print(f"✅ Pomodoro set to {focus} min / {break_time} min")

        else:
            print(
                f"ESP32 error: "
                f"{response.status_code} "
                f"{response.text}"
            )

    except ValueError:
        print("❌ Please enter numbers only.")

    except requests.RequestException as error:
        print(f"Connection failed: {error}")


# =====================================================
# Background Command Worker
# =====================================================

def command_worker():

    while True:

        command = command_queue.get()

        try:
            send_command(command)

        finally:
            command_queue.task_done()


# =====================================================
# Queue Command
# =====================================================

def queue_command(command):
    command_queue.put(command)


# =====================================================
# Keyboard Mapping
# =====================================================

KEYS = {

    "0": "0",   # Normal Eyes
    "1": "1",   # Calendar
    "2": "2",   # Clock
    "3": "3",   # Spotify
    "4": "4",   # Pomodoro Start/Pause
    "5": "5",   # Pomodoro Reset
    "6": "6",   # Pomodoro Screen

}


# =====================================================
# Keyboard Handler
# =====================================================

def handle_keyboard(event):

    # 7 opens custom timer menu
    if event.name == "7":
        set_pomodoro_timer()
        return

    command = KEYS.get(event.name)

    if command is not None:
        queue_command(command)


# =====================================================
# Start Keyboard Controller
# =====================================================

def start_keyboard():

    print()
    print("==================================")
    print("      MoMo Keyboard Controller")
    print("==================================")
    print()
    print("0 → Normal Eyes")
    print("1 → Calendar")
    print("2 → Time / Date")
    print("3 → Spotify")
    print("4 → Pomodoro Start / Pause")
    print("5 → Pomodoro Reset")
    print("6 → Pomodoro Screen")
    print("7 → Set Focus / Break Time")
    print()
    print("CTRL + C → Stop")
    print()

    worker = threading.Thread(
        target=command_worker,
        daemon=True
    )
    worker.start()

    keyboard.on_press(handle_keyboard)

    try:
        keyboard.wait()

    except KeyboardInterrupt:
        print("\nKeyboard controller stopped.")

    finally:
        keyboard.unhook_all()


# =====================================================
# Main
# =====================================================

if __name__ == "__main__":
    start_keyboard()