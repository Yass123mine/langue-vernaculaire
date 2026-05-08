#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ================= STRUCTURES ================= */

typedef struct {
    int niveau;
    int coeurs;
    int pieces;
} Progression;

typedef struct {
    int id;
    char titre[50];
    char contenu[300];
} Lecon;

typedef struct {
    char question[200];
    char choix[4][100];
    int bonnereponse;
} Question;

/* ================= DONNÉES LEÇONS EWONDO ================= */

Lecon lecons_ewondo_n1[] = {
    {1, "Salutations",
     "Bembekiri     = Bonjour\n"
     "Mbolo         = Bonjour (familier)\n"
     "Mbolo minkie  = Bonsoir\n"
     "Ntem          = Au revoir\n"
     "A ye ?        = Comment ca va ?\n"
     "Ma ye         = Ca va bien\n"},
    {2, "La famille",
     "Tara          = Pere\n"
     "Nna           = Mere\n"
     "Ngon          = Enfant\n"
     "Ndom          = Frere\n"
     "Mebele        = Soeur\n"
     "Nkukuma       = Grand-pere\n"
     "Ya            = Grand-mere\n"},
    {3, "Les chiffres",
     "Abe           = Un\n"
     "Bele          = Deux\n"
     "Lala          = Trois\n"
     "Bini          = Quatre\n"
     "Tan           = Cinq\n"
     "Sambe         = Six\n"
     "Zambe         = Sept\n"},
    {4, "La nourriture",
     "Okok          = Feuilles de manioc\n"
     "Kpem          = Manioc\n"
     "Mbanga        = Noix de palme\n"
     "Nnam          = Nourriture\n"
     "Mame          = Poisson\n"
     "Soa           = Viande\n"},
    {5, "Le corps",
     "Owu           = Tete\n"
     "Mesep         = Yeux\n"
     "Ening         = Nez\n"
     "Owomo         = Bouche\n"
     "Mebaa         = Mains\n"
     "Mekon         = Pieds\n"}
};

Lecon lecons_ewondo_n2[] = {
    {1, "Se presenter",
     "Dze wa ?       = Comment tu t'appelles ?\n"
     "Ma dze...      = Je m'appelle...\n"
     "Fia wa ?       = D'ou viens-tu ?\n"
     "Ma fi Yaounde  = Je viens de Yaounde\n"
     "Abe wa ?       = Quel age as-tu ?\n"},
    {2, "Les couleurs",
     "Owondo         = Noir\n"
     "Oton           = Blanc\n"
     "Ngon owondo    = Rouge\n"
     "Bikoe          = Vert\n"
     "Osol           = Bleu\n"},
    {3, "Les animaux",
     "Nnam           = Poulet\n"
     "Ngoe           = Singe\n"
     "Ngon mbu       = Chien\n"
     "Koe            = Chevre\n"
     "Ndzom          = Elephant\n"},
    {4, "La maison",
     "Nda            = Maison\n"
     "Abaa           = Porte\n"
     "Fene           = Fenetre\n"
     "Nda minko      = Cuisine\n"
     "Bele nda       = Chambre\n"},
    {5, "Le temps",
     "Bikoe          = Matin\n"
     "Awoeng         = Midi\n"
     "Minkie         = Soir\n"
     "Fong           = Nuit\n"
     "Biko           = Aujourd'hui\n"
     "Biko si        = Demain\n"}
};

/* ================= DONNÉES LEÇONS FULFULDE ================= */

Lecon lecons_fulfulde_n1[] = {
    {1, "Salutations",
     "Jam nyalli    = Bonjour\n"
     "Jam waali     = Bonsoir\n"
     "A jam ?       = Comment ca va ?\n"
     "Jam tan       = Ca va bien\n"
     "Yo Allah jaro = Merci\n"
     "Aw dowa       = Au revoir\n"},
    {2, "La famille",
     "Baaba         = Pere\n"
     "Yumma         = Mere\n"
     "Biddo         = Enfant\n"
     "Mawdo         = Grand-pere\n"
     "Dow           = Mari\n"
     "Debbo         = Femme\n"},
    {3, "Les chiffres",
     "Go'o          = Un\n"
     "Didi          = Deux\n"
     "Tati          = Trois\n"
     "Nay           = Quatre\n"
     "Jowi          = Cinq\n"
     "Jeego'o       = Six\n"
     "Jeedidi       = Sept\n"},
    {4, "La nourriture",
     "Nyaamdu       = Nourriture\n"
     "Haako         = Legumes\n"
     "Horde         = Mil\n"
     "Kosam         = Lait\n"
     "Teew          = Viande\n"
     "Liingu        = Poisson\n"},
    {5, "Le corps",
     "Hoore         = Tete\n"
     "Gite          = Yeux\n"
     "Hinere        = Nez\n"
     "Hunuko        = Bouche\n"
     "Juude         = Mains\n"
     "Kojde         = Pieds\n"}
};

/* ================= DONNÉES LEÇONS DUALA ================= */

Lecon lecons_duala_n1[] = {
    {1, "Salutations",
     "Idib'a bwam   = Bonjour\n"
     "Ebiamu'a bwam = Bonsoir\n"
     "Bio lo jai ?  = Comment vas-tu ?\n"
     "E mala bwam   = Ca va bien\n"
     "Na som jita   = Merci beaucoup\n"
     "Ponda nipèpè  = A bientot\n"},
    {2, "La famille",
     "Tata          = Pere\n"
     "Mama          = Mere\n"
     "Mwana         = Enfant\n"
     "Ndabo         = Frere\n"
     "Loba          = Soeur\n"
     "Nkuku         = Grand-pere\n"},
    {3, "Les chiffres",
     "Mboa          = Un\n"
     "Bale          = Deux\n"
     "Lalo          = Trois\n"
     "Bone          = Quatre\n"
     "Tan           = Cinq\n"
     "Sambe         = Six\n"
     "Sambe na mboa = Sept\n"},
    {4, "La nourriture",
     "Nyama         = Viande\n"
     "Tolo          = Poisson\n"
     "Miondo        = Baton de manioc\n"
     "Mbanga        = Noix de palme\n"
     "Nnam          = Repas\n"},
    {5, "Le corps",
     "Mutu          = Tete\n"
     "Miso          = Yeux\n"
     "Minu          = Bouche\n"
     "Maboko        = Mains\n"
     "Makolo        = Pieds\n"}
};

/* ================= DONNÉES LEÇONS GHOMALA ================= */

Lecon lecons_ghomala_n1[] = {
    {1, "Salutations",
     "Fokdzu pepun  = Bonjour\n"
     "Aam ga ku     = Comment ca va ?\n"
     "A le te ze    = Ca va bien\n"
     "Ke cet        = Salut\n"
     "Na som        = Merci\n"},
    {2, "La famille",
     "Fo            = Pere\n"
     "Na            = Mere\n"
     "Kwi           = Enfant\n"
     "Nda           = Frere\n"
     "Mfo           = Chef\n"
     "Nkumu         = Grand-pere\n"},
    {3, "Les chiffres",
     "Tek           = Un\n"
     "Paa           = Deux\n"
     "Tat           = Trois\n"
     "Nen           = Quatre\n"
     "Taan          = Cinq\n"
     "Tuk           = Six\n"
     "Mfu           = Sept\n"},
    {4, "La nourriture",
     "Nkwi          = Viande\n"
     "Taa           = Legumes\n"
     "Kwa           = Manger\n"},
    {5, "Le corps",
     "Nto           = Tete\n"
     "Mbii          = Yeux\n"
     "Nuu           = Bouche\n"
     "Nko           = Mains\n"
     "Nku           = Pieds\n"}
};

/* ================= QUIZ EWONDO NIVEAU 1 ================= */

Question quiz_ewondo_n1[] = {
    {"Que signifie 'Bembekiri' ?",
     {"Au revoir", "Bonjour", "Merci", "Comment ca va"}, 1},
    {"Comment dit-on 'Mere' en Ewondo ?",
     {"Tara", "Ngon", "Nna", "Ya"}, 2},
    {"Que signifie 'Ma ye' ?",
     {"Ca va bien", "Bonsoir", "Au revoir", "Je m'appelle"}, 0},
    {"Comment dit-on 'Deux' en Ewondo ?",
     {"Abe", "Lala", "Bele", "Bini"}, 2},
    {"Que signifie 'Nnam' ?",
     {"Poisson", "Viande", "Nourriture", "Manioc"}, 2}
};

/* ================= QUIZ EWONDO NIVEAU 2 ================= */

Question quiz_ewondo_n2[] = {
    {"Comment demande-t-on 'Comment tu t'appelles ?' en Ewondo ?",
     {"Fia wa ?", "Dze wa ?", "A ye ?", "Abe wa ?"}, 1},
    {"Que signifie 'Owondo' ?",
     {"Blanc", "Rouge", "Vert", "Noir"}, 3},
    {"Comment dit-on 'Maison' en Ewondo ?",
     {"Abaa", "Fene", "Nda", "Bele nda"}, 2},
    {"Que signifie 'Minkie' ?",
     {"Matin", "Midi", "Soir", "Nuit"}, 2},
    {"Comment dit-on 'Elephant' en Ewondo ?",
     {"Ngoe", "Ndzom", "Koe", "Nnam"}, 1}
};

/* ================= QUIZ FULFULDE ================= */

Question quiz_fulfulde[] = {
    {"Que signifie 'Jam nyalli' ?",
     {"Au revoir", "Merci", "Bonjour", "Bonsoir"}, 2},
    {"Comment dit-on 'Mere' en Fulfulde ?",
     {"Baaba", "Yumma", "Debbo", "Mawdo"}, 1},
    {"Que signifie 'Jam tan' ?",
     {"Ca va bien", "Bonsoir", "Au revoir", "Merci"}, 0},
    {"Comment dit-on 'Cinq' en Fulfulde ?",
     {"Go'o", "Tati", "Nay", "Jowi"}, 3},
    {"Que signifie 'Teew' ?",
     {"Poisson", "Lait", "Viande", "Legumes"}, 2}
};

/* ================= QUIZ DUALA ================= */

Question quiz_duala[] = {
    {"Que signifie 'Idib'a bwam' ?",
     {"Bonsoir", "Bonjour", "Au revoir", "Merci"}, 1},
    {"Comment dit-on 'Merci beaucoup' en Duala ?",
     {"Ponda nipèpè", "Bio lo jai", "Na som jita", "E mala bwam"}, 2},
    {"Que signifie 'Mwana' ?",
     {"Pere", "Mere", "Enfant", "Frere"}, 2},
    {"Comment dit-on 'Deux' en Duala ?",
     {"Mboa", "Lalo", "Bale", "Bone"}, 2},
    {"Que signifie 'Nyama' ?",
     {"Poisson", "Viande", "Repas", "Legumes"}, 1}
};

/* ================= QUIZ GHOMALA ================= */

Question quiz_ghomala[] = {
    {"Que signifie 'Fokdzu pepun' ?",
     {"Bonsoir", "Merci", "Au revoir", "Bonjour"}, 3},
    {"Comment dit-on 'Mere' en Ghomala ?",
     {"Fo", "Na", "Kwi", "Nda"}, 1},
    {"Que signifie 'A le te ze' ?",
     {"Bonjour", "Au revoir", "Ca va bien", "Merci"}, 2},
    {"Comment dit-on 'Cinq' en Ghomala ?",
     {"Tek", "Tat", "Taan", "Paa"}, 2},
    {"Que signifie 'Nkwi' ?",
     {"Legumes", "Poisson", "Repas", "Viande"}, 3}
};

/* ================= MENUS ================= */

void menuConnexion() {
    printf("\n*=*=* BIENVENUE SUR CAMlex *=*=*\n");
    printf("1. Nouvel utilisateur\n");
    printf("2. Utilisateur existant\n");
    printf("3. Code oublie\n");
    printf("4. Quitter\n");
    printf("Votre choix : ");
}

void menuPrincipal() {
    printf("\n===== MENU PRINCIPAL =====\n");
    printf("1. Lecons\n");
    printf("2. Commencer un quiz\n");
    printf("3. Voir ma progression\n");
    printf("4. Deconnexion\n");
    printf("5. Quitter\n");
    printf("Votre choix : ");
}

/* ================= UTILISATEURS ================= */

void creerUtilisateur(char code[], char langue[]) {
    FILE *f;
    char nom[50];
    char email[100];
    int choixLangue;

    printf("\nEntrez votre nom : ");
    scanf("%49s", nom);

    printf("Entrez votre adresse email : ");
    scanf("%99s", email);

    printf("Choisissez une langue :\n");
    printf("1. Ewondo\n2. Fulde\n3. Duala\n4. Bulu\n5. Gomala\n");
    printf("Votre choix : ");
    scanf("%d", &choixLangue);

    switch(choixLangue) {
        case 1: strcpy(langue, "Ewondo"); break;
        case 2: strcpy(langue, "Fulde");  break;
        case 3: strcpy(langue, "Duala");  break;
        case 4: strcpy(langue, "Bulu");   break;
        case 5: strcpy(langue, "Gomala"); break;
        default: strcpy(langue, "Ewondo"); break;
    }

    sprintf(code, "U%d", rand() % 9000 + 1000);

    FILE *check;
    char ligne[200];
    char codeExistant[20];
    int codeUnique = 0;

    while (!codeUnique) {
        codeUnique = 1;
        check = fopen("users.txt", "r");
        if (check != NULL) {
            while (fgets(ligne, sizeof(ligne), check)) {
                sscanf(ligne, "%[^;]", codeExistant);
                if (strcmp(codeExistant, code) == 0) {
                    codeUnique = 0;
                    sprintf(code, "U%d", rand() % 9000 + 1000);
                    break;
                }
            }
            fclose(check);
        }
    }

    // Format : CODE;NOM;EMAIL;LANGUE
    f = fopen("users.txt", "a");
    if (f != NULL) {
        fprintf(f, "%s;%s;%s;%s\n", code, nom, email, langue);
        fclose(f);
    }

    char filename[40];
    sprintf(filename, "save_%s.txt", code);
    f = fopen(filename, "w");
    if (f != NULL) {
        fprintf(f, "niveau=1\ncoeurs=5\npieces=0\n");
        fclose(f);
    }

    printf("\nCompte cree avec succes !\n");
    printf("Votre code utilisateur : %s\n", code);
    printf("IMPORTANT : Notez bien ce code pour vous reconnecter !\n");
    printf("Langue choisie : %s\n", langue);
}

int connecterUtilisateur(char code[], char langue[]) {
    char codeEntre[20];
    char ligne[200];
    char codeF[20], nomF[50], emailF[100], langueF[20];
    FILE *f;

    printf("\nEntrez votre code utilisateur : ");
    scanf("%19s", codeEntre);

    f = fopen("users.txt", "r");
    if (f == NULL) {
        printf("Aucun utilisateur enregistre.\n");
        return 0;
    }

    while (fgets(ligne, sizeof(ligne), f)) {
        sscanf(ligne, "%[^;];%[^;];%[^;];%s", codeF, nomF, emailF, langueF);
        if (strcmp(codeF, codeEntre) == 0) {
            strcpy(code, codeF);
            strcpy(langue, langueF);
            fclose(f);
            printf("\nBonjour %s ! Connexion reussie.\n", nomF);
            printf("Langue : %s\n", langue);
            return 1;
        }
    }

    fclose(f);
    printf("Code utilisateur introuvable.\n");
    return 0;
}

void codeOublie() {
    char nom[50];
    char email[100];
    char ligne[200];
    char codeF[20], nomF[50], emailF[100], langueF[20];
    char nouveauCode[20];
    char tousLesLignes[100][200];
    int nbLignes = 0;
    int trouve = 0;
    char ancienCode[20];
    FILE *f;

    printf("\n===== CODE OUBLIE =====\n");
    printf("Entrez votre nom : ");
    scanf("%49s", nom);
    printf("Entrez votre email : ");
    scanf("%99s", email);

    f = fopen("users.txt", "r");
    if (f == NULL) {
        printf("Aucun utilisateur enregistre.\n");
        return;
    }

    while (fgets(ligne, sizeof(ligne), f)) {
        strcpy(tousLesLignes[nbLignes], ligne);
        nbLignes++;

        sscanf(ligne, "%[^;];%[^;];%[^;];%s", codeF, nomF, emailF, langueF);

        if (strcmp(nomF, nom) == 0 && strcmp(emailF, email) == 0) {
            trouve = 1;
            strcpy(ancienCode, codeF);
            sprintf(nouveauCode, "U%d", rand() % 9000 + 1000);
            sprintf(tousLesLignes[nbLignes - 1],
                    "%s;%s;%s;%s\n", nouveauCode, nomF, emailF, langueF);
        }
    }
    fclose(f);

    if (!trouve) {
        printf("Aucun compte trouve avec ce nom et cet email.\n");
        return;
    }

    f = fopen("users.txt", "w");
    if (f != NULL) {
        for (int i = 0; i < nbLignes; i++)
            fprintf(f, "%s", tousLesLignes[i]);
        fclose(f);
    }

    char ancienFichier[40], nouveauFichier[40];
    sprintf(ancienFichier, "save_%s.txt", ancienCode);
    sprintf(nouveauFichier, "save_%s.txt", nouveauCode);
    rename(ancienFichier, nouveauFichier);

    printf("\nVotre nouveau code : %s\n", nouveauCode);
    printf("Notez-le bien !\n");
}

/* ================= PROGRESSION ================= */

Progression chargerProgression(char code[]) {
    Progression p;
    FILE *f;
    char filename[40];

    p.niveau = 1;
    p.coeurs = 5;
    p.pieces = 0;

    sprintf(filename, "save_%s.txt", code);
    f = fopen(filename, "r");
    if (f == NULL) return p;

    fscanf(f, "niveau=%d\ncoeurs=%d\npieces=%d",
           &p.niveau, &p.coeurs, &p.pieces);
    fclose(f);
    return p;
}

void sauvegarderProgression(char code[], Progression p) {
    FILE *f;
    char filename[40];

    sprintf(filename, "save_%s.txt", code);
    f = fopen(filename, "w");
    if (f == NULL) {
        printf("Erreur sauvegarde.\n");
        return;
    }
    fprintf(f, "niveau=%d\ncoeurs=%d\npieces=%d\n",
            p.niveau, p.coeurs, p.pieces);
    fclose(f);
}

void afficherProgression(Progression p) {
    printf("\n===== MA PROGRESSION =====\n");
    printf("Niveau actuel : %d / 2\n", p.niveau);
    printf("Coeurs        : ");
    for (int i = 0; i < p.coeurs; i++) printf("<3 ");
    printf("\nPieces        : %d\n", p.pieces);
}

/* ================= LEÇONS ================= */

void afficherLecon(int niveau, int numLecon, char langue[]) {
    Lecon *lecon;

    if (strcmp(langue, "Ewondo") == 0) {
        lecon = (niveau == 1) ? &lecons_ewondo_n1[numLecon - 1]
                              : &lecons_ewondo_n2[numLecon - 1];
    } else if (strcmp(langue, "Fulde") == 0) {
        lecon = &lecons_fulfulde_n1[numLecon - 1];
    } else if (strcmp(langue, "Duala") == 0) {
        lecon = &lecons_duala_n1[numLecon - 1];
    } else if (strcmp(langue, "Gomala") == 0) {
        lecon = &lecons_ghomala_n1[numLecon - 1];
    } else {
        printf("\nLecons pour '%s' en cours de preparation.\n", langue);
        return;
    }

    printf("\n========================================\n");
    printf("  %s - LECON %d : %s\n", langue, numLecon, lecon->titre);
    printf("========================================\n");
    printf("%s\n", lecon->contenu);
    printf("----------------------------------------\n");
    printf("Appuyez sur Entree pour continuer...");
    getchar();
    getchar();
}

int lancerQuiz(int niveau, Progression *p, char code[], char langue[]) {
    Question *quiz;
    int nbBonnes = 0;
    int choix;
    int seuilReussite = 3;

    if (strcmp(langue, "Ewondo") == 0) {
        quiz = (niveau == 1) ? quiz_ewondo_n1 : quiz_ewondo_n2;
    } else if (strcmp(langue, "Fulde") == 0) {
        quiz = quiz_fulfulde;
    } else if (strcmp(langue, "Duala") == 0) {
        quiz = quiz_duala;
    } else if (strcmp(langue, "Gomala") == 0) {
        quiz = quiz_ghomala;
    } else {
        printf("\nQuiz pour '%s' en cours de preparation.\n", langue);
        return 0;
    }

    printf("\n========================================\n");
    printf("   QUIZ %s - NIVEAU %d\n", langue, niveau);
    printf("   Il faut %d bonnes reponses sur 5\n", seuilReussite);
    printf("========================================\n");

    for (int i = 0; i < 5; i++) {
        printf("\nQuestion %d : %s\n", i + 1, quiz[i].question);
        printf("1. %s\n", quiz[i].choix[0]);
        printf("2. %s\n", quiz[i].choix[1]);
        printf("3. %s\n", quiz[i].choix[2]);
        printf("4. %s\n", quiz[i].choix[3]);
        printf("Votre reponse (1-4) : ");
        scanf("%d", &choix);

        if (choix - 1 == quiz[i].bonnereponse) {
            printf("Bonne reponse ! +10 pieces\n");
            nbBonnes++;
            p->pieces += 10;
        } else {
            printf("Mauvaise reponse. La bonne reponse etait : %s\n",
                   quiz[i].choix[quiz[i].bonnereponse]);
            p->coeurs--;
            printf("Il vous reste %d coeur(s).\n", p->coeurs);
        }

        if (p->coeurs <= 0) {
            printf("\nPlus de coeurs ! Revenez plus tard.\n");
            sauvegarderProgression(code, *p);
            return 0;
        }
    }

    printf("\n========================================\n");
    printf("Resultat : %d / 5 bonnes reponses\n", nbBonnes);

    if (nbBonnes >= seuilReussite) {
        printf("BRAVO ! Quiz reussi !\n");
        if (p->niveau < 2) {
            p->niveau++;
            p->pieces += 50;
            printf("Niveau %d debloque ! +50 pieces\n", p->niveau);
        } else {
            printf("Felicitations ! Vous avez termine tous les niveaux !\n");
        }
    } else {
        printf("Echec. Il fallait %d bonnes reponses.\n", seuilReussite);
        printf("Repassez les lecons et reessayez !\n");
    }

    sauvegarderProgression(code, *p);
    return (nbBonnes >= seuilReussite) ? 1 : 0;
}

void leconMenu(char langue[], Progression *p, char code[]) {
    int choix;

    while (1) {
        printf("\n===== %s - Niveau %d =====\n", langue, p->niveau);
        printf("1. Lecon 1 : Salutations\n");
        printf("2. Lecon 2 : La famille\n");
        printf("3. Lecon 3 : Les chiffres\n");
        printf("4. Lecon 4 : La nourriture\n");
        printf("5. Lecon 5 : Le corps\n");
        printf("6. Lancer le quiz\n");
        printf("7. Retour menu principal\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        if (choix >= 1 && choix <= 5) {
            afficherLecon(p->niveau, choix, langue);
        } else if (choix == 6) {
            lancerQuiz(p->niveau, p, code, langue);
        } else if (choix == 7) {
            break;
        } else {
            printf("Choix invalide.\n");
        }
    }
}

/* ================= PROGRAMME PRINCIPAL ================= */

int main() {
    int choixConnexion, choixMenu;
    char codeUtilisateur[20];
    char langueUtilisateur[20];
    int connecte = 0;
    int programmeActif = 1;
    Progression prog;

    srand(time(NULL));

    while (programmeActif) {
        connecte = 0;

        /* ----- MENU CONNEXION ----- */
        while (!connecte) {
            menuConnexion();
            scanf("%d", &choixConnexion);

            if (choixConnexion == 1) {
                creerUtilisateur(codeUtilisateur, langueUtilisateur);
                connecte = 1;
            } else if (choixConnexion == 2) {
                connecte = connecterUtilisateur(codeUtilisateur, langueUtilisateur);
                if (!connecte)
                    printf("Connexion echouee. Reessayez.\n");
            } else if (choixConnexion == 3) {
                codeOublie();
            } else if (choixConnexion == 4) {
                printf("Au revoir !\n");
                return 0;
            } else {
                printf("Choix invalide.\n");
            }
        }

        prog = chargerProgression(codeUtilisateur);

        /* ----- MENU PRINCIPAL ----- */
        do {
            menuPrincipal();
            scanf("%d", &choixMenu);

            if (choixMenu == 1) {
                leconMenu(langueUtilisateur, &prog, codeUtilisateur);
            } else if (choixMenu == 2) {
                lancerQuiz(prog.niveau, &prog, codeUtilisateur, langueUtilisateur);
            } else if (choixMenu == 3) {
                afficherProgression(prog);
            } else if (choixMenu == 4) {
                sauvegarderProgression(codeUtilisateur, prog);
                printf("\nDeconnexion...\n");
                break;
            } else if (choixMenu == 5) {
                sauvegarderProgression(codeUtilisateur, prog);
                printf("\nFin du programme.\n");
                programmeActif = 0;
                break;
            } else {
                printf("Choix invalide.\n");
            }
        } while (choixMenu != 5);
    }

    return 0;
}