/*!
  \file    graphic.cc
  \authors Adrian Morel : 50 %, Luca Jiménez Glur 50 %
		   345863               345448
  \date    Mai 2022
  \brief   Implémentation du module "graphic".
*/

//dependencies
#include "graphic_gui.h"

enum Couleurs {ROUGE, VERT, BLEU, JAUNE, MAGENTA, CYAN};

static const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr);

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr)
{
	ptcr = & cr;
}

//gestion des couleurs
void set_color(int color, bool light)
{
	color %= 6;
	double added(0);
	if (light) added = 0.7;
	switch (color)
	{
		case ROUGE  : (*ptcr)->set_source_rgb(1., added, added); break;
		case VERT   : (*ptcr)->set_source_rgb(added, 1, added); break;
		case BLEU   : (*ptcr)->set_source_rgb(added, added, 1.); break;
		case JAUNE  : (*ptcr)->set_source_rgb(1+added, 0.85+added, added); break;
		case MAGENTA: (*ptcr)->set_source_rgb(1., added, 1.); break;
		case CYAN   : (*ptcr)->set_source_rgb(added, 0.95, 0.95);
	}
}

void set_color_highlight(int color)
{
	color %= 6;
	switch (color)
	{
		case ROUGE  : (*ptcr)->set_source_rgba(1  , 0.8 , 0.8 , .3); break;
		case VERT   : (*ptcr)->set_source_rgba(0.8, 1   , 0.8 , .3); break;
		case BLEU   : (*ptcr)->set_source_rgba(0.8, 0.8 , 1   , .3); break;
		case JAUNE  : (*ptcr)->set_source_rgba(1  , 1   , 0.8 , .3); break;
		case MAGENTA: (*ptcr)->set_source_rgba(1  , 0.8 , 1   , .3); break;
		case CYAN   : (*ptcr)->set_source_rgba(0.8, 1   , 1   , .3);
	}
}

//remplissage d'une case
void graphic_draw_cell(double x, double y)
{
	(*ptcr)->set_line_width(sq_width);
	x *= sq_width;
	y *= sq_width;
	y += sq_width/2;
	(*ptcr)->move_to(x, y);
	(*ptcr)->line_to(x+sq_width, y);
	(*ptcr)->stroke();	
}

// dessin de la grille et du bord de l'espace de simulation
void draw_grid()
{
	// bord
	(*ptcr)->set_source_rgb(1., 1., 1.);
	(*ptcr)->set_line_width(sq_width);
	(*ptcr)->rectangle(sq_width/2.,            sq_width/2.,
				  taille_dessin-sq_width, taille_dessin-sq_width);
	(*ptcr)->stroke();
	
	// grille
	(*ptcr)->set_source_rgb(.55, .55, .55);
	(*ptcr)->set_line_width(.3);
	for (double i(0) ; i <= taille_dessin ; i += sq_width)
	{
		(*ptcr)->move_to(i,0);
		(*ptcr)->line_to(i,taille_dessin);
	}
	for (double i(0) ; i <= taille_dessin ; i += sq_width)
	{
		(*ptcr)->move_to(0,i);
		(*ptcr)->line_to(taille_dessin,i);
	}
	(*ptcr)->stroke();
}

//différents types de carrés
void graphic_draw_border(double x, double y, double side)
{
	(*ptcr)->set_line_width(sq_width*(1./3.));
	
	x *= sq_width;
	x += sq_width/2.;
	y *= sq_width;
	y += sq_width/2.;
	side *= sq_width;
	side -= sq_width;

	(*ptcr)->rectangle(x, y, side, side);

	(*ptcr)->stroke();
}
void graphic_draw_square_fill(double x, double y, double side)
{
	(*ptcr)->set_line_width(sq_width);
	
	x *= sq_width;
	y *= sq_width;
	side *= sq_width;

	(*ptcr)->rectangle(x, y, side, side);
	(*ptcr)->fill();
	
	(*ptcr)->stroke();
}

void graphic_draw_losange(double x, double y)
{
	(*ptcr)->set_source_rgb(1., 1., 1.);
	(*ptcr)->set_line_width(sq_width*0.707);
	x *= sq_width;
	x += sq_width/4.;
	y *= sq_width;
	y += sq_width/4.;
	(*ptcr)->move_to(x, y);
	(*ptcr)->line_to(x+sq_width/2., y+sq_width/2.);
	(*ptcr)->stroke();
}


//motifs
void graphic_draw_checker(int x, int y, int side)
{
	for (int i(x); i < x+side ; ++i)
	{
		for (int j(y); j < y+side ; ++j)
		{
			if (((i-x)%2 and not((j-y)%2)) or ((j-y)%2 and not((i-x)%2)))
			{
				graphic_draw_cell(i,j);
			}
		}
	}
}

void graphic_draw_plus(int x, int y, int side)
{
	for (int i(x); i < x+side ; ++i)
	{
		for (int j(y); j < y+side ; ++j)
		{
			if ((i-x)%2 or (j-y)%2)
			{
				graphic_draw_cell(i,j);
			}
		}
	}
}
