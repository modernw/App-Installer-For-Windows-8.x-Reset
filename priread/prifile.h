#pragma once
#include <atlbase.h>
#include <string>
#include <vector>
#include <functional>

struct destruct
{
	std::function <void ()> endtask = nullptr;
	destruct (std::function <void ()> init): endtask (init) {}
	~destruct () { if (endtask) endtask (); }
};
typedef struct LargeInt
{
	LARGE_INTEGER val;
	LargeInt (LONGLONG v) { val.QuadPart = v; }
	operator LARGE_INTEGER() const { return val; }
} lint;

struct head
{
	CHAR szMagic [8] = {};
	WORD wPlaceholder1 = -1, // 0
		wPlaceholder2 = 0; // 1
	DWORD dwFileSize = 0,
		dwToCOffset = 0,
		dwSectStartOffset = 0;
	WORD wSectCount = 0,
		wPlaceholder3 = 0; // 0xFFFF
	DWORD dwPlaceholder4 = -1; // 0
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
	DWORD dwChkCode = 0, // 0xDEFFFADE
		dwTotalFileSize = 0;
	CHAR szMagic [8] = {};
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
	CHAR szIdentifier [16] = {0};
	WORD wFlags = 0;
	WORD wSectFlags = 0;
	DWORD dwSectQualifier = 0;
	DWORD dwSectOffset = 0;
	DWORD dwSectLength = 0;
};
struct section_header
{
	CHAR szIdentifier [16] = {0};
	DWORD dwQualifier = 0;
	WORD wFlags = 0;
	DWORD dwLength = 0;
	DWORD dwPlaceholder1 = -1; // 0
	bool valid () const { return szIdentifier [0] && !dwPlaceholder1; }
};
struct section_check
{
	DWORD dwChkCode = 0, // 0xDEF5FADE
		dwSectLength = 0;
	bool valid (const section_header &h) const { return dwChkCode == 0xDEF5FADE && dwSectLength == h.dwLength; }
};
struct substream
{
	IStream *&ifile;
	UINT64 offset = 0,
		size = 0;
	substream (IStream *&ifile, UINT64 ofs = 0, UINT64 siz = 0): ifile (ifile), offset (ofs), size (siz) {}
};
struct section
{
	section_header head;
	section_check foot;
	substream childst;
};
class prifile
{
	private:
	IStream *pfile = nullptr;
	head header; foot footer;
	std::vector <tocentry> toclist;
	public:
	bool load (IStream *ifile)
	{
		if (!ifile) return false;
		header = head ();
		footer = foot ();
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
		bool res = inittoc ();
		return true;
	} 
	bool inittoc ()
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
};