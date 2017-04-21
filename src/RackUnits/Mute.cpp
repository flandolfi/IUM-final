//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    Mute.cpp											//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../RackUnits.h"

Mute::Mute(const wxDouble& width, const wxDouble& height) {
	Priority = 3;
	Height = height;
	Width = width;
	Name = wxString(" Mute ");
	close = new Button(wxString(wxT("✕")), 0, 0, TRANSPARENT | NO_BLUR | NO_BORDER, BTN_RACK_UNIT);
	collapse = new Button(wxString(wxT("▽")), 0, 0, TRANSPARENT | NO_BLUR | NO_BORDER, BTN_RACK_UNIT);
	close->SetSize(_DEFAULT_R_, _DEFAULT_R_);
	collapse->SetSize(_DEFAULT_R_, _DEFAULT_R_);
	InitTriggers(true);
	opts = new RadioButton(280, 0);
	opts->AddOption(wxString("On Press"));
	opts->AddOption(wxString("On Release"));
	opts->AddOption(wxString("Toggle"));
	opts->CenterY(60);
	opts->SetOnChangeFunction((void (*)(void*)) OnMuteChange, (void*) this);
}

void Mute::Paint(wxGraphicsContext* gc) {
	wxDouble w, h, i1, i2;
	gc->PushState();
	gc->Translate(OffsetDD.m_x, OffsetDD.m_y + OffsetPos);
	DrawUnitContainer(gc);
	gc->Clip(0, GetUnitPosition(), Width, GetHeight());
	gc->Translate(0, Position + TitleBarWidth - OffsetY);
	DrawTriggerMenu(gc);
	DrawBox(gc, wxString(" Trigger Mode "), 250, 150);
	opts->Paint(gc);
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), L1);
	gc->GetTextExtent("Mute ON/OFF", &w, &h, &i1, &i2);
	gc->DrawText("Mute ON/OFF", 620, (Height - h)*0.5);
	gc->SetPen(L1);
	gc->StrokeLine(620 + w + 4, Height*0.5, _LED_POS_ - _LED_R_ - 4, Height*0.5);
	if(Level == 0) {
		wxColour tr(L3.Red(), L3.Green(), L3.Blue(), 0);
		gc->SetBrush(gc->CreateRadialGradientBrush(_LED_POS_, Height*0.5,
				_LED_POS_, Height*0.5, _BLUR_R_, L3, tr));
		gc->SetPen(wxNullPen);
		gc->DrawEllipse(_LED_POS_ - _BLUR_R_, Height*0.5 - _BLUR_R_,
				_BLUR_R_*2, _BLUR_R_*2);
		gc->SetPen(L3);
		gc->SetBrush(L3);
		gc->DrawEllipse(_LED_POS_ - _LED_R_, Height*0.5 - _LED_R_,
				_LED_R_*2, _LED_R_*2);
	} else {
		gc->SetPen(D3);
		gc->SetBrush(D3);
		gc->DrawEllipse(_LED_POS_ - _LED_R_, Height*0.5 - _LED_R_,
				_LED_R_*2, _LED_R_*2);
	}
	gc->Translate(0, OffsetY - Position - TitleBarWidth);
	gc->ResetClip();
	DrawFading(gc);
	gc->PopState();
}

bool Mute::Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p_w)
{
	wxPoint2DDouble p(p_w.m_x, p_w.m_y - Position - TitleBarWidth);
	return opts->Contains(gc, p) || RackUnit::Contains(gc, p_w);
}

void Mute::UpdateStatus() {
	if(GetTrigger() < 0) {
		Level = -1;
		return;
	}

	bool st = Monitor::GetKey((unsigned int) GetTrigger()) > 0;

	switch(opts->Selected()) {
		case 0: //On Press
			Level = st? 0 : -1;
			break;

		case 1: //On Release
			Level = st? -1 : 0;
			break;

		case 2: //Toggle
			Level = (st != last_status)? (Level - (int) st) % 2 : Level;
			break;

		default:
			break;
	}

	last_status = st;
}

Mute::~Mute()
{
	delete opts;
	delete close;
	delete collapse;
}

void Mute::Click() {
	if(Button::CurrentButton == close)
		Close();
	else if(Button::CurrentButton == collapse) {
		if (IsCollapsed())
			Expand();
		else
			Collapse();
	}
}

void Mute::OnMuteChange(RackUnit* unit) {
	unit->UpdateStatus();
}