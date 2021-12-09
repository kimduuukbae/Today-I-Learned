#pragma once

#define FailedAssert(value)					\
{											\
	if (FAILED(value)) [[unlikely]] { assert(ERROR); }	\
}										


#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }

