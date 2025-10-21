#pragma once
#include <atlbase.h>
#include <string>
#include <vector>
#include <functional>
#include <type_traits>
#include <memory>
#include "nstring.h"
#include "localeex.h"

struct destruct
{
	std::function <void ()> endtask = nullptr;
	destruct (std::function <void ()> init): endtask (init) {}
	~destruct () { if (endtask) endtask (); }
};
template <typename T, typename BASE_INT> struct LargeIntBase
{
	BASE_INT val;
	LargeIntBase () { val.QuadPart = 0; }
	LargeIntBase (T v) { val.QuadPart = v; }
	LargeIntBase (const LargeIntBase &other) { val.QuadPart = other.val.QuadPart; }
	LargeIntBase (const BASE_INT &other) { val = other; }
	operator BASE_INT () const { return val; }
	operator T () const { return val.QuadPart; }
	explicit operator bool () const { return val.QuadPart != 0; }
	T *ptr_num () { return &val.QuadPart; }
	BASE_INT *ptr_union () { return &val; }
	size_t sizeof_num () const { return sizeof (val.QuadPart); }
	size_t sizeof_union () const { return sizeof (val); }
	T num () const { return val.QuadPart; }
	BASE_INT win_union () const { return val; }
	LargeIntBase operator + (const LargeIntBase &rhs) const { return LargeIntBase (val.QuadPart + rhs.val.QuadPart); }
	LargeIntBase operator - (const LargeIntBase &rhs) const { return LargeIntBase (val.QuadPart - rhs.val.QuadPart); }
	LargeIntBase operator * (const LargeIntBase &rhs) const { return LargeIntBase (val.QuadPart * rhs.val.QuadPart); }
	LargeIntBase operator / (const LargeIntBase &rhs) const { return LargeIntBase (val.QuadPart / rhs.val.QuadPart); }
	LargeIntBase operator % (const LargeIntBase &rhs) const { return LargeIntBase (val.QuadPart % rhs.val.QuadPart); }
	LargeIntBase &operator += (const LargeIntBase &rhs) { val.QuadPart += rhs.val.QuadPart; return *this; }
	LargeIntBase &operator -= (const LargeIntBase &rhs) { val.QuadPart -= rhs.val.QuadPart; return *this; }
	LargeIntBase &operator *= (const LargeIntBase &rhs) { val.QuadPart *= rhs.val.QuadPart; return *this; }
	LargeIntBase &operator /= (const LargeIntBase &rhs) { val.QuadPart /= rhs.val.QuadPart; return *this; }
	LargeIntBase &operator %= (const LargeIntBase &rhs) { val.QuadPart %= rhs.val.QuadPart; return *this; }
	LargeIntBase operator & (const LargeIntBase &rhs) const { return LargeIntBase (val.QuadPart & rhs.val.QuadPart); }
	LargeIntBase operator | (const LargeIntBase &rhs) const { return LargeIntBase (val.QuadPart | rhs.val.QuadPart); }
	LargeIntBase operator ^ (const LargeIntBase &rhs) const { return LargeIntBase (val.QuadPart ^ rhs.val.QuadPart); }
	LargeIntBase operator << (int n) const { return LargeIntBase (val.QuadPart << n); }
	LargeIntBase operator >> (int n) const { return LargeIntBase (val.QuadPart >> n); }
	LargeIntBase &operator &= (const LargeIntBase &rhs) { val.QuadPart &= rhs.val.QuadPart; return *this; }
	LargeIntBase &operator |= (const LargeIntBase &rhs) { val.QuadPart |= rhs.val.QuadPart; return *this; }
	LargeIntBase &operator ^= (const LargeIntBase &rhs) { val.QuadPart ^= rhs.val.QuadPart; return *this; }
	LargeIntBase &operator <<= (int n) { val.QuadPart <<= n; return *this; }
	LargeIntBase &operator >>= (int n) { val.QuadPart >>= n; return *this; }
	LargeIntBase &operator ++ () { ++val.QuadPart; return *this; }
	LargeIntBase operator ++ (int) { LargeIntBase tmp (*this); ++val.QuadPart; return tmp; }
	LargeIntBase &operator -- () { --val.QuadPart; return *this; }
	LargeIntBase operator -- (int) { LargeIntBase tmp (*this); --val.QuadPart; return tmp; }
	bool operator <  (const LargeIntBase &rhs) const { return val.QuadPart <  rhs.val.QuadPart; }
	bool operator >  (const LargeIntBase &rhs) const { return val.QuadPart >  rhs.val.QuadPart; }
	bool operator <= (const LargeIntBase &rhs) const { return val.QuadPart <= rhs.val.QuadPart; }
	bool operator >= (const LargeIntBase &rhs) const { return val.QuadPart >= rhs.val.QuadPart; }
	bool operator == (const LargeIntBase &rhs) const { return val.QuadPart == rhs.val.QuadPart; }
	bool operator != (const LargeIntBase &rhs) const { return val.QuadPart != rhs.val.QuadPart; }
	bool operator ! () const { return !val.QuadPart; }
	LargeIntBase operator ~ () const { return LargeIntBase (~val.QuadPart); }
};
typedef LargeIntBase <LONGLONG, LARGE_INTEGER> LargeInt, lint;
typedef LargeIntBase <ULONGLONG, ULARGE_INTEGER> ULargeInt, ulint;
template <typename ct, typename tr = std::char_traits <ct>, typename al = std::allocator <ct>> class basic_priid: public std::basic_nstring <ct, tr, al>
{
	using base = std::basic_nstring <ct, tr, al>;
	public:
	using typename base::size_type;
	using typename base::value_type;
	using base::base;
	basic_priid (const ct *buf, size_t sz = 16): base (buf, sz) {}
	template <std::size_t N> basic_priid (const ct (&arr) [N]) : base (arr, N) {}
};
typedef basic_priid <CHAR> pri_sectid;
std::string ReadStringEndwithNullA (IStream *pStream)
{
	if (!pStream) return "";
	std::string result;
	char ch = 0;
	ULONG cbRead = 0;
	while (true)
	{
		HRESULT hr = pStream->Read (&ch, 1, &cbRead);
		if (FAILED (hr) || cbRead == 0) break; 
		if (ch == '\0') break;
		result.push_back (ch);
	}
	return result;
}
std::wstring ReadStringEndwithNullW (IStream *pStream)
{
	if (!pStream) return L"";
	std::wstring result;
	WCHAR ch = 0;
	ULONG cbRead = 0;
	while (true)
	{
		HRESULT hr = pStream->Read (&ch, sizeof (WCHAR), &cbRead);
		if (FAILED (hr) || cbRead < sizeof (WCHAR)) break;
		if (ch == L'\0') break; 
		result.push_back (ch);
	}
	return result;
}
std::string ReadStringEndwithNull (IStream *ifile, std::string &ret)
{
	return ret = ReadStringEndwithNullA (ifile);
}
std::wstring ReadStringEndwithNull (IStream *ifile, std::wstring &ret)
{
	return ret = ReadStringEndwithNullW (ifile);
}

//											 0         1
//											 012345678901234
#define PRI_SECT_ID_PRI_DESCRIPTOR			"[mrm_pridescex]"
#define PRI_SECT_ID_HIERARCHICAL_SCHEMA		"[mrm_hschema]  "
#define PRI_SECT_ID_HIERARCHICAL_SCHEMAEX	"[mrm_hschemaex]"
#define PRI_SECT_ID_DECISION_INFO			"[mrm_decn_info]"
#define PRI_SECT_ID_RESOURCE_MAP			"[mrm_res_map__]"
#define PRI_SECT_ID_RESOURCE_MAP2			"[mrm_res_map2_]"
#define PRI_SECT_ID_DATA_ITEM				"[mrm_dataitem] "
#define PRI_SECT_ID_REVERSE_MAP				"[mrm_rev_map]  "
#define PRI_SECT_ID_REFERENCED_FILE			"[def_file_list]"
enum class SectionType
{
	Unknown,              // 未知类型 - 处理未识别的节区
	PriDescriptor,        // PRI 描述符 - 包含文件整体结构和引用信息
	HierarchicalSchema,	  // 层次结构模式 - 定义资源命名空间和层次结构
	HierarchicalSchemaEx, // 层次结构模式 - 定义资源命名空间和层次结构
	DecisionInfo,         // 决策信息 - 管理资源限定符和决策逻辑
	ResourceMap,          // 资源映射 - 将资源项映射到具体候选值
	ResourceMap2,         // 资源映射 - 将资源项映射到具体候选值
	DataItem,             // 数据项 - 存储实际的资源数据
	ReverseMap,           // 反向映射 - 提供从资源名到ID的映射
	ReferencedFile        // 引用文件 - 管理引用的外部文件
};

struct head
{
	// 版本标识符 / version identifier
	// “mrm_pri0”：客户端 6.2.1（Windows 8）。
	// “mrm_pri1”：客户端 6.3.0（Windows 8.1）。
	// “mrm_prif”：Windows Phone 6.3.1。
	// “mrm_pri2”：Universal 10.0.0（Windows 10）。
	CHAR szMagic [8] = {};		
	WORD wPlaceholder1 = -1,	// 未知，0 / unknown, zero
		wPlaceholder2 = 0;		// 未知，1 / unknown, one
	DWORD dwFileSize = 0,		// 文件总大小 / total file size
		dwToCOffset = 0,		// 目录表偏移 / offset of table of contents
		dwSectStartOffset = 0;	// 第一节的偏移 / offset of first section
	WORD wSectCount = 0,		// 节数量 / number of sections
		wPlaceholder3 = 0;		// 未知，0xFFFF / unknown, 0xFFFF
	DWORD dwPlaceholder4 = -1;	// 未知，0 / unknown, zero
	bool valid ()
	{
		CHAR m7 = szMagic [7];
		destruct endt ([this, m7] () {
			if (m7) szMagic [7] = m7;
		});
		szMagic [7] = '\0';
		if (!stricmp (szMagic, "mrm_pri")) return false;
		switch (m7)
		{
			case '0': case '1': case 'f': case 'F': break;
			default: return false;
		}
		if (wPlaceholder1 != 0) return false;
		if (wPlaceholder2 != 1) return false;
		if (wPlaceholder3 != 0xFFFF) return false;
		if (dwPlaceholder4 != 0) return false;
		return true;
	}
};
struct foot
{
	DWORD dwChkCode = 0;		// 0xDEFFFADE
	DWORD dwTotalFileSize = 0;	// total file size, as in header
	CHAR szMagic [8] = {0};		// version identifier, as in header
	bool valid (const head &fh)
	{
		if (dwChkCode != 0xDEFFFADE) return false;
		if (dwTotalFileSize != fh.dwFileSize) return false;
		for (int i = 0; i < 8; i ++)
		{
			if (szMagic [i] != fh.szMagic [i] && tolower (szMagic [i]) != tolower (fh.szMagic [i])) return false;
		}
		return true;
	}
};
struct tocentry
{
	CHAR szIdentifier [16] = {0};	// 节标识符 / section identifier
	WORD wFlags = 0;				// 标志 / flags
	WORD wSectFlags = 0;			// 节标志 / section flags
	DWORD dwSectQualifier = 0;		// 节限定符 / section qualifier
	DWORD dwSectOffset = 0;			// 节偏移（相对于第一节偏移） / section offset (relative to offset of first section)
	DWORD dwSectLength = 0;			// 节长度 / section length
};
struct section_header
{
	CHAR szIdentifier [16] = {0};	// 节标识符 / section identifier
	DWORD dwQualifier = 0;			// 节限定符 / section qualifier
	WORD wFlags = 0;				// 标志 / flags
	WORD wSectFlags = 0;			// 节标志 / section flags
	DWORD dwLength = 0;				// 节长度 / section length
	DWORD dwPlaceholder1 = -1;		// 未知，0 / unknown, zero
	bool valid () const { return szIdentifier [0] && !dwPlaceholder1; }
};
struct section_check
{
	DWORD dwChkCode = 0;	// 魔数 0xF5DEDEFA
	DWORD dwSectLength = 0;	// 节长度（与节头中的相同） / section length, as in section header
	bool valid (const section_header &h) const { return dwChkCode == 0xDEF5FADE && dwSectLength == h.dwLength; }
};
struct substream
{
	IStream *&ifile;
	ULONGLONG offset = 0;
	ULONGLONG size = 0;
	substream (IStream *&ifile, ulint ofs = 0, ulint siz = 0): ifile (ifile), offset (ofs), size (siz) {}
	void set (ulint p_offset, ulint p_size)
	{
		offset = p_offset;
		size = p_size;
	}
	HRESULT seek ()
	{
		ULARGE_INTEGER pos = {};
		HRESULT hr = ifile->Seek (lint (0), STREAM_SEEK_CUR, &pos);
		if (FAILED (hr)) return hr;
		if (pos.QuadPart == offset) return S_OK;
		return ifile->Seek (lint (offset), STREAM_SEEK_SET, nullptr);
	}
};
struct section
{
	section_header head;
	section_check foot;
	substream childst;
	// 请从 type 方法获取类型，而不是直接通过 sect_type 来读取（因为未初始化）
	SectionType sect_type = SectionType::Unknown;
	section (IStream *&ifile): childst (ifile) {}
	bool valid () const { return head.valid () && foot.valid (head); }
	SectionType type ()
	{
		if (sect_type == SectionType::Unknown)
		{
			pri_sectid pid (head.szIdentifier, 16);
			if (pid.equals (PRI_SECT_ID_PRI_DESCRIPTOR)) sect_type = SectionType::DecisionInfo;
			else if (pid.equals (PRI_SECT_ID_HIERARCHICAL_SCHEMA)) sect_type = SectionType::HierarchicalSchema;
			else if (pid.equals (PRI_SECT_ID_HIERARCHICAL_SCHEMAEX)) sect_type = SectionType::HierarchicalSchemaEx;
			else if (pid.equals (PRI_SECT_ID_DECISION_INFO)) sect_type = SectionType::DecisionInfo;
			else if (pid.equals (PRI_SECT_ID_RESOURCE_MAP)) sect_type = SectionType::ResourceMap;
			else if (pid.equals (PRI_SECT_ID_RESOURCE_MAP2)) sect_type = SectionType::ResourceMap2;
			else if (pid.equals (PRI_SECT_ID_DATA_ITEM)) sect_type = SectionType::DataItem;
			else if (pid.equals (PRI_SECT_ID_REVERSE_MAP)) sect_type = SectionType::ReverseMap;
			else if (pid.equals (PRI_SECT_ID_REFERENCED_FILE)) sect_type = SectionType::ReferencedFile;
			else sect_type = SectionType::Unknown;
		}
		return sect_type;
	}
};
namespace pri
{
	// PriDescriptorFlags

	enum class PRI_SEC_DESP: DWORD
	{
		INVALID = 0,
		AUTOMERGE = 0x1, // AutoMerge
		DEPLOY_MERGEABLE = 0x2, // IsDeploymentMergeable
		DEPLOY_MERGE_RESULT = 0x4, // IsDeploymentMergeResult
		AUTO_MERGE_RESULT = 0x8 // IsAutomergeMergeResult
	};

	// HierarchicalSchema & HierarchicalSchemaEx

	typedef struct _HSCHEMA_VERSION_INFO
	{
		WORD wMajor = 0;		// 主版本号 / major version
		WORD wMinor = 0;		// 次版本号 / minor version
		DWORD dwUnknown1 = -1;	// 未知，0
		// 校验和（checksum）
		// checksum: a CRC32-based checksum computed on the unique name, 
		// the name, the section indices of the Resource Map Section 
		// and Data Item Section, and the names of all scopes and items
		DWORD dwCheckSum = 0;	
		DWORD dwScopeCount = 0;	// scope 数量
		DWORD dwItemCount = 0;	// item 数量
	} HSCHEMA_VERSION_INFO;
	// 资源名称（scope/item）按以下格式存储（示例字段）：parent scope index、full path 长度、
	// 名字首字母（大写）、名字长度、名字偏移、index property 等。

	typedef struct _SCOPE_ITEM_INFO
	{
		WORD wParentScopeIndex = -1;	// parent scope index
		WORD wFullPathLength = 0;		// length of full path
		WCHAR wchUpperFirst = L'\0';	// uppercase first character of name, '\0' if name is empty
		// length of name in characters, null-terminator excluded, 0 if the length is bigger than 255
		// 在原作者的 C# 代码中是这么读取的：uint nameOffset = binaryReader.ReadUInt16() | (uint)((flags & 0xF) << 16);
		// 所以不能直接使用成员 bNameLength。请使用结构体中 name_offset () 方法来获取值
		BYTE bNameLength = 0;			
		BYTE bFlags = 0;				// flags and upper bits of name offset
		WORD wNameOffset = 0;			// offset of name in ASCII or Unicode name block in characters
		// index property
		// bits 0-3: upper bits 16-19 of name offset
		// bit 4: set if resource name is a scope, unset if it is an item
		// bit 5 : set if name is stored in the ASCII name block, unset if 
		// it is stored in the Unicode name block
		WORD wIndexProp = 0;
		bool is_scope () const { return bFlags & 0x10; }
		bool name_in_ascii () const { return bFlags & 0x20; }
		DWORD name_offset () const { return (DWORD)wNameOffset | ((DWORD)(bFlags & 0xF) << 16); }
		DWORD index () const { return wIndexProp; }
	} SCOPE_ITEM_INFO;
	typedef struct _SCOPE_EX_INFO
	{
		WORD wScopeIndex = 0;	// scope index
		WORD wChildCount = 0;	// child count
		WORD wFirstChild = 0;	// scope or item index of first child, all other children follow sequentially
		WORD wUnknown1 = 0;		// unknown, zero
		bool valid () const { return !wUnknown1; }
	} SCOPE_EX_INFO;
	// For each item, sorted by the index property, follows:	0	uint16	item index
	typedef WORD ITEM_INDEX;
	class RES_MAP_SCOPE; 
	// 如何使用（示例）：
	// auto root = std::make_shared <RES_MAP_SCOPE> (0, nullptr, L"root");

	enum class RES_MAP_OBJTYPE
	{
		ENTRY = 0,
		SCOPE = 1,
		ITEM = 2
	};
	class RES_MAP_ENTRY
	{
		public:
		using ScopePtr = std::weak_ptr <RES_MAP_SCOPE>;
		ITEM_INDEX wIndex = 0;
		std::wstring strName;
		RES_MAP_ENTRY (ITEM_INDEX index = 0, ScopePtr parent = {}, const std::wstring &name = L"", RES_MAP_OBJTYPE type = RES_MAP_OBJTYPE::ENTRY):
			wIndex (index), strName (name), rmsParent (parent), eType (type) {}
		virtual ~RES_MAP_ENTRY () = default;
		std::wstring full_name (WCHAR divide = L'\\')
		{
			if (strFullName.empty ())
			{
				if (auto parent = rmsParent.lock ()) strFullName = parent->full_name (divide) + divide + strName;
				else strFullName = strName;
			}
			return strFullName;
		}
		void refresh_full_name () { strFullName.clear (); }
		// 安全调用
		void change_parent (ScopePtr parent) 
		{ 
			rmsParent = parent; 
			refresh_full_name ();
		}
		auto get_parent () { return rmsParent; }
		static auto make (ITEM_INDEX index = 0, ScopePtr parent = {}, const std::wstring &name = L"")
		{
			return std::make_shared <RES_MAP_ENTRY> (index, parent, name, RES_MAP_OBJTYPE::ENTRY);
		}
		RES_MAP_OBJTYPE type () const { return eType; }
		protected:
		ScopePtr rmsParent;
		std::wstring strFullName; 
		RES_MAP_OBJTYPE eType;
	};
	// 如何使用（示例）：
	// auto folder = root->add_child <RES_MAP_SCOPE> (1, nullptr, L"folder");
	// auto file = folder->add_child <RES_MAP_ENTRY> (2, nullptr, L"file.txt");
	class RES_MAP_SCOPE: public RES_MAP_ENTRY, public std::enable_shared_from_this <RES_MAP_SCOPE>
	{
		public:
		using EntryPtr = std::shared_ptr <RES_MAP_ENTRY>;
		using ScopePtr = std::shared_ptr <RES_MAP_SCOPE>;
		explicit RES_MAP_SCOPE (ITEM_INDEX index = 0, ScopePtr parent = nullptr, const std::wstring &name = L""): 
			RES_MAP_ENTRY (index, parent, name, RES_MAP_OBJTYPE::SCOPE) {}
		template <typename T, typename ... Args> std::shared_ptr <T> add_child (Args &&...args)
		{
			static_assert (std::is_base_of <RES_MAP_ENTRY, T>::value, "T must derive from RES_MAP_ENTRY");
			auto child = std::make_shared <T> (std::forward <Args> (args) ...);
			child->rmsParent = this->shared_from_this ();
			vecChild.push_back (child);
			return child;
		}
		const std::vector <EntryPtr> &children () const { return vecChild; }
		static auto make (ITEM_INDEX index = 0, ScopePtr parent = {}, const std::wstring &name = L"")
		{
			return std::make_shared <RES_MAP_SCOPE> (index, parent, name, RES_MAP_OBJTYPE::SCOPE);
		}
		std::vector <EntryPtr> vecChild;
	};
	class RES_MAP_ITEM: public RES_MAP_ENTRY
	{
		public:
		using EntryPtr = std::shared_ptr <RES_MAP_ENTRY>;
		using ScopePtr = std::shared_ptr <RES_MAP_SCOPE>;
		using ItemPtr = std::shared_ptr <RES_MAP_ITEM>;
		RES_MAP_ITEM (ITEM_INDEX index = 0, std::shared_ptr <RES_MAP_SCOPE> parent = nullptr, const std::wstring &name = L""): 
			RES_MAP_ENTRY (index, parent, name, RES_MAP_OBJTYPE::ITEM) {}
		static auto make (ITEM_INDEX index = 0, ScopePtr parent = {}, const std::wstring &name = L"")
		{
			return std::make_shared <RES_MAP_ITEM> (index, parent, name, RES_MAP_OBJTYPE::ITEM);
		}
	};

	// DecisionInfo

	typedef struct _DECISION_INFO
	{
		WORD wFirstQualiIndex = 0;	// index of the first qualifier set index in the index table
		WORD wQualiSetsCount = 0;	// number of qualifiers sets in decision
	} DECISION_INFO;
	typedef struct _QUALIFIER_SET_INFO
	{
		WORD wFirstQualiIndex = 0;	// index of the first qualifier index in the index table // firstQualifierIndexIndex
		WORD wQualiSetsCount = 0;	// number of qualifiers in qualifier set // numQualifiersInSet
	} QUALIFIER_SET_INFO;
	typedef struct _QUALIFIER_INFO
	{
		WORD wDistQualiIndex = 0;	// index of distinct qualifier
		WORD wPriority = 0;			// priority
		WORD wFallbackScore = -1;	// fallback score, values range from 0 to 1000
		WORD wUnknown1 = -1;		// unknown, zero
	} QUALIFIER_INFO;
	enum class QUALIFIER_TYPE: WORD
	{
		LANGUAGE = 0,		// 语言 (0)
		CONTRAST = 1,		// 对比度 (1)
		SCALE = 2,			// 比例 (2)
		HOMEREGION = 3,		// 主屏幕区域 (3)
		TARGETSIZE = 4,		// 目标尺寸 (4)
		LAYOUTDIR = 5,		// 布局方向 (5)
		THEME = 6,			// 主题 (6)
		ALTERNATEFORM = 7,	// 替代格式 (7)
		DXFEATURELEVEL = 8,	// DX 功能等级 (8)
		CONFIG = 9,			// 配置 (9)
		DEVICEFAMILY = 10,	// 设备系列 (10)
		CUSTOM = 11,		// 自定义 (11)
	};
	typedef struct _DISTINCE_QUALIFIER_INFO
	{
		WORD wUnknown1 = 0;			// unknown
		WORD wQualiType = 0;		// qualifier type
		WORD wUnknown2 = 0;			// unknown
		WORD wUnknown3 = 0;			// unknown
		WORD wQualiValueOffset = 0;	// offset of qualifier value in qualifier value block, in characters
	} DISTINCE_QUALIFIER_INFO;
	typedef struct _QUALIFIER
	{
		ITEM_INDEX wIndex = 0;
		QUALIFIER_TYPE eType = QUALIFIER_TYPE::CUSTOM;
		WORD wPriority = 0;
		DOUBLE dFallbackScope = 0;
		std::wstring swValue = 0;
		_QUALIFIER (ITEM_INDEX index = 0, QUALIFIER_TYPE type = QUALIFIER_TYPE::CUSTOM, WORD priority = 0, DOUBLE fallbackScope = 0, const std::wstring &value = L""):
			wIndex (index), eType (type), wPriority (priority), dFallbackScope (fallbackScope), swValue (value) {}
	} QUALIFIER;
	typedef struct _QUALIFIER_SET
	{
		WORD wIndex = 0;
		std::vector <QUALIFIER> verQuals;
		_QUALIFIER_SET (WORD index = 0, std::vector <QUALIFIER> &quals = {}):
			wIndex (index), verQuals (quals) {}
	} QUALIFIER_SET;
	typedef struct _DECISION
	{
		WORD wIndex = 0;
		std::vector <QUALIFIER_SET> verQualSets;
		_DECISION (WORD index, std::vector <QUALIFIER_SET> &qualsets = {}):
			wIndex (index), verQualSets (qualsets) {}
	} DECISION;

	// ResourceMap & ResourceMap2

	typedef struct _HSCHEMA_REF_BLOCK
	{
		HSCHEMA_VERSION_INFO verHschema;	// hierarchical schema version info
		WORD wUniIdLength = 0;				// length of unique id in characters, null-terminator included
		WORD wUnknown1 = -1;				// unknown, zero
		DWORD dwUnknown2 = 0;				// unknown
		DWORD dwUnknown3 = 0;				// unknown
		std::wstring swUniqueId = L"";		// unique id
	} HSCHEMA_REF_BLOCK;
	enum class RES_VALUE_TYPE: DWORD
	{
		STRING = 0,			// String (0)
		PATH = 1,			// Path (1)
		EMBEDDEDDATA = 2,	// EmbeddedData (2)
		ASCIISTRING = 3,	// AsciiString (3)
		UTF8STRING = 4,		// Utf8String (4)
		ASCIIPATH = 5,		// AsciiPath (5)
		UTF8PATH = 6		// Utf8Path (6)
	};
	typedef struct _RES_VALUE_TYPE_TABLE
	{
		DWORD dwUnknown1 = 0;	// unknown, 4
		DWORD dwResType = -1;	// resource value type
	} RES_VALUE_TYPE_TABLE;
	typedef struct _ITEM_ITEMINFO_GROUP_TABLE_ENTRY
	{
		WORD wFirstIndexProperty = 0;	// index property of first resource item
		WORD wItemInfoGroupIndex = 0;	// index of iteminfo group
	} ITEM_ITEMINFO_GROUP_TABLE_ENTRY;
	typedef struct _ITEMINFO_GROUP_TABLE_ENTRY
	{
		WORD wItemInfoCount = 0;	// number of iteminfos in this group
		WORD wFirstItemIndex = 0;	// index of the first iteminfo in this group
	} ITEMINFO_GROUP_TABLE_ENTRY;
	typedef struct _ITEM_ITEMINFO_TABLE_ENTRY
	{
		WORD wDecisionIndex = 0;	// index of decision
		WORD wFirstCandiIndex = 0;	// index of first candidate
	} ITEM_ITEMINFO_TABLE_ENTRY;
	typedef struct _TABLE_EXT_BLOCK
	{
		DWORD dwItemAdditEntCount = 0;		// number of additional entries of the item to iteminfo group table
		DWORD dwItemGroupAdditEntCount = 0;	// number of additional entries of the item info group table
		DWORD dwItemTableAdditEntCount = 0;	// number of additional entries of the iteminfo table
	} TABLE_EXT_BLOCK;
	typedef BYTE CANDIDATE_TYPE;
	typedef struct _CANDIDATE0_DATA
	{
		BYTE bResValueType = 0;		// resource value type, specified as an index into the resource value type table
		WORD wEmbeddedLength = 0;	// embedded data length
		DWORD dwEmbeddedOffset = 0;	// offset of the embedded data in the embedded data block
	} CANDIDATE0_DATA;
	typedef struct _CANDIDATE1_DATA
	{
		BYTE bResValueType = 0;		// resource value type, specified as an index into the resource value type table
		WORD wSrcFile = 0;			// source file
		WORD wDataIndex = 0;		// index of the data item storing the data
		WORD wSectIndex = 0;		// section index of the Data Item Section storing the data
	} CANDIDATE1_DATA;

	// DataItem

	typedef struct _STORED_STRING_INFO
	{
		WORD wStringOffset = 0;	// string offset, relative to start of stored data
		WORD wStringLength = 0; // string length in bytes
	} STORED_STRING_INFO;
	typedef struct _STORED_BLOB_INFO
	{
		DWORD dwBlobOffset = 0;	// blob offset, relative to start of stored data
		DWORD dwBlobLength = 0;	// blob length in bytes
	} STORED_BLOB_INFO;

	// ReferencedFile

	typedef struct _REF_FOLDER_INFO
	{
		WORD wUnknown1 = -1;			// unknown, zero
		WORD wParentIndex = 0xFFFF;		// index of parent folder, 0xFFFF if no parent exists (root)
		WORD wFolderCount = 0;			// number of folders in this folder
		WORD wFirstFolderIndex = 0;		// index of first folder in this folder
		WORD wFileCount = 0;			// number of files in this folder
		WORD wFirstFileIndex = 0;		// index of first file in this folder
		WORD wFolderNameLength = 0;		// length of folder name in characters
		WORD wFolderFullPathLength = 0;	// length of full folder path
		DWORD dwFolderNameOffset = 0;	// offset of folder name in Unicode name block
	} REF_FOLDER_INFO;
	typedef struct _REF_FILE_INFO
	{
		WORD wUnknown1 = 0;				// unknown
		WORD wParentIndex = 0;			// index of parent folder
		WORD wFileFullPathLength = 0;	// length of full file path
		WORD wFileNameLength = 0;		// length of file name in characters
		DWORD dwFileNameOffset = 0;		// offset of file name in Unicode name block
	} REF_FILE_INFO;

	struct basic_sect
	{
		section &sect;
		basic_sect (section &s): sect (s) {}
		explicit operator section () { return sect; }
		SectionType type () const { return sect.type (); }
	};
	struct basic_sect_func
	{
		public:
		virtual bool valid () = 0;
		virtual void reset () = 0;
		virtual bool parse () = 0;
	};
	struct sect_pridesp; // PriDescriptor	PRI 描述符 - 包含文件整体结构和引用信息
	struct sect_hierasche; // HierarchicalSchema & HierarchicalSchemaEx	层次结构模式 - 定义资源命名空间和层次结构
	struct sect_decinfo; // DecisionInfo	决策信息 - 管理资源限定符和决策逻辑
	struct sect_resmap; // ResourceMap & ResourceMap2	资源映射 - 将资源项映射到具体候选值
	struct sect_dataitem; // DataItem	数据项 - 存储实际的资源数据
	struct sect_revmap; // ReverseMap	反向映射 - 提供从资源名到ID的映射
	struct sect_reffile; // ReferencedFile	引用文件 - 管理引用的外部文件
	struct sect_unknown; // Unknown	未知类型 - 处理未识别的节区

	template <typename SectionType> struct ref_sect
	{
		static_assert (std::is_base_of <basic_sect, SectionType>::value, "SectionType must derive from basic_sect");
		int index;
		ref_sect (int sect_index = -1): index (sect_index) {}
		using sect_type = SectionType;
		explicit operator int () { return index; }
		bool valid () const { return index != -1; }
		void reset () { index = -1; }
	};
	struct sect_pridesp: public basic_sect, public basic_sect_func
	{
		sect_pridesp (section &s): basic_sect (s)
		{
			if (s.type () != SectionType::PriDescriptor) throw std::exception ("Error: Section type error.");
		}
		struct
		{
			WORD wFlags = 0;				// 标志 / flags
			WORD wIncFileListIndex = -1;	// 包含文件列表节（Included File List）索引，若不存在则为 0xFFFF
			WORD wUnknown1 = -1;			// 未知，0
			WORD wHieraScheCount = 0;		// Hierarchical Schema 节数量
			WORD wDecInfoCount = 0;			// Decision Info 节数量
			WORD wResMapCount = 0;			// Resource Map 节数量
			WORD wResMapBegIndex = -1;		// 主资源映射（primary resource map）的节索引，若无则 0xFFFF
			WORD wRefFileCount = 0;			// Referenced File 节数量
			WORD wDataItemCount = 0;		// Data Item 节数量
			WORD wUnknown2 = -1;			// 未知，0
		} content;
		std::vector <ref_sect <sect_hierasche>> vec_ref_hs;
		std::vector <ref_sect <sect_decinfo>> vec_ref_deci;
		std::vector <ref_sect <sect_resmap>> vec_ref_rm;
		std::vector <ref_sect <sect_reffile>> vec_ref_rf;
		std::vector <ref_sect <sect_dataitem>> vec_ref_dati;
		ref_sect <sect_resmap> primary_resmap;
		bool valid () { return content.wUnknown1 == 0 && content.wUnknown2 == 0; }
		void reset ()
		{
			vec_ref_hs.clear ();	
			vec_ref_deci.clear ();
			vec_ref_rm.clear ();
			vec_ref_rf.clear ();	
			vec_ref_dati.clear ();	
			primary_resmap.reset ();
			content = {0, -1, -1, 0, 0, 0, -1, 0, 0, -1};
		}
		bool parse ()
		{
			reset ();
			sect.childst.seek ();
			DWORD dwContent = 0;
			sect.childst.ifile->Read (&content, sizeof (content), &dwContent);
			if (!valid ()) return false;
			if (content.wResMapBegIndex != 0xFFFF) primary_resmap.index = content.wResMapBegIndex;
			for (size_t i = 0; i < content.wHieraScheCount; i ++)
			{
				ref_sect <sect_hierasche> ref_sect_hs;
				UINT16 u16 = 0;
				sect.childst.ifile->Read (&u16, sizeof (u16), nullptr);
				ref_sect_hs.index = u16;
				vec_ref_hs.push_back (ref_sect_hs);
			}
			for (size_t i = 0; i < content.wDecInfoCount; i ++)
			{
				ref_sect <sect_decinfo> ref_sect_di;
				UINT16 u16 = 0;
				sect.childst.ifile->Read (&u16, sizeof (u16), nullptr);
				ref_sect_di.index = u16;
				vec_ref_deci.push_back (ref_sect_di);
			}
			for (size_t i = 0; i < content.wResMapCount; i ++)
			{
				ref_sect <sect_resmap> ref_sect_rm;
				UINT16 u16 = 0;
				sect.childst.ifile->Read (&u16, sizeof (u16), nullptr);
				ref_sect_rm.index = u16;
				vec_ref_rm.push_back (ref_sect_rm);
			}
			for (size_t i = 0; i < content.wRefFileCount; i ++)
			{
				ref_sect <sect_reffile> ref_sect_rf;
				UINT16 u16 = 0;
				sect.childst.ifile->Read (&u16, sizeof (u16), nullptr);
				ref_sect_rf.index = u16;
				vec_ref_rf.push_back (ref_sect_rf);
			}
			for (size_t i = 0; i < content.wDataItemCount; i ++)
			{
				ref_sect <sect_dataitem> ref_sect_di;
				UINT16 u16 = 0;
				sect.childst.ifile->Read (&u16, sizeof (u16), nullptr);
				ref_sect_di.index = u16;
				vec_ref_dati.push_back (ref_sect_di);
			}
			return true;
		}
	};
	struct sect_hierasche: public basic_sect, public basic_sect_func
	{
		struct
		{
			struct
			{
				WORD wUnknown1 = 0;					// 未知，1
				WORD wUniqRMNameLen = 0;			// 资源映射唯一名长度（字符数，含终止符）
				WORD wResMapNameLen = 0;			// 资源映射名称长度（字符数，含终止符）
				WORD wUnknown2 = -1;				// unknown, zero
			} part1;
			struct
			{
				// hname 标识符（仅 extended 存在）
				// hname identifier: only present in the extended Hierarchical Schema Section. 
				// Observed values are "[def_hnames] \0" and "[def_hnamesx] \0".
				CHAR szHNameExt [16] = {0};
			} part2;
			struct
			{
				HSCHEMA_VERSION_INFO verSchema;		// 层次化 schema 版本信息
			} part3;
			struct
			{
				std::wstring swUniqueRMName = L"";	// 资源映射的唯一名（unique name）
				std::wstring swResMapName = L"";	// name of resource map
				WORD wUnknown3 = -1;				// unknown, zero
				WORD wMaxFullPathLength = 0;		// length of longest full path of all resource names
				WORD wUnknown3_5 = -1;				// unknown, zero
				DWORD dwResNameCount = 0;			// number of resource names, usually number of scopes + items
				DWORD dwScopeCount = 0;				// number of scopes
				DWORD dwItemsCount = 0;				// number of items
				DWORD dwUniNameLemgth = 0;			// length of Unicode name block
				DWORD dwUnknown4 = 0;				// unknown
													// unknown at 70 + ?: only present in the extended Hierarchical 
													// Schema Section and if hname identifier is "[def_hnamesx] \0".
				DWORD dwUnknown5 = 0;
				void *get_buf_first_dir () { return &wUnknown3; }
				size_t get_buf_size_of ()
				{
					return sizeof (wUnknown3) + sizeof (wMaxFullPathLength) + sizeof (wUnknown3_5) +
						sizeof (dwScopeCount) + sizeof (dwItemsCount) + sizeof (dwResNameCount) +
						sizeof (dwUniNameLemgth) + sizeof (dwUnknown4) +
						sizeof (dwUnknown5);
				}
			} part4;
		} content;
		BOOL ex = FALSE;
		BOOL exHName = FALSE;
		std::vector <SCOPE_ITEM_INFO> vec_scope_and_items;
		std::vector <SCOPE_EX_INFO> vec_scope_ex;
		std::vector <ITEM_INDEX> vec_item_index;
		std::vector <RES_MAP_SCOPE::ScopePtr> vec_scopes;
		std::vector <RES_MAP_ITEM::ItemPtr> vec_items;
		sect_hierasche (section &s): basic_sect (s)
		{
			if (s.type () != SectionType::HierarchicalSchema && s.type () != SectionType::HierarchicalSchemaEx) throw std::exception ("Error: Section type error.");
			if (s.type () == SectionType::HierarchicalSchemaEx) ex = TRUE;
		}
		bool valid ()
		{
			return content.part1.wUnknown1 == 1 &&
				content.part1.wUnknown2 == 0 &&
				content.part3.verSchema.dwUnknown1 == 0 &&
				content.part4.wUnknown3 == 0 &&
				content.part4.wUnknown3_5 == 0 &&
				content.part4.dwResNameCount == content.part3.verSchema.dwScopeCount + content.part3.verSchema.dwItemCount &&
				content.part4.dwScopeCount == content.part3.verSchema.dwScopeCount &&
				content.part4.dwItemsCount == content.part3.verSchema.dwItemCount &&
				content.part4.dwUniNameLemgth == content.part3.verSchema.dwItemCount &&
				content.part4.swUniqueRMName.length () == content.part1.wUniqRMNameLen &&
				content.part4.swResMapName.length () == content.part1.wResMapNameLen - 1;
		}
		void reset ()
		{
			vec_scope_and_items.clear ();
			vec_scope_ex.clear ();
			vec_item_index.clear ();
			vec_scopes.clear ();
			vec_items.clear ();
			ZeroMemory (&content.part1, sizeof (content.part1));
			content.part1.wUnknown2 = -1;
			ZeroMemory (&content.part2, sizeof (content.part2));
			ZeroMemory (&content.part3, sizeof (content.part3));
			content.part3.verSchema.dwUnknown1 = -1;
			content.part4.swUniqueRMName = L"";
			content.part4.swResMapName = L"";
			ZeroMemory (content.part4.get_buf_first_dir (), content.part4.get_buf_size_of ());
			content.part4.wUnknown3 = -1;
			content.part4.wUnknown3_5 = -1;
		}
		bool parse ()
		{
			reset ();
			sect.childst.seek ();
			sect.childst.ifile->Read (&content.part1, sizeof (content.part1), nullptr);
			if (ex)
			{
				sect.childst.ifile->Read (&content.part2, sizeof (content.part2), nullptr);
				if (pri_sectid (content.part2.szHNameExt, 16).equals ("[def_hnamesx]")) exHName = true;
				else if (pri_sectid (content.part2.szHNameExt, 16).equals ("[def_hnames]")) exHName = false;
				else return false;
			}
			else exHName = false;
		#define readfromistream(_variable_) sect.childst.ifile->Read (&(_variable_), sizeof (_variable_), nullptr)
			sect.childst.ifile->Read (&content.part3, sizeof (content.part3), nullptr);
			ReadStringEndwithNull (sect.childst.ifile, content.part4.swUniqueRMName);
			ReadStringEndwithNull (sect.childst.ifile, content.part4.swResMapName);
			readfromistream (content.part4.wUnknown3);
			readfromistream (content.part4.wMaxFullPathLength);
			readfromistream (content.part4.wUnknown3_5);
			readfromistream (content.part4.dwResNameCount);
			readfromistream (content.part4.dwScopeCount);
			readfromistream (content.part4.dwItemsCount);
			readfromistream (content.part4.dwUniNameLemgth);
			readfromistream (content.part4.dwUnknown4);
			if (exHName) readfromistream (content.part4.dwUnknown5);
			size_t silen = content.part3.verSchema.dwScopeCount + content.part3.verSchema.dwItemCount;
			for (size_t cnt = 0; cnt < silen; cnt ++)
			{
				SCOPE_ITEM_INFO siinfo;
				readfromistream (siinfo);
				vec_scope_and_items.push_back (siinfo);
			}
			for (size_t cnt = 0; cnt < content.part3.verSchema.dwScopeCount; cnt ++)
			{
				SCOPE_EX_INFO sxinfo;
				readfromistream (sxinfo);
				vec_scope_ex.push_back (sxinfo);
			}
			for (size_t cnt = 0; cnt < content.part3.verSchema.dwItemCount; cnt ++)
			{
				ITEM_INDEX iindex = 0;
				readfromistream (iindex);
				vec_item_index.push_back (iindex);
			}
			ULARGE_INTEGER pos = {};
			sect.childst.ifile->Seek (lint (0), STREAM_SEEK_CUR, &pos);
			ulint posw = pos; // unicodeDataOffset
			ulint posa = ulint (pos) + ulint (content.part4.dwUniNameLemgth); // asciiDataOffset
			vec_scopes.resize (content.part3.verSchema.dwScopeCount);
			vec_items.resize (content.part3.verSchema.dwItemCount);
			for (size_t i = 0; i < silen; i ++)
			{
				lint pos;
				auto &sai = vec_scope_and_items [i];
				if (sai.name_in_ascii ()) pos = posa + lint (sai.name_offset ());
				else pos = posw + lint ((LONGLONG)sai.name_offset () * 2);
				sect.childst.ifile->Seek (pos, STREAM_SEEK_SET, nullptr);
				std::wstring name = L"";
				if (sai.wFullPathLength != 0)
				{
					if (sai.name_in_ascii ()) name = StringToWString (ReadStringEndwithNullA (sect.childst.ifile));
					else name = ReadStringEndwithNullW (sect.childst.ifile);
				}
				else name = L"";
				auto index = sai.index ();
				if (sai.is_scope ())
				{
					auto it = vec_scopes.begin () + index;
					if (*it != nullptr) throw std::exception ("Error: invalid scope data in HierarchicalSchema(-ex) Section");
					else *it = RES_MAP_SCOPE::make (index, nullptr, name);
				}
				else
				{
					auto it = vec_items.begin () + index;
					if (*it != nullptr) throw std::exception ("Error: invalid item data in HierarchicalSchema(-ex) Section");
					else *it = RES_MAP_ITEM::make (index, nullptr, name);
				}
			}
			for (size_t i = 0; i < silen; i ++)
			{
				auto &sai = vec_scope_and_items [i];
				ITEM_INDEX index = sai.index () & 0xFFFF;
				ITEM_INDEX parent = vec_scope_and_items [vec_scope_and_items [i].wParentScopeIndex].index () & 0xFFFF;
				if (parent != 0xFFFF)
				{
					if (sai.is_scope ())
					{
						if (parent != index)
						{
							auto it = vec_scopes.begin () + index;
							(*it)->change_parent (*(vec_scopes.begin () + parent));
						}
					}
					else
					{
						auto it = vec_items.begin () + index;
						(*it)->change_parent (*(vec_scopes.begin () + parent));
					}
				}
			}
			for (size_t i = 0; i < content.part3.verSchema.dwScopeCount; i ++)
			{
				auto &s = vec_scope_ex [i];
				auto &sp = vec_scopes [i];
				for (size_t j = 0; j < s.wChildCount; j ++)
				{
					auto sai = vec_scope_and_items [s.wFirstChild + j];
					if (sai.is_scope ()) sp->vecChild.push_back (vec_scopes [(UINT16)(sai.index () & 0xFFFF)]);
					else sp->vecChild.push_back (vec_items [(UINT16)(sai.index () & 0xFFFF)]);
				}
			}
		#ifdef readfromistream
		#undef readfromistream
		#endif
			return valid ();
		}
	};
	struct sect_decinfo: public basic_sect, public basic_sect_func
	{
		sect_decinfo (section &s): basic_sect (s)
		{
			if (s.type () != SectionType::DecisionInfo) throw std::exception ("Error: Section type error.");
		}
		struct
		{
			WORD wDistQualiCount = 0;		// 不同的 distinct qualifiers 数量 / number of distinct qualifiers
			WORD wQualifierCount = 0;		// qualifiers 数量
			WORD wQualSetsCount = 0;		// qualifier sets 数量
			WORD wDecisionCount = 0;		// decisions 数量 / number of decisions
			WORD wEntriesCount = 0;			// index table 条目数 / number of entries in the index table
			WORD wQualiValueLength = 0;		// qualifier value block 长度（字符数） / length of qualifier value block in characters
		} content;
		std::vector <QUALIFIER_SET> vec_qua_set;
		std::vector <QUALIFIER> vec_qua;
		std::vector <DECISION> vec_dec;
		bool valid () 
		{ 
			return content.wDecisionCount ||
				content.wQualifierCount ||
				content.wDistQualiCount ||
				content.wQualSetsCount ||
				content.wEntriesCount ||
				content.wQualiValueLength ||
				vec_qua.size () ||
				vec_qua_set.size () ||
				vec_dec.size () ||
				0;
		}
		void reset ()
		{
			vec_qua.clear ();
			vec_qua_set.clear ();
			vec_dec.clear ();
			ZeroMemory (&content, sizeof (content));
		}
		bool parse ()
		{
			reset ();
			IStream *&fp = sect.childst.ifile;
			sect.childst.seek ();
			fp->Read (&content, sizeof (content), nullptr);
			std::vector <DECISION_INFO> vec_dec_info;
			std::vector <QUALIFIER_SET_INFO> vec_qua_set_info;
			std::vector <QUALIFIER_INFO> vec_qua_info;
			std::vector <DISTINCE_QUALIFIER_INFO> vec_dis_qua_info;
		#define counter(_count_, _variable_) for (size_t _variable_ = 0, _counter_##_variable_##_total_ = _count_; _variable_ < _counter_##_variable_##_total_; _variable_ ++)
			counter (content.wDecisionCount, i)
			{
				DECISION_INFO dec;
				fp->Read (&dec, sizeof (dec), nullptr);
				vec_dec_info.push_back (dec);
			}
			counter (content.wQualSetsCount, i)
			{
				QUALIFIER_SET_INFO qua_set;
				fp->Read (&qua_set, sizeof (qua_set), nullptr);
				vec_qua_set_info.push_back (qua_set);
			}
			counter (content.wQualifierCount, i)
			{
				QUALIFIER_INFO qua;
				fp->Read (&qua, sizeof (qua), nullptr);
				if (qua.wUnknown1 != 0) throw std::exception ("Error: invalid data in DecisionInfo Section");
				vec_qua_info.push_back (qua);
			}
			counter (content.wDistQualiCount, i)
			{
				DISTINCE_QUALIFIER_INFO dist;
				fp->Read (&dist, sizeof (dist), nullptr);
				vec_dis_qua_info.push_back (dist);
			}
			std::vector <ITEM_INDEX> indexs;
			counter (content.wEntriesCount, i)
			{
				ITEM_INDEX index;
				fp->Read (&index, sizeof (index), nullptr);
				indexs.push_back (index);
			}
			ULARGE_INTEGER ul;
			fp->Seek (lint (0), STREAM_SEEK_CUR, &ul);
			ulint dspos = ul;
			counter (content.wQualifierCount, i)
			{
				auto dinfo = vec_dis_qua_info [vec_qua_info [i].wDistQualiIndex];
				auto qinfo = vec_qua_info [i];
				fp->Seek (lint (dspos + ulint (dinfo.wQualiValueOffset * 2)), STREAM_SEEK_SET, nullptr);
				std::wstring value = ReadStringEndwithNullW (fp);
				QUALIFIER qual (i, (QUALIFIER_TYPE)dinfo.wQualiType, qinfo.wPriority, (double)qinfo.wFallbackScore * 0.001, value);
				vec_qua.push_back (qual);
			}
			counter (content.wQualSetsCount, i)
			{
				std::vector <QUALIFIER> quals;
				auto qset = vec_qua_set_info [i];
				counter (qset.wQualiSetsCount, j)
				{
					auto &ind = indexs [qset.wFirstQualiIndex + j];
					auto &qual = vec_qua [ind];
					quals.push_back (qual);
				}
				vec_qua_set.emplace_back (QUALIFIER_SET (i, quals));
			}
			counter (content.wDecisionCount, i)
			{
				auto &dec = vec_dec_info [i];
				std::vector <QUALIFIER_SET> qsets;
				counter (dec.wQualiSetsCount, j)
				{
					auto &ind = indexs [dec.wFirstQualiIndex + j];
					auto qset = qsets [ind];
					qsets.emplace_back (qset);
				}
				vec_dec.push_back (DECISION (i, qsets));
			}
		#ifdef counter
		#undef counter
		#endif
			return valid ();
		}
	};
	struct sect_resmap: public basic_sect
	{
		sect_resmap (section &s): basic_sect (s)
		{
			if (s.type () != SectionType::ResourceMap && s.type () != SectionType::ResourceMap2) throw std::exception ("Error: Section type error.");
			if (s.type () == SectionType::ResourceMap2) ver2 = true;
			else ver2 = false;
		}
		struct
		{
			WORD wEnvRefLength = 0;			// length of environment references block
			WORD wRefCount = 0;				// number of references in environment references block
			WORD wHSSectIndex = 0;			// section index of Hierarchical Schema Section
			WORD wHSRefLength = 0;			// length of hierarchical schema reference block
			WORD wDecInfSectIndex = 0;		// section index of Decision Info Section
			WORD wResTypeEntCount = 0;		// number of entries in resource value type table
			WORD wItemEntCount = 0;			// number of entries in item to iteminfo group table
			WORD wItemGroupEntCount = 0;	// number of entries in iteminfo group table
			DWORD dwItemTableEntCount = 0;	// number of entries in iteminfo table
			DWORD dwCandidateCount = 0;		// number of candidates
			DWORD dwEmbededDataCount = 0;	// length of embedded data bloc
			DWORD dwTableExtCount = 0;		// length of table extension block
		} content;
		BOOL ver2 = FALSE;
		bool valid ()
		{
			UINT64 *p = (UINT64 *)&content;
			bool res = false;
			size_t len = sizeof (content) / sizeof (UINT64);
			for (size_t i = 0; i < len; i ++) res = res || p [i];
			return res;
		}
		void reset ()
		{
			UINT64 *p = (UINT64 *)&content;
			bool res = false;
			size_t len = sizeof (content) / sizeof (UINT64);
			for (size_t i = 0; i < len; i ++) p [i] = 0;
		}
		bool parse ()
		{
			reset ();
			auto &fp = sect.childst.ifile;
			sect.childst.seek ();
			ulint sectpos = 0;
			fp->Seek (lint (0), STREAM_SEEK_SET, sectpos.ptr_union ());

		}
	};
	struct sect_dataitem: public basic_sect
	{
		sect_dataitem (section &s): basic_sect (s)
		{
			if (s.type () != SectionType::DataItem) throw std::exception ("Error: Section type error.");
		}
		struct
		{
			DWORD dwUnknown1 = -1;	// unknown, zero
			WORD wStrCount = 0;		// number of stored strings
			WORD wBlobCount = 0;	// number of stored blobs
			WORD wStoredLength = 0;	// total length of stored data
		} content;
	};
	struct sect_reffile: public basic_sect
	{
		sect_reffile (section &s): basic_sect (s)
		{
			if (s.type () != SectionType::ReferencedFile) throw std::exception ("Error: Section type error.");
		}
		struct
		{
			WORD wRootCount = 0;	// number of roots
			WORD wFolderCount = 0;	// number of folders
			WORD wFileCount = 0;	// number of folders
			WORD wUnknown1 = -1;	// unknown, zero
			DWORD dwNameLength = 0;	// length of Unicode name block in characters
		} content;
	};
	struct sect_revmap: public basic_sect
	{
		sect_revmap (section &s): basic_sect (s)
		{
			if (s.type () != SectionType::ReverseMap) throw std::exception ("Error: Section type error.");
		}
		struct
		{
			DWORD dwItemsNumber = 0;
			DWORD dwCheckCode = 0;
			std::vector <DWORD> adwMap;
			WORD wFullPathLength = 0;
			DWORD dwEntries = 0;
			DWORD dwScopes = 0;
			DWORD dwCheckItemsNumber = 0;
			DWORD dwUnicodeDataLength = 0;
			DWORD dwSkipPadding = 0;
			std::vector <SCOPE_ITEM_INFO> aobjScopes;
			std::vector <SCOPE_EX_INFO> aobjScopeExts;
			std::vector <ITEM_INDEX> awItemIndexs;
			LONG lDataOffsetW = 0;
			LONG lDataOffsetA = 0;
		} content;
	};
	struct sect_unknown: public basic_sect
	{
		sect_unknown (section &s): basic_sect (s) {}
		struct
		{
			DWORD dwLength = 0;
			std::vector <BYTE> abContents;
		} content;
	};
}
class prifile
{
	private:
	IStream *pfile = nullptr;
	head header; foot footer;
	std::vector <tocentry> toclist;
	std::vector <section> sectlist;
	public:
	bool close ()
	{
		header = head ();
		footer = foot ();
		toclist.clear ();
		sectlist.clear ();
		if (pfile) pfile->Release ();
		pfile = nullptr;
	}
	bool load (IStream *ifile)
	{
		close ();
		if (!ifile) return false;
		ifile->Seek (lint (0), STREAM_SEEK_SET, nullptr);
		DWORD dwhead = 0, dwfoot = 0;
		ifile->Read (&header, sizeof (header), &dwhead);
		if (!dwhead) return false;
		if (!header.valid ()) return false;
		ifile->Seek (lint (header.dwFileSize - 16), STREAM_SEEK_SET, nullptr);
		ifile->Read (&footer, sizeof (footer), &dwfoot);
		if (!dwfoot) return false;
		if (!footer.valid (header)) return false;
		pfile = ifile;
		inittoc ();
		initsect ();
		return true;
	} 
	void inittoc ()
	{
		toclist.clear ();
		pfile->Seek (lint (header.dwToCOffset), STREAM_SEEK_SET, nullptr);
		for (size_t i = 0; i < header.wSectCount; i ++)
		{
			tocentry toc;
			DWORD dwRead;
			pfile->Read (&toc, sizeof (toc), &dwRead);
			toclist.push_back (toc);
		}
	}
	void initsect ()
	{
		sectlist.clear ();
		for (size_t i = 0; i < header.wSectCount; i ++)
		{
			pfile->Seek (lint (header.dwSectStartOffset + toclist [i].dwSectOffset), STREAM_SEEK_SET, nullptr);
			section sect (pfile);
			DWORD dwHead = 0, dwFoot = 0;
			pfile->Read (&sect.head, sizeof (sect.head), &dwHead);
			pfile->Seek (lint (sect.head.dwLength - 16 - 24), STREAM_SEEK_CUR, nullptr);
			pfile->Read (&sect.foot, sizeof (sect.foot), &dwFoot);
			pfile->Seek (lint (32 - sect.head.dwLength), STREAM_SEEK_CUR, nullptr);
			ULARGE_INTEGER pos = {};
			pfile->Seek (lint (0), STREAM_SEEK_CUR, &pos);
			sect.childst.set (
				pos.QuadPart,
				sect.head.dwLength - 16 - 24
			);
			sectlist.push_back (sect);
		}
	}
};