//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    WiiMIDI.h 											//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __WIIMIDI_H__
#define __WIIMIDI_H__

#include "WiiMIDI_GUI.h"
#include "WindowTypes.h"
#include "RackUnits.h"
#include "Monitor.h"
#include <wx/utils.h>
#include <wx/process.h>

//Error Codes
#define ERR_NO_CTRL_FOUND	1
#define ERR_NO_MON			2
#define ERR_CTRL_NOT_FOUND  4
#define ERR_WIIMOTE_THREAD	8
#define ERR_WIIMOTE_GONE	16
#define ERR_NO_MIDI_PORTS	32
#define ERR_NO_MIDI_SEQ		64

wxDECLARE_EVENT(wxEVT_WIIMOTE_CLOSE, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_WIIMOTE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_END_PROCESS, wxProcessEvent);

class WiimoteThread;

class Interface : public wxFrame
{
	private:
		Rack *rack;
		Monitor *monitor;
		ErrorMessage *err_window;
		RackManager *rack_manager;
		MenuPanel *menu_panel;
		MidiSettings *midi_settings;
		CtrlSettings *ctrl_settings;
		MonitorWindow* monitor_window;
		wxProcess *midi_seq;
		bool js_ok, invalidate, timer_ticks, mess_sent = false;
		int scrolling, midi_count = 0;
		long pid;

	protected:
		WiimoteThread *m_pThread;
		wxCriticalSection m_pThreadCS;
		
		DECLARE_EVENT_TABLE()
	
	public:
        wxTimer *timer;
        int wiimote_num, num_ctrls;
		Interface(const wxString& title);
		~Interface();
	    void OnPaint(wxPaintEvent& evt);
	    void OnMouseEvents(wxMouseEvent& evt);
	    void OnWiimoteEvents(wxCommandEvent& evt);
	    void OnWiimoteClose(wxCommandEvent& evt);
	    void OnMidiSeqClose(wxProcessEvent& evt);
	    void OnClose(wxCloseEvent& evt);
	    void OnTimerTick(wxTimerEvent& evt);
	    void DoStartThread();
	    bool Enumerate();

	friend class WiimoteThread;
};

class WiimoteThread : public wxThread
{
	public:
		int wiimote_num;
		WiimoteThread(Interface* handler, int num);
		~WiimoteThread();

	protected:
		struct xwii_iface *iface;
		virtual ExitCode Entry();
		Interface *m_pHandler;

	private:
		int RunInterface(struct xwii_iface *iface);
		char* GetDev(int num);
};

class WiiMIDI : public wxApp
{
	public:
		virtual bool OnInit();
};

#endif
