//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    Interface.cpp										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WiiMIDI.h"
#include "../../img/WM.xpm"

#define CMD_TIMIDITY	"timidity -iA"
#define NAME_TIMIDITY	"TiMidity"
#define CMD_ZYN			"aoss zynaddsubfx -U -O OSS"
#define NAME_ZYN		"ZynAddSubFX"
#define CMD_PHASEX		"phasex -G -A alsa -a hw:1,0 synth_preset_files/WiiMIDI.phx"
#define NAME_PHASEX		"phasex"
#define CMD_AMS			"ams -n -A -c hw:1,0 synth_preset_files/SineSolo.ams"
#define NAME_AMS		"ams"

#define MIDI_SEQ_CMD 	CMD_AMS
#define MIDI_SEQ_NAME	NAME_AMS

// Event table for Interface
BEGIN_EVENT_TABLE(Interface, wxFrame)
	EVT_PAINT(Interface::OnPaint)
	EVT_COMMAND(wxID_ANY, wxEVT_WIIMOTE_CLOSE, Interface::OnWiimoteClose)
	EVT_COMMAND(wxID_ANY, wxEVT_WIIMOTE_EVENT, Interface::OnWiimoteEvents)
	EVT_MOUSE_EVENTS(Interface::OnMouseEvents)
	EVT_CLOSE(Interface::OnClose)
	EVT_TIMER(0, Interface::OnTimerTick)
	EVT_END_PROCESS(0, Interface::OnMidiSeqClose)
END_EVENT_TABLE()

wxDEFINE_EVENT(wxEVT_WIIMOTE_CLOSE, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_WIIMOTE_EVENT, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_END_PROCESS, wxProcessEvent);

Interface::Interface(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxPoint(200, 100), wxSize(_cs_W_ + _TAB_BORDER_, _cs_H_ + _TAB_BORDER_ + 30),
		wxDEFAULT_FRAME_STYLE &	~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
	SetIcon(WM_xpm);
	SetSize(_CM_W_, _CM_H_ + 25);
	CentreOnScreen();
	Update();
	Refresh();
	err_window = new ErrorMessage(this);
	midi_seq = new wxProcess(this, 0);
	wxMessageOutputDebug().Printf("Interface: Starting Midi Sequencer...");
	pid = wxExecute(wxString(MIDI_SEQ_CMD), wxEXEC_ASYNC, midi_seq);
	if(pid <= 0)
		err_window->SetError(ERR_NO_MIDI_SEQ, wxString("Cannot start Midi Sequencer."));
	monitor = new Monitor(wxRect2DDouble(0, _CM_H_ - 145, _CM_W_, 145));
	timer = new wxTimer(this, 0);
	menu_panel = new MenuPanel(this);
	rack = new Rack(wxRect2DDouble(0, _PANEL_H_, _CM_W_, _CM_H_ - 145 - _PANEL_H_));
	rack_manager = new RackManager(this);
	midi_settings = new MidiSettings(this);
	ctrl_settings = new CtrlSettings(this);
	monitor_window = new MonitorWindow(this);
    wiimote_num = 1;
    num_ctrls = 0;
    m_pThread = NULL;
    js_ok = false;
	invalidate = false;
    timer_ticks = false;
    scrolling = 0;
	Monitor::SetCtrlWindow(ctrl_settings);
	Monitor::SetMonitorWindow(monitor_window);
	Rack::SetRackManager(rack_manager);
    Animation::Timer = timer;
	for(int i = 0; pid > 0 && wxProcess::Exists((int) pid) &&
			wxExecute(wxString("aconnect WiiMIDI:0 " MIDI_SEQ_NAME ":0"), wxEXEC_SYNC); i++) {
		wxMessageOutputDebug().Printf("Interface: Connecting to Midi Port.. [%d]", i + 1);
		wxMilliSleep(100);
	}
	Rack::InitMidiOut();
}

Interface::~Interface()
{
	delete menu_panel;
	delete monitor;
	delete timer;
	delete rack;
	delete err_window;
	delete rack_manager;
	delete midi_settings;
	delete ctrl_settings;
	delete monitor_window;
	delete midi_seq;
}

bool Interface::Enumerate()
{
	struct xwii_monitor *mon;
	char *ent;

    num_ctrls = 0;

	mon = xwii_monitor_new(false, false);

	if (!mon) {
		err_window->SetError(ERR_NO_MON, wxString("Cannot create monitor"));
		return false;
	}

	while ((ent = xwii_monitor_poll(mon))) {
		num_ctrls++;
		free(ent);
	}

	if(!num_ctrls) {
		err_window->SetError(ERR_NO_CTRL_FOUND, wxString("No controllers found!")); 
		return false;
	}
	
	if (wiimote_num > num_ctrls) {
		err_window->SetError(ERR_NO_CTRL_FOUND, wxString("Controller not found!")); 
		return false;
	}

	xwii_monitor_unref(mon);

	return true;
}

void Interface::OnMidiSeqClose(wxProcessEvent &evt) {
	err_window->SetError(ERR_NO_MIDI_SEQ, wxString("Midi Sequencer is no longer running."));
}

void Interface::OnWiimoteClose(wxCommandEvent& evt)
{
    menu_panel->GetMenu(0)->GetButton(1)->Disable();
    menu_panel->GetMenu(0)->GetButton(0)->Enable();
	m_pThread = NULL;
	js_ok = false;
	Refresh();
	Update();
}

void Interface::OnTimerTick(wxTimerEvent& evt)
{
	if(Scrollbar::Scrolling != NULL && scrolling != 0) {
	    if (scrolling > 0)
	    	Scrollbar::Scrolling->DecreasePosition();
	    else if (scrolling < 0)
	    	Scrollbar::Scrolling->IncreasePosition();
	    invalidate = true;
	}

	if(midi_count == 0)
		mess_sent = false;
	else
		midi_count = (midi_count + 1) % 3;

    if (Animation::IsOn())
    { 
        if(timer_ticks) {
			Animation::UpdateAll();
			invalidate = true;
		}
		timer_ticks = !timer_ticks;
    } else if (!js_ok && !scrolling)
        timer->Stop();

    if(invalidate)
    {
		Refresh();
        Update();
    }
}

void Interface::OnWiimoteEvents(wxCommandEvent& evt)
{
	struct xwii_event *event = (struct xwii_event*) evt.GetClientData();
	int key;
	switch (event->type)
	{
		case XWII_EVENT_ACCEL:
			Monitor::UpdateAcceleration(event->v.abs[0].x, event->v.abs[0].y, event->v.abs[0].z);
			Rack::Update(KEY_ACC_X);
			Rack::Update(KEY_ACC_Y);
			Rack::Update(KEY_ACC_Z);
			invalidate = true;
		break;

		case XWII_EVENT_KEY:
			if(event->v.key.code >= 0 && event->v.key.code < 11) {
				key = event->v.key.code;
				Monitor::UpdateKeyStatus((unsigned int) key, event->v.key.state);
				Rack::Update((unsigned int) key);
				invalidate = true;
			}
		break;

		case XWII_EVENT_MOTION_PLUS:
			Monitor::UpdateMotionPlus(event->v.abs[0].x, event->v.abs[0].y, event->v.abs[0].z);
			Monitor::UpdateAbsMP(event->v.abs[0].x, event->v.abs[0].y, event->v.abs[0].z);
			Rack::Update(KEY_MP_X);
			Rack::Update(KEY_MP_Y);
			Rack::Update(KEY_MP_Z);
			invalidate = true;
		break;

		default:
		break;
	}

	free(event);
	if(!mess_sent) {
		Rack::SendMessage();
		mess_sent = true;
	}
}

void Interface::DoStartThread()
{
	wxMessageOutputDebug().Printf("Interface: Connecting controller...");
    if (Enumerate())
    {
		wxMessageOutputDebug().Printf("Creating Thread...");
		m_pThread = new WiimoteThread(this, wiimote_num);
        if (m_pThread->Run() != wxTHREAD_NO_ERROR )
        {
            wxMessageOutputDebug().Printf("Can't create the thread!");
            delete m_pThread;
            m_pThread = NULL;
        } else {
            menu_panel->GetMenu(0)->GetButton(0)->Disable();
            menu_panel->GetMenu(0)->GetButton(1)->Enable();
        	js_ok = true;
            timer->Start(_TIMER_INT_);
        }
    }
}

void Interface::OnMouseEvents(wxMouseEvent& evt)
{
	int ret, wheel;
	bool on_rack = false, on_monitor = false;
	wxClientDC dc(this);
	wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

	if(gc)
	{
		if(evt.LeftDown()) {
			if (Window::CurrentWindow != NULL)
				Window::CurrentWindow->Contains(gc, evt.GetPosition());
			else if (!menu_panel->Contains(gc, evt.GetPosition())) {
				if(ContextMenu::CurrentMenu != NULL)
					ContextMenu::CurrentMenu->Contains(gc, evt.GetPosition());
				else if(!monitor->Contains(gc, evt.GetPosition()) &&
						(on_rack = rack->Contains(gc, evt.GetPosition())))
					RackUnit::StartDragging(evt.GetPosition());
			}
			if(TabsPanel::Current != NULL)
				TabsPanel::Current->Click();
			else
				switch (Button::CurrentID) {
					case 0:
						ContextMenu::Deactivate();
						break;

					case BTN_CTX_MENU:
						ContextMenu::Click();
						ContextMenu::Deactivate();
						break;

					case BTN_CHECKBOX:
						CheckBox::Click();
						break;

					case BTN_SCR_PLUS:
						scrolling = -1;
						timer->Start(_TIMER_INT_);
						if(Scrollbar::Current != NULL && Scrollbar::Scrolling == NULL)
							Scrollbar::Current->StartScrolling(evt.GetPosition());
						break;

					case BTN_SCR_MINUS:
						scrolling = 1;
						timer->Start(_TIMER_INT_);
						if(Scrollbar::Current != NULL && Scrollbar::Scrolling == NULL)
							Scrollbar::Current->StartScrolling(evt.GetPosition());
						break;

					case BTN_SCROLL:
						scrolling = 0;
						if(Scrollbar::Current != NULL && Scrollbar::Scrolling == NULL)
							Scrollbar::Current->StartScrolling(evt.GetPosition());
						break;

					default:
						Button::Click();
						break;
				}
			RadioButton::Click();
        } else if (evt.LeftUp()) {
			RackUnit::Drop();
            if (Button::LastClicked != NULL && Button::LastClicked == Button::CurrentButton)
                switch (Button::CurrentID) {
                    case BTN_CTRL:
                    case BTN_SETT:
					case BTN_DROPDOWN:
                        break;

					case BTN_RACK:
						if(Rack::ActiveUnits.empty())
							menu_panel->GetMenu(1)->GetButton(1)->Disable();
						else
							menu_panel->GetMenu(1)->GetButton(1)->Enable();
						break;

                    case BTN_ERR:
                        err_window->Click();
						ContextMenu::Deactivate();
                        break;

                    case BTN_CTRL_0:
                        DoStartThread();
						ContextMenu::Deactivate();
                        break;

                    case BTN_CTRL_1:
                    	{
							wxCriticalSectionLocker enter(m_pThreadCS);
							if (m_pThread)
							{
								wxMessageOutputDebug().Printf("Interface: deleting thread");
								if (m_pThread->Delete() != wxTHREAD_NO_ERROR )
									wxMessageOutputDebug().Printf("Can't delete the thread!");
							}
						}
						ContextMenu::Deactivate();
                        break;

					case BTN_RACK_0:
						rack_manager->Show();
						ContextMenu::Deactivate();
						break;

					case BTN_RACK_1:
						Rack::RemoveAll();
						ContextMenu::Deactivate();
						break;

					case BTN_RACK_MAN:
						rack_manager->Click();
						ContextMenu::Deactivate();
                        break;

					case BTN_SETT_0:
						Enumerate();
						if(num_ctrls > 0) {
							ctrl_settings->UpdateCtrls(num_ctrls);
							ctrl_settings->Show();
						} else
							err_window->SetError(ERR_NO_CTRL_FOUND, wxString("No controllers found!"));
						ContextMenu::Deactivate();
						break;

					case BTN_CTRL_SETT:
						ret = ctrl_settings->Click();
						if (ret >= 0)
							wiimote_num = ret;
						ContextMenu::Deactivate();
						break;

					case BTN_SETT_1:
						midi_settings->Show();
						ContextMenu::Deactivate();
						break;

					case BTN_MIDI_SETT:
						midi_settings->Click();
						ContextMenu::Deactivate();
						break;

					case BTN_RACK_UNIT:
						RackUnit::ClickCurrent();
						ContextMenu::Deactivate();
						break;

					case BTN_MONITOR:
						monitor_window->Show();
						ContextMenu::Deactivate();
						break;

					case BTN_MONITOR_WIN:
						monitor_window->Click();
						ContextMenu::Deactivate();
						break;

                    default:
						ContextMenu::Deactivate();
                        break;
                }
            Button::UnClick();
            scrolling = 0;
            if(Scrollbar::Scrolling != NULL)
            	Scrollbar::Scrolling->EndScrolling();
        } else if(!evt.LeftIsDown() && Scrollbar::Scrolling == NULL) {
			Button::ResetCurrentButton();
			if (Window::CurrentWindow != NULL)
				Window::CurrentWindow->Contains(gc, evt.GetPosition());
			else if (!menu_panel->Contains(gc, evt.GetPosition())) {
				if(ContextMenu::CurrentMenu != NULL)
					ContextMenu::CurrentMenu->Contains(gc, evt.GetPosition());
				else if(!(on_monitor = monitor->Contains(gc, evt.GetPosition())))
					on_rack = rack->Contains(gc, evt.GetPosition());
			}
		} else if(evt.LeftDClick())
			switch (Button::CurrentID) {
				case BTN_RACK_MAN:
					rack_manager->DoubleClick();
					break;

				default:
					break;
			}

		if(evt.RightDown()) {
			ContextMenu::Deactivate();
		} else if(evt.RightUp()) {
			unsigned int corner;
			if(on_monitor) {
				corner = ((evt.GetPosition().x > _CM_W_ - monitor->GetCTX()->Width)? 1 : 0) +
						((evt.GetPosition().y > _CM_H_ - monitor->GetCTX()->Height)? 2 : 0);
				monitor->GetCTX()->SetLocation(evt.GetPosition(), corner);
				monitor->GetCTX()->SetCurrent();
			} else if(on_rack) {
				corner = ((evt.GetPosition().x > _CM_W_ - rack->GetCTX()->Width)? 1 : 0) +
						((evt.GetPosition().y > _CM_H_ - rack->GetCTX()->Height)? 2 : 0);
				rack->GetCTX()->SetLocation(evt.GetPosition(), corner);
				rack->GetCTX()->SetCurrent();
			}
		}

		if(on_rack && (wheel = evt.GetWheelRotation()) && evt.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
			rack->MouseWheel(wheel);

        if(Scrollbar::Scrolling != NULL && !scrolling)
        	Scrollbar::Scrolling->Drag(evt.GetPosition());

        if(RackUnit::IsDragging()) {
        	RackUnit::Drag(evt.GetPosition());
        	// if(rack->GetScrollbar()->IsVisible()) {
        	// 	if(evt.GetPosition().y - rack->GetArea().m_y > rack->GetArea().m_height*0.9)
        	// 		rack->GetScrollbar()->IncreasePosition();
        	// 	else if(evt.GetPosition().y - rack->GetArea().m_y < rack->GetArea().m_height*0.1)
        	// 		rack->GetScrollbar()->DecreasePosition();
        	// }
        }

		if (timer->IsRunning())
			invalidate = true;
		else
		{
			Refresh();
			Update();
		}

		delete gc;
	}
}

void Interface::OnPaint(wxPaintEvent& evt)
{	
	wxBufferedPaintDC dc(this);
	wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

	if(gc)
	{
		dc.SetBackground(wxBrush(D2));
		dc.Clear();

		rack->Paint(gc);
        monitor->Paint(gc);
		menu_panel->Paint(gc);

		if(Window::CurrentWindow != NULL)
			Window::CurrentWindow->Paint(gc);

		ContextMenu::Paint(gc);

		invalidate = false;
		
		delete gc;
	}
}

void Interface::OnClose(wxCloseEvent&)
{
	if (timer->IsRunning())
		timer->Stop();

	Animation::DeleteAll();
	wxMessageOutputDebug().Printf("Interface: Closing Midi Sequencer...");
	if(pid > 0 && wxProcess::Exists((int) pid) && wxKill(pid) < 0)
		wxKill(pid, wxSIGKILL);
	
	{
		wxCriticalSectionLocker enter(m_pThreadCS);
		if (m_pThread) 
		{
			wxMessageOutputDebug().Printf("Interface: Deleting thread...");
			if (m_pThread->Delete() != wxTHREAD_NO_ERROR )
				wxMessageOutputDebug().Printf("Can't delete the thread!");
		}
	} 

	while (1)
	{
		{ 
			wxCriticalSectionLocker enter(m_pThreadCS);
			if (!m_pThread)
				break;
		}
		wxThread::This()->Sleep(1);
	}

	Destroy();
}
