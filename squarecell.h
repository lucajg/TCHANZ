/*!
  \file    squarecell.h
  \authors Adrian Morel, Luca Jiménez Glur
		   345863        345448
  \date    Mai 2022
  \brief   Interface du module "squarecell".
*/

#ifndef SQUARECELL_H
#define SQUARECELL_H
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>

enum Direction {N, NE, E, SE, S, SW, W, NW};

struct Square; //predeclarations
struct Vecteur;

struct Point
{
	int x;
	int y;
	Point(int x, int y);
	void coordinate_check() const; //validation d'un point
	bool in_square(Square s) const;
};

struct Square
{
	//constructeurs
	Square(int x = 0, int y = 0, int s = 1, bool centered = false);
	Square(Point origin, int s, bool centered = false);
	
	//champs
	Point origin;
	int   side;
	bool  centered;
	
	//tests
	void validation_de_carre() const;
	void side_positivity_check()const;
	void centered_check() const;
	bool in_grid() const;
	bool within(Square bigS) const;
	bool superpose(Square s2) const;
	Point first_superposition() const;
	bool spaceFree() const;
	bool enough_space(int target) const;
	Square find_space(int target, bool centered = false) const;
	bool contact_sq(Square sq_2, bool corners_count = false);
	bool touching_world_border(int dirction) const;
	
	//outils pour grille de booléens
	void draw() const;
	void erase() const;
	
	//points particuliers
	Point lower_left() const;
	Point lower_right() const;
	Point upper_right() const;
	Point upper_left() const;
	
	//ajustement
	Square equivalent_non_centered() const;
	Square equivalent_centered() const;
	
	//graphics
	void draw_sq_full(const int& color, bool light = false) const;
	void draw_sq_border(const int& color) const;
	void draw_sq_losange() const;
	void draw_sq_checker(const int& color) const;
	void draw_sq_plus(const int& color) const;
	void draw_sq_highlight(const int& color) const;
	
	//diagonal movement
	int best_direction_diagonal(Point p) const;
	
	int straight(Vecteur to_target) const;
	
	int NE_SE(Vecteur to_target) const;
	int nb_sup_NE_SE(Vecteur to_target) const;     //count superposition for trajectory
	int nb_sup_SE_NE(Vecteur to_target) const;
	
	int NW_SW(Vecteur to_target) const;
	int nb_sup_NW_SW(Vecteur to_target) const;
	int nb_sup_SW_NW(Vecteur to_target) const;
	
	int NE_NW(Vecteur to_target) const;
	int nb_sup_NE_NW(Vecteur to_target) const;
	int nb_sup_NW_NE(Vecteur to_target) const;
	
	int SE_SW(Vecteur to_target) const;
	int nb_sup_SE_SW(Vecteur to_target) const;
	int nb_sup_SW_SE(Vecteur to_target) const;
};

struct Vecteur
{
	//constructeurs
	Vecteur(int x, int y);
	Vecteur(Point p);
	Vecteur(Point p1, Point p2);
	Vecteur(Square sq, Point p);
	Vecteur(Square sq_1, Square Sq_2);
	
	//mesures
	double norme_euclidienne() const;
	int norme_inf() const;
	int coord_min() const;
	
	//attributs
	int x;
	int y;
};

//fonctions pour la gestion de detection d'erreurs
bool error_found();
void reset_grid();
void set_error_found(bool ef);
void set_lecture(bool _lect);

//random engine
int random_coordinate();
bool random_bool(double probability);

//access to 128 constant
int get_g_max();
#endif
