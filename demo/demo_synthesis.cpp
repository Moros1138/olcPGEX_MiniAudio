#include "olcPixelGameEngine.h"
#include "olcPGEX_MiniAudio.h"

constexpr int NOTE_COUNT = 17;
constexpr float thirtyFramesPerSecond = 1.0f / 30.f;
constexpr float PI = 3.14159f;

class DemoSynthesis : public olc::PixelGameEngine
{
    // NOTICE
    //
    // This example is highly inspired by the Sound Synthesizer Part 3 series created by javidx9!
    // Checkout the original at https://github.com/OneLoneCoder/synth/blob/master/main3a.cpp
    // and the accompanying video at https://www.youtube.com/watch?v=kDuvruJTjOs
    //

public:
    DemoSynthesis()
    {
        sAppName = "Demo MiniAudio - Custom Audio Synthesis";
    }
    
public:

    bool OnUserCreate() override
    {
		instruments.emplace_back(std::make_unique<Harmonica>());
		instruments.emplace_back(std::make_unique<Bell>());
		instruments.emplace_back(std::make_unique<Bell8>());
		instruments.emplace_back(std::make_unique<SawAnalog>());

		// This is it... This is how we interface into the miniaudio engine any noise we want heard!
		// Fill up the noiseLeftChannel and noiseRightChannel with a value at any given moment...
		// Accumulate fElapsedTime so that we know how much total time has passed so we can play the correct synthesized sound based on delta time.
        noiseCallbackFunc = [this](float& noiseLeftChannel, float& noiseRightChannel, const float fElapsedTime)->void{
			noiseLeftChannel = noiseRightChannel = 0.f;

			for(Note& note : notes)
			{
				bool noteFinished{false};
				float noise = note.instrument->sound(audioRuntime, note, noteFinished); 

				noiseLeftChannel += noise * lerp(1.f, 0.f, pan/2 + 0.5f); // This lerp formula shrinks the -1 to 1 range to 0 to 1, which determines percentage of the total noise to play in each side.
				noiseRightChannel += noise * lerp(0.f, 1.f, pan/2 + 0.5f);
			}

			// Each frame the callback receives will give us a duration that frame takes up in real time.
			audioRuntime += fElapsedTime;
		};

		// And finally... Tell the system this is where to send audio requests to... Now we can modify audio!
		ma.SetNoiseCallback(noiseCallbackFunc);

		//Initialize an instrument for all the notes.
		for(Note& note : notes)
		{
			note.instrument = instruments[selectedInstrumentInd].get();
		}

        return true;
    }
    
    bool OnUserUpdate(float fElapsedTime) override
    {
        fElapsedTime = (fElapsedTime > thirtyFramesPerSecond) ? thirtyFramesPerSecond : fElapsedTime;
        
        olc::Pixel backgroundCol{olc::VERY_DARK_GREY};

		bool keyPressed{false};

		for(Note& note : notes)
		{
			note.instrument->volume = 0.1f;

			if(GetKey(note.key).bHeld)
			{
				// Note is not active yet, so we will play it now.
				if(!note.active)
				{
					note.on = audioRuntime;
					note.active=true;
				}
				else
				if(note.off > note.on) // We pressed the key during its release phase...
					note.on = audioRuntime;

				keyPressed = true;
			}
			else
			{
				if(note.off < note.on)
				{
					note.off = audioRuntime;
				}

				note.active = false;
			}
		}

		if(GetKey(olc::UP).bPressed)
			selectedInstrumentInd = (selectedInstrumentInd + 1) % instruments.size();

		if(GetKey(olc::DOWN).bPressed)
		{
			selectedInstrumentInd--;
			while(selectedInstrumentInd < 0)
				selectedInstrumentInd += instruments.size();
		}

		if(GetKey(olc::RIGHT).bHeld)
			volume = std::min(1.f, volume + 1.f * fElapsedTime);
		if(GetKey(olc::LEFT).bHeld)
			volume = std::max(0.f, volume - 1.f * fElapsedTime);

		if(GetKey(olc::O).bHeld)
			pan = std::max(-1.f, pan - 1.f * fElapsedTime);
		if(GetKey(olc::P).bHeld)
			pan = std::min(1.f, pan + 1.f * fElapsedTime);

		if(GetKey(olc::R).bPressed)
		{
			selectedInstrumentInd = 0;
			volume = 0.1f;
			pan = 0.f;
		}

		// Update all the notes' instruments.
		for(Note& note : notes)
		{
			note.instrument = instruments[selectedInstrumentInd].get();
		}

		instruments[selectedInstrumentInd]->volume = volume;

        if(keyPressed)
            backgroundCol = olc::VERY_DARK_BLUE;
		
        GradientFillRectDecal({}, {float(ScreenWidth()), ScreenHeight()/2.f}, olc::BLACK, backgroundCol, backgroundCol, olc::BLACK);
        GradientFillRectDecal({0.f, ScreenHeight()/2.f}, {float(ScreenWidth()), ScreenHeight()/2.f}, backgroundCol, olc::BLACK, olc::BLACK, backgroundCol);

		DrawStringDecal({},"Instrument: <"+ instruments[selectedInstrumentInd]->name +"> UP, DOWN");
		DrawStringDecal({0.f, 8.f},"Volume: <"+ std::to_string(volume) +"> LEFT, RIGHT");
		DrawStringDecal({0.f, 16.f},"Pan: <"+ std::to_string(pan) +"> O, P");

		DrawStringDecal({0.f, 120.f},"Reset Settings <R>");

        olc::vf2d pianoStrSize{GetTextSize(piano)};
        DrawRotatedStringDecal({ScreenWidth()/2.f, ScreenHeight() - pianoStrSize.y/2}, piano, 0.f, pianoStrSize/2, olc::WHITE, {0.65f, 1.f});

        #if defined(__EMSCRIPTEN__)
            return true;
        #else
            return !GetKey(olc::ESCAPE).bPressed;
        #endif
    }

    // The instance of the audio engine, no fancy config required.
    olc::MiniAudio ma;

	std::function<void(float& noiseLeftChannel, float& noiseRightChannel, const float fElapsedTime)> noiseCallbackFunc;

	double audioRuntime{}; // A running timer of how long the audio engine has been running.

	float volume{0.1f};
	float pan{0.0f}; // -1.f for only left channel, 1.f for only right channel
	
	struct Instrument;

	struct Note
	{
		std::string displayStr;
		olc::Key key;
		int id;		// Position in scale
		float on;	// Time note was activated
		float off;	// Time note was deactivated
		bool active;
		Instrument*instrument;

		Note(std::string displayStr, olc::Key key, int id)
		:displayStr(displayStr), key(key), id(id){
			on = 0.0;
			off = 0.0;
			active = false;
		}
	};

    std::array<Note,NOTE_COUNT>notes{
        Note
        {"G#", olc::A,		-1}, //This is not a typo. javid's original scale formula starts at "A", so this will retrieve the note ID prior to that one.
        {"A" , olc::Z,		0},
        {"A#", olc::S,		1},
        {"B" , olc::X,		2},
        {"C" , olc::C,		3},
        {"C#", olc::F,		4},
        {"D" , olc::V,		5},
        {"D#", olc::G,		6},
        {"E" , olc::B,		7},
        {"F" , olc::N,		8},
        {"F#", olc::J,		9},
        {"G" , olc::M,		10},
        {"G#", olc::K,		11},
        {"A" , olc::COMMA,	12},
        {"A#", olc::L,		13},
        {"B" , olc::PERIOD,	14},
        {"C" , olc::OEM_2,	15},
    };

	int selectedInstrumentInd{};
	std::vector<std::unique_ptr<Instrument>>instruments;

    const std::string piano{
	    "  | |   |   |   |   | |   |   |   |   | |   | |   |   |   |\n"
	    "A | | S |   |   | F | | G |   |   | J | | K | | L |   |   |\n"
	    "__| |___|   |   |___| |___|   |   |___| |___| |___|   |   |__\n"
	    "|     |     |     |     |     |     |     |     |     |     |\n"
	    "|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |\n"
	    "|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|"
    };

    enum class Oscillator
	{
		SINE,
		SQUARE,
		TRIANGLE,
		SAW_ANALOG,
		SAW_DIGITAL,
		NOISE,
	};

	float lerp(float n1,float n2,double t){
		return float(n1*(1-t)+n2*t);
	}

	// Converts frequency (Hz) to angular velocity
	static float w(const float hertz)
	{
		return hertz * 2.0 * PI;
	}

	static float osc(const float time, const float hertz, const Oscillator type = Oscillator::SINE,
		const float LFOHertz = 0.0, const float LFOAmplitude = 0.0, float custom = 50.0)
	{

		float freq = w(hertz) * time + LFOAmplitude * hertz * (sin(w(LFOHertz) * time));

		switch (type)
		{
			case Oscillator::SINE: // Sine wave bewteen -1 and +1
				return sin(freq);

			case Oscillator::SQUARE: // Square wave between -1 and +1
				return sin(freq) > 0 ? 1.0 : -1.0;

			case Oscillator::TRIANGLE: // Triangle wave between -1 and +1
				return asin(sin(freq)) * (2.0 / PI);

			case Oscillator::SAW_ANALOG: // Saw wave (analogue / warm / slow)
			{
				float dOutput = 0.0;
				for (float n = 1.0; n < custom; n++)
					dOutput += (sin(n*freq)) / n;
				return dOutput * (2.0 / PI);
			}

			case Oscillator::SAW_DIGITAL:
				return atan(tan(freq));

			case Oscillator::NOISE:
				return 2.0 * ((float)rand() / (float)RAND_MAX) - 1.0;

			default:
				return 0.0;
		}
	}

	//Abstract envelope class
	struct Envelope
	{
		virtual float amplitude(const float time, const float timeOn, const float timeOff) = 0;
	};

	struct EnvelopeADSR : public Envelope
	{
		float attackTime;
		float decayTime;
		float sustainAmplitude;
		float releaseTime;
		float startAmplitude;

		EnvelopeADSR()
		{
			attackTime = 0.1;
			decayTime = 0.1;
			sustainAmplitude = 1.0;
			releaseTime = 0.2;
			startAmplitude = 1.0;
		}

		virtual float amplitude(const float time, const float timeOn, const float timeOff)
		{
			float dAmplitude = 0.0;
			float dReleaseAmplitude = 0.0;

			if (timeOn > timeOff) // Note is on
			{
				float dLifeTime = time - timeOn;

				if (dLifeTime <= attackTime)
					dAmplitude = (dLifeTime / attackTime) * startAmplitude;

				if (dLifeTime > attackTime && dLifeTime <= (attackTime + decayTime))
					dAmplitude = ((dLifeTime - attackTime) / decayTime) * (sustainAmplitude - startAmplitude) + startAmplitude;

				if (dLifeTime > (attackTime + decayTime))
					dAmplitude = sustainAmplitude;
			}
			else // Note is off
			{
				float dLifeTime = timeOff - timeOn;

				if (dLifeTime <= attackTime)
					dReleaseAmplitude = (dLifeTime / attackTime) * startAmplitude;

				if (dLifeTime > attackTime && dLifeTime <= (attackTime + decayTime))
					dReleaseAmplitude = ((dLifeTime - attackTime) / decayTime) * (sustainAmplitude - startAmplitude) + startAmplitude;

				if (dLifeTime > (attackTime + decayTime))
					dReleaseAmplitude = sustainAmplitude;

				dAmplitude = ((time - timeOff) / releaseTime) * (0.0 - dReleaseAmplitude) + dReleaseAmplitude;
			}

			// Amplitude should not be negative
			if (dAmplitude <= 0.000)
				dAmplitude = 0.0;

			return dAmplitude;
		}
	};

	static float scale(const int noteID)
	{
		return 256 * pow(1.0594630943592952645618252949463, noteID);
	}

	static float envelope(const float time, Envelope &env, const float timeOn, const float timeOff)
	{
		return env.amplitude(time, timeOn, timeOff);
	}

	//An abstract struct for other instruments
	struct Instrument
	{
		float volume;
		std::string name;
		EnvelopeADSR env;
		virtual float sound(const float time, Note&n, bool &bNoteFinished) = 0;
	};

	struct Bell : public Instrument
	{
		Bell()
		{
			env.attackTime = 0.01;
			env.decayTime = 1.0;
			env.sustainAmplitude = 0.0;
			env.releaseTime = 1.0;

			volume = 1.0;
			name = "Bell";
		}

		virtual float sound(const float time, Note&n, bool &noteFinished) override
		{
			float amplitude = envelope(time, env, n.on, n.off);
			if (amplitude <= 0.0) noteFinished = true;

			float sound =
				+ 1.00 * osc(n.on - time, scale(n.id + 12), Oscillator::SINE, 5.0, 0.001)
				+ 0.50 * osc(n.on - time, scale(n.id + 24))
				+ 0.25 * osc(n.on - time, scale(n.id + 36));

			return amplitude * sound * volume;
		}

	};

	struct Bell8 : public Instrument
	{
		Bell8()
		{
			env.attackTime = 0.01;
			env.decayTime = 0.5;
			env.sustainAmplitude = 0.8;
			env.releaseTime = 1.0;

			volume = 1.0;
			name = "Bell 8-bit";
		}

		virtual float sound(const float time, Note&n, bool &noteFinished) override
		{
			float amplitude = envelope(time, env, n.on, n.off);
			if (amplitude <= 0.0) noteFinished = true;

			float sound =
				+1.00 * osc(n.on - time, scale(n.id), Oscillator::SQUARE, 5.0, 0.001)
				+ 0.50 * osc(n.on - time, scale(n.id + 12))
				+ 0.25 * osc(n.on - time, scale(n.id + 24));

			return amplitude * sound * volume;
		}

	};

	struct Harmonica : public Instrument
	{
		Harmonica()
		{
			env.attackTime = 0.05;
			env.decayTime = 1.0;
			env.sustainAmplitude = 0.95;
			env.releaseTime = 0.1;

			volume = 1.0;
			name = "Harmonica";
		}

		virtual float sound(const float time, Note&n, bool &noteFinished) override
		{
			float amplitude = envelope(time, env, n.on, n.off);
			if (amplitude <= 0.0) noteFinished = true;

			float sound =
				//+ 1.0  * synth::osc(n.on - dTime, synth::scale(n.id-12), synth::OSC_SAW_ANA, 5.0, 0.001, 100)
				+ 1.00 * osc(n.on - time, scale(n.id), Oscillator::SQUARE, 5.0, 0.001)
				+ 0.50 * osc(n.on - time, scale(n.id + 12), Oscillator::SQUARE)
				+ 0.05  * osc(n.on - time, scale(n.id + 24), Oscillator::NOISE);

			return amplitude * sound * volume;
		}

	};

	struct SawAnalog : public Instrument
	{
		SawAnalog()
		{
			env.attackTime = 0.05;
			env.decayTime = 1.0;
			env.sustainAmplitude = 0.95;
			env.releaseTime = 0.1;

			volume = 1.0;
			name = "Analog Saw";
		}

		virtual float sound(const float time, Note&n, bool &noteFinished) override
		{
			float amplitude = envelope(time, env, n.on, n.off);
			if (amplitude <= 0.0) noteFinished = true;

			float sound =
				+ 1.00 * osc(n.on - time, scale(n.id), Oscillator::SAW_ANALOG, 5.0, 0.001);

			return amplitude * sound * volume;
		}

	};
};

int main()
{
    DemoSynthesis demo;
    if (demo.Construct(320, 180, 4, 4))
        demo.Start();
    return 0;
}
