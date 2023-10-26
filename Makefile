# fichier Makefile
# Adrian Morel, Luca Jimenez Glur
# 345863		345448
# Avril 2022

# Definitions de macros

CXX     = g++
CXXFLAGS = -g -Wall -std=c++11
CXXFILES = projet.cc simulation.cc fourmiliere.cc fourmi.cc nourriture.cc \
		   message.cc squarecell.cc error_squarecell.cc gui.cc graphic.cc
LINKING = `pkg-config --cflags gtkmm-3.0`
LDLIBS = `pkg-config --libs gtkmm-3.0`
OFILES = $(CXXFILES:.cc=.o)

# Definition de la premiere regle

projet: $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) -o projet $(LDLIBS)

# Definitions de cibles particulieres

depend:
	@echo " *** MISE A JOUR DES DEPENDANCES ***"
	@(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	  $(CXX) -MM $(CXXFLAGS) $(CXXFILES) | \
	  egrep -v "/usr/include" \
	 ) >Makefile.new
	@mv Makefile.new Makefile

clean:
	@echo " *** EFFACE MODULES OBJET ET EXECUTABLE ***"
	@/bin/rm -f *.o *.x *.cc~ *.h~ projet

#
# -- Regles de dependances generees automatiquement
#
# DO NOT DELETE THIS LINE
projet.o: projet.cc gui.h simulation.h fourmiliere.h fourmi.h \
 squarecell.h constantes.h nourriture.h message.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
	
simulation.o: simulation.cc message.h simulation.h fourmiliere.h fourmi.h \
 squarecell.h constantes.h nourriture.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
	
fourmiliere.o: fourmiliere.cc message.h fourmiliere.h fourmi.h \
 squarecell.h constantes.h nourriture.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
	
fourmi.o: fourmi.cc nourriture.h squarecell.h constantes.h message.h \
 fourmi.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
	
nourriture.o: nourriture.cc message.h nourriture.h squarecell.h \
 constantes.h
message.o: message.cc message.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
	
squarecell.o: squarecell.cc error_squarecell.h graphic.h squarecell.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
	
error_squarecell.o: error_squarecell.cc error_squarecell.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
	
gui.o: gui.cc simulation.h fourmiliere.h fourmi.h squarecell.h \
 constantes.h nourriture.h graphic_gui.h graphic.h gui.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
	
graphic.o: graphic.cc graphic_gui.h graphic.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
