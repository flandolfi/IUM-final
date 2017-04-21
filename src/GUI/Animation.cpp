//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    Animation.cpp										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include <valarray>
#include "../WiiMIDI_GUI.h"

list<Animation*> Animation::Animations;
wxTimer* Animation::Timer = NULL;

Animation::Animation(wxDouble* v, const wxDouble& st, const wxDouble& e, const wxDouble& sp)
{
	Value = v;
	StartsWith = st;
	EndsWith = e;
	speed = sp;
	t = 0;
	queued = NULL;
}

Animation* Animation::Find(wxDouble* val)
{
	list<Animation*>::iterator i;
	bool found = false;
	for(i = Animations.begin(); i != Animations.end(); i++)
		if((*i)->Value == val) {
			found = true;
			break;
		}
	return found? (*i) : NULL;
}

Animation* Animation::Add(wxDouble *val, const wxDouble& start, const wxDouble& end, const wxDouble& speed)
{
	Animation* ptr = Find(val);
	if (ptr == NULL)
	{
		Animation* anim = new Animation(val, start, end, speed);
        Animations.push_back(anim);
		ptr = anim;
	} else {
		ptr->StartsWith = start;
		ptr->EndsWith = end;
		ptr->t = 0;
		ptr->speed = speed;
	}

    if (!Timer->IsRunning())
        Timer->Start(_TIMER_INT_);

    return ptr;
}

Animation* Animation::Queue(Animation *ptr) {
	if(queued != NULL)
		return queued->Queue(ptr);
	queued = ptr;
	return this;
}

Animation* Animation::Queue(wxDouble *val, const wxDouble &start, const wxDouble &end, wxDouble const &speed) {
	if(queued != NULL)
		return queued->Queue(val, start, end, speed);
	queued = new Animation(val, start, end, speed);
	return queued;
}

wxDouble Animation::smooth_fun(const wxDouble& t)
{
	return (2.0 - t)*t*(EndsWith - StartsWith) + StartsWith;
}

void Animation::Update()
{
	t = t + _SPEED_FACT_*speed;
	if (t >= 1.0) {
		*Value = smooth_fun(1);
		if (queued != NULL) {
			Animations.push_back(queued);
			queued = NULL;
		}
		if(CustomFunction != NULL)
			(*CustomFunction)(fn_arg);
		Animations.remove(this);
	} else
		*Value = smooth_fun(t);
}

void Animation::UpdateAll()
{
	list<Animation*>::reverse_iterator i;
	for (i = Animations.rbegin(); i != Animations.rend(); ++i)
		(*i)->Update();
}

bool Animation::IsOn()
{
	return !Animations.empty();
}

void Animation::DeleteAll()
{
	Animations.clear();
}

void Animation::QueueFunction(void (*fn)(void*), void* arg) {
	CustomFunction = fn;
	fn_arg = arg;
}