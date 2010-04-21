#ifndef _NISTFIELD_H
#define _NISTFIELD_H

#include "common.h"

typedef enum { fmtUNK, fmtRAW, fmtBMP, fmtJPG, fmtWSQ, fmtJP2, fmtFX4, fmtCBEFF } IWImageFormat;

// A combination of all the possible properties any EBTS image record type
// could ever ask for. Not all props apply to all record types.
typedef struct
{
	long		  lWidth;
	long		  lHeight;
	long		  lScaleUnits;
	long		  lHPS;
	long		  lVPS;
	char		  szCompression[10];
	char		  szColorSpace[10];
	long		  lBitsPerPixel;
	long		  lImageFormat;
	long		  lCompressionAlgo;	// 0 = none, 1 = WSQ/JPG
} IWNISTImageInfo;

#define MAX_SUBFIELDS_PER_FIELD (int)200

class CSubFieldItem
{
public:
	int m_nSubField;
	int m_nSubFieldItem;
	CStdString m_sData;

	int GetLength();
	int SetItemData(int nSubField, int nSubFieldItem, CStdString sData);

public:
	CSubFieldItem::CSubFieldItem(const CSubFieldItem& obj)  { CopyObj(obj); }
	CSubFieldItem& CSubFieldItem::operator=(const CSubFieldItem& rhs) { CopyObj(rhs); return *this; }

	CSubFieldItem() { m_nSubField = m_nSubFieldItem = 0; m_sData.Empty(); }

	void CopyObj(const CSubFieldItem& obj);
};

class CNISTField
{
private:
	void InitVars();

public:
	CNISTField();
	CNISTField(long lRecordType, long lFieldID);
	virtual ~CNISTField();

	CNISTField::CNISTField(const CNISTField& obj)  { CopyObj(obj); }
	CNISTField& CNISTField::operator=(const CNISTField& rhs) { CopyObj(rhs); return *this; }
	void CopyObj(const CNISTField& obj);

	int m_nRecordType;
	int m_nField; // 1.01within the record
	std::vector<CSubFieldItem*> m_SubFieldAry; // field data, subfields, etc

	int m_nRecordLen; // set at write time
	bool m_bWriteRecordSeperator;

	BYTE* m_pImageData;

	int m_nImageLen;
	IWImageFormat m_ImageFormat;

	int SetSubField(int nSubField, int nSubFieldItem, CStdString sData);
	int RemoveSubField(int nSubField);

	int GetNumSubfields();

	// AddItem() does both add and edit
	int AddItem(int RecordType, int Field, int Subfield, int Item, CStdString sData);

	int SetImageData(BYTE* pImage, int nImageLen);

	int GetSubfieldCount() { return m_SubFieldAry.size(); }
	CSubFieldItem *GetSubfieldItem(int nSubfield, int nItem);
	int FindItem(int nSubfield, int Item, CStdString& sData);
	int GetNumItems(int nSubfield, int* pnCount);

	int GetWriteLen();
	int Write(FILE *pFile);
    int Write(TCHAR **ppData, int *poffset);

	static CStdString ImageExtFromImageFormat(IWImageFormat fmt);
	static IWImageFormat ImageFormatFromImageExt(CStdString sFormat);

	IWImageFormat GetImageFormatFromHeader(int nRecordType, const BYTE* pImage);
};

extern char *g_szImageFormats[];

#endif // _NISTFIELD_H
