//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    MIDISettings.cpp									//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WindowTypes.h"
#include "../RackUnits.h"

MidiSettings::MidiSettings(wxFrame* owner)
{
	Parent = owner;
	channel = Rack::GetChannel();
	area = wxRect2DDouble(0, 0, _DEFAULT_W_*2.3, 80);
	area.SetCentre(wxPoint2DDouble(Parent->GetClientSize().GetWidth()*0.5, Parent->GetClientSize().GetHeight()*0.5));
	bt_ok = new Button(wxString("Ok"), area.m_width - _DEFAULT_W_, area.m_height - _DEFAULT_H_, NO_FILL, BTN_MIDI_SETT);
	bt_cancel = new Button(wxString("Cancel"), area.m_width - _DEFAULT_W_*2.3, area.m_height - _DEFAULT_H_, NO_FILL, BTN_MIDI_SETT);
	bt_left = new Button(wxString("<"), area.m_width*0.6, _DEFAULT_H_*0.25, NO_BLUR | NO_BORDER, BTN_MIDI_SETT);
	bt_right = new Button(wxString(">"), area.m_width*0.6 + _DEFAULT_H_*0.5 + 3*_FONT_SIZE_,
		_DEFAULT_H_*0.25, TRANSPARENT | NO_BLUR | NO_BORDER, BTN_MIDI_SETT);
	bt_left->SetSize(_DEFAULT_R_, _DEFAULT_R_);
	bt_right->SetSize(_DEFAULT_R_, _DEFAULT_R_);
	if(channel == 0)
		bt_left->Disable();
	else if(channel == 15)
		bt_right->Disable();
}

MidiSettings::~MidiSettings()
{
	delete bt_ok;
	delete bt_cancel;
	delete bt_left;
	delete bt_right;
}

void MidiSettings::Paint(wxGraphicsContext* gc)
{
	wxString str;
	wxDouble w, h;
	DrawBackground(gc);
	DrawContainer(gc, wxString("MIDI Settings"), area);
	gc->PushState();
	gc->Translate(area.m_x, area.m_y);
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), L1);
	str.Printf("%d", channel);
	gc->GetTextExtent(str, &w, &h, NULL, NULL);
	gc->DrawText(wxString("MIDI Channel: "), area.m_width*0.1, (_DEFAULT_H_ - h)*0.5);
	gc->DrawText(str, bt_left->GetCentre().m_x + (bt_right->GetCentre().m_x - bt_left->GetCentre().m_x - w)*0.5, (_DEFAULT_H_ - h)*0.5);
	bt_left->Paint(gc);
	bt_right->Paint(gc);
	bt_ok->Paint(gc);
	bt_cancel->Paint(gc);
	gc->PopState();
}

bool MidiSettings::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p_w)
{
	if(!area.Contains(p_w))
		return false;

	wxPoint2DDouble p(p_w.m_x - area.m_x, p_w.m_y - area.m_y);

	if(bt_left->IsEnabled() && bt_left->Contains(p, gc)) {
		bt_left->SetStyle(LIGHT_THEME | NO_BLUR | NO_BORDER);
		return true;
	}

	bt_left->SetStyle(NO_BLUR | NO_BORDER);

	if(bt_right->IsEnabled() && bt_right->Contains(p, gc)) {
		bt_right->SetStyle(LIGHT_THEME | NO_BLUR | NO_BORDER);
		return true;
	}

	bt_right->SetStyle(NO_BLUR | NO_BORDER);

	return bt_ok->Contains(p, gc) || bt_cancel->Contains(p, gc);
}

int MidiSettings::Click()
{
	if(Button::LastClicked == bt_ok) {
		Rack::SetChannel(channel);
		Hide();
		return 0;
	} else if (Button::LastClicked == bt_cancel)
		Hide();
	else if(Button::LastClicked == bt_left) {
		channel = channel > 0? channel - 1 : 0;
		if(channel == 0) {
			bt_left->SetStyle(NO_BLUR | NO_BORDER);
			bt_left->Disable();
		}
		bt_right->Enable();
	} else if(Button::LastClicked == bt_right) {
		channel = channel < 15? channel + 1 : 15;
		if(channel == 15) {
			bt_right->SetStyle(NO_BLUR | NO_BORDER);
			bt_right->Disable();
		}
		bt_left->Enable();
	}
	return -1;
}

