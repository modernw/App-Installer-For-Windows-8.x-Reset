#include <Windows.h>
#include <set>
#include <msclr/marshal_cppstd.h>
#include <ShObjIdl.h>
#include <MsHTML.h>
#include <ExDisp.h>
#include <atlbase.h>
#include "cmdargs.h"
#include "themeinfo.h"
#include "mpstr.h"
#include "initfile.h"
#include "resource.h"
#include "vemani.h"
#include "ieshell.h"
using namespace System;
using namespace System::Runtime::InteropServices;

#ifdef _DEBUG
#define DEBUGMODE true
#else
#define DEBUGMODE false
#endif

LPCWSTR g_lpAppId = L"Microsoft.DesktopAppInstaller";
auto &g_identity = g_lpAppId;
auto &m_idenName = g_lpAppId;
struct iconhandle
{
	HICON hIcon = nullptr;
	iconhandle (HICON hIcon = nullptr): hIcon (hIcon) {}
	~iconhandle () { try { if (hIcon) DestroyIcon (hIcon); hIcon = nullptr; } catch (...) {} }
};
iconhandle g_hIconMain (LoadRCIcon (IDI_ICON_MAIN));
initfile g_initfile (CombinePath (GetProgramRootDirectoryW (), L"config.ini"));
ref class MainHtmlWnd;
msclr::gcroot <MainHtmlWnd ^> g_mainwnd;
vemanifest g_vemani (
	IsFileExists (CombinePath (GetProgramRootDirectoryW (), L"VisualElementsManifest.xml")) ?
	CombinePath (GetProgramRootDirectoryW (), L"VisualElementsManifest.xml") :
	CombinePath (GetProgramRootDirectoryW (), L"AppInstaller.VisualElementsManifest.xml")
);
resxmldoc g_scaleres (
	IsFileExists (CombinePath (GetProgramRootDirectoryW (), L"VisualElements\\scale.xml")) ?
	CombinePath (GetProgramRootDirectoryW (), L"VisualElements\\scale.xml") :
	CombinePath (GetProgramRootDirectoryW (), L"VisualElementsManifest.xml")
);

HRESULT GetWebBrowser2Interface (System::Windows::Forms::WebBrowser ^fwb, IWebBrowser2 **output)
{
	if (fwb == nullptr || output == nullptr) return E_INVALIDARG;
	*output = nullptr;
	Object ^activeX = fwb->ActiveXInstance;
	if (activeX == nullptr) return E_FAIL;
	IntPtr pUnk = Marshal::GetIUnknownForObject (activeX);
	if (pUnk == IntPtr::Zero) return E_FAIL;
	HRESULT hr = ((IUnknown *)pUnk.ToPointer ())->QueryInterface (IID_IWebBrowser2, (void **)output);
	Marshal::Release (pUnk);
	return hr;
}

public ref class SplashForm: public System::Windows::Forms::Form
{
	public:
	using PictureBox = System::Windows::Forms::PictureBox;
	using Timer = System::Windows::Forms::Timer;
	private:
	PictureBox ^picbox;
	Timer ^timer;
	double opastep = 0.05;
	void InitForm ()
	{
		this->DoubleBuffered = true;
		InitializeComponent ();
		this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
		this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
		this->ShowInTaskbar = false;
		this->AllowTransparency = true;
		this->Opacity = 1.0;
	}
	void InitializeComponent ()
	{
		double dDpi = GetDPI () * 0.01;
		this->picbox = gcnew System::Windows::Forms::PictureBox ();
		this->picbox->Size = System::Drawing::Size (620 * dDpi, 300 * dDpi);
		this->picbox->BackColor = System::Drawing::Color::Transparent;
		picbox->Anchor = System::Windows::Forms::AnchorStyles::None;
		picbox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
	}
	void OnFadeTimer (Object ^sender, EventArgs ^e)
	{
		auto fadeTimer = timer;
		auto opacityStep = opastep;
		if (this->Opacity > 0)
		{
			this->Opacity -= opacityStep;
		}
		else
		{
			fadeTimer->Stop ();
			this->Close ();
		}
	}
	void OnLoad (Object ^sender, EventArgs ^e)
	{
		this->ChangePosAndSize ();
		this->Visible = true;
	}
	void OnResize (Object ^sender, EventArgs ^e)
	{
		if (IsHandleCreated && picbox->IsHandleCreated)
		{
			Drawing::Size sz = this->ClientSize;
			this->picbox->Location = Drawing::Point (
				(sz.Width - picbox->Width) * 0.5,
				(sz.Height - picbox->Height) * 0.5
			);
		}
	}
	void OnResizeOwner (Object ^sender, EventArgs ^e) { this->ChangePosAndSize (); }
	void OnLocationChangedOwner (Object ^sender, EventArgs ^e) { this->ChangePosAndSize (); }
	protected:
	virtual void OnHandleCreated (EventArgs^ e) override
	{
		Form::OnHandleCreated (e);
		if (Environment::OSVersion->Version->Major >= 6)
		{
			INT mr = 0;
			MARGINS margins = {mr, mr, mr, mr};
			HRESULT hr = DwmExtendFrameIntoClientArea ((HWND)this->Handle.ToPointer (), &margins);
		}
	}
	public:
	SplashForm (System::String ^imgpath, System::Drawing::Color backcolor, System::Windows::Forms::Form ^owner)
	{
		if (owner != nullptr) this->Owner = owner;
		InitForm ();
		std::wstring filefullpath = MPStringToStdW (imgpath);
		if (filefullpath.find (L'%') != filefullpath.npos) filefullpath = ProcessEnvVars (filefullpath);
		filefullpath = GetFullPathName (imgpath ? MPStringToStdW (imgpath) : L"");
		try
		{
			auto img = System::Drawing::Image::FromFile (gcnew System::String (filefullpath.c_str ()));
			if (img != nullptr) picbox->Image = img;
		}
		catch (...) { }
		if (backcolor != Drawing::Color::Transparent)
		{
			picbox->BackColor = backcolor;
			this->BackColor = backcolor;
		}
		if (this->Owner != nullptr)
		{
			this->Owner->Resize += gcnew System::EventHandler (this, &SplashForm::OnResizeOwner);
			this->Owner->LocationChanged += gcnew System::EventHandler (this, &SplashForm::OnLocationChangedOwner);
		}
		this->Controls->Add (picbox);
		this->Resize += gcnew EventHandler (this, &SplashForm::OnResize);
		timer = gcnew System::Windows::Forms::Timer ();
		timer->Interval = 15;
		timer->Tick += gcnew System::EventHandler (this, &SplashForm::OnFadeTimer);
		this->Load += gcnew EventHandler (this, &SplashForm::OnLoad);
	}
	void ChangePosAndSize ()
	{
		if (this->Owner && this->Owner->IsHandleCreated)
		{
			this->Owner->Update ();
			System::Drawing::Point pt = this->Owner->PointToScreen (this->Owner->ClientRectangle.Location);
			this->Location = pt;
			this->Size = this->Owner->ClientSize;
		}
		else if (this->Parent && this->Parent->IsHandleCreated)
		{
			this->Parent->Update ();
			System::Drawing::Point pt = this->Parent->PointToScreen (this->Parent->ClientRectangle.Location);
			this->Location = pt;
			this->Size = this->Parent->ClientSize;
		}
		if (IsHandleCreated && picbox->IsHandleCreated)
		{
			Drawing::Size sz = this->ClientSize;
			this->picbox->Location = Drawing::Point (
				(sz.Width - picbox->Width) * 0.5,
				(sz.Height - picbox->Height) * 0.5
			);
		}
	}
	void SetSplashImage (System::Drawing::Image ^img) { if (picbox) picbox->Image = img; }
	void SetSplashImage (System::String ^imgpath) { try { SetSplashImage (System::Drawing::Image::FromFile (imgpath)); } catch (...) {} }
	void SetSplashImage (const std::wstring &imgpath) { SetSplashImage (CStringToMPString (imgpath)); }
	void SetSplashBackgroundColor (System::Drawing::Color color) { picbox->BackColor = color; this->BackColor = color; }
	// 渐变消失
	void FadeOut () { timer->Start (); }
	// 立即消失
	void FadeAway () { this->Visible = false; this->Close (); }
	~SplashForm ()
	{
		if (this->Owner != nullptr)
		{
			this->Owner->Resize -= gcnew System::EventHandler (this, &SplashForm::OnResizeOwner);
			this->Owner->LocationChanged -= gcnew System::EventHandler (this, &SplashForm::OnLocationChangedOwner);
		}
	}
};

[ComVisible (true)]
public ref class MainHtmlWnd: public System::Windows::Forms::Form
{
	public:
	using WebBrowser = System::Windows::Forms::WebBrowser;
	private:
	WebBrowser ^webui;
	SplashForm ^splash;
	public:
	[ComVisible (true)]
	ref class IBridge
	{
		private:
		MainHtmlWnd ^wndinst = nullptr;
		public:
		IBridge (MainHtmlWnd ^wnd): wndinst (wnd) {}
		ref class _I_System
		{
			private:
			MainHtmlWnd ^wndinst = nullptr;
			public:
			ref class _I_UI
			{
				private:
				MainHtmlWnd ^wndinst = nullptr;
				public:
				_I_UI (MainHtmlWnd ^wnd): wndinst (wnd) {}
				property int DPIPercent { int get () { return GetDPI (); }}
				property double DPI { double get () { return DPIPercent * 0.01; }}
				void showSplash () { wndinst->SplashScreen->Show (); }
				void fadeAwaySplash () { wndinst->SplashScreen->FadeAway (); }
				void fadeOutSplash () { wndinst->SplashScreen->FadeOut (); }
			};
			private:
			_I_UI ^ui = gcnew _I_UI (wndinst);
			public:
			_I_System (MainHtmlWnd ^wnd): wndinst (wnd) {}
			property _I_UI ^UI { _I_UI ^get () { return ui; } }
		};
		ref class _I_IEFrame
		{
			private:
			MainHtmlWnd ^wndinst = nullptr;
			public:
			_I_IEFrame (MainHtmlWnd ^wnd): wndinst (wnd) {}
			property int Scale
			{
				int get () { return wndinst->PageScale; }
				void set (int value) { return wndinst->PageScale = value; }
			}
			property int Version { int get () { return GetInternetExplorerVersionMajor (); }}
		};
		ref class _I_Storage
		{
			private:
			MainHtmlWnd ^wndinst = nullptr;
			public:
			_I_Storage (MainHtmlWnd ^wnd): wndinst (wnd) {}
			ref class Path
			{

			};
		};
		ref class _I_String
		{
			public:
			ref class _I_NString 
			{
				public:
				bool NEquals (String ^l, String ^r) { return IsNormalizeStringEquals (MPStringToPtrW (l), MPStringToPtrW (r)); }
				bool Empty (String ^l) { return IsNormalizeStringEmpty (MPStringToStdW (l)); }
				int Compare (String ^l, String ^r) { return NormalizeStringCompare (MPStringToPtrW (l), MPStringToPtrW (r)); }
				int Length (String ^l) { return GetNormalizeStringLength (MPStringToStdW (l)); }
			};
			private:
			_I_NString ^nstr = gcnew _I_NString ();
			public:
			property _I_NString ^NString { _I_NString ^get () { return nstr; }}
			String ^Trim (String ^src)
			{
				std::wstring csrc = MPStringToStdW (src);
				return CStringToMPString (::StringTrim (csrc));
			}
			String ^ToLower (String ^src) { return CStringToMPString (StringToLower (MPStringToStdW (src))); }
			String ^ToUpper (String ^src) { return CStringToMPString (StringToUpper (MPStringToStdW (src))); }
		};
		private:
		_I_System ^system = gcnew _I_System (wndinst);
		_I_IEFrame ^ieframe = gcnew _I_IEFrame (wndinst);
		_I_Storage ^storage = gcnew _I_Storage (wndinst);
		_I_String ^str = gcnew _I_String ();
		public:
		property _I_System ^System { _I_System ^get () { return system; }}
		property _I_IEFrame ^IEFrame { _I_IEFrame ^get () { return ieframe; }}
		property _I_Storage ^Storage { _I_Storage ^get () { return storage; }}
		property _I_String ^String { _I_String ^get () { return str; }}
	};
	protected:
	property WebBrowser ^WebUI { WebBrowser ^get () { return this->webui; } }
	property SplashForm ^SplashScreen { SplashForm ^get () { return this->splash; } }
	property int DPIPercent { int get () { return GetDPI (); }}
	property double DPI { double get () { return DPIPercent * 0.01; }}
	void Init ()
	{
		this->Visible = false;
		this->webui = gcnew System::Windows::Forms::WebBrowser ();
		this->SuspendLayout ();
		this->webui->Dock = System::Windows::Forms::DockStyle::Fill;
		this->webui->IsWebBrowserContextMenuEnabled = DEBUGMODE;
		this->webui->AllowWebBrowserDrop = false;
		this->Controls->Add (this->webui);
		if (g_hIconMain.hIcon)
		{
			try { this->Icon = System::Drawing::Icon::FromHandle (IntPtr (g_hIconMain.hIcon)); }
			catch (...) {}
		}
		unsigned ww = 0, wh = 0;
		auto &ini = g_initfile;
		auto setsect = ini ["Settings"];
		if (setsect [L"SavePosAndSizeBeforeCancel"].read_bool ())
		{
			ww = setsect [L"LastWidth"].read_uint (setsect [L"DefaultWidth"].read_uint (rcInt (IDS_DEFAULTWIDTH)));
			wh = setsect [L"LastHeight"].read_uint (setsect [L"DefaultHeight"].read_uint (rcInt (IDS_DEFAULTHEIGHT)));
		}
		else
		{
			ww = setsect [L"DefaultWidth"].read_uint (rcInt (IDS_DEFAULTWIDTH));
			wh = setsect [L"DefaultHeight"].read_uint (rcInt (IDS_DEFAULTHEIGHT));
		}
		this->MinimumSize = System::Drawing::Size (
			setsect [L"MinimumWidth"].read_uint (rcInt (IDS_MINWIDTH)) * DPI, 
			setsect [L"MinimumHeight"].read_uint (rcInt (IDS_MINHIEHGT)) * DPI
		);
		this->ClientSize = System::Drawing::Size (ww * DPI, wh * DPI);
		this->WindowState = (System::Windows::Forms::FormWindowState)setsect [L"LastWndState"].read_int ((int)System::Windows::Forms::FormWindowState::Normal);
		this->Text = rcString (IDS_WINTITLE);
		this->ResumeLayout (false);
		webui->ObjectForScripting = gcnew IBridge (this);
		this->webui->DocumentCompleted += gcnew System::Windows::Forms::WebBrowserDocumentCompletedEventHandler (this, &MainHtmlWnd::OnDocumentCompleted);
		this->webui->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler (this, &MainHtmlWnd::OnPreviewKeyDown_WebBrowser);
		this->Resize += gcnew System::EventHandler (this, &MainHtmlWnd::OnResize);
		this->Load += gcnew EventHandler (this, &MainHtmlWnd::OnCreate);
		this->ResizeEnd += gcnew EventHandler (this, &MainHtmlWnd::OnResizeEnd);
	}
	void OnDocumentCompleted (Object ^sender, System::Windows::Forms::WebBrowserDocumentCompletedEventArgs ^e)
	{
		if (e->Url->ToString () == webui->Url->ToString ())
		{

			ExecScript ("Windows.UI.DPI.mode = 1");
			ExecScript ("Bridge.Frame.scale = Bridge.Frame.scale * Bridge.UI.dpi");
			splash->FadeOut ();
		}
	}
	void OnCreate (System::Object ^sender, System::EventArgs ^e)
	{
		splash->Owner = this;
		splash->ChangePosAndSize ();
		splash->Show ();
		splash->Update ();
		webui->Navigate (CStringToMPString (CombinePath (GetProgramRootDirectoryW (), L"html\\install.html")));
	}
	void OnPreviewKeyDown_WebBrowser (System::Object ^sender, System::Windows::Forms::PreviewKeyDownEventArgs ^e)
	{
		if (e->KeyCode == System::Windows::Forms::Keys::F5 || (e->KeyCode == System::Windows::Forms::Keys::R && e->Control))
			e->IsInputKey = true;
	}
	void OnResize (Object ^sender, EventArgs ^e)
	{
	}
	void OnResizeEnd (Object ^sender, EventArgs ^e)
	{

	}


	public:
	MainHtmlWnd ()
	{
		splash = gcnew SplashForm (
			gcnew String (g_vemani.splash_screen_image (L"App").c_str ()),
			StringToColor (gcnew String (g_vemani.splash_screen_backgroundcolor (L"App").c_str ())),
			this
		);
		System::Windows::Forms::Application::DoEvents ();
		Init ();
	}
	Object ^CallScriptFunction (String ^lpFuncName, ... array <Object ^> ^alpParams)
	{
		try { return this->webui->Document->InvokeScript (lpFuncName, alpParams); }
		catch (Exception ^e) {}
		return nullptr;
	}
	Object ^CallScriptFunction (String ^lpScriptName)
	{
		try { return this->webui->Document->InvokeScript (lpScriptName); }
		catch (Exception ^e) {}
		return nullptr;
	}
	Object ^InvokeCallScriptFunction (String ^lpFuncName, ... array <Object ^> ^alpParams)
	{
		try
		{
			if (this->InvokeRequired) return (Object ^)this->Invoke (gcnew Func <String ^, array <Object ^> ^, Object ^> (this, &MainHtmlWnd::CallScriptFunction), lpFuncName, alpParams);
			else return CallScriptFunction (lpFuncName, alpParams);
		}
		catch (Exception ^e) {}
		return nullptr;
	}
	Object ^InvokeCallScriptFunction (String ^lpScriptName)
	{
		try
		{
			if (this->InvokeRequired) return (Object ^)this->Invoke (gcnew Func <String ^, Object ^> (this, &MainHtmlWnd::CallScriptFunction), lpScriptName);
			else return CallScriptFunction (lpScriptName);
		}
		catch (Exception ^e) {}
		return nullptr;
	}
	Object ^ExecScript (... array <Object ^> ^alpScript) { return InvokeCallScriptFunction ("eval", alpScript); }
	property int PageScale
	{
		int get ()
		{
			CComPtr <IWebBrowser2> web2;
			HRESULT hr = GetWebBrowser2Interface (webui, &web2);
			if (FAILED (hr)) return 0;
			VARIANT v;
			VariantInit (&v);
			hr = web2->ExecWB (OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DODEFAULT, nullptr, &v);
			if (FAILED (hr) || v.vt != VT_I4) return 0;
			int val = v.lVal;
			VariantClear (&v);
			return val;
		}
		void set (int value)
		{
			CComPtr <IWebBrowser2> web2;
			HRESULT hr = GetWebBrowser2Interface (webui, &web2);
			if (FAILED (hr)) return;
			VARIANT v;
			VariantInit (&v);
			v.vt = VT_I4;
			v.lVal = value;
			web2->ExecWB (OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &v, nullptr);
		}
	}
};
using MainWnd = MainHtmlWnd;
std::vector <std::wstring> LoadFileListW (const std::wstring &filePath)
{
	std::vector <std::wstring> result;
	HANDLE hFile = CreateFileW (
		filePath.c_str (),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (hFile == INVALID_HANDLE_VALUE) return result;
	LARGE_INTEGER fileSize {};
	if (!GetFileSizeEx (hFile, &fileSize) || fileSize.QuadPart == 0)
	{
		CloseHandle (hFile);
		return result;
	}
	DWORD size = static_cast <DWORD> (fileSize.QuadPart);
	std::vector <WCHAR> buf;
	std::wstring buffer;
	buffer.resize (size / sizeof (wchar_t) + 2 + 2);
	DWORD readBytes = 0;
	ReadFile (hFile, buf.data (), size, &readBytes, nullptr);
	buffer += buf.data ();
	CloseHandle (hFile);
	buffer [readBytes / sizeof (wchar_t)] = L'\0';
	size_t start = 0;
	while (true)
	{
		size_t pos = buffer.find (L'\n', start);
		std::wstring line;
		if (pos == std::wstring::npos)
		{
			line = buffer.substr (start);
		}
		else
		{
			line = buffer.substr (start, pos - start);
			start = pos + 1;
		}
		if (!line.empty () && line.back () == L'\r') line.pop_back ();
		if (!line.empty ()) result.push_back (line);
		if (pos == std::wstring::npos) break;
	}
	return result;
}
enum class CMDPARAM: DWORD
{
	NONE = 0b000,
	SILENT = 0b001,
	VERYSILENT = 0b011,
	MULTIPLE = 0b100
};
DWORD CmdMapsToFlags (std::map <cmdkey, cmdvalue> cmdpairs, std::set <std::wnstring> &files, std::set <std::wnstring> &uris)
{
	DWORD dwret = 0;
	for (auto &it : cmdpairs)
	{
		switch (it.first.type)
		{
			case paramtype::file: {
				if (IsFileExists (it.first.key)) files.insert (it.first.key);
			} break;
			case paramtype::uri: {
				uris.insert (it.first.key);
			} break;
			default:
			case paramtype::string: {
				auto &key = it.first;
				auto &value = it.second;
				if (key.key.equals (L"silent")) dwret |= (DWORD)CMDPARAM::SILENT;
				else if (key.key.equals (L"verysilent")) dwret |= (DWORD)CMDPARAM::SILENT;
				else if (key.key.equals (L"multiple"))
				{
					if (value.type == paramtype::file)
					{
						auto strlist = LoadFileListW (value.value);
						for (auto &it_s : strlist)
						{
							if (std::wnstring::empty (it_s)) continue;
							std::wnstring filepath = it_s;
							std::wstring listdir = GetFileDirectoryW (value.value);
							if (!IsFileExists (filepath)) filepath = ProcessEnvVars (filepath);
							if (!IsFileExists (filepath)) filepath = CombinePath (listdir, filepath);
							if (!IsFileExists (filepath)) filepath = CombinePath (listdir, it_s);
							if (!IsFileExists (filepath)) continue;
							else files.insert (filepath);
						}
					}
				}
			} break;
		}
	}
	if (files.size () > 1) dwret |= (DWORD)CMDPARAM::MULTIPLE;
	return dwret;
}
HRESULT SetCurrentAppUserModelID (PCWSTR appID)
{
	typedef HRESULT (WINAPI *SetAppUserModelIDFunc)(PCWSTR);
	HMODULE shell32 = LoadLibraryW (L"shell32.dll");
	destruct freelib ([&] () {
		if (shell32) FreeLibrary (shell32);
	});
	try
	{
		if (!shell32) return E_FAIL;
		auto SetAppUserModelID = (SetAppUserModelIDFunc)GetProcAddress (shell32, "SetCurrentProcessExplicitAppUserModelID");
		if (!SetAppUserModelID)
		{
			FreeLibrary (shell32);
			return E_FAIL;
		}
		return SetAppUserModelID (appID);
	}
	catch (...) { return E_FAIL; }
	return E_FAIL;
}
[STAThread]
int APIENTRY wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	SetCurrentProcessExplicitAppUserModelID (m_idenName);
	SetProcessDPIAware ();
	{
		// 设置当前目录为程序所在目录
		std::wnstring currdir = GetCurrentProgramPathW ();
		std::wnstring rootdir = GetProgramRootDirectoryW ();
		if (!PathEquals (currdir, rootdir)) SetCurrentDirectoryW (rootdir.c_str ());
	}
	CoInitializeEx (NULL, COINIT_MULTITHREADED);
	destruct relco ([] () {
		CoUninitialize ();
	});
	System::Windows::Forms::Application::EnableVisualStyles ();
	System::Windows::Forms::Application::SetCompatibleTextRenderingDefault (false);
	auto mwnd = gcnew MainHtmlWnd ();
	g_mainwnd = mwnd;
	System::Windows::Forms::Application::Run (mwnd);
	return 0;
}