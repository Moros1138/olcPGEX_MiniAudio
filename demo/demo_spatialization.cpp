#include "olcPixelGameEngine.h"
#include "olcPGEX_MiniAudio.h"
#include <cmath>

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
        ma.SetBackgroundPlay(true);

        song1 = ma.LoadSound("assets/sounds/song1.mp3");
        ma_sound_set_position(ma.GetSound(song1), 0.0f, 0.0f, 0.0f);
        ma_sound_set_attenuation_model(ma.GetSound(song1), ma_attenuation_model_linear);
        ma_sound_set_min_distance(ma.GetSound(song1), 0.0f);
        ma_sound_set_max_distance(ma.GetSound(song1), 20.0f);
        ma_sound_set_max_gain(ma.GetSound(song1), 1.0f);
        ma_sound_set_min_gain(ma.GetSound(song1), 0.0f);

        centerScreen = GetScreenSize() / 2;
        return true;
    }

    olc::vf2d position{0.0f, 0.0f};
    float direction = 0.0f;
    olc::vf2d centerScreen;

    bool OnUserUpdate(float fElapsedTime) override
    {
        fElapsedTime = (fElapsedTime > thirtyFramesPerSecond) ? thirtyFramesPerSecond : fElapsedTime;

        if(GetKey(olc::SPACE).bPressed)
        {
            ma.Toggle(song1);
        }
        
        float forwardVelocity = 0.0f;
        float rotationVelocity = 0.0f;

        if(GetKey(olc::W).bHeld || GetKey(olc::UP).bHeld)    forwardVelocity  += 1.0f;
        if(GetKey(olc::S).bHeld || GetKey(olc::DOWN).bHeld)  forwardVelocity  -= 1.0f;
        if(GetKey(olc::A).bHeld || GetKey(olc::LEFT).bHeld)  rotationVelocity -= 1.0f;
        if(GetKey(olc::D).bHeld || GetKey(olc::RIGHT).bHeld) rotationVelocity += 1.0f;

        direction += rotationVelocity * 5.0f * fElapsedTime;
        olc::vf2d directionVector = olc::vf2d{ cosf(direction), sinf(direction) };
        olc::vf2d velocity = directionVector * forwardVelocity * 20.0f * fElapsedTime;
        position += velocity;
        
        ma_engine_listener_set_direction(ma.GetEngine(), 0, directionVector.x, 0.0f, directionVector.y);
        ma_engine_listener_set_position(ma.GetEngine(), 0, position.x, 0.0f, position.y);
        ma_engine_listener_set_velocity(ma.GetEngine(), 0, velocity.x, 0.0f, velocity.y);
        
        Clear(olc::BLACK);
        
        DrawCircle(centerScreen, 5, olc::YELLOW);
        DrawCircle(centerScreen, 15, olc::MAGENTA);

        DrawCircle(centerScreen + position, 5, olc::WHITE);
        DrawLine(centerScreen + position, centerScreen + position + (directionVector * 5), olc::WHITE);


        DrawStringDecal({5, 5}, \
            "-------- INFO --- CONTROLS -\n" "\n"
            "Forward/Backward (" + std::to_string(forwardVelocity) + ")          W,S or UP,DOWN\n" "\n"
            "Rotation         (" + std::to_string(direction)       + ")          A,D or LEFT,RIGHT\n" "\n"
            "Position         " + position.str()                   + "\n" "\n"
            "Position Mag     (" + std::to_string(position.mag())  + ")\n" "\n"
            "Toggle Sound     (" + (ma.IsPlaying(song1) ? "Playing)    " : "Not Playing)") + "       SPACE\n"
            ,
            olc::WHITE, {0.5f, 0.5f});

        DrawStringDecal({5, 160}, \
            "Music: Joy Ride [Full version] by MusicLFiles\n"
            "Free download: https://filmmusic.io/song/11627-joy-ride-full-version\n"
            "Licensed under CC BY 4.0: https://filmmusic.io/standard-license\n", \
        olc::WHITE, {0.5f, 0.5f});
        
        #if defined(__EMSCRIPTEN__)
            return true;
        #else
            return !GetKey(olc::ESCAPE).bPressed;
        #endif
    }

    olc::MiniAudio ma;
    int song1;
};

int main()
{
    Demo demo;
    if (demo.Construct(320, 180, 4, 4))
        demo.Start();
    return 0;
}
