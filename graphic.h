/*!
  \file    graphic.h
  \authors Adrian Morel : 50 %, Luca Jiménez Glur 50 %
		   345863               345448
  \date    Mai 2022
  \brief   Interface partielle du module "graphic".
*/

#ifndef GRAPHIC_H
#define GRAPHIC_H

constexpr short unsigned taille_dessin(500);

//côté de la grille
constexpr short unsigned g_max(128);

constexpr double sq_width((double)taille_dessin/g_max);

//gestion des couleurs
void set_color(int couleur, bool light = false); 
void set_color_highlight(int color);

//remplissage d'une case
void graphic_draw_cell(double x, double y);

// dessin de la grille et du bord de l'espace de simulation
void draw_grid();

//différents types de carrés
void graphic_draw_border(double x, double y, double side);
void graphic_draw_square_fill(double x, double y, double side);
void graphic_draw_losange(double x, double y);

//motifs
void graphic_draw_checker(int x, int y, int side);
void graphic_draw_plus(int x, int y, int side);


#endif
