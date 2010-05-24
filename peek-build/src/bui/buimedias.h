#ifndef UIMEDIA_H
#define UIMEDIA_H

#include "restypedefines.h"

class MidC
{
public:
	MidC();
	virtual ~MidC();
	bool SetMid(RESOURCE_ID(BYDMediaResT)  ResourceId);
	int16 PlayMid(uint8 Iterations);
	uint8* GetBuffer();
private:
	uint8* BufP;
	uint8 MidType;
	uint32 MidLength;
};

#endif //UIMEDIA_H
