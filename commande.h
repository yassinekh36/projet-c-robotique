#ifndef ROBOT_COMMANDE_H
#define ROBOT_COMMANDE_H

typedef enum {
    AVANCER,
    RECULER,
    TOURNER_GAUCHE,
    TOURNER_DROITE,
    ARRET_URGENCE
} Mouvement;

typedef enum {
    NORMALE,
    URGENTE
} Priorite;

typedef struct {
    Mouvement mouvement;
    int duree;
    int vitesse;
    Priorite priorite;
} Commande;

#endif
