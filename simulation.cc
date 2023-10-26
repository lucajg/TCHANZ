/*!
  \file    simulation.cc
  \authors Adrian Morel : 50 %, Luca Jiménez Glur 50 %
		   345863               345448
  \date    Mai 2022
  \brief   Implémentation du module "simulation".
*/

//dependencies
#include "message.h"
#include "simulation.h"

static Simulation simulation;


Simulation::Simulation() : fourmilieres() {}

//file input
void Simulation::lecture(std::string nom_fichier)
{
    destroy_sim();
    set_lecture(true);
    std::ifstream fichier(nom_fichier); 
    if(!fichier.fail()) 
    {
		std::string line;
		int etat(NBN);
		int nb_restant(0), count_C(0), count_D(0), count_P(0), countHill(-1);
        while(getline(fichier >> std::ws, line)) 
        {
			if(line[0]=='#') continue;
			switch (etat)
			{
				case NBN : case_nbn(nb_restant, line, etat); break;
				case NOURRITURE : case_nourriture(nb_restant, line, etat); break;
				case NBF: case_nbf(nb_restant, line, etat); break;
				case FOURMILIERE :
				{
					case_fourmiliere(nb_restant, line, etat, count_C, count_D,
									 count_P, countHill); break;
				}
				case FOURMI:
				{
					fourmilieres[countHill].case_fourmi(line, etat, count_C, count_D,
														count_P, countHill); break;
				}
				default : break;
			}
			if (error_found())
			{
				destroy_sim();
				return;
			}
        }
	}
	std::cout << message::success();
	set_lecture(false);
}

void Simulation::ecriture(std::string nom_fichier)
{
	std::ofstream outfile(nom_fichier);
	
	//food
	outfile << "# nb food" << std::endl << get_nbN() << std::endl << std::endl;
	outfile << "# food" << std::endl;
	for (auto food : get_aliments()) food.ecriture(outfile);
	outfile << std::endl << std::endl;
	
	//anthills
	outfile << fourmilieres.size() << " # nb anthill" << std::endl << std::endl;
	for (auto fourmiliere : fourmilieres) fourmiliere.ecriture(outfile);
	
	//finish
	outfile.close();
}

//destruction de la structure de données
void Simulation::destroy_sim()
{
	set_lecture(false);
	*this = Simulation();
	destroy_nourriture();
	set_error_found(false);
	reset_grid();
}

std::string Simulation::get_info_fourmiliere(int current_anthill)
{
	return fourmilieres[current_anthill].get_info();
}

void Simulation::draw(int current_anthill)
{
	if ((unsigned)current_anthill != fourmilieres.size())
		fourmilieres[current_anthill].highlight();
	for (const auto& fourmiliere : fourmilieres)
		fourmiliere.draw_anthill();
		
	for (const auto& fourmiliere : fourmilieres)
		fourmiliere.draw_ants();
		
	draw_aliments();
	
}

//actions
void Simulation::add_fourmiliere(Fourmiliere f) { fourmilieres.push_back(f); }

//cases lecture
void Simulation::case_nbn(int& nb_restant, std::string& line, int& etat)
{
	std::istringstream data(line);
	if(!(data >> nb_restant)) exit(EXIT_FAILURE); 
	if(nb_restant == 0) etat = NBF;
	else etat = NOURRITURE;
}

void Simulation::case_nbf(int& nb_restant, std::string& line, int& etat)
{
	std::istringstream data(line);
	if(!(data >> nb_restant)) set_error_found(true); 
	else etat = FOURMILIERE ; 
	nb_fourmilieres = nb_restant;
}

void Simulation::case_fourmiliere (int & nb_restant, std::string & line, int& etat,
		                           int& count_C, int& count_D, int& count_P,
		                           int& countHill)
{
	++countHill;
	if (nb_restant >= 0)
	{
		--nb_restant;
		Fourmiliere temp_f(decodage_ligne_fourmiliere(line));
		temp_f.set_id(countHill);
		temp_f.check_generator_within_home();
		for (auto const& fourmiliere : fourmilieres)
		{
			if (temp_f.homes_overlap(fourmiliere))
			{
				std::cout << message::homes_overlap(fourmiliere.get_id(), countHill);
				set_error_found(true);
			}
		}
		
		count_C = temp_f.get_nbC();
		count_D = temp_f.get_nbD();
		count_P = temp_f.get_nbP();
		
		add_fourmiliere(temp_f);
		
		if (count_C == 0 and count_D == 0 and count_P == 0) etat = FOURMILIERE;
		else etat = FOURMI;
	}
}

//mise a jour
void Simulation::mise_a_jour()
{
	//Génération aléatoire de nourriture dans l’espace libre
	bool create_food = random_bool(food_rate);
	if (create_food) creation_nourriture();

	std::vector<Fourmiliere> copie_fourmilieres(fourmilieres);
	for (size_t i(0) ; i < copie_fourmilieres.size(); ++i)
	{
		growth(copie_fourmilieres[i]);  //ajustement de la fourmiliere 
		
		double p(std::min(1.,
				 copie_fourmilieres[i].get_generator().get_total_food()*birth_rate));
		bool ant_birth = random_bool(p);
		
		copie_fourmilieres[i].next_step(ant_birth, copie_fourmilieres, i);
	}
	
	fourmilieres = copie_fourmilieres;
	
	size_t count(0);
	while (count < fourmilieres.size())
	{
		//destruction des fourmis dont age >= bug_life ou end_of_life == true
		fourmilieres[count].purge();
		
		//destruction de la fourmiliere si end_of_klan
		if (fourmilieres[count].get_generator().get_end_of_klan())
		{
			fourmilieres[count].apocalypse();
			for (size_t i(count) ; i < fourmilieres.size() - 1 ; ++i)
				fourmilieres[i] = fourmilieres[i+1];
			fourmilieres.pop_back();
			--count;
			--nb_fourmilieres;
		}
		++count;
	}
	//guard for when predator fall on top of collector
	for (auto const & fourmiliere : fourmilieres) fourmiliere.draw_sq();
}

void Simulation::creation_nourriture()
{
	int current_trial(0);
	bool success(true);
	do
	{
		success = true;
		++current_trial;
		int x(random_coordinate());
		int y(random_coordinate());
		Square pos_n(x,y,1);
		if (pos_n.spaceFree())
		{
			if (nb_fourmilieres != 0)
			{
				for (auto const& fourmiliere : fourmilieres)
				{
					if (pos_n.within(fourmiliere.get_border()))
					{
						success = false;
					}
				}
			} 
		}
		else success = false;
		if (success)
		{
			add_nourriture(Nourriture(x,y));
			pos_n.draw();
		}
	} while (current_trial <= max_food_trial and (not success));
}

void Simulation::growth(Fourmiliere& fourmiliere)
{
	bool success(true);
	Square sq_f1(fourmiliere.get_border());
	int old_side(sq_f1.side);
	sq_f1.side = fourmiliere.compute_sizeF() + 2;
	int difference(sq_f1.side - old_side);
	
	from_lower_left(sq_f1, success, difference, fourmiliere);
	
	if (not success) from_upper_left(sq_f1, success, difference, fourmiliere);
	
	if (not success) from_upper_right(sq_f1, success, difference, fourmiliere);
	
	if (not success) from_lower_right(sq_f1, success, difference, fourmiliere);
	
	if (success)
	{
		fourmiliere.set_constrained(false);
		fourmiliere.adjust_border(sq_f1);
	}
	else fourmiliere.set_constrained(true);
}

void Simulation::from_lower_left(Square& sq_f1, bool& success, int& difference,
								 Fourmiliere& fourmiliere)
{
	success = true;
	for (auto const& f : fourmilieres)
		if ((f.get_id() != fourmiliere.get_id() and sq_f1.superpose(f.get_border())) or
		    (not sq_f1.in_grid())) success = false;
}

void Simulation::from_upper_left(Square& sq_f1, bool& success, int& difference,
								 Fourmiliere& fourmiliere)
{
	success = true;
	sq_f1.origin.y -= difference; 
	for (auto const& f : fourmilieres)
		if ((f.get_id() != fourmiliere.get_id() and
			sq_f1.superpose(f.get_border())) or (not sq_f1.in_grid()))
			success = false;
}

void Simulation::from_upper_right(Square& sq_f1, bool& success, int& difference,
								  Fourmiliere& fourmiliere)
{
	success = true;
	sq_f1.origin.x -= difference; 
	for (auto const& f : fourmilieres)
		if ((f.get_id() != fourmiliere.get_id() and
			sq_f1.superpose(f.get_border())) or (not sq_f1.in_grid()))
			success = false;
}

void Simulation::from_lower_right(Square& sq_f1, bool& success, int& difference,
								  Fourmiliere& fourmiliere)
{
	success = true;
	sq_f1.origin.y += difference; 
	for (auto const& f : fourmilieres)
		if ((f.get_id() != fourmiliere.get_id() and 
			sq_f1.superpose(f.get_border())) or (not sq_f1.in_grid()))
			success = false;
}

void appel_lecture(char * nom_fichier)
{
	simulation.lecture(nom_fichier);
}

void appel_ecriture(std::string nom_fichier)
{
	simulation.ecriture(nom_fichier);
}

int get_nbF_simulation() { return simulation.get_nbF(); }

std::string get_info_fourmiliere_sim(int current_anthill)
{
	return simulation.get_info_fourmiliere(current_anthill);
}

void draw_sim(int current_anthill)
{
	simulation.draw(current_anthill);
}

void mise_a_jour_sim()
{
	simulation.mise_a_jour();
}
