//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    MenuPanel.cpp										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WiiMIDI_GUI.h"

MenuPanel::MenuPanel(wxFrame *own)
{
    owner = own;
	width = owner->GetSize().GetWidth();
	ctrl = new Button(wxString("Controller"), width - _DEFAULT_W_*3, 0, RECT_SHAPED | LIGHT_THEME | NO_BORDER | BOLD_FONT, BTN_CTRL);
	rack = new Button(wxString("Rack"), width - _DEFAULT_W_*2, 0, RECT_SHAPED | LIGHT_THEME | NO_BORDER | BOLD_FONT, BTN_RACK);
	settings = new Button(wxString("Settings"), width - _DEFAULT_W_, 0, RECT_SHAPED | LIGHT_THEME | NO_BORDER | BOLD_FONT, BTN_SETT);
	wxImage::AddHandler(new wxPNGHandler);
	wxImage img;
	img.LoadFile("./img/WiiMIDI_30px.png");
	logo = new wxBitmap(img);
	const char* s1[] = { "Connect Controller...", "Disconnect Controller" };
	const char* s2[] = { "Add/Remove Units", "Remove All Units" };
	const char* s3[] = { "Controller Settings", "Midi I/O" };
	ctrl_menu = new ContextMenu(width - _DEFAULT_W_*3.5, _PANEL_H_, 2, s1, true, CTX_MENU_PANEL);
	rack_menu = new ContextMenu(width - _DEFAULT_W_*2.5, _PANEL_H_, 2, s2, true, CTX_MENU_PANEL);
	sett_menu = new ContextMenu(width - _DEFAULT_W_*1.5, _PANEL_H_, 2, s3, true, CTX_MENU_PANEL);
    ctrl->SetDependingContextMenu(ctrl_menu);
    rack->SetDependingContextMenu(rack_menu);
    settings->SetDependingContextMenu(sett_menu);
    ctrl_menu->GetButton(0)->SetID(BTN_CTRL_0);
    ctrl_menu->GetButton(1)->SetID(BTN_CTRL_1);
    rack_menu->GetButton(0)->SetID(BTN_RACK_0);
    rack_menu->GetButton(1)->SetID(BTN_RACK_1);
    sett_menu->GetButton(0)->SetID(BTN_SETT_0);
    sett_menu->GetButton(1)->SetID(BTN_SETT_1);
	ctrl_menu->GetButton(1)->Disable();
	rack_menu->GetButton(1)->Disable();
}

MenuPanel::~MenuPanel()
{
	delete ctrl;
	delete rack;
	delete settings;
	delete logo;
	delete ctrl_menu;
	delete rack_menu;
	delete sett_menu;
}

bool MenuPanel::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p)
{
    return ctrl->Contains(p, gc) || rack->Contains(p, gc) || settings->Contains(p, gc);
}

void MenuPanel::Paint(wxGraphicsContext *gc)
{
	gc->SetBrush(wxBrush(L1));
	gc->SetPen(wxPen(L1));
	gc->DrawRectangle(0, 0, width, _PANEL_H_);
	gc->DrawBitmap(*logo, 30, 5, 100, 20);
	ctrl->Paint(gc);
	rack->Paint(gc);
	settings->Paint(gc);
}

ContextMenu* MenuPanel::GetMenu(int i)
{
    switch (i)
    {
        case 0:
            return ctrl_menu;

        case 1:
            return rack_menu;

        case 2:
            return sett_menu;

        default:
            return NULL;
    }
}