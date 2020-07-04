#include "stdafx.h"
#include "Timer.h"

CGameTimer::CGameTimer() {
	/*
		���� ī����(performance Counter) �ϵ��� ������ ������ ���� ī���Ϳ�
		���� ���ļ�(Performance Frequency)�� ����Ͽ� �ð� ������ ����
		���ٸ� ��Ƽ�̵�� Ÿ�̸Ӹ� ����Ѵ� -> �ð������� 0.001�� (1ms)
	*/
	if (::QueryPerformanceFrequency((LARGE_INTEGER*)&performanceFrequency)) {
		bHardwarePerformanceCounter = true;
		::QueryPerformanceCounter((LARGE_INTEGER*)&lastTime);
		timeScale = 1.0f / performanceFrequency;
	}
	else {
		bHardwarePerformanceCounter = false;
		lastTime = ::timeGetTime();
		timeScale = 0.001f;
	}
}

CGameTimer::~CGameTimer(){}

void CGameTimer::tick(float fLockFPS){
	if (bHardwarePerformanceCounter)
		::QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	else
		currentTime = ::timeGetTime();

	float currentTimeElapsed{ static_cast<float>(currentTime - lastTime) * timeScale };
	if (fLockFPS > 0.0f) {
		while (currentTimeElapsed < (1.0f / fLockFPS)) {
			if (bHardwarePerformanceCounter)
				::QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			else
				currentTime = ::timeGetTime();
			currentTimeElapsed = (currentTime - lastTime) * timeScale;
		}
	}
	lastTime = currentTime;

	if (fabsf(currentTimeElapsed - timeElapsed) < 1.0f) {
		::memmove(&frameTime[1], frameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		frameTime[0] = currentTimeElapsed;
		if (sampleCount < MAX_SAMPLE_COUNT) ++sampleCount;
	}

	++framePerSecond;
	fpsTimeElapsed += currentTimeElapsed;
	if (fpsTimeElapsed > 1.0f) {
		currentFrameRate = framePerSecond;
		framePerSecond = 0;
		fpsTimeElapsed = 0.0f;
	}
	timeElapsed = 0.0f;
	for (ULONG i = 0; i < sampleCount; ++i) timeElapsed += frameTime[i];
	if (sampleCount > 0) timeElapsed /= sampleCount;
}

unsigned long CGameTimer::getFrameRate(LPTSTR lpszString, int nCharacters){
	if (lpszString) {
		_itow_s(currentFrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T(" FPS)"));
	}
	return currentFrameRate;
}

float CGameTimer::getElapsedTime(){
	return timeElapsed;
}

