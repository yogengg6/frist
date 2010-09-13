#include "stdafx.h"

//
// M1Image
//

M1Image::M1Image(const BYTE *pImageData, int iImageLen)
{
	m_pBin = pImageData;
	m_iLen = iImageLen;
}

