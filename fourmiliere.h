/*!
  \file    fourmiliere.h
  \authors Adrian Morel, Luca Jiménez Glur
		   345863        345448
  \date    Mai 2022
  \brief   Interface du module "fourmiliere".
*/

#ifndef FOURMILIERE_H
#define FOURMILIERE_H
#include "fourmi.h"
#include "nourriture.h"

class Fourmiliere
{
public:
	//constructeurs
	Fourmiliere(int x = 0, int y =  0, int side = 128,
				Square body_generator = Square(2,2,sizeG,true), double total_food = 0,
				int nbC = 0, int nbD = 0, int nbP = 0);
	Fourmiliere(const Fourmiliere& f);
	~ Fourmiliere() {}
	
	//setter
	void set_id(int& countHill);
	void adjust_border(Square new_border);
	void set_constrained(bool c) { constrained = c; }
	void set_fourmis(Ants& nouvelles_fourmis);
	
	//surcharge de l'operateur d'affectation
	Fourmiliere& operator=(Fourmiliere f);
	
	//tests
	void check_defensor_within_home(Fourmi* d, int& countHill) const;
	void check_generator_within_home() const;
	bool homes_overlap(const Fourmiliere& f2) const;
	
	//getters
	Square 		 get_border()    const { return border; }
	Generator    get_generator() const { return generator; }
	Square 	     get_interior()  const;
	int 		 get_nbC()       const { return nbC; }
	int          get_nbD()       const { return nbD; }
	int          get_nbP()       const { return nbP; }
	unsigned int get_id()        const { return id; }
	Ants get_fourmis() const;
	
	//lecture
	void case_fourmi(std::string& line, int& etat, int& count_C, int& count_D,
					 int& count_P, int& countHill);
	std::string get_info();
	
	//graphics
	void highlight() const;
	void draw_anthill() const;
	void draw_ants() const;
	
	//output
	void ecriture(std::ofstream& outfile) const;
	
	//action
	int compute_sizeF();
	void next_step(bool ant_birth, std::vector<Fourmiliere>& fourmilieres, int id);
	void purge();
	void creation_fourmi();
	void place_ant(const int& type);
	void apocalypse();
	void add_fourmi(Fourmi* f);
	void draw_sq() const;
private:
	//attributs
	Square       border;
	Generator    generator;
	int          nbC;
	int          nbD;
	int          nbP;
	int			 nbT;
	int 		 sizeF;
	unsigned int id;
	
	Ants fourmis;
	
	bool constrained = FREE;
};

//file input
Fourmiliere decodage_ligne_fourmiliere(std::string line);
#endif
