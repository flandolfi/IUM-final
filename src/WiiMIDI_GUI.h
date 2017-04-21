//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    WiiMIDI_GUI.h										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __WIIMIDI_GUI_H__
#define __WIIMIDI_GUI_H__

#include <cerrno>
#include <cmath>
#include <list>
#include <poll.h>
#include "RtMidi.h"
#include "xwiimote.h"
#include "wx/frame.h"
#include "wx/dcbuffer.h"
#include "wx/app.h"
#include "wx/graphics.h"
#include "wx/image.h"
#include "wx/bitmap.h"
#include "wx/thread.h"
#include "wx/string.h"
#include "wx/timer.h"

//Theme Colours
#define _LIGHT_CL1_		"#6289B5"
#define _LIGHT_CL2_		"#7AA9DE"
#define _LIGHT_CL3_		"#ABD3FF"
#define _DARK_CL1_		"#151515"
#define _DARK_CL2_		"#222222"
#define _DARK_CL3_		"#3C3C3C"

//Predefined Values
#define _DEFAULT_W_		80
#define _DEFAULT_H_		30
#define _DEFAULT_R_		(_DEFAULT_H_*0.5)
#define _FONT_SIZE_		10
#define _FONT_NAME_		"Ubuntu Condensed"
#define _cs_H_			400
#define _cs_W_			250
#define _CM_W_			800
#define _CM_H_			600
#define _BLUR_DIM_		8
#define _N_TABS_		4
#define _TAB_DIM_		100
#define _TAB_GAP_		25
#define _TAB_BORDER_	5
#define _PANEL_W_		25
#define _PANEL_H_		30
#define _TAB_S_			0.75 /* from 0.0 to 1.0 */
#define _SPEED_FACT_	0.1
#define _CONT_DIST_		25
#define _NORM_FACTOR_	0.005
#define _TIMER_INT_     33
#define _DELTA_SCR_		0.01
#define _FADE_DIM_		30

//Button Style
#define TRANSPARENT		1
#define NO_BORDER		2
#define NO_FILL			4
#define NO_BLUR			8
#define LIGHT_THEME		16
#define RECT_SHAPED		32
#define BOLD_FONT		64

//Buttons IDs
#define BTN_ERR         1
#define BTN_CTRL        2
#define BTN_RACK        3
#define BTN_SETT        4
#define BTN_CTRL_0		5
#define BTN_CTRL_1		6
#define BTN_CTRL_2		7
#define BTN_CTRL_3		8
#define BTN_RACK_0 		9
#define BTN_RACK_1 		10
#define BTN_RACK_2		11
#define BTN_RACK_3		12
#define BTN_SETT_0 		13
#define BTN_SETT_1		14
#define BTN_SETT_2		15
#define BTN_SETT_3		16
#define BTN_SCR_PLUS	17
#define BTN_SCR_MINUS	18
#define BTN_SCROLL		19
#define BTN_RADIO		20
#define BTN_DROPDOWN	21
#define BTN_RACK_MAN	22
#define BTN_CTRL_SETT	23
#define BTN_MIDI_SETT	24
#define BTN_CHECKBOX	25
#define BTN_RACK_UNIT	26
#define BTN_MONITOR		27
#define BTN_MONITOR_WIN	28
#define BTN_TABS		29
#define BTN_CTX_MENU	30

//ContextMenu Types
#define CTX_MENU_PANEL		1
#define CTX_DROPDOWN_MENU	2
#define CTX_RIGHT_CLICK		3

//ContextMenu Corner IDs
#define CTX_LEFT_UP		0
#define CTX_RIGHT_UP	1
#define CTX_LEFT_DOWN	2
#define CTX_RIGHT_DOWN	3

using namespace std;

typedef enum _notes { C=0, Csh, D, Dsh, E, F, Fsh, G, Gsh, A, Ash, B } note_t;

extern const wxColour L1, L2, L3, D1, D2, D3;
inline wxDouble wxAbs(const wxDouble& d) { return (d >= 0)? d : -d; }
inline wxDouble wxSign(const wxDouble& d) { return (d >= 0)? 1 : -1; }
inline wxDouble wxDegToRad(const wxDouble& d) { return d*M_PI/180; }

class ContextMenu;

class Animation
{
	private:
        static list<Animation*> Animations;
		void (*CustomFunction)(void*) = NULL;
		wxDouble t, speed;
		wxDouble smooth_fun(const wxDouble& t);
		void *fn_arg;
		Animation* queued;
		Animation* Queue(Animation* ptr);
		Animation(wxDouble *val, const wxDouble& start, const wxDouble& end, const wxDouble& speed);

	public:
        static wxTimer* Timer;
		~Animation() { Value = NULL; }
		wxDouble *Value, StartsWith, EndsWith;
		Animation* Queue(wxDouble *val, const wxDouble& start, const wxDouble& end, const wxDouble& speed=2);
		void QueueFunction(void (*fn)(void*), void* arg);
		static Animation* Add(wxDouble *val, const wxDouble& start, const wxDouble& end, const wxDouble& speed=1);
		static Animation* Find(wxDouble *val);
		static void UpdateAll();
		static bool IsOn();
		static void DeleteAll();
		void Update();
		bool operator==(const Animation& a) { return this == &a; }
};

class Button
{
	private:
		wxRect2DDouble area;
		wxString label;
		wxDouble radius;
		wxFont font;
        unsigned int id;
        bool clicked, IsOn;
        bool enabled;

	public:
        static Button *CurrentButton, *LastClicked;
        static unsigned int CurrentID;
		unsigned int style;
        ContextMenu *DependingContextMenu;
		Button(const wxString& label=wxString(""), const wxDouble& x=0,
			const wxDouble& y=0, unsigned int style=0, unsigned int bt_id=0);
		void Paint(wxGraphicsContext *gc);
		void SetLocation(const wxDouble& x, const wxDouble& y);
		wxPoint2DDouble GetLocation();
		unsigned int GetStyle() { return style; }
		void SetStyle(unsigned int s) { style = s; }
		void SetCentre(const wxDouble& x, const wxDouble& y);
		wxPoint2DDouble GetCentre();
		void SetLabel(const wxString& label);
		wxString GetLabel() { return label; }
		void SetSize(const wxDouble& w, const wxDouble& h);
		wxSize GetSize();
		bool Contains(const wxPoint2DDouble& pt, wxGraphicsContext *gc);
        unsigned int GetID() { return id; }
        void SetID(unsigned int i) { id = i; }
        void SetDependingContextMenu(ContextMenu* cm) { DependingContextMenu = cm; }
        void Enable() { enabled = true; }
        void Disable() { enabled = false; }
        bool IsEnabled() { return enabled; }
        static void Click();
        static void UnClick();
        static void ResetCurrentButton();
};

class TabsPanel
{
	private:
		vector<wxString> tabs;
		int current, focus, n_tabs;
		wxRect2DDouble area, tabs_area, free_area;
		void DrawTabs(wxGraphicsContext *gc, int tab);

	public:
		static TabsPanel* Current;
		TabsPanel(const wxRect2DDouble& area, int n_tabs, const char *labels[]);
		wxRect2DDouble GetFreeArea();
		bool Contains(wxGraphicsContext *gc, const wxPoint2DDouble& pt);
		int CurrentTab();
		void Click();
		void Paint(wxGraphicsContext *gc);
};

class CheckBox
{
	private:
		vector<Button> bts;
		vector<wxString> labels;
		vector<bool> checks;
		int num_opts, isOn;
		static CheckBox* Current;

	public:
		wxDouble X, Y;
		CheckBox(const wxDouble& x, const wxDouble& y);
		~CheckBox();
		bool IsChecked(int i);
		void Toggle(int i);
		void SetLocation(const wxDouble& x, const wxDouble& y) { X = x; Y = y; }
		static void Click();
		void CenterY(const wxDouble& y);
		void AddOption(const wxString& label);
		void Paint(wxGraphicsContext *gc);
		void Reset();
		bool Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p);
};

class Scrollbar
{
	private:
		wxRect2DDouble area;
		Button plus, minus, scroller;
		bool isVertical, isVisible;
		wxDouble perc, delta;
		wxPoint2DDouble scroll_from, old_pos;

	public:
		static Scrollbar *Current, *Scrolling;
		Scrollbar(const wxRect2DDouble& area=wxRect2DDouble(), bool isVertical=true);
		void SetVertical();
		void SetHorizontal();
		void SetArea(const wxRect2DDouble& area);
		bool IsVertical() { return isVertical; }
		bool IsVisible() { return isVisible; }
		void SetVisible(bool v) { isVisible = v; }
		void Paint(wxGraphicsContext* gc);
		bool Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p);
		wxDouble GetPercentage();
		void SetPercentage(const wxDouble& p);
		void IncreasePosition();
		void DecreasePosition();
		void ResetPosition();
		void StartScrolling(const wxPoint2DDouble& from) { scroll_from = from; Scrolling = this; old_pos = scroller.GetLocation(); }
		void EndScrolling() { Scrolling = NULL; }
		void Drag(const wxPoint2DDouble& p);
};

class DropdownMenu
{
	private:
		ContextMenu* menu;
		Button* btn;
		wxString def_option;

	public:
		wxDouble X, Y, CTX_Width;
		DropdownMenu(const wxDouble& x, const wxDouble& y, const int& num_opt,
				const char** options, const wxDouble& ctx_width=_DEFAULT_W_*1.5, int def=-1);
		void Paint(wxGraphicsContext *gc);
		bool Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p);
		int GetSelected();
		void SetDefault();
		void SetOnChangeFunction(void (*fn)(void*), void* arg);
};

class ContextMenu
{
    private:
        int IsOn, bts_num, LastSelected;
		wxGraphicsMatrix mat;
        Button **bts;
		Scrollbar *scr;
		void (*custom_fn)(void*) = NULL;
		void* fn_arg = NULL;

	public:
		wxDouble X, Y, Height, Width, OffsetY = 0, OffsetScr = 0;
		unsigned int MenuType;
        static ContextMenu* CurrentMenu;
        static bool IsActive;
        bool Animated, HadDoneAnimation;;
		ContextMenu(const wxDouble& x, const wxDouble& y, const int n, const char** labels,
				bool animated=false, unsigned int type=0, unsigned int style=LIGHT_THEME,
				const wxDouble& width=_DEFAULT_W_*1.5, int def=-1);
		~ContextMenu();
		static void Paint(wxGraphicsContext *gc);
		int Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p);
		int GetLastSelected() { return LastSelected; }
		void SetLocation(const wxPoint2DDouble& p, unsigned int corner=0);
		void SetDefault();
        void SetCurrent();
		void SetTransform(const wxGraphicsMatrix& m) { mat = m; }
		void SetOnChangeFunction(void (*fn)(void*), void* arg) { custom_fn = fn; fn_arg = arg; }
        Button* GetButton(int i);
        static int Click();
        static void PopUp();
        static void Activate();
        static void Deactivate();
        static void Toggle();
		static void OnChange();

	friend class DropdownMenu;
};

class MenuPanel
{
	private:
        wxFrame *owner;
		ContextMenu *ctrl_menu, *rack_menu, *sett_menu;
		wxDouble width;
		Button *ctrl, *rack, *settings;
		wxBitmap *logo;

	public:
		MenuPanel(wxFrame *owner);
		~MenuPanel();
		void Paint(wxGraphicsContext *gc);
		bool Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p);
        ContextMenu* GetMenu(int i);
};

class RadioButton
{
	private:
		vector<Button> bts;
		vector<wxString> labels;
		int selected, num_opts, isOn;
		void (*custom_fn)(void*) = NULL;
		void* fn_arg = NULL;
		static RadioButton* Current;

	public:
		wxDouble X, Y, Width;
		RadioButton(const wxDouble& x, const wxDouble& y);
		~RadioButton();
		int Selected() { return selected; }
		void Select(int i);
		void SetLocation(const wxDouble& x, const wxDouble& y) { X = x; Y = y; }
		static void Click();
		void AddOption(const wxString& label);
		void Paint(wxGraphicsContext *gc);
		void Reset();
		void CenterY(const wxDouble& y);
		void SetOnChangeFunction(void (*fn)(void*), void* arg) { custom_fn = fn; fn_arg = arg; }
		static void OnChange();
		bool Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p);
};

#endif
