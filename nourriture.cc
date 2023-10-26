/*!
  \file    nourriture.cc
  \authors Adrian Morel : 50 %, Luca Jiménez Glur 50 %
		   345863               345448
  \date    Mai 2022
  \brief   Implémentation du module "nourriture".
*/

//dependencies
#include "message.h"
#include "nourriture.h"

static std::vector<Nourriture> aliments;

//constructeur
Nourriture::Nourriture(int x, int y)
	: place(Square(x,y,1)) 
{
	
}

Nourriture::Nourriture(Point p)
	: place(Square(p,1)) 
{
	
}

//graphic
void Nourriture::draw() const 
{
	place.draw_sq_losange();
}

//ecriture
void Nourriture::ecriture(std::ofstream& outfile) const
{
	outfile << place.origin.x << " " << place.origin.y << std::endl;
}

//file input
Nourriture decodage_ligne_nourriture(std::string line)
{
	std::istringstream data(line);
	int x(0), y(0);
	
	if(!(data >> x >> y)) set_error_found(true);
	
	Square place(x,y,1);
	if (not Square(x,y,1).spaceFree())
	{
		std::cout << message::food_overlap(x,y);
		set_error_found(true);
	}
	place.validation_de_carre();
	place.draw();
	
	return Nourriture(x,y);
}

void case_nourriture(int& nb_restant, std::string& line, int& etat)
{
	if (nb_restant > 0)
	{
		add_nourriture(decodage_ligne_nourriture(line));
		
		--nb_restant;
		if (nb_restant == 0)
			etat = NBF;
	}
}

//adder
void add_nourriture(Nourriture n)
{
	n.getPlace().draw();
	aliments.push_back(n);
}

//getter		
int get_nbN()
{
	return aliments.size();
}

std::vector<Nourriture> get_aliments()
{
	return aliments;
}

//destroyer
void destroy_nourriture()
{
	aliments = std::vector<Nourriture>();
}

void erase_nourriture(int indice)
{
	if (not ((unsigned)indice >= aliments.size() or  indice < 0))
	{
		aliments[indice] = aliments[aliments.size()-1];
		aliments.pop_back();
	}
}

//graphic
void draw_aliments()
{
	for (const auto& nourriture : aliments)
		nourriture.draw();
}
