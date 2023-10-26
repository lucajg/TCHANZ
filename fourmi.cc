/*!
  \file    fourmi.cc
  \authors Adrian Morel : 50 %, Luca Jiménez Glur 50 %
		   345863               345448
  \date    Mai 2022
  \brief   Implémentation du module "fourmi".
*/

//dependencies
#include "message.h"
#include "fourmi.h"
#include <algorithm>

static int food_delivered(0);

constexpr double very_big(1000.);

//constructeurs
Fourmi::Fourmi(Square body, int age)
	: body(body), age(age)
{
	if (age < 0) age = 0;
}

Collector::Collector(Square body, int age, bool food)
	: Fourmi(body, age), food(food)
{
	if (not body.spaceFree())
	{
		int x = body.origin.x;
		int y = body.origin.y;
		Point p(body.first_superposition());
		std::cout << message::collector_overlap(x,y, p.x, p.y);
		set_error_found(true);
	}
	body.draw();
}

Defensor::Defensor(Square body, int age)
	: Fourmi(body, age)
{
	if (not body.spaceFree())
	{
		int x = body.origin.x;
		int y = body.origin.y;
		Point p(body.first_superposition());
		std::cout << message::defensor_overlap(x,y, p.x, p.y);
		set_error_found(true);
	}
	body.draw();
}

Predator::Predator(Square body, int age)
	: Fourmi(body, age)
{
	if (not body.spaceFree())
	{
		int x = body.origin.x;
		int y = body.origin.y;
		std::cout << message::predator_overlap(x,y);
		set_error_found(true);
	}
	body.draw();
}

Generator::Generator(Square body, double total_food, int age)
	: Fourmi(body, age), total_food(total_food)
{
	int xg = body.origin.x;
	int yg = body.origin.y;
	if (not body.spaceFree())
	{
		Point p(body.first_superposition());
		std::cout << message::generator_overlap(xg, yg, p.x, p.y);
		set_error_found(true);
	}
	body.draw();
}

//fonctions auxiliares pour constructeur de copie de fourmiliere
Fourmi* Collector::clone() const { return new Collector(*this); }
Fourmi*  Defensor::clone() const { return new  Defensor(*this); }
Fourmi*  Predator::clone() const { return new  Predator(*this); }
Fourmi* Generator::clone() const { return new Generator(*this); }

//getter
bool Fourmi::life_ended() const
{
	if (age >= bug_life or end_of_life) return true;
	return false;
}

//graphics
void Fourmi::draw(const int& id) const
{
	body.draw_sq_full(id);
}

void Collector::draw(const int& id) const
{
	body.draw_sq_checker(id);
	if (food) Square(body.origin.x,body.origin.y,1).draw_sq_losange();
}

void Defensor::draw(const int& id) const
{
	body.draw_sq_plus(id);
}

//output
void Fourmi::ecriture(std::ostream& outfile) const
{
	outfile << body.origin.x << " " << body.origin.y << " ";
}

void Generator::ecriture(std::ostream& outfile) const
{
	Fourmi::ecriture(outfile);
	outfile << total_food << " ";
}

void Collector::ecriture(std::ostream& outfile) const
{
	Fourmi::ecriture(outfile);
	outfile << age << " " << ((food) ? "true" : "false") << std::endl;
}

void Defensor::ecriture(std::ostream& outfile) const
{
	Fourmi::ecriture(outfile);
	outfile << age << std::endl;
}

void Predator::ecriture(std::ostream& outfile) const
{
	Fourmi::ecriture(outfile);
	outfile << age << std::endl;
}

//action
void Fourmi::viellir()
{
	++age;
}

void Fourmi::move(int direction)
{
	undraw_bool();
	Square future_body(body);
	switch (direction)
	{
		case  N: ++future_body.origin.y; break;
		case NE:
		{
			++future_body.origin.y; 
			++future_body.origin.x;
		} break;
		case  E: ++future_body.origin.x; break;
		case SE:
		{
			--future_body.origin.y; 
			++future_body.origin.x;
		} break;
		case  S: --future_body.origin.y; break;
		case SW:
		{
			--future_body.origin.y; 
			--future_body.origin.x;
		} break;
		case  W: --future_body.origin.x; break;
		case NW:
		{
			++future_body.origin.y; 
			--future_body.origin.x;
		} break;
	}
	if (future_body.in_grid() and future_body.spaceFree()) body = future_body;
	body.draw();
}

void Fourmi::die()
{
	end_of_life = true;
}

void Fourmi::check_death(std::vector<Ants>& fourmis_monde, int& id) {}

//mouvement
void Fourmi::undraw_bool() const
{
	body.erase();
}

void Fourmi::draw_bool() const
{
	body.draw();
}

//Generator
void Generator::eat(int nbT)
{
	total_food -= nbT*food_rate;
	if (total_food < food_rate) end_of_klan = true;
}

void Generator::die()
{
	end_of_life = true;
	end_of_klan = true;
}

void Generator::act(Square border, Square anthill, Square body_gen,
					std::vector<Ants>& fourmis_monde, int id, bool constrained)
{
	if (body.origin.x > anthill.origin.x+sizeG/2 and 
		body.origin.y > anthill.origin.y+sizeG/2)
	{
		move(SW);
	}
	else if (body.origin.x > anthill.origin.x+sizeG/2 and 
			 body.origin.y == anthill.origin.y+sizeG/2)
			 move(W);
	else if (body.origin.x == anthill.origin.x+sizeG/2 and 
			 body.origin.y > anthill.origin.y+sizeG/2)
			 move(S);
	else if (body.origin.x < anthill.origin.x+sizeG/2 and 
			 body.origin.y > anthill.origin.y+sizeG/2)
			 move(SE);
	else if (body.origin.x > anthill.origin.x+sizeG/2 and 
			 body.origin.y < anthill.origin.y+sizeG/2)
			 move(NW);
	else if (body.origin.x < anthill.origin.x+sizeG/2 and 
			 body.origin.y < anthill.origin.y+sizeG/2)
			 move(NE);
	else if (body.origin.x < anthill.origin.x+sizeG/2 and 
			 body.origin.y == anthill.origin.y+sizeG/2)
			 move(E);
	else if (body.origin.x == anthill.origin.x+sizeG/2 and 
			 body.origin.y < anthill.origin.y+sizeG/2)
			 move(N);
	if (not body.within(anthill)) die();
}

void Generator::increase_total_food(double val)
{
	total_food+=val;
}

//Collector
void Collector::act(Square border, Square anthill, Square body_gen,
					std::vector<Ants>& fourmis_monde, int id, bool constrained)
{
	if (food == EMPTY) look_for_food(border);
	else back_home(border, body_gen);
}

void Collector::look_for_food(Square border)
{
	int min_dist(0), element(0);
	bool success(false);
	std::vector<Nourriture> aliments = get_aliments();
	if (aliments.size() > 0)
	{
		min_dist = Vecteur(body,aliments[0].getPlace()).norme_inf()+1;
		for (size_t i(0) ; i < aliments.size() ; ++i)
		{
			int dist_to_collec(Vecteur(body, aliments[i].getPlace()).norme_inf());
			if (dist_to_collec < min_dist and reachable(aliments[i].getPlace().origin))
			{
				success = true;
				min_dist = dist_to_collec;
				element = i;
			}
		}
		if (success)
		{
			Point pos_aliment(aliments[element].getPlace().origin);
			if (body.contact_sq(aliments[element].getPlace(),true)) 
			{
				Point pos_before(body.origin);
				aliments[element].getPlace().erase();
				move(body.best_direction_diagonal(pos_aliment));
				if (body.origin.x == pos_before.x and body.origin.y == pos_before.y)
					aliments[element].getPlace().draw();
				else
				{
					food = LOADED;
					erase_nourriture(element);
				}
			}
			else move(body.best_direction_diagonal(pos_aliment));
		}
	}
	if ((not success) and body.superpose(border)) go_outside(border);
}

void Collector::go_outside(Square border)
{
	int dist_inf_g(Vecteur(body.origin,border.lower_left()).norme_inf());
	int dist_sup_g(Vecteur(body.origin,border.upper_left()).norme_inf());
	int dist_sup_d(Vecteur(body.origin,border.upper_right()).norme_inf());
	int dist_inf_d(Vecteur(body.origin,border.lower_right()).norme_inf());
	std::vector<int> distances = { dist_inf_g, dist_sup_g, dist_sup_d, dist_inf_d };
	
	std::sort(distances.begin(),distances.end());
	
	if (distances[0] == distances[1])
	{
		go_outside_equidistance(dist_inf_g, dist_sup_g, dist_sup_d,
								dist_inf_d, distances);
	}
	else
	{
		if (distances[0] == dist_inf_g) move(SW);
		else if (distances[0] == dist_inf_d) move(SE);
		else if (distances[0] == dist_sup_g) move(NW);
		else move(NE);
	}
}

void Collector::go_outside_equidistance(int& dist_inf_g, int& dist_sup_g,
										int& dist_sup_d, int& dist_inf_d,
										std::vector<int>& distances)
{
	int local_g_max(get_g_max());
		
	if (distances[0] == distances[2])
	{
		if (body.origin.x > local_g_max/2 and body.origin.y > local_g_max/2)
			move(SW);
		else if (body.origin.x > local_g_max/2 and body.origin.y < local_g_max/2)
			move(NW);
		else if (body.origin.x < local_g_max/2 and body.origin.y < local_g_max/2)
			move(NE);
		else move(SE);
	}
	else if (dist_sup_g == dist_sup_d and dist_sup_g == distances[0])
	{
		if (body.origin.x > local_g_max/2) move(NW);
		else move(NE);
	}
	else if (dist_sup_g == dist_inf_g and dist_sup_g == distances[0])
	{
		if (body.origin.y > local_g_max/2) move(SW);
		else move(NW);
	}
	else if (dist_inf_g == dist_inf_d and dist_inf_g == distances[0])
	{
		if (body.origin.x > local_g_max/2) move(SW);
		else move(SE);
	}
	else
	{
		if (body.origin.y > local_g_max/2) move(SE);
		else move(NE);
	}
}

bool Collector::reachable(Point p)
{
	bool same_parity_c(body.origin.x%2 == body.origin.y%2);
	bool same_parity_f(p.x % 2 == p.y % 2);
	return ((same_parity_c and same_parity_f) or
			not ((same_parity_c) or (same_parity_f)));
}

void Collector::back_home(Square anthill, Square body_generator)
{
	Point target(anthill.origin.x+anthill.side/2,anthill.origin.y+anthill.side/2);
	if (not reachable(target)) ++target.y;
	move(body.best_direction_diagonal(target));
	if (body.contact_sq(anthill,true))
	{
		food = EMPTY;
		increase_food_delivered();
	}
}

void Collector::check_death(std::vector<Ants>& fourmis_monde, int& id)
{
	for (size_t l(0); l < fourmis_monde.size() ; ++l)
	{
		if (l != (unsigned)id)
		{
			for (auto & fourmi : fourmis_monde[l])
			{
				if (fourmi->get_type() == PREDATOR and 
				   (body.contact_sq(fourmi->get_body(), true)))
				{
					die();
				}
			}
		}
	}
}

//Defensor
void Defensor::act(Square border, Square anthill, Square body_gen,
				   std::vector<Ants>& fourmis_monde, int id, bool constrained)
{
	double dist_inf_g(Vecteur(body.origin,anthill.lower_left()).norme_euclidienne());
	double dist_sup_g(Vecteur(body.origin,anthill.upper_left()).norme_euclidienne());
	double dist_sup_d(Vecteur(body.origin,anthill.upper_right()).norme_euclidienne());
	double dist_inf_d(Vecteur(body.origin,anthill.lower_right()).norme_euclidienne());
	
	double min = std::min(dist_inf_g, dist_sup_g);
	min = std::min(min, dist_sup_d);
	min = std::min(min, dist_inf_d);
	
	if (body.within(anthill))
		stick_to_border(dist_inf_g, dist_sup_g, dist_sup_d, dist_inf_d, min);
		
	if (not body.within(anthill))
		stay_inside(dist_inf_g, dist_sup_g, dist_sup_d, dist_inf_d, min);
		
	if (not body.within(anthill)) die();
}

void Defensor::stick_to_border(double& dist_inf_g, double& dist_sup_g,
							   double& dist_sup_d, double& dist_inf_d, double& min)
{
	if (min == dist_sup_g)
	{
		if (dist_sup_d < dist_inf_g) move(N);
		else move(W);
	}
	else if (min == dist_sup_d)
	{
		if (dist_sup_g < dist_inf_d) move(N);
		move(E);
	}
	else if (min == dist_inf_d)
	{
		if (dist_sup_d < dist_inf_g) move(E);
		else move(S);
	}
	else if (min == dist_inf_g)
	{
		if (dist_sup_g < dist_inf_d) move(W);
		else move(S);
	}
}

void Defensor::stay_inside(double& dist_inf_g, double& dist_sup_g, double& dist_sup_d,
					 double& dist_inf_d, double& min)
{
	if (min == dist_sup_g)
	{
		if (dist_sup_d < dist_inf_g) move(S);
		else move(E);
	}
	else if (min == dist_sup_d)
	{
		if (dist_sup_g < dist_inf_d) move(S);
		else move(W);
	}
	else if (min == dist_inf_d)
	{
		if (dist_sup_d < dist_inf_g) move(W);
		else move(N);
	}
	else if (min == dist_inf_g)
	{
		if (dist_sup_g < dist_inf_d) move(E);
		else move(N);
	}
}

void Defensor::check_death(std::vector<Ants>& fourmis_monde, int& id)
{
	for (size_t l(0); l < fourmis_monde.size() ; ++l)
	{
		if (l != (unsigned)id)
		{
			for (auto & fourmi : fourmis_monde[l])
			{
				if (fourmi->get_type() == COLLECTOR and 
					(body.contact_sq(fourmi->get_body(), true)))
				{
					fourmi->die();
				}
			}
		}
	}
}

//Predator
void Predator::act(Square border, Square anthill, Square body_gen,
				   std::vector<Ants>& fourmis_monde, int id, bool constrained)
{
	if (constrained) act_constrained(border, anthill, body_gen, fourmis_monde, id);
	else act_free(border, anthill, body_gen, fourmis_monde, id);
}

void Predator::check_death(std::vector<Ants>& fourmis_monde, int& id)
{
	for (size_t l(0); l < fourmis_monde.size() ; ++l)
	{
		if (l != (unsigned)id)
		{
			for (auto & fourmi : fourmis_monde[l])
			{
				if (fourmi->get_type() != DEFENSOR and 
					(body.contact_sq(fourmi->get_body(), true) or 
					 body.superpose(fourmi->get_body())))
				{
					fourmi->die();
					if (fourmi->get_type() == PREDATOR) die();
				}
			}
		}
	}
}

void Predator::act_constrained(Square border, Square anthill, Square body_gen,
									std::vector<Ants>& fourmis_monde, int id)
{
	Square target;
	bool success(false);
	bool home_attacked(false);
	double dist_to_target(very_big);
	
	//find closest PREDATOR or COLLECTOR
	for (size_t k(0); k < fourmis_monde.size() ; ++k) 
	{
		if ((unsigned)id != k)
		{
			for (auto & fourmi : fourmis_monde[k])
			{
				if (fourmi->get_type() != DEFENSOR)
				{
					Vecteur to_pot_target(body,fourmi->get_body());
					dist_to_target = std::min(dist_to_target,
											  to_pot_target.norme_euclidienne());
					if (fourmi->get_body().superpose(border)) home_attacked = true;
					if (dist_to_target == to_pot_target.norme_euclidienne() and
						not home_attacked)
					{
						target = fourmi->get_body();
						success = true;
					}
				}
			}
		}
	}
	if (home_attacked) success = false;
	if (success) predator_move(target);
	if (home_attacked and not success) 
		act_free(border, anthill, body_gen, fourmis_monde, id);
}

void Predator::act_free(Square border, Square anthill, Square body_gen,
									std::vector<Ants>& fourmis_monde, int id)
{
	Square target;
	bool success(false);
	double dist_to_target(very_big);
	for (size_t k(0); k < fourmis_monde.size() ; ++k)
	{
		if ((unsigned)id != k)
		{
			for (auto & fourmi : fourmis_monde[k])
			{
				if (fourmi->get_type() != DEFENSOR and
					fourmi->get_body().superpose(border))
				{
					Vecteur to_pot_target(body,fourmi->get_body());
					dist_to_target = std::min(dist_to_target,
											  to_pot_target.norme_euclidienne());
					if (dist_to_target == to_pot_target.norme_euclidienne())
					{
						target = fourmi->get_body();
						success = true;
					}
				}
			}
		}
	}
	if (success) predator_move(target);
	else if (not body.within(anthill)) back_home(anthill);
}


void Predator::predator_move(Square target)
{
	body.erase();
	Vecteur to_target(body,target);
	Square future_body_1(body);
	Square future_body_2(body);
	
	if (to_target.x > 0 and to_target.y >= 0)
		premier_quadrant(future_body_1, future_body_2);
	else if (to_target.x <= 0 and to_target.y > 0)
		deuxieme_quadrant(future_body_1, future_body_2);
	else if (to_target.x < 0 and to_target.y <= 0)
		troisieme_quadrant(future_body_1, future_body_2);
	else quatrieme_quadrant(future_body_1, future_body_2);
	
	double dist_1(Vecteur(future_body_1,target).norme_euclidienne());
	double dist_2(Vecteur(future_body_2,target).norme_euclidienne());
	
	if (dist_1 <= dist_2)
	{
		if ((future_body_1.spaceFree() or future_body_1.superpose(target)) and 
			 future_body_1.in_grid())
		{
			body = future_body_1;
		}
	}
	else
	{
		if ((future_body_2.spaceFree() or target.superpose(future_body_2)) and
			 future_body_2.in_grid())
		{
			body = future_body_2;
		}
	}
	
	body.draw();
	to_target = Vecteur(body,target);
}

void Predator::premier_quadrant(Square& future_body_1, Square& future_body_2)
{
	future_body_1.origin.x++;
	future_body_1.origin.y+=2;
		
	future_body_2.origin.x+=2;
	future_body_2.origin.y++;
}

void Predator::deuxieme_quadrant(Square& future_body_1, Square& future_body_2)
{
	future_body_1.origin.x--;
	future_body_1.origin.y+=2;
	
	future_body_2.origin.x-=2;
	future_body_2.origin.y++;
}

void Predator::troisieme_quadrant(Square& future_body_1, Square& future_body_2)
{
	future_body_1.origin.x-=2;
	future_body_1.origin.y--;
		
	future_body_2.origin.x--;
	future_body_2.origin.y-=2;
}

void Predator::quatrieme_quadrant(Square& future_body_1, Square& future_body_2)
{
	future_body_1.origin.x++;
	future_body_1.origin.y-=2;
		
	future_body_2.origin.x+=2;
	future_body_2.origin.y--;
}

void Predator::back_home(Square anthill)
{

	predator_move(Square(anthill.origin.x+anthill.side/2,
						 anthill.origin.y+anthill.side/2));
}

//file input
Fourmi* decodage_ligne_fourmi(std::string line, int type)
{
	std::istringstream data(line);
	int 			   x(0), y(0), age(0);
	std::string 	   strFood;
	bool 			   food(0);
	Square 			   body;
	Fourmi* 		   nouvelle_fourmi = nullptr;
	if(!(data >> x >> y >> age)) set_error_found(true);
	switch (type)
	{
		case COLLECTOR:
			if(!(data >> strFood)) set_error_found(true);
			if (strFood == "true") food = LOADED;
			body = Square(x,y,sizeC,true);
			body.validation_de_carre();
			if (not (error_found()))
				nouvelle_fourmi = new Collector(body,age,food);
			break;
		case DEFENSOR:
			body = Square(x,y,sizeD,true);
			body.validation_de_carre();
			if (not (error_found()))
				nouvelle_fourmi = new Defensor(body,age);
			break;
		case PREDATOR:
			body = Square(x,y,sizeP,true);
			body.validation_de_carre();
			if (not error_found())
				nouvelle_fourmi = new Predator(body,age);
			break;
	}
	return nouvelle_fourmi;
}

//food delivery
int get_food_delivered()
{
	return food_delivered;
}

void increase_food_delivered()
{
	++food_delivered;
}

void reset_food_delivered()
{
	food_delivered = 0;
}
