#include "commande.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Tâche 2 — Deux files chaînées */
typedef struct Maillon {
    Commande commande;
    struct Maillon *suivant;
} Maillon;

typedef struct {
    Maillon *tete;
    Maillon *queue;
} File;

int enfiler(File *file, Commande commande)
{
    Maillon *nouveau = malloc(sizeof *nouveau);
    if (nouveau == NULL) {
        return 0;
    }

    nouveau->commande = commande;
    nouveau->suivant = NULL;

    if (file->queue == NULL) {
        file->tete = nouveau;
    } else {
        file->queue->suivant = nouveau;
    }

    file->queue = nouveau;
    return 1;
}

int defiler(File *file, Commande *commande)
{
    if (file->tete == NULL) {
        return 0;
    }

    Maillon *ancien = file->tete;
    *commande = ancien->commande;
    file->tete = ancien->suivant;

    if (file->tete == NULL) {
        file->queue = NULL;
    }

    free(ancien);
    return 1;
}

int ajouter_commande_normale(File *file, Commande commande)
{
    commande.priorite = NORMALE;
    return enfiler(file, commande);
}

int ajouter_commande_urgente(File *file, Commande commande)
{
    commande.priorite = URGENTE;
    return enfiler(file, commande);
}

int prendre_prochaine_commande(File *normales, File *urgentes,
                               Commande *commande)
{
    if (defiler(urgentes, commande)) {
        return 1;
    }

    return defiler(normales, commande);
}

void vider_file(File *file)
{
    Commande commande;
    while (defiler(file, &commande)) {
    }
}

/* Tâches 3 et 6 — Capteur et mode démonstration */
int obstacle_detecte(int mode_demo, int normales_executees)
{
    if (mode_demo) {
        return normales_executees == 1;
    }

    return rand() % 4 == 0;
}

int ajouter_contournement(File *urgentes)
{
    Commande actions[] = {
        {ARRET_URGENCE, 0, 0, URGENTE},
        {TOURNER_GAUCHE, 1, 20, URGENTE},
        {AVANCER, 2, 20, URGENTE},
        {TOURNER_DROITE, 1, 20, URGENTE}
    };

    for (size_t i = 0; i < sizeof actions / sizeof actions[0]; i++) {
        if (!ajouter_commande_urgente(urgentes, actions[i])) {
            return 0;
        }
    }

    return 1;
}

const char *nom_mouvement(Mouvement mouvement)
{
    switch (mouvement) {
        case AVANCER:        return "Avancer";
        case RECULER:        return "Reculer";
        case TOURNER_GAUCHE: return "Tourner a gauche";
        case TOURNER_DROITE: return "Tourner a droite";
        case ARRET_URGENCE:  return "Arret d'urgence";
    }

    return "Mouvement inconnu";
}

/* Tâche 5 — Journal des actions */
int ecrire_journal(FILE *journal, Commande commande)
{
    time_t maintenant = time(NULL);
    struct tm *date = localtime(&maintenant);
    char horodatage[32];

    if (date == NULL ||
        strftime(horodatage, sizeof horodatage, "%Y-%m-%d %H:%M:%S", date) == 0) {
        return 0;
    }

    const char *priorite = commande.priorite == URGENTE ? "URGENTE" : "NORMALE";

    return fprintf(journal, "[%s] [%s] %s %d s, vitesse %d\n",
                   horodatage, priorite, nom_mouvement(commande.mouvement),
                   commande.duree, commande.vitesse) >= 0;
}

/* Tâche 4 — Exécution simulée */
int executer_commandes(File *normales, File *urgentes, FILE *journal,
                       int mode_demo)
{
    int normales_executees = 0;
    int capteur_verifie = 0;
    Commande commande;

    while (normales->tete != NULL || urgentes->tete != NULL) {
        if (urgentes->tete == NULL && !capteur_verifie) {
            capteur_verifie = 1;

            if (obstacle_detecte(mode_demo, normales_executees)) {
                puts("Obstacle detecte !");
                if (!ajouter_contournement(urgentes)) {
                    fputs("Impossible d'ajouter le contournement.\n", stderr);
                    return 0;
                }
            }
        }

        prendre_prochaine_commande(normales, urgentes, &commande);
        const char *priorite = commande.priorite == URGENTE
                               ? "COMMANDE URGENTE"
                               : "Commande normale";

        printf("%s : %s %d s, vitesse %d\n", priorite,
               nom_mouvement(commande.mouvement), commande.duree,
               commande.vitesse);

        if (!ecrire_journal(journal, commande)) {
            fputs("Impossible d'ecrire dans le journal.\n", stderr);
            return 0;
        }

        if (commande.priorite == NORMALE) {
            normales_executees++;
            capteur_verifie = 0;
        }
    }

    return 1;
}

int main(int argc, char *argv[])
{
    int mode_demo = argc == 2 && strcmp(argv[1], "--demo") == 0;
    if (argc > 1 && !mode_demo) {
        fprintf(stderr, "Utilisation : %s [--demo]\n", argv[0]);
        return EXIT_FAILURE;
    }

    File normales = {NULL, NULL};
    File urgentes = {NULL, NULL};
    Commande trajet[] = {
        {AVANCER, 2, 30, NORMALE},
        {TOURNER_DROITE, 1, 20, NORMALE},
        {RECULER, 2, 25, NORMALE}
    };

    srand((unsigned) time(NULL));

    for (size_t i = 0; i < sizeof trajet / sizeof trajet[0]; i++) {
        if (!ajouter_commande_normale(&normales, trajet[i])) {
            fputs("Impossible d'ajouter les commandes normales.\n", stderr);
            vider_file(&normales);
            return EXIT_FAILURE;
        }
    }

    FILE *journal = fopen("journal_robot.txt", "a");
    if (journal == NULL) {
        perror("journal_robot.txt");
        vider_file(&normales);
        return EXIT_FAILURE;
    }

    int succes = executer_commandes(&normales, &urgentes, journal, mode_demo);

    vider_file(&normales);
    vider_file(&urgentes);
    if (fclose(journal) == EOF) {
        perror("journal_robot.txt");
        succes = 0;
    }

    return succes ? EXIT_SUCCESS : EXIT_FAILURE;
}