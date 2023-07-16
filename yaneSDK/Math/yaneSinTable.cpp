#include "stdafx.h"
#include "yaneSinTable.h"
#include <math.h>

namespace yaneuraoGameSDK3rd {
namespace Math {

const double PI = 3.1415926535897932384626433832795;

CSinTable::CSinTable(){
	int i;
	for(i=0;i<512;i++){
		m_alCosTable[i] = (LONG)(cos(i*PI/256) * 65536);
	}

	for(i=0;i<256;i++){
		m_alAtanTable[i] = (WORD)(atan(i/(double)256)*(65536/2)/PI);
	}
}

const CSinTable gSinTable;

} // end of namespace Math
} // end of namespace yaneuraoGameSDK3rd
