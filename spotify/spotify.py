import time
import requests

from spotify_client import get_current_track


# =====================================================
# Configuration
# =====================================================

ESP32_IP = "10.164.92.49"
ESP32_PORT = 80

UPDATE_INTERVAL = 2


# =====================================================
# Send Spotify State To ESP32
# =====================================================

def send_to_esp32(track):

    if track is None:
        return

    url = f"http://{ESP32_IP}:{ESP32_PORT}/spotify"

    params = {
        "song": track["name"],
        "artist": track["artist"],
        "playing": str(track["is_playing"]).lower(),
        "progress": track["progress_ms"],
        "duration": track["duration_ms"],
    }

    try:

        response = requests.get(
            url,
            params=params,
            timeout=2
        )

        if response.status_code != 200:

            print(
                f"ESP32 error: "
                f"{response.status_code} "
                f"{response.text}"
            )

    except requests.RequestException as error:

        print(
            f"ESP32 connection error: "
            f"{error}"
        )


# =====================================================
# Clear Spotify Data
# =====================================================

def send_empty():

    url = f"http://{ESP32_IP}:{ESP32_PORT}/spotify"

    params = {
        "song": "",
        "artist": "",
        "playing": "false",
        "progress": 0,
        "duration": 0,
    }

    try:

        requests.get(
            url,
            params=params,
            timeout=2
        )

    except requests.RequestException:

        pass


# =====================================================
# Spotify Monitor
# =====================================================

def monitor_spotify():

    print()
    print("==================================")
    print("       MoMo Spotify Display")
    print("==================================")
    print("Sending Spotify data to ESP32...")
    print()
    print("Keyboard controls are handled")
    print("by controller/keyboard.py")
    print()
    print("CTRL+C → Stop")
    print()

    last_song = None
    last_playing = None

    try:

        while True:

            try:

                track = get_current_track()

                if track is None:

                    if last_song is not None:

                        print(
                            "\nNothing is currently playing."
                        )

                        send_empty()

                    last_song = None
                    last_playing = None

                else:

                    song_changed = (
                        last_song != track["name"]
                    )

                    state_changed = (
                        last_playing != track["is_playing"]
                    )

                    if song_changed or state_changed:

                        status = (
                            "PLAYING"
                            if track["is_playing"]
                            else "PAUSED"
                        )

                        print()
                        print(
                            "========== Spotify =========="
                        )

                        print(
                            f"Song     : "
                            f"{track['name']}"
                        )

                        print(
                            f"Artist   : "
                            f"{track['artist']}"
                        )

                        print(
                            f"Album    : "
                            f"{track['album']}"
                        )

                        print(
                            f"Status   : "
                            f"{status}"
                        )

                        print(
                            f"Progress : "
                            f"{track['progress_ms'] / 1000:.0f}s / "
                            f"{track['duration_ms'] / 1000:.0f}s"
                        )

                        print(
                            "=============================="
                        )

                        last_song = track["name"]
                        last_playing = track["is_playing"]

                    # -----------------------------------------
                    # Send Spotify state every 2 seconds
                    # -----------------------------------------

                    send_to_esp32(track)

                time.sleep(UPDATE_INTERVAL)

            except Exception as error:

                print()
                print(
                    f"Spotify error: {error}"
                )

                print(
                    "Retrying in 5 seconds..."
                )

                time.sleep(5)

    except KeyboardInterrupt:

        print()
        print("Spotify display stopped.")


# =====================================================
# Main
# =====================================================

if __name__ == "__main__":

    monitor_spotify()