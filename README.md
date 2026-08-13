# MoMo or Modular Organization & Monitoring Operator

I had too many tabs open. GitHub in one tab, solving leetcode somewhere else, Spotify buried under all of it. I would lose track of the submissions, forget upcoming deadlines, and get pulled into whatever tab I opened next. It was not helping me focus, it was just more stuff to check.

I wanted something that did not need a tab. Something on my desk that I could just glance at.

So I am building **MoMo**.

## Stack

### Hardware
- ESP32 (38-pin NodeMCU, ESP-WROOM-32)
- 0.96" 128x64 I2C OLED display
- Breadboard and jumper wires for now, no soldering yet

### Software
- Arduino/C++ firmware on the ESP32
- Python on the laptop side for pulling and processing data
- Communication over USB serial right now, moving to Wi-Fi and HTTP later
- Built and tested with PlatformIO
- Simulated in Wokwi before touching real hardware

## What it does

- Time and date
- Pomodoro countdown
- Current focus task
- Upcoming submission and deadline notifications
- Robot expressions that react to what is happening
- What's playing on Spotify
- GitHub streak
- Whether today's LeetCode problem is solved

The screens rotate on their own, but I can jump to one directly when I want to check something.

## Why this is not just another ESP32 clock

Most desk display projects are one thing wired to a screen and printing text. That is not really what I wanted to build.

The ESP32 does not handle the actual data logic. It receives a small string of data and draws it. The laptop side handles the processing and decides what the device should show, keeping the ESP32 firmware lightweight as new features are added.

The face is not just static icons either. There is a small state machine behind it with idle, focus, and reaction states, so the robot's expression changes based on what is happening. When a Pomodoro session ends or a deadline is approaching, it reacts instead of just displaying text.

It is wired over USB right now to keep the first version simple and reliable. Later, the ESP32 will communicate with the laptop over Wi-Fi, removing the cable.

## Status

Right now it is at the first stage. ESP32 boots, OLED is wired up, and the robot eyes blink, all verified in simulation before touching the real hardware.

Everything else gets added stage by stage from here.
