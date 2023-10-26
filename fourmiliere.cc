/*!
  \file    fourmiliere.cc
  \authors Adrian Morel : 50 %, Luca Jiménez Glur 50 %
		   345863               345448
  \date    Mai 2022
  \brief   Implémentation du module "fourmiliere".
*/

#include <iomanip>

//dependencies
#include "message.h"
#include "fourmiliere.h"

//constructeurs
Fourmiliere::Fourmiliere(int x, int y, int side, Square body_generator, 
						 double total_food, int nbC, int nbD, int nbP)
	: border(x,y,side), generator(body_generator, total_food),
	  nbC(nbC), nbD(nbD), nbP(nbP), nbT(1+nbC+nbD+nbP), sizeF(side-2)
{
	border.validation_de_carre();
}

Fourmiliere::Fourmiliere(const Fourmiliere& f)
	: border(f.border.origin.x, f.border.origin.y, f.border.side),
	  generator(f.generator), nbC(f.nbC), nbD(f.nbD),
	  nbP(f.nbP), nbT(f.nbT), sizeF(f.sizeF), id(f.id)
{
	for (auto const& fourmi : f.fourmis) add_fourmi(fourmi->clone());
}

//setter
void Fourmiliere::set_id(int& countHill)
{
	if (countHill >= 0) id = countHill;
}

void Fourmiliere::adjust_border(Square new_border)
{
	border = new_border;
	sizeF = border.side-2;
}

void Fourmiliere::set_fourmis(Ants& nouvelles_fourmis)
{
	for(size_t j(0); j < fourmis.size() ; ++j)
	{
		fourmis[j] = std::unique_ptr<Fourmi>(nouvelles_fourmis[j]->clone());
	}
}

//surcharge de l'operateur d'affectation
Fourmiliere& Fourmiliere::operator=(Fourmiliere f)
{
	if (this == &f) return *this;
	border      = f.border;
	generator   = f.generator;
	nbC         = f.nbC; 
	nbD 		= f.nbD;
	nbP 		= f.nbP;
	nbT 		= f.nbT;
	sizeF       = f.sizeF;
	id			= f.id;
	std::swap(fourmis, f.fourmis);
	return *this;
}

//tests
void Fourmiliere::check_defensor_within_home(Fourmi* d, int& countHill) const
{
	if (not error_found())
	{
		if (!(d->get_body().within(get_interior())))
		{
			int x = d->get_body().origin.x;
			int y = d->get_body().origin.y;
			std::cout << message::defensor_not_within_home(x, y, countHill);
			set_error_found(true);
		}
	}
}

void Fourmiliere::check_generator_within_home() const
{	
	if (not (generator.get_body().within(get_interior()))) 
	{
		int xg = generator.get_body().origin.x;
		int yg = generator.get_body().origin.y;
		std::cout << message::generator_not_within_home(xg, yg, id);
		set_error_found(true);
	}
}

bool Fourmiliere::homes_overlap(const Fourmiliere& f2) const
{
	if (border.superpose(f2.border)) return true;
	return false;
}

//getter
Square Fourmiliere::get_interior() const
{
	return Square(border.origin.x+1, border.origin.y+1, border.side-2);
}

Ants Fourmiliere::get_fourmis() const
{
	Ants copie_fourmis;
	for (auto const& fourmi : fourmis)
		copie_fourmis.push_back(std::unique_ptr<Fourmi>(fourmi->clone()));
	return copie_fourmis;
}

//lecture
void Fourmiliere::case_fourmi(std::string& line, int& etat, int& count_C, int& count_D,
							  int& count_P, int& countHill)
{
	if (count_C > 0)
	{
		add_fourmi(decodage_ligne_fourmi(line, COLLECTOR));
		--count_C;
	}
	else if (count_D > 0)
	{
		Fourmi* fourmi_lue(decodage_ligne_fourmi(line, DEFENSOR));
		check_defensor_within_home(fourmi_lue, countHill);
		add_fourmi(fourmi_lue);
		--count_D;
	}
	else if (count_P > 0)
	{
		add_fourmi(decodage_ligne_fourmi(line, PREDATOR));
		--count_P;
		if (count_P == 0) etat = FOURMILIERE;
	}
	if (count_C == 0 and count_P == 0 and count_D == 0) etat = FOURMILIERE;
}

std::string Fourmiliere::get_info()
{
	std::ostringstream str_stream;
	str_stream << std::setprecision(6) << generator.get_total_food();
	
	std::string total_food_str = str_stream.str();
	
	std::string info;
	
	info =           "id: " + std::to_string(id)  + 
		   "\nTotal food: " + total_food_str      +
		        "\n\nnbC: " + std::to_string(nbC) + 
		          "\nnbD: " + std::to_string(nbD) +
		          "\nnbP: " + std::to_string(nbP) ;
			
	return info;
}

//graphics
void Fourmiliere::highlight() const
{
	border.draw_sq_highlight(id);
}

void Fourmiliere::draw_anthill() const
{
	border.draw_sq_border(id);
	generator.draw(id);
}

void Fourmiliere::draw_ants() const
{
	for (const auto& fourmi : fourmis) fourmi->draw(id);
}

//output
void Fourmiliere::ecriture(std::ofstream& outfile) const
{
	outfile << "	" << border.origin.x << " "
					  << border.origin.y << " " 
					  << border.side << " ";
	generator.ecriture(outfile);
	outfile << nbC << " " << nbD << " " << nbP << " # anthill #" << id+1 << std::endl;
	
	//fourmis
	for (int type(COLLECTOR) ; type < GENERATOR ; ++type)
	{
		std::string type_str;
		switch (type)
		{
			case COLLECTOR: type_str = "collector"; break;
			case DEFENSOR : type_str = "defensor"; break;
			case PREDATOR : type_str = "predator";
		}
		outfile << "	# " + type_str + "(s):" << std::endl;
		for (const auto& fourmi : fourmis)
		{
			if (fourmi->get_type() == type)
			{
				outfile << " 	";
				fourmi->ecriture(outfile);
			}
		}
		outfile << std::endl;
	}
}

//action
int Fourmiliere::compute_sizeF()
{
	return sqrt(4*(pow(sizeG,2)+pow(sizeC,2)*nbC+pow(sizeD,2)*nbD+pow(sizeP,2)*nbP));
}

void Fourmiliere::next_step(bool ant_birth,
							std::vector<Fourmiliere>& copie_fourmilieres, int id)
{
	std::vector<Ants> fourmis_monde;
	
	for(size_t i(0) ; i < copie_fourmilieres.size() ; ++i)
		fourmis_monde.push_back(copie_fourmilieres[i].get_fourmis());

	generator.eat(nbT);				   //consommation generator
	if (ant_birth) creation_fourmi();  //naissance (age =0)								
	generator.act(border, get_interior(), generator.get_body(),
							fourmis_monde, id, constrained); //déplacement generator
		
	if (not generator.get_end_of_klan())
	{
		for (auto const& fourmi : fourmis)
		{
			fourmi->viellir();
			fourmi->check_death(fourmis_monde, id);
			if (not fourmi->life_ended())
			{
				fourmi->act(border, get_interior(), generator.get_body(),
							fourmis_monde, id, constrained);
				fourmi->check_death(fourmis_monde, id);
			}
			
			for(size_t i(0) ; i < copie_fourmilieres.size() ; ++i)
			{
				if ((unsigned)id != i)
					copie_fourmilieres[i].set_fourmis(fourmis_monde[i]);
			}
		}
		if (get_food_delivered() > 0)
		{
			generator.increase_total_food(val_food*get_food_delivered());
			reset_food_delivered();
		}
	}
}

void Fourmiliere::purge()
{
	for (size_t i(0) ; i < fourmis.size() ; ++i)
	{
		if (fourmis[i]->life_ended())
		{
			switch (fourmis[i]->get_type())
			{
				case COLLECTOR: nbC--; break;
				case DEFENSOR:  nbD--; break;
				case PREDATOR:  nbP--; 
			}
			
			nbT--;
			
			fourmis[i]->undraw_bool();
			if (fourmis[i]->get_food() == LOADED)
			{
				add_nourriture(Nourriture(fourmis[i]->get_body().origin));
			}
				
			fourmis[i].reset();
			std::swap(fourmis[i], fourmis[fourmis.size()-1]);
			fourmis.pop_back();
			--i;
		}
	}
}

void Fourmiliere::creation_fourmi()
{
	int type(COLLECTOR);
	if (constrained)
	{
		if ((double)nbC/(nbT-1) < prop_constrained_collector or nbT == 1)
			type = COLLECTOR;
		else if ((double)nbD/(nbT-1) < prop_constrained_defensor)
			type = DEFENSOR;
		else type = PREDATOR;
	}
	else
	{
		if ((double)nbC/(nbT-1) < prop_free_collector or nbT == 1)
			type = COLLECTOR;
		else if ((double)nbD/(nbT-1.) < prop_free_defensor)
			type = DEFENSOR;
		else type = PREDATOR;
	}
	place_ant(type);
}

void Fourmiliere::place_ant(const int& type)
{
	Square future_body;
	switch (type)
	{
		case COLLECTOR:
		{
			if (get_interior().enough_space(3))
			{
				future_body = get_interior().find_space(3,1);
				++nbC;
				++nbT;
				add_fourmi(new Collector(future_body,0,0));
			}
		} break;
		case DEFENSOR:
		{
			if (get_interior().enough_space(3))
			{
				future_body = get_interior().find_space(3,1);
				++nbD;
				++nbT;
				add_fourmi(new Defensor(future_body,0));
			}
		} break;
		case PREDATOR: 
		{
			if (get_interior().enough_space(1))
			{
				future_body = get_interior().find_space(1,1);
				++nbP;
				++nbT;
				add_fourmi(new Predator(future_body,0));
			}
		}
	}
}

void Fourmiliere::apocalypse()
{
	generator.undraw_bool();
	for (auto const& fourmi : fourmis) fourmi->die();
	purge();
}

void Fourmiliere::add_fourmi(Fourmi* f)
{
	if (f != nullptr) fourmis.push_back(std::unique_ptr<Fourmi>(f)); 
}

void Fourmiliere::draw_sq() const
{
	for (auto const & fourmi : fourmis) fourmi->get_body().draw();
}

//file input
Fourmiliere decodage_ligne_fourmiliere(std::string line)
{
	std::istringstream data(line); 
	int x(0), y(0), side(0), xg(0), yg(0), nbC(0), nbD(0), nbP(0);
	double total_food(0);
	if(!(data >> x >> y >> side >> xg >> yg >> total_food >> nbC >> nbD >> nbP))
		set_error_found(true);
		
	Square espace_generator(xg, yg, sizeG, true);
	return Fourmiliere(x, y, side, espace_generator, total_food, nbC, nbD, nbP);
}
