//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    ContextMenu.cpp									//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WiiMIDI_GUI.h"

ContextMenu* ContextMenu::CurrentMenu = NULL;
bool ContextMenu::IsActive = false;

ContextMenu::ContextMenu(const wxDouble& x, const wxDouble& y, const int n, const char** label,
		bool animated, unsigned int type, unsigned int style, const wxDouble& w, int def)
{
    Animated = animated;
    MenuType = type;
    HadDoneAnimation = false;
	mat = wxGraphicsMatrix();
	LastSelected = def;
	bts = new Button*[n];
	bts_num = n;
	IsOn = -1;
	wxDouble width = w;
	wxDouble height = _DEFAULT_H_*0.7;
    X = x;
    Y = y;
    Width = width;
    Height = height*n;
	for(int i = 0; i < n; i++)
	{
		bts[i] = new Button(wxString(label[i]), 0, height*i, RECT_SHAPED | NO_BORDER | style, BTN_CTX_MENU);
		bts[i]->SetSize(width, height);
	}
	scr = new Scrollbar(wxRect2DDouble(width*0.9, 0, width*0.1, height), true);
}

ContextMenu::~ContextMenu()
{
	for(int i = 0; i < bts_num; i++)
		delete bts[i];
	delete[] bts;
	delete scr;
}

void ContextMenu::Paint(wxGraphicsContext *gc)
{
    if (CurrentMenu != NULL && IsActive) {
		wxDouble gc_w, gc_h, off;
		gc->PushState();
		wxGraphicsMatrix m(CurrentMenu->mat);
		gc->GetSize(&gc_w, &gc_h);
		if(!m.IsNull()) {
			m.Invert();
			m.TransformPoint(&gc_w, &gc_h);
			gc->SetTransform(CurrentMenu->mat);
		}
		CurrentMenu->OffsetScr = std::fmax(CurrentMenu->Y + CurrentMenu->Height - gc_h + 1, 0);
		CurrentMenu->scr->SetArea(wxRect2DDouble(CurrentMenu->Width*0.85, 5, CurrentMenu->Width*0.1,
				CurrentMenu->Height - CurrentMenu->OffsetScr - 10));
		CurrentMenu->scr->SetVisible(CurrentMenu->OffsetScr > 0 && CurrentMenu->OffsetScr > CurrentMenu->OffsetY);
		off = CurrentMenu->scr->GetPercentage()*CurrentMenu->OffsetScr;
		gc->Clip(CurrentMenu->X, CurrentMenu->Y, CurrentMenu->Width, CurrentMenu->Height - CurrentMenu->OffsetScr);
        gc->Translate(CurrentMenu->X, CurrentMenu->Y - CurrentMenu->OffsetY - off);
        for (int i = 0; i < CurrentMenu->bts_num; i++)
            CurrentMenu->bts[i]->Paint(gc);
		gc->ResetClip();
		gc->Translate(0, CurrentMenu->OffsetY + off);
		if(CurrentMenu->scr->IsVisible()) {
			wxColour tr(D1.Red(), D1.Green(), D1.Blue(), 0);
			gc->SetPen(wxNullPen);
			if(CurrentMenu->scr->GetPercentage() > 0) {
				gc->SetBrush(gc->CreateLinearGradientBrush(0, 0, 0, _FADE_DIM_*0.5, D1, tr));
				gc->DrawRectangle(0, 0, CurrentMenu->Width, _FADE_DIM_*0.5);
			}
			if(CurrentMenu->scr->GetPercentage() < 1) {
				gc->SetBrush(gc->CreateLinearGradientBrush(0, CurrentMenu->Height - CurrentMenu->OffsetScr - _FADE_DIM_*0.5, 0,
						CurrentMenu->Height - CurrentMenu->OffsetScr, tr, D1));
				gc->DrawRectangle(0, CurrentMenu->Height - CurrentMenu->OffsetScr - _FADE_DIM_*0.5, CurrentMenu->Width, _FADE_DIM_*0.5);
			}
		}
		wxGraphicsPath p = gc->CreatePath();
		p.MoveToPoint(0, 0);
		p.AddLineToPoint(0, std::fmin(CurrentMenu->Height - CurrentMenu->OffsetY, CurrentMenu->Height - CurrentMenu->OffsetScr));
		p.AddLineToPoint(CurrentMenu->Width, std::fmin(CurrentMenu->Height - CurrentMenu->OffsetY,
				CurrentMenu->Height - CurrentMenu->OffsetScr));
		p.AddLineToPoint(CurrentMenu->Width, 0);
		gc->SetPen(wxPen(L1));
		gc->StrokePath(p);
		CurrentMenu->scr->Paint(gc);
        gc->PopState();
    }
}

int ContextMenu::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p_w)
{
	wxPoint2DDouble p(p_w.m_x - X, p_w.m_y - Y), p_off;
	wxGraphicsMatrix m(mat);

	if(!m.IsNull()) {
		m.Invert();
		m.TransformPoint(&p.m_x, &p.m_y);
	}
	p_off = wxPoint2DDouble(p.m_x, p.m_y + OffsetScr*scr->GetPercentage());
	IsOn = -1;
	if(!scr->Contains(gc, p))
		for(int i = 0; i < bts_num; i++)
			if(bts[i]->Contains(p_off, gc))
			{
				IsOn = i;
				break;
			}

	return IsOn;
}

int ContextMenu::Click()
{
	if(CurrentMenu != NULL && CurrentMenu->IsOn != -1)
    	CurrentMenu->LastSelected = CurrentMenu->IsOn;
	else
		return -1;
	OnChange();
    return CurrentMenu->LastSelected;
}

void ContextMenu::PopUp()
{
	if(CurrentMenu != NULL && CurrentMenu->Animated && !CurrentMenu->HadDoneAnimation) {
		CurrentMenu->OffsetY = CurrentMenu->Height;
		Animation::Add(&(CurrentMenu->OffsetY), CurrentMenu->Height, 0);
		CurrentMenu->HadDoneAnimation = true;
	}
}

void ContextMenu::SetCurrent()
{
    if(CurrentMenu != this && CurrentMenu != NULL)
        CurrentMenu->HadDoneAnimation = false; 
    CurrentMenu = this;
    Activate();
}

Button* ContextMenu::GetButton(int i)
{
    if (i >= 0 && i < bts_num)
        return bts[i];
    else
        return NULL;
}

void ContextMenu::Deactivate()
{
    if (CurrentMenu != NULL && CurrentMenu->Animated)
        CurrentMenu->HadDoneAnimation = false;
    CurrentMenu = NULL;
	IsActive = false;
}

void ContextMenu::Toggle()
{
    if(IsActive)
        Deactivate();
	else
		Activate();
}

void ContextMenu::Activate() {
	PopUp();
	IsActive = true;
}

void ContextMenu::OnChange() {
	if(CurrentMenu != NULL && CurrentMenu->custom_fn != NULL)
		CurrentMenu->custom_fn(CurrentMenu->fn_arg);
}

void ContextMenu::SetDefault() {
	LastSelected = -1;
	if(custom_fn != NULL)
		custom_fn(fn_arg);
}

void ContextMenu::SetLocation(const wxPoint2DDouble &p, unsigned int corner) {
	switch(corner) {
		case CTX_LEFT_UP:
			X = p.m_x;
			Y = p.m_y;
			break;

		case CTX_RIGHT_UP:
			X = p.m_x - Width;
			Y = p.m_y;
			break;

		case CTX_LEFT_DOWN:
			X = p.m_x;
			Y = p.m_y - Height;
			break;

		case CTX_RIGHT_DOWN:
			X = p.m_x - Width;
			Y = p.m_y - Height;
			break;

		default:
			break;
	}
}
