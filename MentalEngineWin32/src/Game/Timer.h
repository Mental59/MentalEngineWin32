#pragma once

namespace Game
{
	class Timer
	{
	public:
		Timer();

		float GetTotalTimeInSeconds() const;
		float GetDeltaTimeInSeconds() const;

		void Reset();
		void Start();
		void Stop();
		void Tick();
	};
}
