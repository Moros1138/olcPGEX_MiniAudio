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
        /**
         * turn on background play so sound
         * continues to play even if the app
         * doesn't have focus.
         */
        ma.SetBackgroundPlay(true);

        /**
         * load assets/sounds/song1.mp3 and
         * keep track of it's id for later use
         */
        song1 = ma.LoadSound("assets/sounds/song1.mp3");
        
        /**
         * ADVANCED NOTES
         * 
         * Using ma.GetSound to get a pointer to the ma_sound
         * associated with the sample ID, in this case song1.
         * 
         * this allows us to use the miniaudio apis directly
         * for anything that the PGEX doesn't abstract.
         * 
         * Note:    in miniaudio spatialization, y is the
         *          world UP axis. since this demo is 2d
         *          we set the Y of all the miniaudio coords
         *          to 0.0f.
         */
        ma_sound_set_position(ma.GetSound(song1), 0.0f, 0.0f, 0.0f);
        
        /**
         * using a linear attenuation allows us to accurately
         * set the min/max distance at which the sound plays
         * 
         * side effect of this method is it doesn't change
         * volume of the sample as you move away. solution
         * detailed below.
         */
        ma_sound_set_attenuation_model(ma.GetSound(song1), ma_attenuation_model_linear);
        
        /**
         * min distance = 0
         * max distance = 20 (15 for radius of sound, 5 to cover radius of listener)
         */
        ma_sound_set_min_distance(ma.GetSound(song1), 0.0f);
        ma_sound_set_max_distance(ma.GetSound(song1), 20.0f);
        
        // calculate the center of the screen
        centerScreen = GetScreenSize() / 2;
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        fElapsedTime = (fElapsedTime > thirtyFramesPerSecond) ? thirtyFramesPerSecond : fElapsedTime;

        if(GetKey(olc::SPACE).bPressed)
        {
            /**
             * start or stop playback of song1
             */
            ma.Toggle(song1);
        }
        
        float forwardVelocity = 0.0f;
        float rotationVelocity = 0.0f;

        if(GetKey(olc::W).bHeld || GetKey(olc::UP).bHeld)    forwardVelocity  += 1.0f;
        if(GetKey(olc::S).bHeld || GetKey(olc::DOWN).bHeld)  forwardVelocity  -= 1.0f;
        if(GetKey(olc::A).bHeld || GetKey(olc::LEFT).bHeld)  rotationVelocity -= 1.0f;
        if(GetKey(olc::D).bHeld || GetKey(olc::RIGHT).bHeld) rotationVelocity += 1.0f;

        rotation += rotationVelocity * 5.0f * fElapsedTime;
        /**
         * change the direction our listener is facing
         * using the newly calculated rotation
         */
        olc::vf2d directionVector = olc::vf2d{ cosf(rotation), sinf(rotation) };
        
        /**
         * calculate the velocity our listener is moving
         * in the newly calculated direction vector
         */
        olc::vf2d velocity = directionVector * forwardVelocity * 20.0f * fElapsedTime;
        
        /**
         * add velocity to position, thus moving the listener
         */
        position += velocity;
        
        /**
         * As noted above, linear attenuation doesn't account
         * sample's volume. this is perfectly fine for situations
         * where you simply want a sound to play or not. however
         * if you want to simulate distance, we need to calculate
         * the volume of the sample.
         */
        float volume = std::clamp(1.0f - (position.mag() / 20.0f), 0.0f, 1.0f);
        ma_sound_set_volume(ma.GetSound(song1), volume);
        
        /**
         * ADVANCED NOTES
         * 
         * Using ma.Engine to get a pointer to the ma_engine
         * associated with the pgex.
         * 
         * this allows us to use the miniaudio apis directly
         * for anything that the PGEX doesn't abstract.
         */
        ma_engine_listener_set_direction(ma.GetEngine(), 0, directionVector.x, 0.0f, directionVector.y);
        ma_engine_listener_set_position(ma.GetEngine(), 0, position.x, 0.0f, position.y);
        ma_engine_listener_set_velocity(ma.GetEngine(), 0, velocity.x, 0.0f, velocity.y);
        
        Clear(olc::BLACK);
        
        /**
         * draw a cirlce representing the sound
         * offset by the center of the screen.
         */
        DrawCircle(centerScreen, 5, olc::YELLOW);

        /**
         * draw a circle representing the range of
         * the sound offset by the center of the
         * screen.
         */
        DrawCircle(centerScreen, 15, olc::MAGENTA);

        /**
         * draw a circle at the listener position offset
         * by the center of the screen along with a line
         * to indicate the direction the listener is facing.
         */
        DrawCircle(centerScreen + position, 5, olc::WHITE);
        DrawLine(centerScreen + position, centerScreen + position + (directionVector * 5), olc::WHITE);


        DrawStringDecal({5, 5}, \
            "-------- INFO --- CONTROLS -\n" "\n"
            "Forward/Backward (" + std::to_string(forwardVelocity) + ")          W,S or UP,DOWN\n" "\n"
            "Rotation         (" + std::to_string(rotation)        + ")          A,D or LEFT,RIGHT\n" "\n"
            "Position         " + position.str()                   + "\n" "\n"
            "Position Mag     (" + std::to_string(position.mag())  + ")\n" "\n"
            "Toggle Sound     (" + (ma.IsPlaying(song1) ? "Playing)    " : "Not Playing)") + "       SPACE\n"
            "Volume           (" + std::to_string(volume) + ")\n" "\n"
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

    // add the miniaudio pgex to PGE
    olc::MiniAudio ma;
    // keep track of the id the sample
    int song1;
    // position of our listener
    olc::vf2d position{0.0f, 0.0f};
    // rotational direction of our listener
    float rotation = 0.0f;
    // center of the screen
    olc::vf2d centerScreen;


};

int main()
{
    Demo demo;
    if (demo.Construct(320, 180, 4, 4))
        demo.Start();
    return 0;
}
