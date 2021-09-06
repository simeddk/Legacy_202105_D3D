#pragma once

class Time
{
public:
	static Time* Get();

	static void Create();
	static void Delete();

	static bool Stopped() { return isTimerStopped; }
	static float Delta() { return isTimerStopped ? 0.0f : timeElapsed; }

	void Update();
	void Print();

	void Start();
	void Stop();

	float FPS() const { return framePerSecond; }
	float Running() const { return runningTime; }

private:
	Time(void);
	~Time(void);

	static Time* instance;///< 싱글톤 객체

	static bool isTimerStopped;///< 타이머 중지
	static float timeElapsed;///< 이전 프레임으로부터 경과시간


	INT64 ticksPerSecond;///< 초당 틱카운트
	INT64 currentTime;///< 현재 시간
	INT64 lastTime;///< 이전시간
	INT64 lastFPSUpdate;///< 마지막 FPS 업데이트 시간
	INT64 fpsUpdateInterval;///< fps 업데이트 간격

	UINT frameCount;///< 프레임 수
	float runningTime;///< 진행 시간
	float framePerSecond;///< FPS
};

///////////////////////////////////////////////////////////////////////////////

class Timer
{
public:
	Timer();
	~Timer();

	void Start(function<void()> func, int milliSec, UINT repeat = 0);
	void Stop();

private:
	mutex m;

	bool bComplete;
	UINT count;
};

///////////////////////////////////////////////////////////////////////////////

class Performance
{
public:
	Performance();

	void Start();
	float End();

private:
	__int64 tick;
	__int64 start, end;
};