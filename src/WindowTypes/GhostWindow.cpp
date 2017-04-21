//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    GhostWindow.cpp									//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WindowTypes.h"

GhostWindow::GhostWindow() {}

GhostWindow::~GhostWindow() {}

bool GhostWindow::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p) {
	return false;
}

void GhostWindow::Paint(wxGraphicsContext *gc) {
	if(Parent == NULL)
		return;

	Parent->DrawBackground(gc);
	if(Parent->AlphaBackground == 0 && CurrentWindow == this)
		CurrentWindow = NULL;
}
int GhostWindow::Click() {
	return -1;
}

void GhostWindow::Hide(Window* w) {
	Parent = w;
	Parent->AlphaBackground = 200;
	Animation::Add(&(Parent->AlphaBackground), 200, 0);
	CurrentWindow = this;
}