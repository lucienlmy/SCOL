# SCOL
Allows you to load and run RAGE scripts in Grand Theft Auto V Enhanced. It also exposes new native commands that extend the capabilities of RAGE Scripting Language. See the documentation [here](https://github.com/ShinyWasabi/SCOL/blob/main/NATIVES.md).

An example script to test is available [here](https://github.com/ShinyWasabi/SCOL/raw/main/hello_world.sco), and a full singleplayer mod menu with source code can be found [here](https://github.com/ShinyWasabi/RageMenu.git).

If you ever need for whatever reason, you can convert YSC files into SCO using [YSC2SCO.exe](https://github.com/ShinyWasabi/SCOL/raw/main/YSC2SCO.exe).

## Supported Formats
- `.sco`
- `.ysc`
- `.ysc.full` (uncompressed with OpenIV or CodeWalker)

## Installation
- Get `SCOL.asi` from `.\out\build\Release` after building the project.
- Download [xinput1_4.dll](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/x64-latest/xinput1_4-x64.zip).
- Place both files in the game directory where `GTA5_Enhanced.exe` is located.
- Launch the game. Make sure BattlEye is disabled.

## Usage
- By default, scripts are loaded from the game's current directory. You can change this in `SCOL.json`.
- The default key to reload all scripts is `F5`. This is also configurable in the `.json` file.
- When a script is loaded for the first time, a new section is added to `SCOL.json`:
```json
"examplescript": {
    "Args": [],
    "CleanupFunction": 0,
    "StackSize": 1424
}
```
You can edit these values as needed for your script. For `CleanupFunction`, you can pass the address of the function that you want to be called whenever scripts are reloaded. Note that it must not take any arguments.

You can override game scripts by placing custom scripts in the directory specified by `ScriptOverridesFolder`. When the game attempts to load a script, its hash is checked against the scripts in that directory; if a matching hash is found, the custom script is loaded instead of the original.