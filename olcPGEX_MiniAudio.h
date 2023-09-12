#pragma once
/*
	olcPGEX_MiniAudio.h

	+-------------------------------------------------------------+
	|           MiniAudio PGE eXtension v0.01                     |
	| "What do you need? Samples.. Lots of Samples.." - moros1138 |
	+-------------------------------------------------------------+

	What is this?
	~~~~~~~~~~~~~
    

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2023 Moros Smith <moros1138@gmail.com>

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	1. Redistributions or derivations of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce the above
	copyright notice. This list of conditions and the following	disclaimer must be
	reproduced in the documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its contributors may
	be used to endorse or promote products derived from this software without specific
	prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS	"AS IS" AND ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
	SHALL THE COPYRIGHT	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL,	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
	TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
	BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
	SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/@Moros1138
	GitHub:		https://www.github.com/Moros1138
	Homepage:	https://www.moros1138.com
*/

#include "olcPixelGameEngine.h"
#include <exception>

#ifdef OLC_PGEX_MINIAUDIO
#define MINIAUDIO_IMPLEMENTATION
#endif

#include "miniaudio.h"

namespace olc
{
    class MiniAudio : public olc::PGEX
    {
    public:
        MiniAudio();
        ~MiniAudio();
        virtual bool OnBeforeUserUpdate(float& fElapsedTime) override;
        static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
        
        const int LoadSound(const std::string& path);
        
        void Play(const int id, const bool loop = false);
        void Stop(const int id);
        void Pause(const int id);
        void Toggle(const int id, bool rewind = false);
        
        void SetVolume(const int id, float& volume);
        void SetPan(const int id, float& pan);
        void SetPitch(const int id, float& pitch);
        
        void Seek(const int id, const unsigned long long milliseconds);
        void Seek(const int id, float& location);

        void Forward(const int id, const unsigned long long milliseconds);
        void Rewind(const int id, const unsigned long long milliseconds);
        
        unsigned long long GetCursorMilliseconds(const int id);
        float GetCursorFloat(const int id);

        std::vector<ma_sound*> vecSounds;

    private:        
        ma_context context;
        ma_device device;
        ma_engine engine;
        ma_resource_manager resourceManager;
        int sampleRate;
    };

    struct MiniAudioDeviceException : public std::exception
    {
        const char* what() const throw()
        {
            return "Failed to initialize a device.";
        }
    };
    
    struct MiniAudioResourceManagerException : public std::exception
    {
        const char* what() const throw()
        {
            return "Failed to initialize the resource manager.";
        }
    };

    struct MiniAudioEngineException : public std::exception
    {
        const char* what() const throw()
        {
            return "Failed to initialize the audio engine.";
        }
    };

    struct MiniAudioSoundException : public std::exception
    {
        const char* what() const throw()
        {
            return "Failed to initialize a sound.";
        }
    };

}


#ifdef OLC_PGEX_MINIAUDIO
#undef OLC_PGEX_MINIAUDIO

namespace olc
{

    MiniAudio::MiniAudio() : olc::PGEX(true)
    {
        sampleRate = 48000;
        
        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = ma_format_f32;
        deviceConfig.playback.channels = 2;
        deviceConfig.sampleRate = sampleRate;
        deviceConfig.dataCallback = MiniAudio::data_callback;
        deviceConfig.pUserData = &engine;

        if(ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS)
            throw MiniAudioDeviceException();

        ma_resource_manager_config resourceManagerConfig = ma_resource_manager_config_init();
        resourceManagerConfig.decodedFormat     = ma_format_f32;
        resourceManagerConfig.decodedChannels   = 0;
        resourceManagerConfig.decodedSampleRate = sampleRate;
        
        #ifdef __EMSCRIPTEN__
            resourceManagerConfig.jobThreadCount = 0;                           
            resourceManagerConfig.flags |= MA_RESOURCE_MANAGER_FLAG_NON_BLOCKING;
            resourceManagerConfig.flags |= MA_RESOURCE_MANAGER_FLAG_NO_THREADING;
        #endif

        if(ma_resource_manager_init(&resourceManagerConfig, &resourceManager) != MA_SUCCESS)
            throw MiniAudioResourceManagerException();
        
        ma_engine_config engineConfig = ma_engine_config_init();
        engineConfig.pDevice = &device;
        engineConfig.pResourceManager = &resourceManager; 
        
        if(ma_engine_init(&engineConfig, &engine) != MA_SUCCESS)
            throw MiniAudioEngineException();
        
    }

    MiniAudio::~MiniAudio()
    {
        for(auto sound : vecSounds)
        {
            ma_sound_uninit(sound);
            delete sound;
        }
            
        ma_resource_manager_uninit(&resourceManager);

        ma_engine_uninit(&engine);        
    }

    bool MiniAudio::OnBeforeUserUpdate(float& fElapsedTime)
    {
        #ifdef __EMSCRIPTEN__
        ma_resource_manager_process_next_job(&resourceManager);
        #endif

        return false;
    }

    void MiniAudio::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        ma_engine_read_pcm_frames((ma_engine*)(pDevice->pUserData), pOutput, frameCount, NULL);
    }

    const int MiniAudio::LoadSound(const std::string& path)
    {
        const int id = vecSounds.size();
        vecSounds.push_back(new ma_sound());
        
        if(ma_sound_init_from_file(&engine, path.c_str(), MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, NULL, NULL, vecSounds.at(id)) != MA_SUCCESS)
            throw MiniAudioSoundException();
        
        return id;
    }
    
    void MiniAudio::Play(const int id, const bool loop)
    {
        if(ma_sound_is_playing(vecSounds.at(id)))
        {
            ma_sound_seek_to_pcm_frame(vecSounds.at(id), 0);
            return;
        }
        
        ma_sound_set_looping(vecSounds.at(id), loop);
        ma_sound_start(vecSounds.at(id));
    }

    void MiniAudio::Stop(const int id)
    {
        ma_sound_seek_to_pcm_frame(vecSounds.at(id), 0);
        ma_sound_stop(vecSounds.at(id));
    }

    void MiniAudio::Pause(const int id)
    {
        auto it = vecSounds.begin() + id;
        ma_sound_stop(vecSounds.at(id));
    }

    void MiniAudio::Toggle(const int id, bool rewind)
    {
        if(ma_sound_is_playing(vecSounds.at(id)))
        {
            ma_sound_stop(vecSounds.at(id));

            if(rewind)
                ma_sound_seek_to_pcm_frame(vecSounds.at(id), 0);
            
            return;
        }
        
        ma_sound_start(vecSounds.at(id));
    }
    
    void MiniAudio::SetVolume(const int id, float& volume)
    {
        ma_sound_set_volume(vecSounds.at(id), volume);
    }

    void MiniAudio::SetPan(const int id, float& pan)
    {
        ma_sound_set_pan(vecSounds.at(id), pan);
    }
    
    void MiniAudio::SetPitch(const int id, float& pitch)
    {
        ma_sound_set_pitch(vecSounds.at(id), pitch);
    }
    
    void MiniAudio::Seek(const int id, const unsigned long long milliseconds)
    {
        unsigned long long frame = (milliseconds * engine.sampleRate) / 1000;
        
        ma_sound_seek_to_pcm_frame(vecSounds.at(id), frame);
    }

    void MiniAudio::Seek(const int id, float& location)
    {
        unsigned long long length;
        ma_sound_get_length_in_pcm_frames(vecSounds.at(id), &length);

        unsigned long long frame = length * location;
        
        ma_sound_seek_to_pcm_frame(vecSounds.at(id), frame);
    }


    void MiniAudio::Forward(const int id, const unsigned long long milliseconds)
    {
        unsigned long long cursor;
        ma_sound_get_cursor_in_pcm_frames(vecSounds.at(id), &cursor);

        unsigned long long frame = (milliseconds * engine.sampleRate) / 1000;
        ma_sound_seek_to_pcm_frame(vecSounds.at(id), cursor + frame);
    }

    void MiniAudio::Rewind(const int id, const unsigned long long milliseconds)
    {
        unsigned long long cursor;
        ma_sound_get_cursor_in_pcm_frames(vecSounds.at(id), &cursor);

        unsigned long long frame = (milliseconds * engine.sampleRate) / 1000;
        ma_sound_seek_to_pcm_frame(vecSounds.at(id), cursor - frame);
    }

    unsigned long long MiniAudio::GetCursorMilliseconds(const int id)
    {
        unsigned long long cursor;
        ma_sound_get_cursor_in_pcm_frames(vecSounds.at(id), &cursor);

        cursor /= sampleRate;
        cursor /= 1000;
        
        return cursor;
    }

    float MiniAudio::GetCursorFloat(const int id)
    {
        unsigned long long cursor;
        ma_sound_get_cursor_in_pcm_frames(vecSounds.at(id), &cursor);

        unsigned long long length;
        ma_sound_get_length_in_pcm_frames(vecSounds.at(id), &length);

        return (float)cursor / length;
    }

} // olc

#endif
