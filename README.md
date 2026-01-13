**Arduino Mini Dashboard**

A small collection of simple apps (sketches) you can deploy to a microcontroller board to show different information on a tiny display. This repository currently contains one app: `bitcoin_tracker`. More apps will be added over time; each app will live in its own folder with the files needed to deploy it.

**Hardware**:
- **Board**: Any Arduino-compatible board that the sketch supports (e.g. Pro Mini, Uno, Nano, ESP32 — check the sketch for exact requirements).
- **Display**: Small OLED or similar display as used by the sketches.

**Software / Tools**:
- **Arduino IDE** (for manual upload) or
- **arduino-cli** (for command-line build/upload).

```
apps/
	bitcoin_tracker/       # holds the sketch and assets for the bitcoin tracker
```

Each app folder should contain the sketch (an `.ino` or `.cpp`/`.h` pair) and any required assets or libraries (or a `README.md` describing dependencies).

**Deploying the `bitcoin_tracker` app**

Arduino IDE:

1. Open `bitcoin_ticker.ino` (or the sketch folder if it is moved into an app folder).
2. Select the board and serial port in the IDE.
3. Click Upload.

arduino-cli (example):

1. Install `arduino-cli` and configure your board FQBN (follow Arduino CLI docs).
2. From the sketch folder (the folder that contains the `.ino` file) run:

```bash
arduino-cli compile --fqbn <your_fqbn> .
arduino-cli upload -p /dev/ttyUSB0 --fqbn <your_fqbn> .
```

Replace `<your_fqbn>` with your board identifier (e.g. `arduino:avr:uno`) and `/dev/ttyUSB0` with your serial device.

**Adding new apps**
- Create a new folder at the repo root named for the app.
- Place the sketch (`.ino`) and any assets there.
- Add a short `README.md` inside the app folder describing hardware, dependencies, and upload notes.

**License**
See the repository `LICENSE`.