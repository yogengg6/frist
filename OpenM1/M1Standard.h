#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include "DOMTreeErrorReporter.hpp"

using namespace xercesc_3_1;


// Abstract placeholder for either a sectiondef or an itemdef
class M1ContainerDef
{
public:
	M1ContainerDef(M1Standard* pStandard);

	virtual void DebugOutput();

	CStdString	GetName() { return m_sName; }
	void		SetName(const CStdString& s) { m_sName = s; }
	CStdString	GetDescription() { return m_sDescription; }
	void		SetDescription(const CStdString& s) { m_sDescription = s; }
	CStdString	GetMinOccurrences() { return m_sMinOccurrences; }
	void		SetMinOccurrences(const CStdString& s) { m_sMinOccurrences = s; }
	CStdString	GetMaxOccurrences() { return m_sMaxOccurrences; }
	void		SetMaxOccurrences(const CStdString& s) { m_sMaxOccurrences = s; }
	bool		GetReserved() { return m_bReserved; }
	void		SetReserved(const CStdString& s) { m_bReserved = (s.Compare(_T("true")) == 0); }

	void		SetIsItemDef(bool b) { m_bIsItem = b; }
	bool		GetIsItemDef() { return m_bIsItem; }
	CStdString	GetTypeString() { return m_bIsItem ? _T("item") : _T("section"); }

	M1Standard	*GetStandard() { return m_pStandard; }
	void		SetStandard(M1Standard* pStandard) { m_pStandard = pStandard; }

private:
	CStdString	m_sName;
	CStdString	m_sDescription;
	CStdString	m_sMinOccurrences;
	CStdString	m_sMaxOccurrences;
	bool		m_bReserved;

	M1Standard	*m_pStandard;

	bool		m_bIsItem;
};


class CNameValuePair
{
public:
	CNameValuePair();
	CNameValuePair(const CStdString &sName, const CStdString &sValue);

	CStdString	GetName() { return m_sName; }
	void		SetName(const CStdString& s) { m_sName = s; }
	CStdString	GetValue() { return m_sValue; }
	void		SetValue(const CStdString& s) { m_sValue = s; }

private:
	CStdString	m_sName;
	CStdString	m_sValue;
};


// Definition of an itemdef
class M1ItemDef : public M1ContainerDef
{
public:
	M1ItemDef(M1Standard* pStandard);
	~M1ItemDef();

	void DebugOutput();

	M1ItemFormat	GetFormat() { return m_format; }
	void			SetFormat(const CStdString& s) { m_format = StringToFormat(s); }
	CStdString		GetMinValue() { return m_sMin; }
	void			SetMinValue(const CStdString& s) { m_sMin = s; }
	CStdString		GetMaxValue() { return m_sMax; }
	void			SetMaxValue(const CStdString& s) { m_sMax = s; }
	CStdString		GetDefaultValue() { return m_sDefaultValue; }
	void			SetDefaultValue(const CStdString& s) { m_sDefaultValue = s; }

	void					AddListItem(const CNameValuePair &nameValuePair);
	list<CNameValuePair>&	GetListItems() { return m_listItems; }

	bool			IsMacro();

	static M1ItemFormat StringToFormat(const CStdString &sFormat);
	static CStdString	FormatToString(M1ItemFormat format);

private:
	// An item is made up of all that is necessary to define the conditions
	// for defining the format and limitations of the data within.

	M1ItemFormat	m_format;
	CStdString		m_sMin;
	CStdString		m_sMax;
	CStdString		m_sDefaultValue;

	list<CNameValuePair>	m_listItems;
};


// Definition of a sectiondef
class M1SectionDef : public M1ContainerDef
{
public:
	M1SectionDef(M1Standard* pStandard);
	~M1SectionDef();

	void DebugOutput();

	void		AddContainerDef(M1ContainerDef *pContainerDef, bool bIsItem);
	void		ResetContainerDefList();
	void		GetNextContainerDef(M1ContainerDef **ppContainerDef);

	M1ContainerDef* FindContainerDef(const CStdString& sName, bool bIsItemDef);

	list<M1ContainerDef*>&	GetContainerDefList() { return m_containerDefs; }

private:
	// A section is made up of a series of itemdefss and other sectiondefs
	list<M1ContainerDef*>::iterator		m_it;
	list<M1ContainerDef*>				m_containerDefs;
};


// The whole standard definition
class M1Standard : public M1SectionDef
{
public:
	M1Standard();
	~M1Standard();

	bool		LoadFromFile(const CStdString &sFile, CStdString &sError);
	void		TraverseSectionDef(M1SectionDef *pSectionDef, DOMNode *node);
	void		TraverseItemDef(M1ItemDef *pItem, DOMNode *node);
	void		TraverseListitem(CNameValuePair &nameValuePair, DOMNode *node);

	TCHAR* CreateNewStringSlot(CStdString s) { return m_stringSlots.AddNew(s); };

private:
	CStringSlots m_stringSlots;
};
