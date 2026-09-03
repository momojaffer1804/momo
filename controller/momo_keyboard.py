import keyboard
import requests


# =====================================================
# ESP32 Configuration
# =====================================================

ESP32_IP = "10.164.92.49"
ESP32_PORT = 80


# =====================================================
# Send Command To ESP32
# =====================================================

def send_command(command):

    url = f"http://{ESP32_IP}:{ESP32_PORT}/action"

    params = {
        "cmd": command
    }

    try:

        response = requests.get(
            url,
            params=params,
            timeout=2
        )

        if response.status_code == 200:

            print(f"\nMoMo → {command}")

        else:

            print(
                f"\nESP32 error: "
                f"{response.status_code} "
                f"{response.text}"
            )

    except requests.RequestException as error:

        print(
            f"\nESP32 connection error: "
            f"{error}"
        )


# =====================================================
# Keyboard Mapping
# =====================================================

KEYS = {
    "0": "0",
    "3": "3",
}


# =====================================================
# Keyboard Handler
# =====================================================

def handle_keyboard(event):

    command = KEYS.get(event.name)

    if command is not None:

        send_command(command)


# =====================================================
# Start Keyboard Controller
# =====================================================

def start_keyboard():

    print()
    print("==================================")
    print("       MoMo Keyboard Controller")
    print("==================================")
    print()
    print("0 → Normal eyes")
    print("3 → Spotify")
    print("CTRL+C → Stop")
    print()

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