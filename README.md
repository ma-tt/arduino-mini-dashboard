## Arduino Mini Dashboard

---

A small collection of compact apps (Arduino/ESP sketches) that display useful information on a tiny OLED. The repository currently includes one app: the Bitcoin tracker. Additional apps will be added into their own folders as they are created.

---

## Hardware

- Board: any Arduino/ESP-compatible board supported by the sketch (examples: Pro Mini, Uno, Nano, ESP32). Check each sketch for exact requirements.
- Display: SSD1306-compatible 128x64 OLED (I2C). Wiring and pins vary per sketch.

---

## Software & Tools

- Arduino IDE — for manual editing and upload
- arduino-cli — for building and uploading from the command line
- Libraries (installed via Library Manager or `arduino-cli`): `ArduinoJson`, `Adafruit GFX Library`, `Adafruit SSD1306` (individual sketches list exact dependencies)

---

## Repository Structure

Example layout:

```text
apps/
	bitcoin_tracker/    # app folder with sketch and assets
	another_app/        # future app
```

Each app should contain its sketch (`.ino` or `.cpp`/`.h`) and a small `README.md` describing hardware, library dependencies, and any special setup.

---

## Deploying the `bitcoin_tracker` App

Arduino IDE:

1. Open the folder that contains `bitcoin_ticker.ino` in the IDE.
2. Select the correct board and serial port.
3. Click Upload.

arduino-cli (example):

```bash
cd path/to/bitcoin_ticker
arduino-cli compile --fqbn <your_fqbn> .
arduino-cli upload -p /dev/ttyUSB0 --fqbn <your_fqbn> .
```

Replace `<your_fqbn>` with your board FQBN (for example `esp32:esp32:esp32dev`) and `/dev/ttyUSB0` with your serial device path.

---

## Adding New Apps

1. Create a new folder at the repository root named after the app.
2. Add the sketch and required assets.
3. Include a `README.md` in the app folder that lists hardware, libraries, and deploy instructions.

---

## Contributing

- Open a pull request with a new app folder or improvements.
- If a sketch requires non-standard libraries, include installation instructions or a `libraries.txt`.

---

## License

See the repository `LICENSE` file.