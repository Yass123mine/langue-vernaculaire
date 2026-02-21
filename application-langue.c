#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
/*================STRUCTURES==============*/
typedef struct
{
    int niveau;
    int coeurs;
    int pieces;
} progression;
typedef struct
{
    int id;
    char titre[50];
    char contenue[300];
} Lecon ;
typedef struct
{
    int numero;
    Lecon lecons[5];
    int quizRequis;//nombres de bonne reponses pour débloquer le niveau suivant
} Niveau;
/*================ MNUS =============*/
void menuconnexion()
{
    printf("\n***=****=************ BIENVENUE SUR CAMLEX************=****=***\n");
    printf("1.Nouvel utilisateur\n");
    printf("2.Utilisateur existant\n");
    printf("3.Quitter\n");
    printf("votre choix :   ");

}
/*==========================Utilisateurs======================*/
void creerUtilisateur(char code[],char langue[])
{
    FILE *f;
    char nom[50];
    int age , choixlangue;
    printf("\n Entrez votre nom :  ");
    scanf("%49s",nom);
    printf("choisissez une langue :   \n");
    printf("1.ewondo\n2. fulbe\n3. duala\n4. bulu\n5. gomala\n");
    printf("votre choix: ");
    scanf("%d",&choixlangue);
    switch(choixlangue)
    {
    case 1:
        strcpy(langue,"Ewondo");
        break;
    case 2:
        strcpy(langue,"fulde");
        break;
    case 3:
        strcpy(langue,"duala");
        break;
    case 4:
        strcpy(langue,"bulu");
        break;
    case 5:
        strcpy(langue,"gomala");
        break;
    default:
        strcpy(langue,"Ewondo");
        break;

    }
    printf("entrez votre age :  ");
    scanf("%d",&age);
    sprintf(code,"U%d",rand() % 10000);
    f=fopen("users.txt","a");
    if(f!=NULL)
    {
        fprintf(f,"%s;%s;%d;%s\n",code,nom,age,langue);
        fclose(f);
    }
    printf("\nconnexion reussie !\n");
    printf("votre code utilisateur : %s\n",code);
    printf("langue choisie : %s\n", langue);
    return 1;
}
/*====================PROGRESSION=================*/
progression chargerProgression(char code[])
{
    progression p;
    FILE*f;
    char filename[30];
    sprintf(filename,"save_%s.txt",code);
    f=fopen(filename,"r");
    if(f=NULL)
    {
        p.niveau=1;
        p.coeurs=5;
        p.pieces=0;
        return p;
    }
    fscanf(f,"niveau=%d\ncoeurs=%d\npieces=%d",&p.niveau,&p.coeurs,&p.pieces);
    fclose(f);
    return p;
}
void sauvegarderprogression(char code[],progression p)
{
    FILE *f;
    char filename[30];
    sprintf(filename,"save_%s.txt",code);
    f=fopen(filename,"w");
    if(f==NULL)
    {
        printf("Erreur sauvegarde progression.\n");
        return;
    }
    fprintf(f,"niveau=%d\ncoeurs=%d\npieces=%d\n",p.niveau,p.coeurs,p.pieces);
    fclose(f);
}
void afficherprogression(progression p)
{
    printf("\n==============PROGRESSION CAMLEX===============\n");
    printf("niveau :%d\n",p.niveau);
    printf("coeurs :%d\n",p.coeurs);
    printf("pieces :%d\n",p.pieces);
}
/*=============================LECONS=======================*/
void afficherLecon(int niveau,int lecon,char langue[])
{
    printf("\n=====%s - Niveau %d,Lecon%d=====\n",langue,niveau,lecon);
    printf("le contenu de la lecon : a venir.......\n");
}
void afficherQuiz(int niveau,char langue[])
{
    printf("\n===== %s - Niveau %d,Quiz=====\n",langue,niveau);
    printf("Quiz : à venir...\n");
    printf("Reussir le quiz donnera accès au niveau suivant.\n");
}
void leconMenu(char langue[])
{
    int niveau = 1;
    int i;
    int choix;
    while (niveau <= 5)
    {
        printf("\n===== %s - Niveau %d=====\n",langue,niveau);
       for( i=1;i<=5;i++)
        {
            printf("%d.Lecon");
        }
    }
}

