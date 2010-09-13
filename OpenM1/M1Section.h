//
// Definitions for the following classes:
//
// M1ItemValue
// M1Container
// M1Item
// M1Section
//

// Encapsulates data that can be of various formats
class M1ItemValue
{
public:
	M1ItemValue();
	M1ItemValue(const M1ItemValue& m);
	M1ItemValue& operator= (const M1ItemValue& other);
	~M1ItemValue();

	void CopyFrom(const M1ItemValue& m);

	void ResetData();

	M1ItemFormat	GetFormat() const { return m_format; }
	void			SetFormat(M1ItemFormat format) { m_format = format; }

	void SetString(const CStdString& s);
	void SetNumber(int n, M1ItemFormat fmt = IF_undefined);
	void SetBinary(const BYTE* pBin, int iBufLen);

	CStdString	GetString() const { return m_sValue; }
	int			GetNumber() const { return m_iValue; }
	BYTE*		GetBinary(int *piLen) const { *piLen = m_iLen; return m_pBin; }

	bool IsMacro();

	static bool IsNumericType(const M1ItemFormat fmt);
	static bool IsStringType(const M1ItemFormat fmt);
	static bool IsBinaryType(const M1ItemFormat fmt);

	int		   CalculateBitLength();

	CStdString DebugClassDump();

private:
	 M1ItemFormat		m_format;

	CStdString			m_sValue;
	int					m_iValue;
	BYTE*				m_pBin;
	int					m_iLen;	
};

// Abstract placeholder for either a section or an item
class M1Container
{
public:
	M1Container(const CStdString& sName, int index, M1Section* pParent);

	virtual CStdString DebugClassDump() = 0;
	virtual int		   CalculateBitLength() = 0;

	const CStdString GetName() { return m_sName; }
	void			 SetName(const CStdString& s) { m_sName = s; }
	int			 GetIndex() const { return m_index; }
	void		 SetIndex(int index) { m_index = index; }
	void		 SetParent(M1Section* pParent) { m_pParent = pParent; }
	M1Section&	 GetParent() { return *m_pParent; }
	void		 SetIsItem(bool b) { m_bIsItem = b; }
	bool		 GetIsItem() { return m_bIsItem; }
	CStdString	 GetTypeString() { return m_bIsItem ? _T("item") : _T("section"); }

private:
	CStdString	 m_sName;
	int			 m_index;
	bool		 m_bIsItem;
	M1Section*   m_pParent;
};

// The item is a name/value pair with an index and a reference to the original standard
class M1Item : public M1Container
{
public:
	M1Item(const CStdString& sName, int index, M1ItemFormat fmtFinal, M1Section* pParent);
	~M1Item();

	M1ItemValue*	GetValue() { return &m_value; }
	void			SetValue(const M1ItemValue& v) { m_value = v; }
	void			SetValue(const CStdString& s);

	M1ItemFormat	GetFormatFinal() { return m_formatFinal; }
	void			SetFormatFinal(M1ItemFormat format) { m_formatFinal = format; }

	int				CalculateBitLength();

	CStdString		DebugClassDump();

private:
	M1ItemFormat	m_formatFinal;
	M1ItemValue		m_value;
};

class M1SectionDef;

// M1Expectation/M1Expectations classes, define what the file reader is to "expect" when
// loading a package, in terms of "occurrences" and "length", for example.

enum M1ExpectationType { ET_undefined, ET_occurrences, ET_length };

class M1Expectation
{
public:
	M1Expectation();

	void SetParams(const CStdString& sIdentifier, int iValue);

	void SetType(M1ExpectationType type) { m_type = type; }
	M1ExpectationType GetType() { return m_type; }
	CStdString GetIdentifier() { return m_sIdentifier; }
	int GetValue() { return m_iValue; }

private:
	M1ExpectationType	m_type;
	CStdString			m_sIdentifier;
	int					m_iValue;
};

class M1Expectations
{
public:
	M1Expectations();

	void Add(M1Expectation& ex);
	int Find(M1ExpectationType type, const CStdString& sIdentifier);
	void RemoveLastFound();

	list<M1Expectation>& GetExpectationList() { return m_expectations; }

	CStdString	DebugClassDump();

private:
	list<M1Expectation>				m_expectations;
	list<M1Expectation>::iterator	m_it;
};

// Contents of a section, that contains a list of items and other sections
class M1Section : public M1Container
{
public:
	M1Section(const CStdString& sName, int index, M1Section* pParent);
	~M1Section();

	M1Section*	Section(const CStdString& sSectionName, int iSection, bool *pbNewSectionAdded = NULL);

	void		SetItemString(const CStdString& sItemName, int iItem, const CStdString& sValue);
	void		SetItemNumber(const CStdString& sItemName, int iItem, int iValue);
	void		SetItemBit(const CStdString& sItemName, int iItem, bool bValue);
	void		SetItemImage(const CStdString& sItemName, int iItem, BYTE* pImage, int iImageLen);

	bool		GetItemString(const CStdString& sItemName, int iItem, CStdString& sValue);
	bool		GetItemNumber(const CStdString& sItemName, int iItem, int& iValue);
	bool		GetItemImage(const CStdString& sItemName, int iItem, BYTE** ppImage, int* piImageLen);

	list<M1Container*>&	GetContainerList() { return m_Containers; }
	void SetContainerList(list<M1Container*> containers) { m_Containers = containers; }

	bool		Validate(M1Standard* pStandard,  CStdString& sLog, CStdString& sError);
	bool		GetIsValidated() { return m_bValidated; }

	bool		BitsetPopBytes(boost::dynamic_bitset<BYTE>& bitset, int iBytes, void *pBytes);

	bool		WriteToFile(const CStdString& sPath);
	bool		WriteToMem(BYTE** ppMem, int* piLen);
	void		WriteSectionToBitset(M1Section& section, boost::dynamic_bitset<BYTE>& bitset);

	bool		ReadFromFile(const CStdString& sPath, M1Standard& standard, CStdString& sError);
	bool		ReadFromMem(BYTE* pMem, int iLen, M1Standard& standard, CStdString& sError);
	void		ReadSectionFromBitset(M1Section& section, M1SectionDef& sectionDef, boost::dynamic_bitset<BYTE>& bitset, M1Expectations& expectations, CStdString& sError);
	void		ReadValueFromBitset(M1ItemValue& value, M1ItemFormat format, int iValueLen, boost::dynamic_bitset<BYTE>& bitset, CStdString& sError);

	int			CalculateBitLength(M1Section& section);
	int			CalculateBitLength();

	TCHAR*		CreateNewStringSlot(CStdString s) { return m_stringSlots.AddNew(s); };

	CStdString	DebugClassDump();

private:
	list<M1Container*>	 m_Containers;
	M1Section*			 m_pSectionValidated;
	bool				 m_bValidated;

	M1Container* FindContainer(const CStdString& sName, int iItem);
	M1Item*		 FindItem(const CStdString& sItemName, int iItem);
	M1Section*	 FindSection(const CStdString& sSectionName, int iSection);
	M1Item*		 Item(const CStdString& sItemName, int iItem, bool *pbNewItemAdded = NULL);

	bool		 AdjustSection(M1Section& section, M1SectionDef& sectionDef, M1Section& sectionAdjusted,
							   CStdString& sLog, CStdString& sError);
	bool		 CheckOccurrences(M1ContainerDef& containerDef, int iNum, CStdString& sLog, CStdString& sError);
	bool		 CheckIndices(list<M1Container*>& containerList, CStdString& sLog, CStdString& sError);
	void		 AddReservedContainer(M1Section& section, M1ContainerDef& containerDef, CStdString& sLog);
	int			 InterpretStringValue(const CStdString& s);
	bool		 IsNumerical(const CStdString& s);

	bool		 ValidateSection(M1Section& section, M1SectionDef& sectionDef, CStdString& sLog, CStdString& sError);
	bool		 ValidateItem(M1Item& item, M1ItemDef& itemDef, CStdString& sLog, CStdString& sError);

	void		 ResolveSectionMacros(M1Section& section, M1Section& headSection, bool& bRet, CStdString& sLog, CStdString& sError);
	bool		 ResolveItemMacro(M1Item& item, M1Section& section, M1Section& headSection, CStdString& sLog, CStdString& sError);
	bool		 ParseMacro(const CStdString& sIn, CStdString& sMacro, CStdString& sIdentifier);
	void		 FindContainerPreceedingItem(const M1Item& item, const CStdString &sContainerName, const M1Section& startSection, M1Container** ppCurrentContainer);
	void		 FindContainerSucceedingItem(const M1Item& item, const CStdString &sContainerName, const M1Section& startSection, M1Container** ppCurrentContainer);
	void		 CountContainerOccurrences(const CStdString &sContainerName, const M1Section& section, int* piNum);

	CStdString	 DebugSectionDump(M1Section& mainSection, int depth);

	CStringSlots m_stringSlots;
};
