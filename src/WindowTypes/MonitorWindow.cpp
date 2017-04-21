//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    MonitorWindow.cpp									//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WindowTypes.h"
#include "../Monitor.h"

MonitorWindow::MonitorWindow(wxFrame* owner) {
	Parent = owner;
	area = wxRect2DDouble(0, 0, Parent->GetClientSize().GetWidth()*0.7, Parent->GetClientSize().GetHeight()*0.7);
	area.SetCentre(wxPoint2DDouble(Parent->GetClientSize().GetWidth()*0.5, Parent->GetClientSize().GetHeight()*0.5));
	bt_close = new Button(wxString("Close"), area.m_width - _DEFAULT_W_, area.m_height - _DEFAULT_H_, NO_FILL, BTN_MONITOR_WIN);
	const char *labels[] = { "Accelerometer", "Gyroscope", "Buttons" };
	tabs = new TabsPanel(wxRect2DDouble(-_PANEL_W_, 0, area.m_width + _PANEL_W_, area.m_height - _DEFAULT_H_*1.5 + _PANEL_W_),
			3, labels);
}

MonitorWindow::~MonitorWindow() {
	delete bt_close;
	delete tabs;
}

void MonitorWindow::Paint(wxGraphicsContext *gc) {
	DrawBackground(gc);
	DrawContainer(gc, wxString("Monitor"), area);
	gc->PushState();
	gc->Translate(area.m_x, area.m_y);
	tabs->Paint(gc);
	bt_close->Paint(gc);
	switch(tabs->CurrentTab())
	{
		case 0:
			Monitor::DrawSphere(gc, wxRect2DDouble(50, 30, area.m_width*0.3, area.m_width*0.3), false);
			Monitor::DrawMonitorBar(gc, Monitor::GetAbsAcc(), wxRect2DDouble(50 + area.m_width*0.4, 50, area.m_width*0.4, area.m_height*0.3), false);
			Monitor::DrawAccValues(gc, wxRect2DDouble(0, 50 + area.m_height*0.4, area.m_width*0.8, area.m_height*0.2), false);
			break;

		case 1:
			Monitor::DrawGyroscope(gc, wxRect2DDouble(50, 30, area.m_width*0.3, area.m_width*0.3), false);
			Monitor::DrawMonitorBar(gc, Monitor::GetMP(), wxRect2DDouble(50 + area.m_width*0.4, 50, area.m_width*0.4, area.m_height*0.3), false);
			Monitor::DrawMPValues(gc, wxRect2DDouble(0, 50 + area.m_height*0.4, area.m_width*0.8, area.m_height*0.2), false);
			break;

		case 2:
			Monitor::DrawButtonsMonitor(gc, wxRect2DDouble(50, 30, area.m_width*0.8, area.m_width*0.25), false);
			Monitor::DrawBtsValues(gc, wxRect2DDouble(100, 50 + area.m_height*0.3, area.m_width*0.8, area.m_height*0.4), false);
			break;

		default:
			break;
	}
	gc->PopState();
}

bool MonitorWindow::Contains(wxGraphicsContext *gc, const wxPoint2DDouble &p_w) {
	wxPoint2DDouble p(p_w.m_x - area.m_x, p_w.m_y - area.m_y);
	return bt_close->Contains(p, gc) || tabs->Contains(gc, p);
}

int MonitorWindow::Click() {
	if(Button::LastClicked == bt_close)
		Hide();

	tabs->Click();

	return 0;
}