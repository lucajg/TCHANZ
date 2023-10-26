/*!
  \file    gui.h
  \authors Adrian Morel, Luca Jiménez Glur
		   345863               345448
  \date    Mai 2022
  \brief   Interface du module "gui".
*/

#ifndef GUI_H
#define GUI_H

#include "graphic_gui.h"

struct Frame // Model Framing and window parameters
{
	double xMin; // frame parameters
	double xMax;
	double yMin;
	double yMax;
	double asp;  // frame aspect ratio
	int height;  // window height
	int width;   // window width
	int getMinXY() { return ((xMin > yMin) ? xMin : yMin); }
	int getMinHW() { return ((width > height) ? height : width); }
};

class MyArea : public Gtk::DrawingArea
{
public:
	MyArea();
	virtual ~MyArea();
	void setFrame(Frame f);
	void adjustFrame();
protected:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
	Frame frame;
};

class Timer : public Gtk::Window
{
public:
	Timer();
protected:
	// game grid
	MyArea m_Area;
	
	// Member data:
	Gtk::Box m_Box_Main, m_Box_Control, m_Box_General, m_Box_Info, m_Box_Fourmiliere;
	Gtk::Frame m_Frame_General, m_Frame_Info, m_Frame_Fourmiliere;
	Gtk::Label m_Label_nbFood, m_Label_FourmInfo;
	Gtk::Button m_Button_Exit, m_Button_Open, m_Button_Save, m_Button_Start_Stop;
	Gtk::Button m_Button_Step, m_Button_Previous, m_Button_Next;
	
	// Keep track of the timer status (created or not)
	bool timer_added;
	
	// to store a timer disconnect request
	bool disconnect;
	
	// This constant is initialized in the constructor's member initializer:
	const int timeout_value;
	
	// This is the standard prototype of the Timer callback function
	bool on_timeout(); 
	
	// button signal handlers
	void on_button_exit();
	void on_button_open_clicked();
	void on_button_save_clicked();
	void on_button_start_stop();
	void on_button_step();
	void on_button_previous();
	void on_button_next();
	
	// Keyboard signal handler:
	bool on_key_press_event(GdkEventKey * key_event);
	
	void change_affichage_info(int current_anthill);
	
	void refresh_nb_food();
};

#endif
