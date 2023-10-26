/*!
  \file    graphic_gui.h
  \authors Adrian Morel, Luca Jiménez Glur
		   345863               345448
  \date    Mai 2022
  \brief   Interface complète du module "graphic".
*/

#ifndef GRAPHIC_GUI_H
#define GRAPHIC_GUI_H

#include <gtkmm/drawingarea.h>
#include <cairomm/context.h>
#include <glibmm/main.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>

//dependencies
#include "graphic.h"

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr);

#endif
