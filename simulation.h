/*!
  \file    simulation.h
  \authors Adrian Morel, Luca Jiménez Glur
		   345863        345448
  \date    Mai 2022
  \brief   Interface du module "simulation".
*/

#ifndef SIMULATION_H
#define SIMULATION_H
#include "fourmiliere.h"


class Simulation
{
public:
	Simulation();
	~Simulation() {}
	
	//file_input
	void lecture(std::string nom_fichier);
	void ecriture(std::string nom_fichier);
	
	//destruction de la structure de données
	void destroy_sim();
	
	//getters
	int get_nbF() { return nb_fourmilieres; }
	std::string get_info_fourmiliere(int current_anthill);
	void draw(int id);
	
	//mise a jour
	void mise_a_jour();
	void creation_nourriture();
	void growth(Fourmiliere& fourmiliere);
	void from_lower_left(Square& sq_f1, bool& success, int& difference,
						 Fourmiliere& fourmiliere);
	void from_upper_left(Square& sq_f1, bool& success, int& difference,
						 Fourmiliere& fourmiliere);
	void from_upper_right(Square& sq_f1, bool& success, int& difference,
						  Fourmiliere& fourmiliere);
	void from_lower_right(Square& sq_f1, bool& success, int& difference,
						  Fourmiliere& fourmiliere);
private:
	//attributs
	std::vector<Fourmiliere> fourmilieres;
	int nb_fourmilieres = 0;
	
	//actions
	void add_fourmiliere(Fourmiliere f);
	
	//cases lecture
	void case_nbn(int& nb_restant, std::string& line, int& etat);
	
	void case_nbf(int& nb_restant, std::string& line, int& etat);
	void case_fourmiliere(int& nb_restant, std::string& line, int& etat, int& count_C,
	                    int& count_D, int& count_P, int& countHill);
	                    
};

void appel_lecture(char * nom_fichier);
void appel_ecriture(std::string nom_fichier);
int get_nbN_simulation();
int get_nbF_simulation();
std::string get_info_fourmiliere_sim(int current_anthill);
void draw_sim(int current_anthill);
void mise_a_jour_sim();
#endif


