# olcPGEX_MiniAudio

This extension abstracts the very robust and powerful miniaudio
library. It provides simple loading and playback of WAV and MP3
files. Because it's built on top of [miniaudio](https://miniaud.io), it requires next
to no addictional build configurations in order to be built
for cross-platform.

# Features

Cross-Platform, out-of-the-box. Easily use in your Linux, Windows, MacOS, and Emscripten projects.

### Loading Features
* Loads WAV files
* Loads MP3 files

### Playback Features
* Play a sample in one off, or looping mode.
* Stop a sample and reset it for future playback.
* Pause a sample.
* Toggle (Play/Pause), convenience function.

### Seeking Features
* Seek to a position in the sample by milliseconds, or by float.
* Seek forward from current position by milliseconds.
* Seek back (rewind) from current position by milliseconds.

### Expression Features
* Set volume of a sample, by float 0.0f is mute, 1.0f is full
* Set pan of a sample, by float -1.0f is left, 1.0f is right, 0.0f is center
* Set pitch of a sample, by float 1.0f is normal pitch

### Misc Getter Features
* Get the current position in the sample, in milliseconds.
* Get the current position in the sample, as float 0.0f is start, 1.0f is end.

# Usage

Add miniaudio.h and olcPGEX_MiniAudio.h to your project. (THE PGEX EXPECTS miniaudio.h to be either in the same directory, or in the include path of your toolchain)

Excerpts from demo.cpp, [for full example click here](demo/demo.cpp).


```cpp
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_MINIAUDIO
#include "olcPGEX_MiniAudio.h"
```

Declare an instance of the PGEX in your PGE derived class like so..

```cpp
    // The instance of the audio engine, no fancy config required.
    olc::MiniAudio ma;
    
    // To keep track of our sample ID
    int song1;
```

In OnUserCreate, load your sounds

```cpp
    bool OnUserCreate() override
    {
        // Load a sample from a file, currently decodes WAV and MP3
        // files, out-of-the-box without further coding or configuration.
        // returns a sample ID (int), for future control calls.
        song1 = ma.LoadSound("assets/sounds/song1.mp3");

        return true;
    }
```

Then use them in your OnUserUpdate, like so

```cpp
        if(GetKey(olc::SPACE).bPressed)
            ma.Toggle(song1);
```
# Building

For Windows MSVC and Emscripten the instructions match any other olcPixelGameEngine!

For Linux, add ``-ldl`` to the build command..

TODO: instructions for MacOS, I'm not a Mac user and have no way to test it!

That's it!

# Acknowledgements

I'd like to give a special thanks for JavidX9 (aka OneLoneCoder), AniCator, JustinRichardsMusic, and everybody else who was a part of that audiophile conversation when I asked for help! Your patience and feedback made this project possible. Thank you!
