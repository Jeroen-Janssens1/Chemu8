#pragma once
#include <SDL3/SDL.h>

class Audio
{
public:
	Audio();
	~Audio();

	Audio(const Audio&) = delete;
	Audio& operator=(const Audio&) = delete;
	Audio(Audio&&) = delete;
	Audio& operator=(Audio&&) = delete;

	// Initialize audio device. Returns true on success.
	bool Initialize(int sampleRate = 48000);

	// Start/stop playing tone
	void SetPlaying(bool playing);

	// Must be called periodically (each frame) to feed the audio stream
	void Update(float deltaSeconds);

private:
	SDL_AudioSpec     m_Spec{};
	SDL_AudioStream* m_Stream;
	bool m_Playing{false};
	int m_SampleRate{48000};
	float m_Phase{0.0f};
	float m_Frequency{440.0f}; // A4 tone
};
