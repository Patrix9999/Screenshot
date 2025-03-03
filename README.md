# Introduction

This Gothic plugin adds the ability to create and save screenshots into `Game/Screens/` subfolder.  
It works for every Gothic game, you can use it to create awesome screenshots that will last forever by just pressing `Print Screen` key.

# Settings

The plugin can be configured by changing settings in `Gothic.ini` file.  
Those config settings are automatically generated when you start a new game.

Below you can find the default configuration with option descriptions:

```ini
[Screenshot]
sfx=carve02.wav			; Sound that is being played when you press PrintScreen key, can be set to empty text for no sound
file_type=jpg			; Produced screenshot file type, supported types are: jpg, png, bmp
jpg_quality=95			; JPG quality, set this value in range <0, 100>
border_fix=1			; Enable border fix for fullscreen game? Useful for ppl that are playing on System Pack
```

# Usage

- Download the Screenshot.dll from [releases page](https://github.com/Patrix9999/Screenshot/releases)
- Place it in `Game/System/autorun` directory