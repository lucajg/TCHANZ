/*!
  \file    projet.cc
  \authors Adrian Morel : 50 %, Luca Jiménez Glur 50 %
		   345863               345448
  \date    Mai 2022
  \brief   Implémentation du module principal "projet".
*/

#include <gtkmm/application.h>
#include <gtkmm/window.h>

#include "gui.h"
#include "simulation.h"
#include "message.h"

int main(int argc, char *argv[])
{	
	if (argc == 2) appel_lecture(argv[1]);
	
	int fake_argc(1);
	auto app = Gtk::Application::create(fake_argc, argv);
	
	Timer fenetre;
	fenetre.set_resizable(true);
	if (argc == 2) fenetre.set_title(argv[1]);
	
	return app->run(fenetre);
}
