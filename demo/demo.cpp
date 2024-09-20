#include "olcPixelGameEngine.h"
#include "olcPGEX_MiniAudio.h"

constexpr float thirtyFramesPerSecond = 1.0f / 30.f;

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
        return true;
    }
    
    bool OnUserUpdate(float fElapsedTime) override
    {
        #if defined(__EMSCRIPTEN__)
            return true;
        #else
            return !GetKey(olc::ESCAPE).bPressed;
        #endif
    }

    olc::MiniAudio ma;
};

int main()
{
    Demo demo;
    if (demo.Construct(320, 180, 4, 4))
        demo.Start();
    return 0;
}
