# Project Overview

**Dublin Bus 2 Go** is a native Pebble smartwatch application designed to provide real-time passenger information (RTPI) for Dublin Bus services. It allows users to find nearest bus stops based on their location, check bus arrival times, and save favorite stops for quick access.

The project is a hybrid application consisting of:
*   **C Code (Watch App):** Runs on the Pebble device, handling the user interface and display logic.
*   **JavaScript Code (PebbleKit JS):** Runs on the connected smartphone, handling geolocation, network requests to Dublin Bus APIs, and data parsing.

## Tech Stack

*   **Platforms:** Pebble `aplite` (Classic/Steel) and `basalt` (Time/Time Steel).
*   **SDK:** Pebble SDK 3.0.
*   **Languages:** C (Standard C99 for Pebble), JavaScript (ES5 for PebbleKit JS).
*   **Build System:** `waf` (Python-based build system used by Pebble).

# Architecture

## Watch Application (C)
The C application follows the standard Pebble event loop model.
*   **Entry Point:** `src/app.c` initializes the AppMessage system and launches the main UI (Home).
*   **UI Structure:** The app appears to be view-based, with `src/home.c` likely managing the main menu/list.
*   **Communication:** Uses `AppMessage` to exchange dictionaries with the JS component.

## Phone Application (JS)
The JavaScript component (`src/app.js`) acts as a proxy between the watch and external services.
*   **Data Sources:**
    *   **RTPI Data:** Fetches real-time arrival info via a SOAP XML endpoint (`http://rtpi.dublinbus.ie/DublinBusRTPIService.asmx`).
    *   **Stop Locations:** Fetches stop coordinates via a proprietary JSON endpoint (`https://dublinbus.ie/...`).
*   **Storage:** Uses `localStorage` on the phone to persist the user's list of saved/favorite stops.
*   **Geolocation:** Uses the phone's native Geolocation API to find nearby stops.

# Building and Running

The project uses the standard Pebble build commands. Ensure you have the Pebble SDK installed.

**Build:**
```bash
pebble build
```

**Install (to simulator or phone):**
```bash
pebble install --emulator basalt
# OR
pebble install --phone <PHONE_IP>
```

**Logs:**
```bash
pebble logs
```

# Key Files

*   **`src/app.c`**: The main C entry point. Handles initialization and the main event loop.
*   **`src/app.js`**: The core JavaScript logic. Handles API requests, XML parsing, and location services.
*   **`appinfo.json`**: Project metadata, including UUID, resources, capabilities (location), and AppKey definitions (mapping string keys to integers for AppMessage).
*   **`wscript`**: The build configuration file for `waf`. It defines how C and JS files are compiled and bundled.
*   **`src/js/`**: Directory where `app.js` and other JS modules are located/concatenated from.

# Development Notes

*   **Legacy Code:** The JS code contains fallbacks for "Apple" parsing, suggesting legacy WebKit support or workarounds for specific XML parsing issues on iOS.
*   **Hardcoded Config:** There appears to be a hardcoded local IP address (`http://192.168.1.7:8080/`) for the configuration page in `src/app.js`, which should likely be updated or made configurable for production.
*   **HTTP/HTTPS:** The RTPI service uses HTTP, while the map service uses HTTPS. Mixed content might be an issue depending on the execution environment strictness.
