//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    PitchController.cpp								//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../RackUnits.h"

PitchController::PitchController(const wxDouble& width, const wxDouble& height)
{
	Height = height;
	Width = width;
	wxRect2DDouble rect(450, 60 - _DEFAULT_R_*1.5, 250, _DEFAULT_R_);
	sens = new Scrollbar(rect, false);
	rect.m_y += _DEFAULT_R_*2;
	vel = new Scrollbar(rect, false);
	vel->SetPercentage(0.5);
	sens->SetPercentage(0.5);
	Name = wxString(" Pitch Controller ");
	close = new Button(wxString(wxT("✕")), 0, 0, TRANSPARENT | NO_BLUR | NO_BORDER, BTN_RACK_UNIT);
	collapse = new Button(wxString(wxT("▽")), 0, 0, TRANSPARENT | NO_BLUR | NO_BORDER, BTN_RACK_UNIT);
	close->SetSize(_DEFAULT_R_, _DEFAULT_R_);
	collapse->SetSize(_DEFAULT_R_, _DEFAULT_R_);
	InitTriggers(false);
	OffsetY = 0;
}

void PitchController::Paint(wxGraphicsContext *gc)
{
	gc->PushState();
	gc->Translate(OffsetDD.m_x, OffsetDD.m_y + OffsetPos);
	DrawUnitContainer(gc);
	gc->Clip(0, GetUnitPosition(), Width, GetHeight());
	gc->Translate(0, Position + TitleBarWidth - OffsetY);
	DrawTriggerMenu(gc);
	DrawBox(gc, wxString(" Pitch "), 250, 80);
	DrawBox(gc, wxString(" Settings "), 350, 370);
	gc->DrawText("Sensitivity: ", 350 + _DEFAULT_R_, 60 - _DEFAULT_R_ - _FONT_SIZE_*0.85);
	gc->DrawText("Velocity: ", 350 + _DEFAULT_R_, 60 + _DEFAULT_R_ - _FONT_SIZE_*0.85);
	DrawCurrentNote(gc, wxRect2DDouble(265, 35, 50, 50));
	sens->Paint(gc);
	vel->Paint(gc);
	gc->Translate(0, OffsetY - Position - TitleBarWidth);
	gc->ResetClip();
	DrawFading(gc);
	gc->PopState();
}

bool PitchController::Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p_w)
{
	wxPoint2DDouble p(p_w.m_x, p_w.m_y - Position - TitleBarWidth);
	return sens->Contains(gc, p) || vel->Contains(gc, p) || RackUnit::Contains(gc, p_w);
}


void PitchController::UpdateStatus() {
	if(GetTrigger() < 0) {
		Note = -1;
		Bend = -1;
		return;
	}

	wxDouble data;
	int bd;

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
			Note = -1;
			Bend = -1;
			return;
	}

	data = -data;
	Note = Note < 0? 60 : Note;
	Bend = Bend < 0? 0x2000 : Bend;

	if(wxAbs(data) >= (1 - sens->GetPercentage())*0.5) {
		data -= data*sens->GetPercentage()*0.5;
		bd = Bend + (int) round(data*(0.2 + vel->GetPercentage()*0.8)*1000.0);
		Bend = bd % 0x3FFF;
		Note += (bd / 0x3FFF)*_PB_TONE_QTS_;
		if(Bend < 0) {
			Bend += 0x3FFF;
			Note -= _PB_TONE_QTS_;
		}
		SetNote(Note);
		SetBend(Bend); 
	}
}

PitchController::~PitchController()
{
	delete sens;
	delete vel;
	delete close;
	delete collapse;
}

void PitchController::DrawCurrentNote(wxGraphicsContext *gc, const wxRect2DDouble& area)
{	
	int n_qts = 0x2000/_PB_TONE_QTS_;
	int delta_n = Bend*_PB_TONE_QTS_/0x3FFF - (_PB_TONE_QTS_ + 1)/2;
	int n = (Note + delta_n) % 12;
	int octave = (Note + delta_n)/12 - 1; 
	if(n < 0) {
		n += 12;
		octave--;
	}
	int b_norm = (Bend % (2*n_qts)) - n_qts; 
	bool H, HL, HR, M, LL, LR, L, sharp = true;
	wxDouble length = fmin(area.m_height*0.5, area.m_width*0.5);
	wxGraphicsPath arrow_up, arrow_down, path;
	wxGraphicsMatrix mat;
	wxString oct;
	wxColour col_up, col_down, col_center;

	if(b_norm > 0.1*n_qts) {
		col_up = L3;
		col_down = D2;
		col_center = D2;
	} else if(b_norm < -0.1*n_qts) {
		col_up = D2;
		col_down = L3;
		col_center = D2;
	} else {
		col_up = D2;
		col_down = D2;
		col_center = L3;
	}
	
	oct.Printf("%d", octave);

	switch ((note_t) n)
	{
		case C:
			sharp = false;
		case Csh:
			H = true;
			HL = true;
			HR = false;
			M = false;
			LL = true;
			LR = false;
			L = true;
		break;

		case D:
			sharp = false;
		case Dsh:
			H = true;
			HL = true;
			HR = true;
			M = false;
			LL = true;
			LR = true;
			L = true;
		break;

		case E:
			sharp = false;
			H = true;
			HL = true;
			HR = false;
			M = true;
			LL = true;
			LR = false;
			L = true;
		break;

		case F:
			sharp = false;
		case Fsh:
			H = true;
			HL = true;
			HR = false;
			M = true;
			LL = true;
			LR = false;
			L = false;
		break;

		case G:
			sharp = false;
		case Gsh:
			H = true;
			HL = true;
			HR = false;
			M = false;
			LL = true;
			LR = true;
			L = true;
		break;

		case A:
			sharp = false;
		case Ash:
			H = true;
			HL = true;
			HR = true;
			M = true;
			LL = true;
			LR = true;
			L = false;
		break;

		case B:
			sharp = false;
			H = true;
			HL = true;
			HR = true;
			M = true;
			LL = true;
			LR = true;
			L = true;
		break;
	}

	gc->PushState();
	gc->Translate(area.m_x + (area.m_width - length)*0.5, area.m_y + (area.m_height - length*2));
	path = gc->CreatePath();
	mat = gc->CreateMatrix();
	gc->SetFont(wxFontInfo(_FONT_SIZE_ + 2).FaceName(_FONT_NAME_).Italic(), sharp? L3 : D2);
	gc->DrawText(wxString("#"), length*1.1, 0);
	gc->SetFont(wxFontInfo(_FONT_SIZE_ + 2).FaceName(_FONT_NAME_), L3);
	gc->DrawText(oct, length*1.1, length*1.8);
	arrow_up = gc->CreatePath();
	arrow_down = gc->CreatePath();
	arrow_up.MoveToPoint(-length*0.5, 0);
	arrow_up.AddLineToPoint(-length*0.75, length*0.2);
	arrow_up.AddLineToPoint(-length*0.25, length*0.2);
	arrow_up.CloseSubpath();
	arrow_down.MoveToPoint(-length*0.5, length*2);
	arrow_down.AddLineToPoint(-length*0.75, length*1.8);
	arrow_down.AddLineToPoint(-length*0.25, length*1.8);
	arrow_down.CloseSubpath();
	gc->SetBrush(col_up);
	gc->FillPath(arrow_up);
	gc->SetBrush(col_down);
	gc->FillPath(arrow_down);
	gc->SetBrush(col_center);
	gc->DrawEllipse(-length*0.75, length*0.75, length*0.5, length*0.5);

	path.MoveToPoint(length*0.01, 0);
	path.AddLineToPoint(length*0.06, -length*0.05);
	path.AddLineToPoint(length*0.94, -length*0.05);
	path.AddLineToPoint(length*0.99, 0);
	path.AddLineToPoint(length*0.94, length*0.05);
	path.AddLineToPoint(length*0.06, length*0.05);
	path.CloseSubpath();
	gc->SetBrush(H? L3 : D2);
	gc->FillPath(path);
	gc->Translate(0, length);
	gc->SetBrush(M? L3 : D2);
	gc->FillPath(path);
	gc->Translate(0, length);
	gc->SetBrush(L? L3 : D2);
	gc->FillPath(path);
	gc->Rotate(-M_PI*0.5);
	gc->SetBrush(LL? L3 : D2);
	gc->FillPath(path);
	gc->Translate(length, 0);
	gc->SetBrush(HL? L3 : D2);
	gc->FillPath(path);
	gc->Translate(0, length);
	gc->SetBrush(HR? L3 : D2);
	gc->FillPath(path);
	gc->Translate(-length, 0);
	gc->SetBrush(LR? L3 : D2);
	gc->FillPath(path);
	gc->PopState();
}

void PitchController::Click() {
	if(Button::CurrentButton == close)
		Close();
	else if(Button::CurrentButton == collapse) {
		if (IsCollapsed())
			Expand();
		else
			Collapse();
	}
}