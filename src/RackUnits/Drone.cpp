//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    Drone.cpp											//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../RackUnits.h"

Drone::Drone(const wxDouble& width, const wxDouble& height)
{
	Priority = 2;
	Height = height;
	Width = width;
	Name = wxString(" Drone ");
	close = new Button(wxString(wxT("✕")), 0, 0, TRANSPARENT | NO_BLUR | NO_BORDER, BTN_RACK_UNIT);
	collapse = new Button(wxString(wxT("▽")), 0, 0, TRANSPARENT | NO_BLUR | NO_BORDER, BTN_RACK_UNIT);
	close->SetSize(_DEFAULT_R_, _DEFAULT_R_);
	collapse->SetSize(_DEFAULT_R_, _DEFAULT_R_);
	InitTriggers(true);
	const char* notes[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
	const char* octaves[] = { "-1", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
	note = new DropdownMenu(315, (Height - _DEFAULT_H_)*0.5, 12, notes, _DEFAULT_W_*0.7, 0);
	octave = new DropdownMenu(475, (Height - _DEFAULT_H_)*0.5, 12, octaves, _DEFAULT_W_*0.7, 5);
	note->SetOnChangeFunction((void (*)(void*)) Drone::OnNoteChange, (void*) this);
	octave->SetOnChangeFunction((void (*)(void*)) Drone::OnNoteChange, (void*) this);
	OffsetY = 0;
}

void Drone::Paint(wxGraphicsContext *gc)
{
	wxDouble w, h, i1, i2;
	gc->PushState();
	gc->Translate(OffsetDD.m_x, OffsetDD.m_y + OffsetPos);
	DrawUnitContainer(gc);
	gc->Clip(0, GetUnitPosition(), Width, GetHeight());
	gc->Translate(0, Position + TitleBarWidth - OffsetY);
	DrawTriggerMenu(gc);
	DrawBox(gc, wxString(" Drone Note "), 250, 340);
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), L1);
	gc->GetTextExtent("Note: ", &w, &h, &i1, &i2);
	gc->DrawText(wxString("Note: "), 280, (Height - h)*0.5);
	gc->GetTextExtent("Octave: ", &w, &h, &i1, &i2);
	gc->DrawText(wxString("Octave: "), 430, (Height - h)*0.5);
	note->Paint(gc);
	octave->Paint(gc);
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), L1);
	gc->GetTextExtent("Activity", &w, &h, &i1, &i2);
	gc->DrawText("Activity", 620, (Height - h)*0.5);
	gc->SetPen(L1);
	gc->StrokeLine(620 + w + 4, Height*0.5, _LED_POS_ - _LED_R_ - 4, Height*0.5);
	if(Note >= 0) {
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

bool Drone::Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p_w)
{
	wxPoint2DDouble p(p_w.m_x, p_w.m_y - Position - TitleBarWidth);
	return note->Contains(gc, p) || octave->Contains(gc, p) || RackUnit::Contains(gc, p_w);
}

void Drone::UpdateStatus() {
	if(GetTrigger() < 0 || note->GetSelected() < 0 || octave->GetSelected() < 0 ||
			Monitor::GetKey((unsigned int) GetTrigger()) == 0) {
		Note = -1;
		Bend = -1;
		return;
	}

	Note = octave->GetSelected()*12 + note->GetSelected();
	Bend = 0x2000;
}

Drone::~Drone()
{
	delete note;
	delete octave;
	delete close;
	delete collapse;
}

void Drone::Click() {
	if(Button::CurrentButton == close)
		Close();
	else if(Button::CurrentButton == collapse) {
		if (IsCollapsed())
			Expand();
		else
			Collapse();
	}
}

void Drone::OnNoteChange(RackUnit* unit) {
	unit->UpdateStatus();
}