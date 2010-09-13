

// Wraps image related functionality
class M1Image
{
public:
	M1Image(const BYTE *pImageData, int iImageLen);

	
private:
	const BYTE	*m_pBin;
	int			m_iLen;
};

