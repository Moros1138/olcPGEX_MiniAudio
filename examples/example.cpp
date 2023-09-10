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
        
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        if(GetKey(olc::Q).bPressed)
            miniaudio.Toggle(song1);

        if(GetKey(olc::W).bPressed)
            miniaudio.Pause(song1);

        if(GetKey(olc::E).bPressed)
            miniaudio.Stop(song1);

        Clear(olc::BLACK);
        
        FillCircle(GetMousePos(), 5 ,olc::RED);

        DrawString({5, 135}, "Music: Joy Ride by MusicLFiles");
        DrawString({5, 150}, "See LICENSE.md or terminal");
        DrawString({5, 165}, "For complete attribution details!");

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