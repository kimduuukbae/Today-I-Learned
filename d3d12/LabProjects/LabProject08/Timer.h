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

	bool bHardwarePerformanceCounter{};	// ��ǻ�Ͱ� performance Counter�� ������ �ִ°�?
	float timeScale{};	// scale Counter�� ��
	float timeElapsed{};	// ������ ������ ���� ������ �ð�
	__int64 currentTime{};	// ������ �ð�
	__int64 lastTime{};		// ������ �������� �ð�
	__int64 performanceFrequency{};	// ��ǻ���� performance frequency

	float frameTime[MAX_SAMPLE_COUNT]{};	// ���� ������ �ð�
	ULONG sampleCount{};	// ������ ������ Ƚ��

	unsigned long currentFrameRate{};	// ������ ������ ����Ʈ
	unsigned long framePerSecond{};		// �ʴ� ������ ��
	float fpsTimeElapsed{};				// ������ ����Ʈ ��� �ҿ� �ð�
};

