#include "stdafx.h"
#include "Timer.h"

CGameTimer::CGameTimer(){
	::QueryPerformanceFrequency((LARGE_INTEGER*)&m_PErformanceFrequencyPerSec);
	::QueryPerformanceCounter((LARGE_INTEGER*)&m_nLastPerformanceCounter);

	m_fTimeScale = 1.0 / (double)m_PErformanceFrequencyPerSec;

	m_nBasePerformanceCounter = m_nLastPerformanceCounter;
	m_nPausedPerformanceCounter = 0;
	m_nStopPerformanceCounter = 0;

	m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_FramePerSecond = 0;
	m_fFPSTimeElapsed = 0.0f;
}

CGameTimer::~CGameTimer(){

}

void CGameTimer::Tick(float fLockFPS){
	
	::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentPerformanceCounter);

	float fTimeElapsed{ static_cast<float>((m_nCurrentPerformanceCounter - m_nLastPerformanceCounter) * m_fTimeScale)};

	if (fLockFPS > 0.0f) {
		while (fTimeElapsed < (1.0f / fLockFPS)) {
			::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentPerformanceCounter);

			fTimeElapsed = static_cast<float>((m_nCurrentPerformanceCounter - m_nLastPerformanceCounter) * m_fTimeScale);
		}
	}
	m_nLastPerformanceCounter = m_nCurrentPerformanceCounter;

	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f) {
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;
		if (m_nSampleCount < MAX_SAMPLE_COUNT) ++m_nSampleCount;
	}

	++m_FramePerSecond;
	m_fFPSTimeElapsed += fTimeElapsed;
	if (m_fFPSTimeElapsed > 1.0f) {
		m_nCurrentFrameRate = m_FramePerSecond;
		m_FramePerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	}

	m_fTimeElapsed = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; ++i)
		m_fTimeElapsed += m_fFrameTime[i];
	if (m_nSampleCount > 0) m_fTimeElapsed /= m_nSampleCount;
}

unsigned long CGameTimer::GetFrameRate(LPTSTR lpszString, int nCharacters){
	if (lpszString) {
		::_itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
		::wcscat_s(lpszString, nCharacters, _T(" FPS)"));
	}
	return m_nCurrentFrameRate;
}

float CGameTimer::GetTimeElapsed(){
	return m_fTimeElapsed;
}
