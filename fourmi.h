/*!
  \file    fourmi.h
  \authors Adrian Morel, Luca Jiménez Glur
		   345863        345448
  \date    Mai 2022
  \brief   Interface du module "fourmi".
*/

#ifndef FOURMI_H
#define FOURMI_H
#include "squarecell.h"
#include "nourriture.h"
#include <memory>

enum Type_fourmi {COLLECTOR, DEFENSOR, PREDATOR, GENERATOR};

class Fourmi;

typedef std::vector<std::unique_ptr<Fourmi>> Ants;

class Fourmi
{
public:
	//constructeur
	Fourmi(Square body, int age);
	virtual ~Fourmi() {}
	
	//methode auxiliaire pour constructeur de copie de fourmiliere
	virtual Fourmi* clone() const = 0;
	
	//getter
	Square get_body() const { return body; }
	virtual int get_type() const = 0;
	bool life_ended() const;
	virtual bool get_food() const { return false; }
	
	//graphics
	virtual void draw(const int& id) const;
	
	//file output
	virtual void ecriture(std::ostream& outfile) const;
	
	//action
	void viellir();
	virtual void move(int direction);
	virtual void act(Square border, Square anthill, Square body_gen,
					 std::vector<Ants>& fourmis_monde, int id, bool constrained)=0;
	virtual void die();
	virtual void check_death(std::vector<Ants>& fourmis_monde, int& id);
		//mouvement
	void undraw_bool() const;
	void draw_bool() const;
	
protected:
	//attributs
	Square body;
	int age;
	bool end_of_life = false;
};

class Generator : public Fourmi
{
public:
	//constructeur
	Generator(Square body, double total_food, int age = 0);
	
	virtual Fourmi* clone() const override;
	
	//getter
	double get_total_food() const { return total_food; }
	virtual int get_type() const override {return GENERATOR;}
	bool get_end_of_klan() { return end_of_klan; }
	
	//output
	virtual void ecriture(std::ostream& outfile) const override;
	
	//action
	void eat(int nbT);
	virtual void die() override;
	virtual void act(Square border, Square anthill, Square body_gen,
					 std::vector<Ants>& fourmis_monde, int id,
					 bool constrained) override;
	void increase_total_food(double val);	
	
private:
	//attributs
	double total_food;
	bool end_of_klan = false;
};

class Collector : public Fourmi
{
public:
	//constructeur
	Collector(Square body, int age, bool food);
	
	//methode auxiliaire pour constructeur de copie de fourmiliere
	virtual Fourmi* clone() const override;
	
	//graphics
	virtual void draw(const int& id) const override;
	
	//getter
	virtual int get_type() const override { return COLLECTOR; }
	virtual bool get_food() const override { return food; }
	
	//output
	virtual void ecriture(std::ostream& outfile) const override;
	
	//action
	virtual void act(Square border, Square anthill, Square body_gen,
					 std::vector<Ants>& fourmis_monde, int id,
					 bool constrained) override;
	void look_for_food(Square border);
	void go_outside(Square border);
	void go_outside_equidistance(int& dist_inf_g, int& dist_sup_g, int& dist_sup_d,
								 int& dist_inf_d, std::vector<int>& distances);
	bool reachable(Point p);
	void back_home(Square anthill, Square body_generator);
	virtual void check_death(std::vector<Ants>& fourmis_monde, int& id) override;
private:
	//attribut
	bool food;
};

class Defensor : public Fourmi
{
public:
	//constructeur
	Defensor(Square body, int age);
	
	//methode auxiliaire pour constructeur de copie de fourmiliere
	virtual Fourmi* clone() const override;
	
	//graphics
	virtual void draw(const int& id) const override;
	
	//getter
	virtual int get_type() const override { return DEFENSOR; }
		
	//output
	virtual void ecriture(std::ostream& outfile) const override;

	//action
	virtual void act(Square border, Square anthill, Square body_gen,
					 std::vector<Ants>& fourmis_monde, int id,
					 bool constrained) override;
	void stick_to_border(double& dist_inf_g, double& dist_sup_g, double& dist_sup_d,
						 double& dist_inf_d, double& min);
	void stay_inside(double& dist_inf_g, double& dist_sup_g, double& dist_sup_d,
					 double& dist_inf_d, double& min);
	virtual void check_death(std::vector<Ants>& fourmis_monde, int& id) override;
private:
};

class Predator : public Fourmi
{
public:
	//constructeur
	Predator(Square body, int age);
	
	//methode auxiliaire pour constructeur de copie de fourmiliere
	virtual Fourmi* clone() const override;

	//getter
	virtual int get_type() const override { return PREDATOR; }
	
	//output
	virtual void ecriture(std::ostream& outfile) const override;
	
	//action
	virtual void act(Square border, Square anthill, Square body_gen,
					 std::vector<Ants>& fourmis_monde, int id,
					 bool constrained) override;
	virtual void check_death(std::vector<Ants>& fourmis_monde, int& id) override;
	void act_constrained(Square border, Square anthill, Square body_gen,
									std::vector<Ants>& fourmis_monde, int id);
	void act_free(Square border, Square anthill, Square body_gen,
									std::vector<Ants>& fourmis_monde, int id);
	void predator_move(Square target);
	void premier_quadrant(Square& future_body_1, Square& future_body_2);
	void deuxieme_quadrant(Square& future_body_1, Square& future_body_2);
	void troisieme_quadrant(Square& future_body_1, Square& future_body_2);
	void quatrieme_quadrant(Square& future_body_1, Square& future_body_2);
	void back_home(Square anthill);
private:
};

//file input
Fourmi* decodage_ligne_fourmi(std::string line, int type);
//food delivery
int get_food_delivered();
void increase_food_delivered();
void reset_food_delivered();
#endif
