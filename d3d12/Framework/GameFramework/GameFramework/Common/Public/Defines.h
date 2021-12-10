#pragma once

#define FailedAssert(value)					\
{											\
	if (FAILED(value)) [[unlikely]] { assert(ERROR); }	\
}										


#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }

#if defined(_M_IX86_FP) && !defined(__AVX__)
#define SSE_INTRINSICS
#endif

#if defined(__SSE__) || defined(__SSE2__) || defined(__SSE3__)
#define SSE_INTRINSICS
#endif

#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__) || defined(__AVX512CD__) || defined(__AVX512BW__) || defined(__AVX512DQ__) || defined(__AVX512VL__)
#define AVX_INTRINSICS
#endif

#if defined(__AVX__) && !defined(__SSE4__)
#define SSE4_INTRINSICS
#endif

#if defined(SSE4_INTRINSICS) && !defined(__SSE3__)
#define SSE3_INTRINSICS
#endif

#if defined(SSE3_INTRINSICS) && !defined(__SSE__)
#define SSE_INTRINSICS
#endif

#if !defined(SSE_INTRINSICS) && !defined(AVX_INTRINSICS)
#define NO_INTRINSICS
#endif
