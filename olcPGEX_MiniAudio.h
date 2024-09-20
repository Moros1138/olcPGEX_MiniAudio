#pragma once
/*
	olcPGEX_MiniAudio.h

	+-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|                     MiniAudio v2.0                          |
	+-------------------------------------------------------------+

	NOTE: UNDER ACTIVE DEVELOPMENT - THERE MAY BE BUGS/GLITCHES
    
    What is this?
	~~~~~~~~~~~~~
    This extension abstracts the very robust and powerful miniaudio
    library. It provides simple loading and playback of WAV and MP3
    files. Because it's built on top of miniaudio, it requires next
    to no addictional build configurations in order to be built
    for cross-platform.

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

#ifdef OLC_PGEX_MINIAUDIO
#define MINIAUDIO_IMPLEMENTATION
#endif

#include "miniaudio.h"

#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <source_location>
#include <string>
#include <vector>

namespace olc
{
    
    
    class MiniAudio : public olc::PGEX
    {
    public:
        MiniAudio();
        ~MiniAudio();
    private:
        ma_device m_device;
        ma_device_config m_device_config;
        
        ma_resource_manager m_resource_manager;
        ma_resource_manager_config m_resource_manager_config;
    };
}

#ifdef OLC_PGEX_MINIAUDIO
#undef OLC_PGEX_MINIAUDIO

static constexpr int            DEVICE_CHANNELS     = 2;
static constexpr ma_format      DEVICE_FORMAT       = ma_format_f32;
static constexpr int            DEVICE_SAMPLE_RATE  = 48000;
static constexpr ma_device_type DEVICE_TYPE         = ma_device_type_playback;

void PGEX_MA_LOG(const std::string_view& message = "", std::source_location location = std::source_location::current())
{
#ifdef DEBUG
    std::filesystem::path filePath{location.file_name()};
    
    std::cout << std::format(
        "{}:{} {}(): {}",
        std::string{filePath.filename()},
        location.line(),
        location.function_name(),
        message
    ) << std::endl;
#endif
}

namespace olc
{
    MiniAudio::MiniAudio() : olc::PGEX(true)
    {
        m_device_config = ma_device_config_init(DEVICE_TYPE);
        m_device_config.playback.format = DEVICE_FORMAT;
        m_device_config.playback.channels = DEVICE_CHANNELS;
        m_device_config.sampleRate = DEVICE_SAMPLE_RATE;
        m_device_config.dataCallback = MiniAudio::data_callback;
        m_device_config.pUserData = this;

        if(ma_device_init(NULL, &m_device_config, &m_device) != MA_SUCCESS)
            throw std::runtime_error{"PGEX_MiniAudio: failed to initialize device"};

        m_resource_manager_config = ma_resource_manager_config_init();
        m_resource_manager_config.decodedFormat     = DEVICE_FORMAT;
        m_resource_manager_config.decodedChannels   = DEVICE_CHANNELS;
        m_resource_manager_config.decodedSampleRate = DEVICE_SAMPLE_RATE;
    
    #ifdef __EMSCRIPTEN__
        m_resource_manager_config.jobThreadCount = 0;                           
        m_resource_manager_config.flags |= MA_RESOURCE_MANAGER_FLAG_NON_BLOCKING;
        m_resource_manager_config.flags |= MA_RESOURCE_MANAGER_FLAG_NO_THREADING;
    #endif

        if(ma_resource_manager_init(&m_resource_manager_config, &m_resource_manager) != MA_SUCCESS)
            throw std::runtime_error{"PGEX_MiniAudio: failed to initialize resource manager"};
    }

    MiniAudio::~MiniAudio()
    {
        PGEX_MA_LOG("Test 2");
    }

} // olc

#endif
