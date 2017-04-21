//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    WindowTypes.h										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __WINDOW_TYPES__
#define __WINDOW_TYPES__

#include "WiiMIDI_GUI.h"

class GhostWindow;

class Window
{
	protected:
		wxFrame* Parent;
		static GhostWindow Ghost;

	public:
		static Window* CurrentWindow;
		wxDouble AlphaBackground;
		static void FitRectToContainer(wxRect2DDouble *rect);
		static void DrawContainer(wxGraphicsContext *gc, const wxString& title, const wxRect2DDouble& area_cont);
		void DrawBackground(wxGraphicsContext *gc);
		virtual ~Window() {}
		virtual void Show();
		virtual void Hide();
		virtual void Paint(wxGraphicsContext *gc) = 0;
		virtual bool Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p) = 0;
		virtual int Click() = 0;

	friend class Animation;
};

class ErrorMessage : public Window
{
	private:
		int err;
		wxString err_str;

	public:
		Button* err_btn;
		ErrorMessage(wxFrame* owner);
		~ErrorMessage();
		void Paint(wxGraphicsContext *gc);
		bool Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p);
		void SetError(const int& err, const wxString& err_str);
		int GetErrorCode() { return err; }
		int Click();
};

class RackManager : public Window
{
	private:
		wxRect2DDouble area;
		Button *bt_left, *bt_right, *bt_ok, *bt_cancel, *selected;
		list<Button*> available_units, active_units;

	public:
		RackManager(wxFrame* owner);
		~RackManager();
		void Paint(wxGraphicsContext* gc);
		void Show();
		void Update();
		bool Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p);
		int Click();
		void DoubleClick();
};

class MidiSettings : public Window
{
	private:
		unsigned int channel;
		wxRect2DDouble area;
		Button *bt_ok, *bt_cancel, *bt_left, *bt_right;

	public:
		MidiSettings(wxFrame* owner);
		~MidiSettings();
		int Click();
		void Paint(wxGraphicsContext* gc);
		bool Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p);
};

class CtrlSettings : public Window
{
	private:
		int ctrl_num, tot_ctrls;
		wxRect2DDouble area;
		Button *bt_ok, *bt_cancel, *bt_left, *bt_right;

	public:
		CtrlSettings(wxFrame* owner);
		~CtrlSettings();
		int Click();
		void UpdateCtrls(int ctrls);
		void Paint(wxGraphicsContext* gc);
		bool Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p);
};

class MonitorWindow : public Window
{
	private:
		Button* bt_close;
		TabsPanel *tabs;
		wxRect2DDouble area;

	public:
		MonitorWindow(wxFrame* owner);
		~MonitorWindow();
		int Click();
		void Paint(wxGraphicsContext *gc);
		bool Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p);
};

class GhostWindow : public Window
{
	private:
		Window* Parent = NULL;

	public:
		GhostWindow();
		~GhostWindow();
		bool Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p);
		void Paint(wxGraphicsContext *gc);
		int Click();
		void Hide(Window* w);
};

#endif