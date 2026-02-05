# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Dublin Bus 2 Go is a Pebble smartwatch app displaying real-time Dublin Bus arrival information (RTPI). It uses a hybrid C/JavaScript architecture with the Pebble AppMessage protocol for communication between the watch and phone companion app.

## Build Commands

```bash
pebble build                    # Compile the project
pebble install --emulator basalt  # Install to color watch emulator
pebble install --emulator aplite  # Install to B&W watch emulator
pebble logs                     # View device logs
```

The build system uses WAF (wscript). Output goes to `build/` with platform-specific subdirectories (aplite for B&W, basalt for color).

## Architecture

### Dual-Layer Design

**Watchapp (C)** - Runs on Pebble watch:
- UI screens using Pebble SDK (Window, MenuLayer, TextLayer, ScrollLayer)
- Handles button input and screen navigation
- Receives data via AppMessage protocol

**Companion App (JavaScript)** - Runs on phone:
- Fetches data from Dublin Bus RTPI APIs
- Performs geolocation and distance calculations
- Manages saved stops in localStorage
- Sends data to watch via AppMessage queue

### Communication Flow

1. Watch sends action request (e.g., ACTION_GET_NEAREST_STOPS)
2. JS companion receives message, makes API call
3. JS populates AppMessage dictionary (max 20 entries per message)
4. appMessageQueue.js ensures reliable delivery with retries
5. Watch displays data in appropriate screen

### Key Source Files

- `src/app.c` - Entry point, initializes message protocol and shows home screen
- `src/app.js` - Main companion logic: API client (db2go), geolocation (db2goLocation)
- `src/app_message.c` - AppMessage protocol handlers, routes incoming messages
- `src/appMessageQueue.js` - Reliable message delivery with retry logic
- `src/home.c` - Main menu (Saved Stops / Nearest)
- `src/stoplist.c` - Displays list of bus stops
- `src/stop.c` - Shows buses arriving at a stop, supports shake-to-refresh

### Data Structures

- `src/Stop.c/.h` - Stop struct (name, id, distance, bearing)
- `src/Bus.c/.h` - Bus struct (route, destination, due time)

### Platform Handling

The code supports two Pebble platforms:
- **aplite** - Original Pebble (B&W, 144x168)
- **basalt** - Pebble Time (color, SDK 3)

Use `#ifdef PBL_COLOR`, `#ifdef PBL_BW`, `#ifdef PBL_SDK_3` guards for platform-specific code.

## API Endpoints

**Stop Details (SOAP XML):**
`https://rtpi.dublinbus.ie/DublinBusRTPIService.asmx` - GetRealTimeStopData

**Nearby Stops (JSON):**
`https://dublinbus.ie/Templates/Public/RoutePlannerService/RTPIMapHandler.ashx` - Bounding box search

## AppMessage Keys

Defined in `appinfo.json` under `appKeys`:
- 0-8: Protocol keys (action, status, name, id, etc.)
- 1000-1019: bus_route_0 through bus_route_19
- 1100-1119: bus_destination_0 through bus_destination_19
- 1200-1219: bus_duein_0 through bus_duein_19
- 2000-2019: stop_name_0 through stop_name_19
- 2100-2119: stop_id_0 through stop_id_19
- 2200-2319: stop_distance and stop_bearing arrays
