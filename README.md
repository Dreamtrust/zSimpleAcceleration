# zSimpleAcceleration

A [Union](https://worldofplayers.ru/threads/37530/) plugin for **Gothic II: Night of the Raven** that lets you toggle a character animation speed multiplier with a configurable hotkey.

## Features

- Toggle fast-animation mode on/off with a single key press
- Configurable hotkey via `gothic.ini` (supports `KEY_*` name strings)
- Configurable speed multiplier via `gothic.ini`
- On-screen feedback message when toggling
- Zero external dependencies — standalone `.vdf`, static CRT

## Installation

1. Download the latest `zSimpleAcceleration.vdf` from the [Releases](../../releases) page.
2. Place it in your Gothic II `Data/` folder:
   ```
   Gothic II/Data/zSimpleAcceleration.vdf
   ```
3. Launch the game. On first run the plugin will write default settings to `gothic.ini`.

## Configuration

After first launch, open `Gothic II/System/gothic.ini` and find the `[ZSIMPLE_ACCELERATION]` section:

```ini
[ZSIMPLE_ACCELERATION]
Hotkey=KEY_U        ; Key to toggle acceleration (default: U)
Multiplier=1.2      ; Animation speed multiplier (default: 1.2x)
```

### Available key names

You can use any `KEY_*` constant below. Values are DirectInput scan codes.

| Name | Key | Name | Key | Name | Key |
|------|-----|------|-----|------|-----|
| `KEY_A` … `KEY_Z` | A–Z letters | `KEY_F1` … `KEY_F12` | F-keys | `KEY_SPACE` | Space |
| `KEY_UP` | Arrow Up | `KEY_DOWN` | Arrow Down | `KEY_LEFT` | Arrow Left |
| `KEY_RIGHT` | Arrow Right | `KEY_INSERT` | Insert | `KEY_DELETE` | Delete |
| `KEY_HOME` | Home | `KEY_END` | End | `KEY_PGUP` | Page Up |
| `KEY_PGDN` | Page Down | `KEY_RETURN` | Enter | `KEY_LSHIFT` | Left Shift |
| `KEY_RSHIFT` | Right Shift | `KEY_LCONTROL` | Left Ctrl | `KEY_RCONTROL` | Right Ctrl |
| `KEY_LALT` | Left Alt | `KEY_RALT` | Right Alt | `KEY_TAB` | Tab |
| `KEY_CAPSLOCK` | Caps Lock | `KEY_BACKSPACE` | Backspace | `KEY_ESCAPE` | Escape |
| `KEY_NUMPAD0`…`KEY_NUMPAD9` | Numpad digits | `KEY_NUMPADENTER` | Numpad Enter | `KEY_NUMLOCK` | Num Lock |

You can also write the raw scan code as a decimal number, e.g. `Hotkey=22` for `KEY_U`.

## Building from source

### Requirements

- [Visual Studio 2022](https://visualstudio.microsoft.com/) with **Desktop development with C++** and **CMake tools** workloads
- [Git](https://git-scm.com/)

### Steps

```bash
git clone --recursive https://github.com/YOUR_USERNAME/zSimpleAcceleration.git
```

Then open the folder in Visual Studio, select the `G2A-Release` configuration and build.

## Project structure

```
src/
  Plugin.hpp    — Core plugin logic (hotkey reading, Game_Loop, render hook)
  Plugin.cpp    — Per-engine version include dispatcher
  DllMain.cpp   — Entry point
vdf/            — VDF packaging assets
```

## License

MIT — see [LICENSE](LICENSE).
