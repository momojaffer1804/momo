import os
import time
import requests

import spotipy
from spotipy.oauth2 import SpotifyOAuth
from dotenv import load_dotenv


# =====================================================
# Configuration
# =====================================================

load_dotenv()

CLIENT_ID = os.getenv("SPOTIFY_CLIENT_ID")
CLIENT_SECRET = os.getenv("SPOTIFY_CLIENT_SECRET")
REDIRECT_URI = os.getenv("SPOTIFY_REDIRECT_URI")

ESP32_IP = "10.164.92.49"

if not CLIENT_ID or not CLIENT_SECRET or not REDIRECT_URI:
    raise RuntimeError(
        "Missing Spotify credentials. "
        "Check your .env file."
    )


# =====================================================
# Spotify Authentication
# =====================================================

scope = (
    "user-read-currently-playing "
    "user-read-playback-state"
)

sp = spotipy.Spotify(
    auth_manager=SpotifyOAuth(
        client_id=CLIENT_ID,
        client_secret=CLIENT_SECRET,
        redirect_uri=REDIRECT_URI,
        scope=scope,
        open_browser=True,
    )
)


# =====================================================
# Get Current Track
# =====================================================

def get_current_track():

    current = sp.current_playback()

    if current is None or current.get("item") is None:
        return None

    track = current["item"]

    return {
        "name": track["name"],
        "artist": ", ".join(
            artist["name"]
            for artist in track["artists"]
        ),
        "album": track["album"]["name"],
        "is_playing": current["is_playing"],
        "progress_ms": current["progress_ms"],
        "duration_ms": track["duration_ms"],
    }


# =====================================================
# Send Track Data To MoMo
# =====================================================

def send_to_momo(track):

    if track is None:
        return

    try:

        url = f"http://{ESP32_IP}/spotify"

        response = requests.post(
            url,
            json=track,
            timeout=2
        )

        print(
            f"MoMo response: "
            f"{response.status_code} - "
            f"{response.text}"
        )

    except requests.RequestException as error:

        print(
            f"MoMo connection error: {error}"
        )


# =====================================================
# Display Track
# =====================================================

def display_track(track):

    if track is None:

        print(
            "\nNothing is currently playing."
        )

        return

    progress = track["progress_ms"] / 1000
    duration = track["duration_ms"] / 1000

    status = (
        "PLAYING"
        if track["is_playing"]
        else "PAUSED"
    )

    print()
    print("========== MoMo Spotify ==========")
    print(f"Song     : {track['name']}")
    print(f"Artist   : {track['artist']}")
    print(f"Album    : {track['album']}")
    print(f"Status   : {status}")
    print(
        f"Progress : "
        f"{progress:.0f}s / {duration:.0f}s"
    )
    print("==================================")


# =====================================================
# Spotify Monitor
# =====================================================

def monitor_spotify():

    print()
    print("==================================")
    print("      MoMo Spotify Monitor")
    print("==================================")
    print("Monitoring Spotify...")
    print(f"ESP32    : {ESP32_IP}")
    print("Press CTRL+C to stop.")
    print()

    last_track = None
    last_playing = None

    while True:

        try:

            track = get_current_track()

            # =========================================
            # Nothing Playing
            # =========================================

            if track is None:

                if last_track is not None:

                    print(
                        "\nNothing is currently playing."
                    )

                last_track = None
                last_playing = None

            # =========================================
            # Track Available
            # =========================================

            else:

                track_changed = (
                    last_track != track["name"]
                    or last_playing != track["is_playing"]
                )

                # Show information only when
                # song/play state changes

                if track_changed:

                    display_track(track)

                    last_track = track["name"]
                    last_playing = track["is_playing"]

                # =====================================
                # Always Send Fresh Progress To MoMo
                # =====================================

                send_to_momo(track)

            # =========================================
            # Update Every 2 Seconds
            # =========================================

            time.sleep(2)

        except KeyboardInterrupt:

            print("\n")
            print("Spotify monitor stopped.")
            break

        except Exception as error:

            print()
            print(f"Spotify error: {error}")
            print("Retrying in 5 seconds...")

            time.sleep(5)


# =====================================================
# Main
# =====================================================

if __name__ == "__main__":

    monitor_spotify()