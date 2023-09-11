#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_MINIAUDIO
#include "olcPGEX_MiniAudio.h"

class MiniAudio_Example : public olc::PixelGameEngine
{
public:
    MiniAudio_Example()
    {
        sAppName = "MiniAudio Example";
    }

    bool OnUserCreate() override
    {
        std::cout << "Music: Joy Ride [Full version] by MusicLFiles\n";
        std::cout << "Free download: https://filmmusic.io/song/11627-joy-ride-full-version\n";
        std::cout << "Licensed under CC BY 4.0: https://filmmusic.io/standard-license\n";

        song1 = miniaudio.LoadSound("assets/sounds/song1.mp3");
        
        
        ma_sound_get_length_in_pcm_frames(miniaudio.vecSounds.at(song1), &length);
        std::cout << "Song 1 Length In PCM Frames " << length << "\n"; 
        return true;
    }
    unsigned long long length;
    unsigned long long cursor;
    float volume = 1.0f;
    float pan    = 0.0f;
    float pitch  = 1.0f;

    bool OnUserUpdate(float fElapsedTime) override
    {
        if(GetKey(olc::LEFT).bHeld)
        {
            pan -= .5f * fElapsedTime;

            if(pan < -1.0f)
                pan = -1.0f;
            
            miniaudio.SetPan(song1, pan);
        }
        
        if(GetKey(olc::RIGHT).bHeld)
        {
            pan += .5f * fElapsedTime;

            if(pan > 1.0f)
                pan = 1.0f;
            
            miniaudio.SetPan(song1, pan);
        }

        if(GetKey(olc::O).bHeld)
        {
            pitch -= .5f * fElapsedTime;
            miniaudio.SetPitch(song1, pitch);
        }

        if(GetKey(olc::P).bHeld)
        {
            pitch += .5f * fElapsedTime;
            miniaudio.SetPitch(song1, pitch);
        }

        if(GetKey(olc::UP).bHeld)
        {
            volume += 1.0f * fElapsedTime;
            
            if(volume > 1.0f)
                volume = 1.0f;
            
            miniaudio.SetVolume(song1, volume);
        }

        if(GetKey(olc::DOWN).bHeld)
        {
            volume -= 1.0f * fElapsedTime;
            
            if(volume < 0.0f)
                volume = 0.0f;
            
            miniaudio.SetVolume(song1, volume);
        }
        
        if(GetKey(olc::Q).bPressed)
            miniaudio.Toggle(song1);

        if(GetKey(olc::W).bPressed)
            miniaudio.Pause(song1);

        if(GetKey(olc::E).bPressed)
            miniaudio.Stop(song1);

        ma_sound_get_cursor_in_pcm_frames(miniaudio.vecSounds.at(song1), &cursor);

        Clear(olc::BLACK);
        
        // Proof of life
        FillCircle(GetMousePos(), 5 ,olc::RED);
        
        DrawStringDecal({5.f,  5.f}, "Volume <"   + std::to_string(volume)   + "> [Up, Down]", olc::WHITE, olc::vf2d{ 0.5f, 0.5f });
        DrawStringDecal({5.f, 10.f}, "Pan <"   + std::to_string(pan)   + "> [Left, Right]", olc::WHITE, olc::vf2d{ 0.5f, 0.5f });
        DrawStringDecal({5.f, 15.f}, "Pitch <" + std::to_string(pitch) + "> [O, P]", olc::WHITE, olc::vf2d{ 0.5f, 0.5f });
        DrawStringDecal({5.f, 20.f}, "Cursor <" + std::to_string(cursor) + "> Length <" + std::to_string(length) + ">", olc::WHITE, olc::vf2d{ 0.5f, 0.5f });
        DrawStringDecal({5.f, 30.f}, "[Q, W, E] {Toggle, Pause, Stop and Rewind}", olc::WHITE, olc::vf2d{ 0.5f, 0.5f });


        // Volume indicator
        float volumeSize = 117.f * volume;
        
        
        
        DrawRect(olc::vf2d{ ScreenWidth() - 20.f,  5 }, { 10, 120 }, olc::WHITE);
        
        FillRect(olc::vf2d{ ScreenWidth() - 18.f,  (117.f + 7.f) - volumeSize }, olc::vf2d{  7, volumeSize }, olc::YELLOW);
        
        // Music Attribution
        DrawStringDecal({5, 165}, "Joy Ride [Full version] by MusicLFiles", olc::WHITE, olc::vf2d{ 0.5f, 0.5f });
        DrawStringDecal({5, 170}, "See LICENSE.md or Terminal for complete attribution details!", olc::WHITE, olc::vf2d{ 0.5f, 0.5f });

        return !GetKey(olc::ESCAPE).bPressed;
    }

    olc::MiniAudio miniaudio;
    int song1;

};

int main(int argc, char** argv)
{
    MiniAudio_Example example;
    
    if(example.Construct(320, 180, 4, 4))
        example.Start();

    return 0;
}