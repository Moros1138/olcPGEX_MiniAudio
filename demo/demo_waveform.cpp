#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_MINIAUDIO
#include "olcPGEX_MiniAudio.h"

constexpr int NOTE_COUNT = 17;
constexpr float thirtyFramesPerSecond = 1.0f / 30.f;

class DemoPiano : public olc::PixelGameEngine
{
    // NOTICE
    //
    // This example is highly inspired by the Sound Synthesizer Part 1 series created by javidx9!
    // Checkout the original at https://github.com/OneLoneCoder/synth/blob/master/main1.cpp
    // and the accompanying video at https://www.youtube.com/watch?v=OSCzKOqtgcA
    //

    using NoteName = std::string;
    using Frequency = float;

public:
    DemoPiano()
    {
        sAppName = "Demo MiniAudio - Waveforms";
    }
    
public:

    bool OnUserCreate() override
    {
        
        for (Note&note : notes)
        {
            // When calling CreateWaveform, you'll be given a unique ID used in all other Waveform functions. Store it somewhere.
            note.waveformId = ma.CreateWaveform(amplitude, note.frequency, selectedWaveform);
        }

        return true;
    }
    
    bool OnUserUpdate(float fElapsedTime) override
    {
        fElapsedTime = (fElapsedTime > thirtyFramesPerSecond) ? thirtyFramesPerSecond : fElapsedTime;
        
        olc::Pixel backgroundCol{olc::VERY_DARK_GREY};

        if(clickToStart)
        {
            if(GetMouse(olc::Mouse::LEFT).bPressed)
                clickToStart=false;

            GradientFillRectDecal({}, {float(ScreenWidth()), ScreenHeight()/2.f}, olc::BLACK, backgroundCol, backgroundCol, olc::BLACK);
            GradientFillRectDecal({0.f, ScreenHeight()/2.f}, {float(ScreenWidth()), ScreenHeight()/2.f}, backgroundCol, olc::BLACK, olc::BLACK, backgroundCol);
            olc::vf2d clickToStartTextSize{GetTextSize("Click to Start!")};
            DrawRotatedStringDecal({ScreenWidth()/2.f, 128.f}, "Click to Start!", 0.f, clickToStartTextSize/2, olc::WHITE, {2.f, 2.f});
            return true;
        }

        bool keyPressed{false};

        if(GetKey(olc::Q).bPressed)
        {
            selectedWaveform = ma_waveform_type((selectedWaveform+1)%4);
        }
        if(GetKey(olc::UP).bPressed)
        {
            amplitude = std::min(1.f, amplitude + 0.1f);
        }
        if(GetKey(olc::DOWN).bPressed)
        {
            amplitude = std::max(0.f, amplitude - 0.1f);
        }
        
        for (Note&note : notes)
        {
            if(GetKey(note.key).bPressed)
            {
                ma.PlayWaveform(note.waveformId);
            }
            if(GetKey(note.key).bReleased)
            {
                ma.StopWaveform(note.waveformId);
            }
            if(GetKey(note.key).bHeld)
            {
                keyPressed=true;
            }
            ma.SetWaveformType(note.waveformId, selectedWaveform);
            ma.SetWaveformAmplitude(note.waveformId, amplitude);
        }
        
        if(keyPressed)
            backgroundCol = olc::VERY_DARK_BLUE;

        GradientFillRectDecal({}, {float(ScreenWidth()), ScreenHeight()/2.f}, olc::BLACK, backgroundCol, backgroundCol, olc::BLACK);
        GradientFillRectDecal({0.f, ScreenHeight()/2.f}, {float(ScreenWidth()), ScreenHeight()/2.f}, backgroundCol, olc::BLACK, olc::BLACK, backgroundCol);

        DrawStringDecal({}, "Waveform Type < " + waveformToName.at(selectedWaveform) + " >   Q");
        std::stringstream s;
        s << std::fixed << std::setprecision(1) << amplitude;
        DrawStringDecal({0,8}, "Amplitude < " + s.str() + " >   UP, DOWN");
        
        for (float drawY{24}; Note&note : notes)
        {
            if(!ma.IsWaveformPlaying(note.waveformId))
                continue;

            if(drawY == 24)
                DrawStringDecal({0.f, drawY - 8.f},"Playing: ");

            std::stringstream notePlayingStr;
            notePlayingStr << std::setw(5) << note.displayName << std::setw(7) << std::fixed << std::setprecision(2) << note.frequency;
            DrawStringDecal({0.f, drawY}, "   "+ notePlayingStr.str());
            drawY += 8;
        }

        olc::vf2d pianoStrSize{GetTextSize(piano)};
        DrawRotatedStringDecal({ScreenWidth()/2.f, 128.f}, piano, 0.f, pianoStrSize/2, olc::WHITE, {0.65f, 1.f});

        #if defined(__EMSCRIPTEN__)
            return true;
        #else
            return !GetKey(olc::ESCAPE).bPressed;
        #endif
    }
    bool OnUserDestroy() override
    {
        
        for (Note&note : notes)
        {
            //Let's be nice and cleanup after ourselves...
            ma.UnloadWaveform(note.waveformId);
        }

        return true;
    }

    // The instance of the audio engine, no fancy config required.
    olc::MiniAudio ma;

    ma_waveform_type selectedWaveform{ma_waveform_type_sine};
    float amplitude{0.1f};

    bool clickToStart{true};

    struct Note
    {
        std::string displayName;
        float frequency;
        olc::Key key;
        int waveformId;
    };

    std::array<Note,NOTE_COUNT>notes{
        Note
        {"G#",  207.65f,    olc::A},
        {"A",   220.00f,    olc::Z},
        {"A#",  233.08f,    olc::S},
        {"B",   246.94f,    olc::X},
        {"C",   261.63f,    olc::C},
        {"C#",  277.18f,    olc::F},
        {"D",   293.66f,    olc::V},
        {"D#",  311.13f,    olc::G},
        {"E",   329.63f,    olc::B},
        {"F",   349.23f,    olc::N},
        {"F#",  369.99f,    olc::J},
        {"G",   392.00f,    olc::M},
        {"G#",  415.30f,    olc::K},
        {"A",   440.00f,    olc::COMMA},
        {"A#",  466.16f,    olc::L},
        {"B",   493.88f,    olc::PERIOD},
        {"C",   523.25f,    olc::OEM_2},
    };

    const std::unordered_map<ma_waveform_type,std::string>waveformToName
    {
        {ma_waveform_type_sine, "SINE"},
        {ma_waveform_type_square, "SQUARE"},
        {ma_waveform_type_triangle, "TRIANGLE"},
        {ma_waveform_type_sawtooth, "SAWTOOTH"},
    };

    const std::string piano{
	    "  | |   |   |   |   | |   |   |   |   | |   | |   |   |   |\n"
	    "A | | S |   |   | F | | G |   |   | J | | K | | L |   |   |\n"
	    "__| |___|   |   |___| |___|   |   |___| |___| |___|   |   |__\n"
	    "|     |     |     |     |     |     |     |     |     |     |\n"
	    "|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |\n"
	    "|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|"
    };

};

int main()
{
    DemoPiano demo;
    if (demo.Construct(320, 180, 4, 4))
        demo.Start();
    return 0;
}
