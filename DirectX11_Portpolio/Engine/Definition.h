#pragma once

#define Check(x) { assert((x) >= (0)); }
#define Assert(x, message) { assert((x) && (message)); }

#define Delete(x)\
{\
	if (x != nullptr)\
	{\
		delete (x);\
		(x) = nullptr;\
	}\
}

#define CheckTrue(x) { if(x == true) return; }
#define CheckTrueResult(x, y) { if(x == true) return y; }

#define CheckFalse(x) { if(x == false) return;}
#define CheckFalseResult(x, y) { if(x == false) return y;}

#define CheckNull(x) { if(x == nullptr) return; }
#define CheckNotNull(x) { if(x != nullptr) return; }
