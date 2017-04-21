//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    Rack.cpp											//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../RackUnits.h"

#define MAX_PRIORITY	3

list<RackUnit*> Rack::ActiveUnits;
list<RackUnit*> Rack::AvailableUnits;
int Rack::Note = -1;
int Rack::Level = -1;
int Rack::Bend = 0x2000;
unsigned int Rack::Channel = 0;
RackManager* Rack::rack_man = NULL;
RtMidiOut* Rack::MidiOut = NULL;
vector<unsigned char> Rack::message;
RackUnit* Rack::Handlers[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
Rack::Rack(const wxRect2DDouble& a)
{
	const char *s[] = { "Show Rack Manager" };
	area = a;
	offset = 0;
	total_h = 0;
	scr = Scrollbar(wxRect2DDouble(area.m_width - _DEFAULT_H_, _DEFAULT_R_, _DEFAULT_R_, area.m_height - _DEFAULT_H_), true);
	scr.SetVisible(false);
	r_click = new ContextMenu(0, 0, 1, s, false, CTX_RIGHT_CLICK, LIGHT_THEME);
	r_click->SetOnChangeFunction((void (*)(void*)) Rack::OnCTXChange, this);
	MidiOut = new RtMidiOut(RtMidi::LINUX_ALSA, string("WiiMIDI"));
	MidiOut->openVirtualPort("WiiMIDI");
	AvailableUnits.push_back((RackUnit*) new PitchController(area.m_width));
	AvailableUnits.push_back((RackUnit*) new MasterVolume(area.m_width));
	AvailableUnits.push_back((RackUnit*) new Mute(area.m_width));
	AvailableUnits.push_back((RackUnit*) new Drone(area.m_width));
}

Rack::~Rack()
{
	list<RackUnit*>::reverse_iterator i;
	ActiveUnits.clear();
	for(i = AvailableUnits.rbegin(); i != AvailableUnits.rend(); ++i)
		delete (*i);
	MidiOut->closePort();
	delete MidiOut;
}

void Rack::InitMidiOut() {
	unsigned int pitch_bend_course = _PB_TONE_QTS_ % 2;
	unsigned int pitch_bend_fine = (_PB_TONE_QTS_*50) % 100;

	message.clear();
	message.push_back((unsigned char) (0xB0 + Channel));
	message.push_back((unsigned char) 126);
	message.push_back((unsigned char) 1);
	MidiOut->sendMessage(&message);
	message[0] = (unsigned char) (0xB0 + Channel);
	message[1] = (unsigned char) 0x64;
	message[2] = (unsigned char) 0x00;
	MidiOut->sendMessage(&message);
	message[0] = (unsigned char) (0xB0 + Channel);
	message[1] = (unsigned char) 0x65;
	message[2] = (unsigned char) 0x00;
	MidiOut->sendMessage(&message);
	message[0] = (unsigned char) (0xB0 + Channel);
	message[1] = (unsigned char) 0x06;
	message[2] = (unsigned char) pitch_bend_course;
	MidiOut->sendMessage(&message);
	message[0] = (unsigned char) (0xB0 + Channel);
	message[1] = (unsigned char) 0x26;
	message[2] = (unsigned char) pitch_bend_fine;
	MidiOut->sendMessage(&message);
	message[0] = (unsigned char) (0xB0 + Channel);
	message[1] = (unsigned char) 0x44;
	message[2] = (unsigned char) 0x40;
	MidiOut->sendMessage(&message);
}

unsigned int Rack::GetChannel() { return Channel; }

void Rack::SetChannel(unsigned int c) { Channel = c < 0? Channel : c > 15? Channel : c; }

void Rack::SetHandler(const int& key, RackUnit* unit)
{
	if(key >= 0 && key < 17)
		if(Handlers[key] != NULL)
			Handlers[key]->ResetTrigger();
		Handlers[key] = unit;
}

RackUnit* Rack::GetHandler(const int& key)
{
	if(key >= 0 && key < 17)
		return Handlers[key];
	return NULL;
}

void Rack::Update(const int& key)
{
	if(key >= 0 && key < 17 && Handlers[key] != NULL)
		Handlers[key]->UpdateStatus();
}

void Rack::RemoveAll()
{
	list<RackUnit*>::reverse_iterator i;
	for(i = ActiveUnits.rbegin(); i != ActiveUnits.rend(); ++i)
		(*i)->Close();
}

wxDouble Rack::GetSize() {
	list<RackUnit*>::iterator i;
	wxDouble tot = 0;
	for(i = ActiveUnits.begin(); i != ActiveUnits.end(); i++)
		tot += (*i)->GetHeight();
	return tot;
}

void Rack::Paint(wxGraphicsContext* gc)
{
	wxDouble y = 0;
	total_h = GetSize();
	scr.SetVisible(total_h > area.m_height);
	if(!scr.IsVisible())
		scr.ResetPosition();
	offset = scr.IsVisible()? scr.GetPercentage()*(area.m_height - total_h) : 0;
	gc->PushState();
	gc->Translate(area.m_x, area.m_y + offset);
	list<RackUnit*>::iterator i;
	for(i = ActiveUnits.begin(); i != ActiveUnits.end(); i++) {
		(*i)->SetYPosition(y);
		if(!RackUnit::IsDragging() || RackUnit::DraggingUnit != *i)
			(*i)->Paint(gc);
		y += (*i)->GetHeight();
	}
	if(RackUnit::IsDragging())
		RackUnit::DraggingUnit->Paint(gc);
	gc->Translate(0, -offset);
	if(scr.IsVisible()) {
		wxColour tr(D1.Red(), D1.Green(), D1.Blue(), 0);
		gc->SetPen(wxNullPen);
		if(scr.GetPercentage() > 0) {
			gc->SetBrush(gc->CreateLinearGradientBrush(0, 0, 0, _FADE_DIM_, D1, tr));
			gc->DrawRectangle(0, 0, area.m_width, _FADE_DIM_);
		}
		if(scr.GetPercentage() < 1) {
			gc->SetBrush(gc->CreateLinearGradientBrush(0, area.m_height - _FADE_DIM_, 0, area.m_height, tr, D1));
			gc->DrawRectangle(0, area.m_height - _FADE_DIM_, area.m_width, _FADE_DIM_);
		}
	}
	scr.Paint(gc);
	gc->PopState();
}

bool Rack::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p_w)
{
	wxPoint2DDouble p(p_w.m_x - area.m_x, p_w.m_y - area.m_y);
	wxPoint2DDouble p_off(p.m_x, p.m_y - offset);
	list<RackUnit*>::iterator i;

	if(area.Contains(p_w) && !(scr.IsVisible() && scr.Contains(gc, p)))
		for(i = ActiveUnits.begin(); i != ActiveUnits.end(); i++)
			if((*i)->Contains(gc, p_off))
				return true;
	RackUnit::Current = NULL;
	return false;
}

void Rack::MouseWheel(int wheel) {
	if(scr.IsVisible()) {
		if(wheel < 0)
			scr.IncreasePosition();
		else if(wheel > 0)
			scr.DecreasePosition();
	}
}

void Rack::Remove(RackUnit* unit)
{
	ActiveUnits.remove(unit);
	unit->Reset();
}

void Rack::Add(RackUnit* unit) {
	if(unit != NULL)
		ActiveUnits.push_back(unit);
}

void Rack::SendMessage() {
	int nt = -1, bn = -1, lv = -1;
	for(int priority = 0; priority <= MAX_PRIORITY; priority++)
		for(int i = 0; i < 17; i++)
			if(Handlers[i] != NULL && Handlers[i]->Priority == priority) {
				nt = Handlers[i]->GetNote() < 0? nt : Handlers[i]->GetNote();
				bn = Handlers[i]->GetBend() < 0? bn : Handlers[i]->GetBend();
				lv = Handlers[i]->GetLevel() < 0? lv : Handlers[i]->GetLevel();
			}

	if(lv >= 0 && lv != Level)
		Level = lv;
	else if(lv < 0 && Level != 0x64)
		Level = 0x64;

	if(bn >= 0 && bn != Bend) {
		Bend = bn;
		message[0] = (unsigned char) (0xE0 + Channel);
		message[1] = (unsigned char) (Bend & 0x003F);
		message[2] = (unsigned char) (Bend >> 7);
		MidiOut->sendMessage(&message);
	} else if(bn < 0 && Bend != 0x2000) {
		Bend = 0x2000;
		message[0] = (unsigned char) (0xE0 + Channel);
		message[1] = (unsigned char) (Bend & 0x003F);
		message[2] = (unsigned char) (Bend >> 7);
		MidiOut->sendMessage(&message);
	}

	if(nt >= 0 && nt != Note) {
		message[0] = (unsigned char) (0x90 + Channel);
		message[1] = (unsigned char) nt;
		message[2] = (unsigned char) Level;
		MidiOut->sendMessage(&message);
		if(Note != -1) {
			message[0] = (unsigned char) (0x90 + Channel);
			message[1] = (unsigned char) Note;
			message[2] = (unsigned char) 0;
			MidiOut->sendMessage(&message);
		}
		Note = nt;
	} else if(Note >= 0 && Level >= 0) {
		message[0] = (unsigned char) (0x90 + Channel);
		message[1] = (unsigned char) Note;
		message[2] = (unsigned char) Level;
		MidiOut->sendMessage(&message);
		if(nt < 0)
			Note = -1;	
	}
}

ContextMenu* Rack::GetCTX() {
	if(RackUnit::Current != NULL && RackUnit::Current->GetCTX() != NULL)
		return RackUnit::Current->GetCTX();
	return r_click;
}

void Rack::OnCTXChange(Rack* r) {
	if(r == NULL)
		return;
	switch(r->GetCTX()->GetLastSelected()) {
		case 0:
			if(rack_man != NULL)
				rack_man->Show();
			break;

		default:
			break;
	}
}