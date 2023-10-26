/*!
  \file    nourriture.h
  \authors Adrian Morel, Luca Jiménez Glur
		   345863        345448
  \date    Mai 2022
  \brief   Interface du module "nourriture".
*/

#ifndef NOURRITURE_H
#define NOURRITURE_H
#include "squarecell.h"
#include "constantes.h"

enum Etat_lecture {NBN, NOURRITURE, NBF, FOURMILIERE, FOURMI};

class Nourriture
{
public:
	//constructeur
	Nourriture(int x, int y);
	Nourriture(Point p);
	
	//getter
	Square getPlace() const { return place; }
	
	//graphic
	void draw() const;
	
	//ecriture
	void ecriture(std::ofstream& outfile) const;
private:
	Square place;
};

//file input
Nourriture decodage_ligne_nourriture(std::string line);
void case_nourriture(int& nb_restant, std::string& line, int& etat);

//adder
void add_nourriture(Nourriture n);

//getter
int get_nbN();
std::vector<Nourriture> get_aliments();

//destroyer
void destroy_nourriture();
void erase_nourriture(int indice);

//graphic
void draw_aliments();
#endif
