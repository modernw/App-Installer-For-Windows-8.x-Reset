// uishell.h

#pragma once

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;

namespace Win32 
{
	using WebBrowserCli = System::Windows::Forms::WebBrowser;
	public ref class SplashForm: Form
	{
		private:
		PictureBox ^picbox;
		Timer ^timer;
		System::Drawing::Image ^splashimg = nullptr;
		System::Drawing::Color background = System::Drawing::Color::Transparent;
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
				if (img != nullptr)
				{
					splashimg = img;
					picbox->Image = img;
				}
			}
			catch (...) {}
			if (splashimg) picbox->Image = splashimg;
			try
			{
				if (backcolor != Drawing::Color::Transparent)
				{
					background = backcolor;
					picbox->BackColor = backcolor;
					this->BackColor = backcolor;
				}
				else
				{
					picbox->BackColor = background;
					this->BackColor = background;
				}
			}
			catch (...) {}
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
		void ReInit ()
		{
			InitForm ();
			picbox = gcnew System::Windows::Forms::PictureBox ();
			picbox->BackColor = background;
			if (splashimg) picbox->Image = splashimg;
			picbox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			picbox->Anchor = System::Windows::Forms::AnchorStyles::None;
			double dDpi = GetDPI () * 0.01;
			picbox->Size = Drawing::Size (620 * dDpi, 300 * dDpi);
			this->BackColor = background;
			this->Controls->Clear ();
			this->Controls->Add (picbox);
			timer = gcnew System::Windows::Forms::Timer ();
			timer->Interval = 15;
			timer->Tick += gcnew EventHandler (this, &SplashForm::OnFadeTimer);
			this->Resize += gcnew EventHandler (this, &SplashForm::OnResize);
			this->Load += gcnew EventHandler (this, &SplashForm::OnLoad);
			ChangePosAndSize ();
			this->Opacity = 1.0;
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
		void SetSplashImage (System::Drawing::Image ^img) { if (picbox && picbox->IsHandleCreated) { splashimg = img; picbox->Image = splashimg; } else splashimg = img; }
		void SetSplashImage (System::String ^imgpath) { try { SetSplashImage (System::Drawing::Image::FromFile (imgpath)); } catch (...) {} }
		void SetSplashImage (const std::wstring &imgpath) { SetSplashImage (CStringToMPString (imgpath)); }
		void SetSplashBackgroundColor (System::Drawing::Color color) { background = color;  picbox->BackColor = color; this->BackColor = color; }
		// 渐变消失
		void FadeOut () { timer->Start (); }
		// 立即消失（并没有）
		void FadeAway () { timer->Start (); }
		~SplashForm ()
		{
			if (this->Owner != nullptr)
			{
				this->Owner->Resize -= gcnew System::EventHandler (this, &SplashForm::OnResizeOwner);
				this->Owner->LocationChanged -= gcnew System::EventHandler (this, &SplashForm::OnLocationChangedOwner);
			}
		}
	};
	public ref class WebBrowserForm: Form
	{
		private:
		WebBrowserCli ^webui;

	};
}
