//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    MasterVolume.cpp									//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../RackUnits.h"

MasterVolume::MasterVolume(const wxDouble& width, const wxDouble& height)
{
	Height = height;
	Width = width;
	wxRect2DDouble rect(450, 60 - _DEFAULT_R_*1.5, 250, _DEFAULT_R_);
	sens = new Scrollbar(rect, false);
	rect.m_y += _DEFAULT_R_*2;
	vel = new Scrollbar(rect, false);
	vel->SetPercentage(0.5);
	sens->SetPercentage(0.5);
	Name = wxString(" Master Volume ");
	close = new Button(wxString(wxT("✕")), 0, 0, TRANSPARENT | NO_BLUR | NO_BORDER, BTN_RACK_UNIT);
	collapse = new Button(wxString(wxT("▽")), 0, 0, TRANSPARENT | NO_BLUR | NO_BORDER, BTN_RACK_UNIT);
	close->SetSize(_DEFAULT_R_, _DEFAULT_R_);
	collapse->SetSize(_DEFAULT_R_, _DEFAULT_R_);
	InitTriggers(false);
	OffsetY = 0;
}

void MasterVolume::Paint(wxGraphicsContext *gc)
{
	gc->PushState();
	gc->Translate(OffsetDD.m_x, OffsetDD.m_y + OffsetPos);
	DrawUnitContainer(gc);
	gc->Clip(0, GetUnitPosition(), Width, GetHeight());
	gc->Translate(0, Position + TitleBarWidth - OffsetY);
	DrawTriggerMenu(gc);
	DrawBox(gc, wxString(" Monitor "), 250, 80);
	DrawBox(gc, wxString(" Settings "), 350, 370);
	gc->DrawText("Sensitivity: ", 350 + _DEFAULT_R_, 60 - _DEFAULT_R_ - _FONT_SIZE_*0.85);
	gc->DrawText("Velocity: ", 350 + _DEFAULT_R_, 60 + _DEFAULT_R_ - _FONT_SIZE_*0.85);
	DrawVolumeMonitor(gc, wxRect2DDouble(265, 35, 50, 50));
	sens->Paint(gc);
	vel->Paint(gc);
	gc->Translate(0, OffsetY - Position - TitleBarWidth);
	gc->ResetClip();
	DrawFading(gc);
	gc->PopState();
}

bool MasterVolume::Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p_w)
{
	wxPoint2DDouble p(p_w.m_x, p_w.m_y - Position - TitleBarWidth);
	return sens->Contains(gc, p) || vel->Contains(gc, p) || RackUnit::Contains(gc, p_w);
}

void MasterVolume::UpdateStatus() {
if(GetTrigger() < 0) {
		Level = -1;
		return;
	}

	wxDouble data;

	switch(GetTrigger())
	{
		case KEY_ACC_X:
			data = Monitor::GetAcc().x;
			break;

		case KEY_ACC_Y:
			data = Monitor::GetAcc().y;
			break;

		case KEY_ACC_Z:
			data = Monitor::GetAcc().z;
			break;

		case KEY_MP_X:
			data = Monitor::GetMP().x;
			break;

		case KEY_MP_Y:
			data = Monitor::GetMP().y;
			break;

		case KEY_MP_Z:
			data = Monitor::GetMP().z;
			break;

		default:
			Level = -1;
			return;
	}

	data = -data;
	Level = Level < 0? 0x64 : Level;

	if(wxAbs(data) >= (1 - sens->GetPercentage())*0.5) {
		data -= data*sens->GetPercentage()*0.5;
		Level = Level + (int) round(data*(0.2 + vel->GetPercentage()*0.8)*10.0);
		SetLevel(Level);
	}
}

MasterVolume::~MasterVolume()
{
	delete sens;
	delete vel;
	delete close;
	delete collapse;
}

void MasterVolume::DrawVolumeMonitor(wxGraphicsContext *gc, const wxRect2DDouble& area)
{
	int i, n_bars = Level/13;
	wxDouble height = 0.1*area.m_height;

	gc->SetPen(wxNullPen);
	for(i = 0; i < 10; i++) {
		if(i > n_bars)
			gc->SetBrush(D2);
		else if(i < 5)
			gc->SetBrush(L1);
		else if(i < 8)
			gc->SetBrush(L2);
		else
			gc->SetBrush(L3);

		gc->DrawRectangle(area.m_x, area.m_y + (9 - i)*height, area.m_width, height*0.9);
	}

}

void MasterVolume::Click() {
	if(Button::CurrentButton == close)
		Close();
	else if(Button::CurrentButton == collapse) {
		if (IsCollapsed())
			Expand();
		else
			Collapse();
	}
}