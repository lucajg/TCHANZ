/*!
  \file    gui.cc
  \authors Adrian Morel : 50 %, Luca Jiménez Glur 50 %
		   345863               345448
  \date    Mai 2022
  \brief   Implémentation du module "gui".
*/

//dependencies
#include "simulation.h"
#include "gui.h"

static Frame default_frame = {-250, 250, -250, 250, 1, 500, 500}; 

static void orthographic_projection(const Cairo::RefPtr<Cairo::Context>& cr, 
									Frame frame);

static int current_anthill(0);

static unsigned int val(1);

MyArea::MyArea()
{
	setFrame(default_frame);
}

MyArea::~MyArea()
{
}

// defining the Model space frame to visualize in the window canvas
void MyArea::setFrame(Frame f)
{
	if((f.xMin <= f.xMax) and (f.yMin <= f.yMax) and (f.height > 0))
	{
		f.asp = f.width/f.height;
		frame = f;
	}
	else
		std::cout << "incorrect Model framing or window parameters" << std::endl;
} 

void MyArea::adjustFrame()
{
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	
	frame.width  = width;
	frame.height = height;

	// Preventing distorsion by adjusting the frame (cadrage)
	// to have the same proportion as the graphical area
	
    // use the reference framing as a guide for preventing distortion
    double new_aspect_ratio((double)width/height);
    if( new_aspect_ratio > default_frame.asp)
    { // keep yMax and yMin. Adjust xMax and xMin
	    frame.yMax = default_frame.yMax ;
	    frame.yMin = default_frame.yMin ;	
	  
	    double delta(default_frame.xMax - default_frame.xMin);
	    double mid((default_frame.xMax + default_frame.xMin)/2);
        // the new frame is centered on the mid-point along X
	    frame.xMax = mid + 0.5*(new_aspect_ratio/default_frame.asp)*delta ;
	    frame.xMin = mid - 0.5*(new_aspect_ratio/default_frame.asp)*delta ;		  	  
    }
    else
    { // keep xMax and xMin. Adjust yMax and yMin
	    frame.xMax = default_frame.xMax ;
	    frame.xMin = default_frame.xMin ;
	  	  
 	    double delta(default_frame.yMax - default_frame.yMin);
	    double mid((default_frame.yMax + default_frame.yMin)/2);
        // the new frame is centered on the mid-point along Y
	    frame.yMax = mid + 0.5*(default_frame.asp/new_aspect_ratio)*delta ;
	    frame.yMin = mid - 0.5*(default_frame.asp/new_aspect_ratio)*delta ;		  	  
    }
}

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	// changing the background color to black
	cr->set_source_rgb(0., 0., 0.);
	cr->paint();
	
	// adjust the frame to prevent distortion 
	adjustFrame();
	orthographic_projection(cr, frame); 
	
	// set context
	graphic_set_context(cr);
	
	// dessin de la grille et du bord
	draw_grid();
	
	// dessin de la simulation
	draw_sim(current_anthill);
	
	return true;
}

Timer::Timer() :
	m_Area(),
	m_Box_Main(Gtk::ORIENTATION_HORIZONTAL, 3),
	m_Box_Control(Gtk::ORIENTATION_VERTICAL, 3),
	m_Box_General(Gtk::ORIENTATION_VERTICAL),
	m_Box_Info(Gtk::ORIENTATION_VERTICAL, 3),
	m_Box_Fourmiliere(Gtk::ORIENTATION_VERTICAL, 3),
	m_Frame_General("General"),
	m_Frame_Info("Info"),
	m_Frame_Fourmiliere("Fourmilière"),
	m_Label_nbFood("nb food: " + std::to_string(get_nbN())),
	m_Label_FourmInfo("None selected"),
	m_Button_Exit("exit", true),
	m_Button_Open("open", true),
	m_Button_Save("save", true),
	m_Button_Start_Stop("start", true),
	m_Button_Step("step", true),
	m_Button_Previous("previous", true),
	m_Button_Next("next", true),
	timer_added(false),
	disconnect(false),
	timeout_value(80) // 500 ms = 0.5 seconds
{
	current_anthill = get_nbF_simulation();
	m_Area.set_size_request(taille_dessin, taille_dessin);
	
	add(m_Box_Main);
	m_Box_Main.set_border_width(5);
	
	m_Box_Main.pack_start(m_Box_Control, Gtk::PACK_SHRINK);
	m_Box_Control.set_border_width(5);
	m_Box_Control.pack_start(m_Frame_General, Gtk::PACK_SHRINK);
	m_Box_Control.set_size_request(175, taille_dessin);
	m_Frame_General.add(m_Box_General);
	m_Box_Control.pack_start(m_Frame_Info, Gtk::PACK_SHRINK);
	m_Frame_Info.add(m_Box_Info);
	m_Box_Info.pack_start(m_Label_nbFood);
	m_Box_Control.pack_start(m_Frame_Fourmiliere);
	m_Frame_Fourmiliere.add(m_Box_Fourmiliere);
	m_Box_General.pack_start(m_Button_Exit);
	m_Box_General.pack_start(m_Button_Open);
	m_Box_General.pack_start(m_Button_Save);
	m_Box_General.pack_start(m_Button_Start_Stop);
	m_Box_General.pack_start(m_Button_Step);
	m_Box_Fourmiliere.pack_start(m_Button_Previous, Gtk::PACK_SHRINK);
	m_Box_Fourmiliere.pack_start(m_Button_Next, Gtk::PACK_SHRINK);
	m_Box_Fourmiliere.pack_start(m_Label_FourmInfo);
	m_Label_FourmInfo.set_yalign(0);
	m_Label_FourmInfo.set_xalign(0);
	
	m_Box_Main.pack_start(m_Area);
	
	// Connect the three buttons:
	m_Button_Exit.signal_clicked().connect(sigc::mem_fun(*this,
										   &Timer::on_button_exit));
	m_Button_Open.signal_clicked().connect(sigc::mem_fun(*this,
										   &Timer::on_button_open_clicked) );
	m_Button_Save.signal_clicked().connect(sigc::mem_fun(*this,
										   &Timer::on_button_save_clicked) );
	m_Button_Start_Stop.signal_clicked().connect(sigc::mem_fun(*this,
											     &Timer::on_button_start_stop));
	m_Button_Step.signal_clicked().connect(sigc::mem_fun(*this,
			                               &Timer::on_button_step));
	m_Button_Previous.signal_clicked().connect(sigc::mem_fun(*this,
										   &Timer::on_button_previous));
	m_Button_Next.signal_clicked().connect(sigc::mem_fun(*this,
										   &Timer::on_button_next));
	show_all_children();
}

bool Timer::on_timeout()
{
	if(disconnect)
	{
		disconnect = false; // reset for next time a Timer is created
		return false; // End of Timer 
	}
	
	std::cout << "This is simulation update number : " << val << std::endl;
	++val; // tic the simulation clock
	
	// A call to make a single update of the simulation is expected here
	int nbF_before = get_nbF_simulation();
	
	mise_a_jour_sim();
	
	int nbF_after = get_nbF_simulation();
	if (nbF_before != nbF_after) current_anthill = nbF_after;
	change_affichage_info(current_anthill);
	
	m_Area.queue_draw();
	
	return true; 
}

// button signal handlers

void Timer::on_button_exit()
{
	exit(0);
}

void Timer::on_button_open_clicked()
{
	bool restart(false);
	if (timer_added)
	{
		on_button_start_stop();
		restart = true;
	}
	Gtk::FileChooserDialog dialog("Please choose a TCHANZ configuration file",
							      Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);
	//Add response buttons the the dialog:
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Open", Gtk::RESPONSE_OK);
	//Show the dialog and wait for a user response:
	int result = dialog.run();
	//Handle the response:
	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{
			if (timer_added) on_button_start_stop();
			std::string filename = dialog.get_filename();
			appel_lecture(&(filename[0]));
			current_anthill = get_nbF_simulation();
			change_affichage_info(current_anthill);
			refresh_nb_food();
			set_title(filename);
			val = 1;
			break;
		}
		case(Gtk::RESPONSE_CANCEL):
		{
			if (restart) on_button_start_stop();
			break;
		}
		default: 
			std::cout << "Unexpected button clicked." << std::endl; break;
	}
	m_Area.queue_draw();
}


void Timer::on_button_save_clicked()
{
	bool restart(false);
	if (timer_added)
	{
		on_button_start_stop();
		restart = true;
	}
	
	Gtk::FileChooserDialog dialog("Please choose a TCHANZ configuration file",
							      Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);
	
	//Add response buttons the the dialog:
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Save", Gtk::RESPONSE_OK);
	
	//Show the dialog and wait for a user response:
	int result = dialog.run();
	
	//Handle the response:
	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{
			//Notice that this is a std::string, not a Glib::ustring.
			std::string filename = dialog.get_filename();
			appel_ecriture(filename);
			break;
		}
		case(Gtk::RESPONSE_CANCEL):
		{
			if (restart) on_button_start_stop();
			break;
		}
		default:
			std::cout << "Unexpected button clicked." << std::endl; break;
	}
	m_Area.queue_draw();
}

void Timer::on_button_start_stop()
{
	if(not timer_added)
	{	  
		m_Button_Start_Stop.set_label("stop");
		Glib::signal_timeout().connect(sigc::mem_fun(*this, &Timer::on_timeout),
									   timeout_value );
		
		timer_added = true;
		
		std::cout << "Timer added" << std::endl;
	}
	else
	{
		m_Button_Start_Stop.set_label("start");
		std::cout << "manually disconnecting the timer " << std::endl;
		disconnect  = true;   
		timer_added = false;
	}
}

void Timer::on_button_step()
{
	if (not timer_added) on_timeout();
}

void Timer::on_button_previous()
{
	--current_anthill;
	if (current_anthill < 0) current_anthill = get_nbF_simulation();
	change_affichage_info(current_anthill);
	m_Area.queue_draw();
}

void Timer::on_button_next()
{
	//~ #ifdef DEBUG
	//~ affiche_sim();
	//~ #endif
	++current_anthill;
	if (current_anthill > (get_nbF_simulation())) current_anthill = 0;
	change_affichage_info(current_anthill);
	m_Area.queue_draw();
}

// Keyboard signal handler:
bool Timer::on_key_press_event(GdkEventKey * key_event)
{
	if(key_event->type == GDK_KEY_PRESS)
	{
		switch(gdk_keyval_to_unicode(key_event->keyval))
		{
			case 's':
				on_button_start_stop(); //change the simulation activation at each call
				return true;
			case '1':
				on_button_step();
				return true;
			case 'n':
				on_button_next();
				return true;
			case 'p':
				on_button_previous();
				return true;
			case 'q':
				on_button_exit();
				return true;
		}
	}
	
	return Gtk::Window::on_key_press_event(key_event);
}

void Timer::change_affichage_info(int current_anthill)
{
	if ((current_anthill) != get_nbF_simulation())
	{
		std::string info = get_info_fourmiliere_sim(current_anthill);
		m_Label_FourmInfo.set_label(info);
	}
	else m_Label_FourmInfo.set_label("None selected");
	refresh_nb_food();
}

void Timer::refresh_nb_food()
{
	m_Label_nbFood.set_label("nb food: " + std::to_string(get_nbN()));
}

static void orthographic_projection(const Cairo::RefPtr<Cairo::Context>& cr, 
									Frame frame)
{
	// déplace l'origine au centre de la fenêtre
	cr->translate(frame.width/2, frame.height/2);
  
	// normalise la largeur et hauteur aux valeurs fournies par le cadrage
	// ET inverse la direction de l'axe Y
	cr->scale(frame.width/(frame.xMax - frame.xMin), 
              -frame.height/(frame.yMax - frame.yMin));
  
	// décalage en bas a gauche
	cr->translate(frame.getMinXY(), frame.getMinXY());
}
