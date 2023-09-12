#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_MINIAUDIO
#include "olcPGEX_MiniAudio.h"

class Demo : public olc::PixelGameEngine
{
public:
    Demo()
    {
        sAppName = "Demo MiniAudio";
    }
    
public:
    bool OnUserCreate() override
    {
        // Load a sample from a file, currently decodes WAV and MP3
        // files, out-of-the-box without further coding or configuration.
        // returns a sample ID (int), for future control calls.
        song1 = ma.LoadSound("assets/sounds/song1.mp3");

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        
        if(GetKey(olc::SPACE).bPressed)
        {
            // Toggle takes a sample ID (int) and either starts playback or pauses playback
            // depending on whether the sample is currently playing, or not.
            ma.Toggle(song1);
        }
            
        
        if(GetKey(olc::MINUS).bHeld)
        {
            pan -= 1.0f * fElapsedTime;
            if(pan < -1.0f) pan = -1.0f;
        }
            
        if(GetKey(olc::EQUALS).bHeld)
        {
            pan += 1.0f * fElapsedTime;
            if(pan > 1.0f) pan = 1.0f;
        }

        if(GetKey(olc::OEM_4).bHeld)
            pitch -= 1.0f * fElapsedTime;

        if(GetKey(olc::OEM_6).bHeld)
            pitch += 1.0f * fElapsedTime;

        if(GetKey(olc::DOWN).bHeld)
        {
            volume -= 1.0f * fElapsedTime;
            if(volume < 0.0f) volume = 0.0f;
        }
            
        if(GetKey(olc::UP).bHeld)
        {
            volume += 1.0f * fElapsedTime;
            if(volume > 1.0f) volume = 1.0f;
        }
            
        
        // Reset pan, pitch, and volume
        if(GetKey(olc::R).bPressed)
        {
            pan = 0.0f;
            pitch = 1.0f;
            volume = 1.0f;
        }
        
        // Set pan, takes a sample ID (int), and a float
        // -1.0 to 1.0 where 0 is center
        ma.SetPan(song1, pan);

        // Set pitch, takes a sample ID (int), and a float
        // 1.0 is normal pitch
        ma.SetPitch(song1, pitch);

        // Set volume, takes a sample ID (int), and a float
        // 0.0 to 1.0 where 1.0 is full volume
        ma.SetVolume(song1, volume);
        
        // Gets the current playback position in the provided sample ID (int),
        // returns float 0.0 to 1.0, nearer 1.0 is near the end
        seek = ma.GetCursorFloat(song1);

        // Draw Instructions and Indicators
        Clear(olc::BLACK);

        DrawStringDecal({5, 5}, \
            "-------- INFO --- CONTROLS -\n"
            "\n"
            "Pan    <" + std::to_string(pan)    + ">   -, =\n"
            "\n"
            "Pitch  <" + std::to_string(pitch)  + ">   [, ]\n"
            "\n"
            "Volume <" + std::to_string(volume) + "> Up, Down\n", \
        olc::WHITE, {0.5f, 0.5f});

        
        olc::vi2d center = (GetScreenSize() / 2);
        
        DrawStringDecal((center - olc::vi2d{0, 24}) - (olc::vf2d(GetTextSize("olcPGEX_MiniAudio Demo")) * 1.5f / 2.0f), "olcPGEX_MiniAudio Demo", olc::WHITE, {1.5f, 1.5f});
        DrawStringDecal(center - (GetTextSize("Hit <SPACE> To Toggle Playback") / 2), "Hit <SPACE> To Toggle Playback", olc::WHITE);
        DrawStringDecal((center + olc::vi2d{0, 16}) - (GetTextSize("Hit <R> TO Reset Pan/Pitch/Volume") / 2), "Hit <R> TO Reset Pan/Pitch/Volume", olc::WHITE);

        DrawStringDecal({5, 160}, \
            "Music: Joy Ride [Full version] by MusicLFiles\n"
            "Free download: https://filmmusic.io/song/11627-joy-ride-full-version\n"
            "Licensed under CC BY 4.0: https://filmmusic.io/standard-license\n", \
        olc::WHITE, {0.5f, 0.5f});
        
        // Draw The Playback Cursor (aka the position in the sound file)
        FillRect({0, 175}, { (int)(ScreenWidth() * seek), 10 }, olc::YELLOW);
        
        #if defined(__EMSCRIPTEN__)
            return true;
        #else
            return !GetKey(olc::ESCAPE).bPressed;
        #endif
    }

    // The instance of the audio engine, no fancy config required.
    olc::MiniAudio ma;
    
    // To keep track of our sample ID
    int song1;
    
    // For demonstration controls, with sensible default values
    float pan    = 0.0f;
    float pitch  = 1.0f;
    float seek   = 0.0f;
    float volume = 1.0f;

};

int main()
{
    Demo demo;
    if (demo.Construct(320, 180, 4, 4))
        demo.Start();
    return 0;
}
