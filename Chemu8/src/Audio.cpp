#include "Audio.h"
#include <cstring>
#include <cmath>
#include <vector>

static const int AUDIO_QUEUE_MIN_SECONDS = 1; // keep ~1 second queued

Audio::Audio()
	: m_Stream(nullptr)
{
}

Audio::~Audio()
{
	if (m_Stream)
	{
		// Pause the stream; SDL will clean up on shutdown. Avoid calling
		// SDL_CloseAudioStream here to maintain compatibility across SDL3 versions.
		SDL_PauseAudioStreamDevice(m_Stream);
		m_Stream = nullptr;
	}
}

bool Audio::Initialize(int sampleRate)
{
	m_SampleRate = sampleRate;
	SDL_AudioSpec audioSpec{};
	audioSpec.channels = 1;
	audioSpec.format = SDL_AUDIO_F32;
	audioSpec.freq = m_SampleRate;
	m_Stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec, NULL, NULL);
	if (!m_Stream) {
		SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
		m_Playing = false;
		return false;
	}

	// Start the stream paused; we'll resume when SetPlaying(true) is called
	SDL_ResumeAudioStreamDevice(m_Stream);
	m_Playing = false;
	m_Phase = 0.0f;
	return true;
}

void Audio::SetPlaying(bool playing)
{
	if (m_Playing == playing) return;
	m_Playing = playing;
	if (!m_Stream) return;
	if (!m_Playing)
	{
		// clear queued audio so it doesn't continue to play
		SDL_ClearAudioStream(m_Stream);
		SDL_PauseAudioStreamDevice(m_Stream);
	}
	else
	{
		SDL_ResumeAudioStreamDevice(m_Stream);
	}
}

void Audio::Update(float deltaSeconds)
{
	(void)deltaSeconds;
	if (!m_Stream) return;
	if (!m_Playing) return;

	const int sampleRate = m_SampleRate;
	const int minQueuedBytes = (sampleRate * (int)sizeof(float)) * AUDIO_QUEUE_MIN_SECONDS;
	int queued = SDL_GetAudioStreamQueued(m_Stream);
	if (queued >= minQueuedBytes) return;

	const int frames = 512;
	std::vector<float> samples(frames);
	for (int i = 0; i < frames; ++i)
	{
		const float freq = m_Frequency;
		samples[i] = std::sinf(m_Phase * 2.0f * 3.14159265f);
		m_Phase += freq / (float)sampleRate;
		if (m_Phase >= 1.0f) m_Phase -= 1.0f;
	}

	SDL_PutAudioStreamData(m_Stream, samples.data(), (int)(samples.size() * sizeof(float)));
}
