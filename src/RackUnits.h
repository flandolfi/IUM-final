//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    RackUnits.h										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __RACK_UNITS_H__
#define __RACK_UNITS_H__

#define _LED_POS_		720
#define _LED_R_			8
#define _BLUR_R_		_LED_R_*2
#define _PB_TONE_QTS_	4

#include "WiiMIDI_GUI.h"
#include "Monitor.h"

class RackUnit : public wxObject
{
	protected:
		int Note = -1, Level = -1, Bend = -1;
		wxDouble Alpha = 0, AlphaDD = 255, Height, Width, Position, OffsetY = 0, TitleBarWidth = 0, OffsetPos = 0;
		static wxPoint2DDouble StartDD;
		wxPoint2DDouble OffsetDD;
		wxString Name;
		DropdownMenu *Trigger;
		ContextMenu *r_click=NULL;
		Button *close, *collapse;
		static list<RackUnit*> next_elements, previous_elements;
		int swap_pos = 0;
		bool button_triggered, collapsed = false, closing = false, is_on_titlebar = false;
		static bool is_dragging;

	public:
		int Priority = 0;
		static RackUnit *Current, *DraggingUnit;
		void SetHeight(const wxDouble& h);
		wxDouble GetHeight();
		wxDouble GetUnitHeight();
		void SetWidth(const wxDouble& w);
		wxDouble GetWidth();
		void SetYPosition(const wxDouble& p);
		wxDouble GetYPosition();
		wxDouble GetUnitPosition();
		void SetName(const wxString& s);
		wxString GetName();
		ContextMenu* GetCTX() { return r_click; }
		void InitTriggers(bool ButtonTriggered);
		void DrawUnitContainer(wxGraphicsContext *gc);
		void DrawTriggerMenu(wxGraphicsContext *gc);
		void DrawBox(wxGraphicsContext *gc, const wxString& label, const wxDouble& x, const wxDouble& width);
		void Collapse();
		void Expand();
		void Close();
		void DrawFading(wxGraphicsContext* gc);
		void Reset();
		void ResetTrigger() { Trigger->SetDefault(); }
		static void OnTriggerChange(RackUnit* unit);
		static void StartDragging(const wxPoint2DDouble& p);
		static void Drag(const wxPoint2DDouble& p);
		static void Drop();
		static void ClickCurrent();
		static void OnCTXChange(RackUnit* ru);
		bool IsCollapsed();
		static bool IsDragging();
		virtual ~RackUnit();
		virtual void Click();
		virtual void Paint(wxGraphicsContext *gc) = 0;
		virtual bool Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p);
		virtual void UpdateStatus() = 0;
		void SetLevel(int l);
		void SetNote(int n);
		void SetBend(int b);
		virtual int GetNote();
		virtual int GetLevel();
		virtual int GetBend();
		int GetTrigger();

		friend class Animation;
};

class Rack
{
	private:
		wxRect2DDouble area;
		wxDouble offset, total_h;
		Scrollbar scr;
		ContextMenu *r_click;
		static vector<unsigned char> message;
		static RtMidiOut* MidiOut;
		static unsigned int Channel;
		static int Note, Level, Bend;
		static RackManager* rack_man;

	public:
		static list<RackUnit*> ActiveUnits;
		static list<RackUnit*> AvailableUnits;
		static RackUnit* Handlers[];
		Rack(const wxRect2DDouble& area);
		~Rack();
		wxDouble GetSize();
		ContextMenu* GetCTX();
		wxRect2DDouble GetArea() { return area; }
		Scrollbar* GetScrollbar() { return &scr; }
		static void SetRackManager(RackManager* rm) { rack_man = rm; }
		static RackManager* GetRackManager() { return rack_man; }
		static void SetChannel(unsigned int c);
		static void InitMidiOut();
		static unsigned int GetChannel();
		static RackUnit* GetHandler(const int& key);
		static void SetHandler(const int& key, RackUnit* unit);
		static void Update(const int& key);
		static void Add(RackUnit* unit);
		static void Remove(RackUnit* unit);
		static void SendMessage();
		static void OnCTXChange(Rack* r);
		void MouseWheel(int wheel);
		static void RemoveAll();
		void Paint(wxGraphicsContext *gc);
		bool Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p);
};

class PitchController : public RackUnit
{
	private:
		Scrollbar *sens, *vel;

	public:
		PitchController(const wxDouble& width, const wxDouble& height=120);
		~PitchController();
		void Paint(wxGraphicsContext* gc);
		bool Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p);
		void UpdateStatus();
		void Click();
		void DrawCurrentNote(wxGraphicsContext *gc, const wxRect2DDouble& area);
		int GetNote() { return Note; }
		int GetBend() { return Bend; }
};

class MasterVolume : public RackUnit
{
	private:
		Scrollbar *sens, *vel;

	public:
		MasterVolume(const wxDouble& width, const wxDouble& height=120);
		~MasterVolume();
		void Paint(wxGraphicsContext* gc);
		bool Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p);
		void UpdateStatus();
		void Click();
		void DrawVolumeMonitor(wxGraphicsContext *gc, const wxRect2DDouble& area);
		int GetLevel() { return Level; }
};

class Mute : public RackUnit
{
	private:
		RadioButton* opts;
		bool last_status = false;
		static void OnMuteChange(RackUnit* unit);

	public:
		Mute(const wxDouble& width, const wxDouble& height=120);
		~Mute();
		void Paint(wxGraphicsContext* gc);
		bool Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p);
		void Click();
		void UpdateStatus();
		int GetLevel() { return Level; }
};

class Drone : public RackUnit
{
	private:
		DropdownMenu *note, *octave;
		static void OnNoteChange(RackUnit* unit);

	public:
		Drone(const wxDouble& width, const wxDouble& height=120);
		~Drone();
		void Paint(wxGraphicsContext* gc);
		bool Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p);
		void Click();
		void UpdateStatus();
		int GetNote() { return Note; }
		int GetBend() { return Bend; }
};

#endif