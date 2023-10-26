/*!
  \file    squarecell.cc
  \authors Adrian Morel : 50 %, Luca Jiménez Glur 50 %
		   345863               345448
  \date    Mai 2022
  \brief   Implémentation du module "squarecell".
*/

#include <random>

//dependencies
#include "error_squarecell.h"
#include "graphic.h"
#include "squarecell.h"

typedef std::vector<std::vector<bool>> Grille;

//grille de booléens
static Grille grid(g_max, std::vector<bool>(g_max,0));

//détection d'erreurs
static bool error_sq_cell(false);

static bool lecture(false);

static std::default_random_engine random_engine;

Point::Point(int x, int y) : x(x), y(y)
{ 
	if (not error_sq_cell) 
	{
		if (lecture) coordinate_check();
	}
}

void Point::coordinate_check() const
{
	if (x > g_max-1 or x < 0)
	{
		if (lecture)
		{
			error_sq_cell = true;
			std::cout << error_squarecell::print_index(x,g_max-1);
		}
	}
	else if (y > g_max-1 or y < 0)
	{
		if (lecture)
		{
			error_sq_cell = true;
			std::cout << error_squarecell::print_index(y,g_max-1);
		}
	}
}

bool Point::in_square(Square s) const
{
	if ((x >= s.lower_left().x and y >= s.lower_left().y) and
		(x <= s.upper_right().x and y <= s.upper_right().y))
		return true;
	return false;
}

//constructeurs
Square::Square(int x, int y, int s, bool centered)
	: origin(x,y), side(s), centered(centered)
{
}

Square::Square(Point origin, int s, bool centered)
	: origin(origin), side(s), centered(centered)
{
}

//tests
void Square::validation_de_carre() const
{
	if (not error_sq_cell)
	{
		if (not in_grid() and lecture) error_sq_cell = true;
		side_positivity_check();
		centered_check();
	}
}

void Square::side_positivity_check() const
{
	if (side <= 0)
	{
		if (lecture) error_sq_cell = true;
	} 
}

void Square::centered_check() const
{
	if (centered and (side % 2 == 0)) 
	{
		if (lecture) error_sq_cell = true;
	}
}

bool Square::in_grid() const
{
	if (centered)
	{
		if ((origin.x - side/2 < 0) or(origin.x + side/2 > g_max-1))
		{
			if (lecture)
				std::cout << error_squarecell::print_outside(origin.x,side,g_max-1);
			return false;
		}
		else if ((origin.y - side/2 < 0) or (origin.y + side/2 > g_max-1))
		{
			if (lecture)
				std::cout << error_squarecell::print_outside(origin.y,side,g_max-1);
			return false;
		}
	}
	else
	{
		if ((origin.x < 0) or (origin.x + side-1 > g_max-1))
		{
			if (lecture)
				std::cout << error_squarecell::print_outside(origin.x,side,g_max-1);
			return false;
		}
		else if ((origin.y < 0) or (origin.y + side-1> g_max-1))
		{
			if (lecture) 
				std::cout << error_squarecell::print_outside(origin.y,side,g_max-1);
			return false;
		}
	}
	return true;
}

bool Square::within(Square bigS) const
{
	if (centered) return equivalent_non_centered().within(bigS);
	else
	{
		if ((side <= bigS.side) and origin.in_square(bigS) and
									lower_right().in_square(bigS) and
									upper_right().in_square(bigS) and
									upper_left().in_square(bigS))
		{
			return true;
		}
	}
	return false;
}

bool Square::superpose(Square s2) const
{
	if (side > s2.side) return s2.superpose(*this);
	if ( lower_left().in_square(s2) or 
		lower_right().in_square(s2) or
		upper_right().in_square(s2) or
		 upper_left().in_square(s2))
	{
		return true;
	}
	return false;
}

Point Square::first_superposition() const
{
	Point superposition(0,0);
	if (centered) return equivalent_non_centered().first_superposition();
	else 
	{
		if (spaceFree())
		{
			return Point(0,0);
		}
		for (int y(origin.y+side-1); y >= origin.y; y--)
		{
			for (int x(origin.x); x < origin.x+side; x++)
			{
				if (grid[g_max-1-y][x] != 0)
				{
					 superposition = Point(x,y);
					 return superposition;
				}
			}
		}
	}
	return superposition;
}

bool Square::spaceFree() const
{
	if (not in_grid()) return true;
	
	if (centered) return equivalent_non_centered().spaceFree();
	
	else 
	{
		for (int x(origin.x); x < origin.x+side; ++x)
		{
			for (int y(origin.y); y < origin.y+side; ++y)
				if (grid[g_max-1-y][x] != 0) return false;
		}
	}
	return true;
}

bool Square::enough_space(int target) const
{
	if (target <= 0 or target > side) return false;
	
	for (int xi(origin.x + side - target) ; xi >= origin.x; --xi)
	{
		for (int yi(origin.y + side - target) ; yi >= origin.y; --yi)
		{
			if (Square(xi, yi, target).spaceFree())
			{
				return true;
			}
		}
	}
	return false;
}

Square Square::find_space(int target, bool ctrd) const
{
	for (int xi(origin.x + side - target) ; xi >= origin.x; --xi)
	{
		for (int yi(origin.y + side - target) ; yi >= origin.y; --yi)
		{
			if (Square(xi, yi, target).spaceFree())
			{
				if (ctrd) return Square(xi,yi,target).equivalent_centered();
				else return Square(xi,yi,target);
			}
		}
	}
	return Square();
}

bool Square::contact_sq(Square sq_2, bool corners_count)
{
	sq_2 = sq_2.equivalent_non_centered();
	if (corners_count)
	{
		--sq_2.origin.x;
		--sq_2.origin.y;
		sq_2.side += 2;
		if (superpose(sq_2)) return true;
		return false;
	}
	++sq_2.origin.y;
	if (not superpose(sq_2))
	{
		--sq_2.origin.y;
		++sq_2.origin.x;
		if (not superpose(sq_2))
		{
			--sq_2.origin.y;
			--sq_2.origin.x;
			if (not superpose(sq_2))
			{
				++sq_2.origin.y;
				--sq_2.origin.x;
				if (not superpose(sq_2)) return false;
			}
		}
	}
	return true;
}

bool Square::touching_world_border(int direction) const
{
	if (centered) return equivalent_non_centered().touching_world_border(direction);
	switch (direction)
	{
		case N: if (origin.y + side == g_max) return true; break;
		case S: if (origin.y == 0) return true; break;
		case E: if (origin.x + side == g_max) return true; break;
		case W: if (origin.x == 0) return true; break;
	}
	return false;
	
}

//outils pour grille de booléens
void Square::draw() const
{
	if (not error_sq_cell)
	{
		if (centered) return equivalent_non_centered().draw();
		else
		{
			for (int x(origin.x); x < origin.x + side; ++x)
			{
				for (int y(origin.y); y < origin.y + side; ++y)
				{
					grid[g_max-1-y][x] = 1;
				}
			}
		}
	}
}

void Square::erase() const
{
	if (centered) return equivalent_non_centered().erase();
	else
	{
		for (int x(origin.x); x < origin.x + side; ++x)
		{
			for (int y(origin.y); y < origin.y + side; ++y)
			{
				grid[g_max-1-y][x] = 0;
			}
		}
	} 
}

//points particuliers
Point Square::lower_left() const
{
	if (side == 1) return origin;
	if (centered) return (equivalent_non_centered().lower_left());
	else return origin;
}

Point Square::lower_right() const
{
	if (side == 1) return origin;
	if (centered) return (equivalent_non_centered().lower_right());
	else return Point((origin).x+side-1,(origin).y);
}

Point Square::upper_right() const
{
	if (side == 1) return origin;
	if (centered) return (equivalent_non_centered().upper_right());
	else return Point((origin).x+side-1,(origin).y+side-1);
}

Point Square::upper_left() const
{	
	if (side == 1) return origin;
	if (centered) return (equivalent_non_centered().upper_left());
	else return Point((origin).x,(origin).y+side-1);
}

//ajustement
Square Square::equivalent_non_centered() const
{
	if (not centered) return (*this);
	return Square(origin.x - (side-1)/2, origin.y - (side-1)/2, side, false);
}

Square Square::equivalent_centered() const
{
	if ((not (side % 2)) or centered) //carres impairs ne peuvent pas etre centres
		return (*this); 
	return Square(origin.x + (side-1)/2, origin.y + (side-1)/2, side, true);
}

//graphics
void Square::draw_sq_full(const int& color, bool light) const  //carré entier
{
	if (centered) return (equivalent_non_centered().draw_sq_full(color, light));
	else
	{
		set_color(color, light);
		graphic_draw_square_fill(origin.x,origin.y,side);
	}
}

void Square::draw_sq_border(const int& color) const  //bord d'un carré
{
	if (centered) return (equivalent_non_centered().draw_sq_border(color));
	else
	{
		set_color(color);
		graphic_draw_border(origin.x,origin.y,side);
	}
}

void Square::draw_sq_losange() const  //carré avec un losange dans chaque case
{
	if (centered) return (equivalent_non_centered().draw_sq_losange());
	else
	{
		for (int i(origin.x); i < origin.x+side; ++i)
		{
			for (int j(origin.y); j < origin.y+side; ++j)
			{
				graphic_draw_losange(i,j);
			}
		}
	}
}

void Square::draw_sq_checker(const int& color) const  //carré avec motif échéquier
{
	if (centered) return (equivalent_non_centered().draw_sq_checker(color));
	else
	{
		set_color(color, false);
		graphic_draw_square_fill(origin.x,origin.y,side);
		set_color(color, true);
		graphic_draw_checker(origin.x,origin.y,side);
	}
}

void Square::draw_sq_plus(const int& color) const  //carré avec motif quadrillage
{
	if (centered) return (equivalent_non_centered().draw_sq_plus(color));
	else
	{
		set_color(color, true);
		graphic_draw_square_fill(origin.x,origin.y,side);
		set_color(color, false);
		graphic_draw_plus(origin.x,origin.y,side);
	}
}

//carré pour surligner (transparence)
void Square::draw_sq_highlight(const int& color) const 
{
	if (centered) return (equivalent_non_centered().draw_sq_highlight(color));
	else
	{
		set_color_highlight(color);
		graphic_draw_square_fill(origin.x,origin.y,side);
	}
}

//diagonal movement
int Square::best_direction_diagonal(Point p) const
{
	erase();
	
	Vecteur to_target(origin,p);

	if (abs(to_target.x) == abs(to_target.y)) return straight(to_target);
	
	if (to_target.x > 0 and abs(to_target.y) <  to_target.x) return NE_SE(to_target);
	
	if (to_target.x < 0 and abs(to_target.y) < -to_target.x) return NW_SW(to_target);
	
	if (to_target.y > 0) return NE_NW(to_target);
	
	return SE_SW(to_target);
}

int Square::straight(Vecteur to_target) const
{
	if (to_target.x > 0)
	{
		draw();
		if (to_target.y > 0) return NE;
		return SE;
	}
	draw();
	if (to_target.y > 0) return NW;
	return SW;
}

int Square::NE_SE(Vecteur to_target) const
{
	int nb_sup1(nb_sup_NE_SE(to_target));
	int nb_sup2(nb_sup_SE_NE(to_target));
	draw();
	if (nb_sup1 < nb_sup2)
	{
		if (touching_world_border(N)) return SE;
		return NE;
	}
	if (touching_world_border(S)) return NE;
	return SE;
}

int Square::nb_sup_NE_SE(Vecteur to_target) const
{
	Point mod_pos(origin);
	int nb_sup(0);
	while (abs(to_target.x) != abs(to_target.y))
	{
		++mod_pos.x;
		++mod_pos.y;
		--to_target.x;
		--to_target.y;
		if ((not Square(mod_pos,side).spaceFree())) ++nb_sup;
	}
	while (to_target.x > 0)
	{
		++mod_pos.x;
		--mod_pos.y;
		--to_target.x;
		++to_target.y;
		if ((not Square(mod_pos,side).spaceFree())) ++nb_sup;
	}
	return nb_sup;
}

int Square::nb_sup_SE_NE(Vecteur to_target) const
{
	Point mod_pos(origin);
	int nb_sup(0);
	while (abs(to_target.x) != abs(to_target.y))
		{
			++mod_pos.x;
			--mod_pos.y;
			--to_target.x;
			++to_target.y;
			if ((not Square(mod_pos,side).spaceFree())) ++nb_sup;
		}
		while (to_target.x > 0)
		{
			++mod_pos.x;
			++mod_pos.y;
			--to_target.x;
			--to_target.y;
			if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
		}
	return nb_sup;
}

int Square::NW_SW(Vecteur to_target) const
{
	int nb_sup1(nb_sup_NW_SW(to_target));
	int nb_sup2(nb_sup_SW_NW(to_target));
	draw();
	if (nb_sup1 < nb_sup2)
	{
		if (touching_world_border(N)) return SW;
		return NW;
	}
	if (touching_world_border(S)) return NW;
	return SW;
}

int Square::nb_sup_NW_SW(Vecteur to_target) const
{
	Point mod_pos(origin);
	int nb_sup(0);
	while (abs(to_target.x) != abs(to_target.y))
	{
		--mod_pos.x;
		++mod_pos.y;
		++to_target.x;
		--to_target.y;
		if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
	}
	while (to_target.x < 0)
	{
		--mod_pos.x;
		--mod_pos.y;
		++to_target.x;
		++to_target.y;
		if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
	}
	return nb_sup;
}

int Square::nb_sup_SW_NW(Vecteur to_target) const
{
	Point mod_pos(origin);
	int nb_sup(0);
	while (abs(to_target.x) != abs(to_target.y))
		{
			--mod_pos.x;
			--mod_pos.y;
			++to_target.x;
			++to_target.y;
			if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
		}
		while (to_target.x < 0)
		{
			--mod_pos.x;
			++mod_pos.y;
			++to_target.x;
			--to_target.y;
			if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
		}
	return nb_sup;
}

int Square::NE_NW(Vecteur to_target) const
{
	int nb_sup1(nb_sup_NE_NW(to_target));
	int nb_sup2(nb_sup_NW_NE(to_target));
	draw();
	if (nb_sup1 < nb_sup2)
	{
		if (touching_world_border(E)) return NW;
		return NE;
	}
	if (touching_world_border(W)) return NE;
	return NW;
}

int Square::nb_sup_NE_NW(Vecteur to_target) const
{
	Point mod_pos(origin);
	int nb_sup(0);
	while (abs(to_target.x) != abs(to_target.y))
	{
		++mod_pos.x;
		++mod_pos.y;
		--to_target.x;
		--to_target.y;
		if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
	}
	while (to_target.y > 0)
	{
		--mod_pos.x;
		++mod_pos.y;
		++to_target.x;
		--to_target.y;
		if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
	}
	return nb_sup;
}

int Square::nb_sup_NW_NE(Vecteur to_target) const
{
	Point mod_pos(origin);
	int nb_sup(0);
	while (abs(to_target.x) != abs(to_target.y))
		{
			--mod_pos.x;
			++mod_pos.y;
			++to_target.x;
			--to_target.y;
			if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
		}
		while (to_target.y > 0)
		{
			++mod_pos.x;
			++mod_pos.y;
			--to_target.x;
			--to_target.y;
			if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
		}
	return nb_sup;
}

int Square::SE_SW(Vecteur to_target) const
{
	int nb_sup1(nb_sup_SE_SW(to_target));
	int nb_sup2(nb_sup_SW_SE(to_target));
	draw();
	if (nb_sup1 < nb_sup2)
	{
		if (touching_world_border(E)) return SW;
		return SE;
	}
	if (touching_world_border(W)) return SE;
	return SW;
}

int Square::nb_sup_SE_SW(Vecteur to_target) const
{
	Point mod_pos(origin);
	int nb_sup(0);
	while (abs(to_target.x) != abs(to_target.y))
	{
		++mod_pos.x;
		--mod_pos.y;
		--to_target.x;
		++to_target.y;
		if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
	}
	while (to_target.y < 0)
	{
		--mod_pos.x;
		--mod_pos.y;
		++to_target.x;
		++to_target.y;
		if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
	}
	return nb_sup;
}

int Square::nb_sup_SW_SE(Vecteur to_target) const
{
	Point mod_pos(origin);
	int nb_sup(0);
	while (abs(to_target.x) != abs(to_target.y))
		{
			--mod_pos.x;
			--mod_pos.y;
			++to_target.x;
			++to_target.y;
			if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
		}
		while (to_target.y < 0)
		{
			++mod_pos.x;
			--mod_pos.y;
			--to_target.x;
			++to_target.y;
			if ((not Square(mod_pos,side).spaceFree()) ) ++nb_sup;
		}
	return nb_sup;
}

//constructeurs
Vecteur::Vecteur(int x, int y) : x(x), y(y) {}
Vecteur::Vecteur(Point p) : x(p.x), y(p.y) {}
Vecteur::Vecteur(Point p1, Point p2)
	: x(p2.x-p1.x), y(p2.y-p1.y)
{
}

Vecteur::Vecteur(Square sq, Point p) : x(p.x-sq.origin.x), y(p.y-sq.origin.y)
{
}

Vecteur::Vecteur(Square sq1, Square sq2)
	: x(sq2.origin.x-sq1.origin.x), y(sq2.origin.y-sq1.origin.y)
{
}

//mesures
double Vecteur::norme_euclidienne() const
{
	return sqrt(x*x+y*y);
}

int Vecteur::norme_inf() const
{
	return std::max(abs(x),abs(y));
}

int Vecteur::coord_min() const
{
	return std::min(x,y);
}

//fonctions pour la gestion de detection d'erreurs
bool error_found()
{
	return error_sq_cell;
}

void reset_grid()
{ 
	grid = Grille(g_max, std::vector<bool>(g_max,0));
}

void set_error_found(bool ef)
{
	error_sq_cell = ef;
}

void set_lecture(bool _lect)
{
	lecture = _lect;
}

//random engine
int random_coordinate()
{
	std::uniform_int_distribution<unsigned> u(1,g_max-2);
	return u(random_engine);
}

bool random_bool(double probability)
{
	std::bernoulli_distribution rand_b(probability);
	return rand_b(random_engine);
}

//access to 128 constant
int get_g_max()
{
	return g_max;
}
