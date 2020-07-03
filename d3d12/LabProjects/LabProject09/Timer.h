#pragma once

class CGameTimer{
public:
	CGameTimer();
	virtual ~CGameTimer();

	void tick(float fLockFPS = 0.0f);
	unsigned long getFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	float getElapsedTime();
private:
	static constexpr int MAX_SAMPLE_COUNT{ 50 };

	bool bHardwarePerformanceCounter{};	// 컴퓨터가 performance Counter를 가지고 있는가?
	float timeScale{};	// scale Counter의 양
	float timeElapsed{};	// 마지막 프레임 이후 지나간 시간
	__int64 currentTime{};	// 현재의 시간
	__int64 lastTime{};		// 마지막 프레임의 시간
	__int64 performanceFrequency{};	// 컴퓨터의 performance frequency

	float frameTime[MAX_SAMPLE_COUNT]{};	// 누적 프레임 시간
	ULONG sampleCount{};	// 누적된 프레임 횟수

	unsigned long currentFrameRate{};	// 현재의 프레임 레이트
	unsigned long framePerSecond{};		// 초당 프레임 수
	float fpsTimeElapsed{};				// 프레임 레이트 계산 소요 시간
};

