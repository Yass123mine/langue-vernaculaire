#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WIN_W         960
#define WIN_H         640
#define FPS           60.0

#define NB_NIVEAUX    2
#define NB_LECONS     5
#define NB_QUESTIONS  5
#define NB_CHOIX      4
#define MAX_MOTS      8

#define FONT_HUGE     52
#define FONT_LARGE    32
#define FONT_MEDIUM   20
#define FONT_SMALL    15
#define FONT_TINY     12
#define RADIUS_BTN    14
#define RADIUS_CARD   18

#define COL_BG           al_map_rgb(245,248,252)
#define COL_PRIMARY      al_map_rgb(74,144,217)
#define COL_PRIMARY_DK   al_map_rgb(53,118,189)
#define COL_PRIMARY_LT   al_map_rgb(180,214,245)
#define COL_SECONDARY    al_map_rgb(96,125,139)
#define COL_SECONDARY_DK al_map_rgb(69,90,100)
#define COL_WHITE        al_map_rgb(255,255,255)
#define COL_DARK         al_map_rgb(45,52,54)
#define COL_GREEN        al_map_rgb(46,204,113)
#define COL_GREEN_DK     al_map_rgb(39,174,96)
#define COL_RED          al_map_rgb(231,76,60)
#define COL_RED_DK       al_map_rgb(192,57,43)
#define COL_GOLD         al_map_rgb(243,156,18)
#define COL_GOLD_DK      al_map_rgb(211,130,7)
#define COL_CARD         al_map_rgb(255,255,255)
#define COL_CARD_BORDER  al_map_rgb(210,220,235)
#define COL_TEXT_HINT    al_map_rgb(150,160,175)
#define COL_PHOENIX_R    al_map_rgb(231,76,60)
#define COL_PHOENIX_O    al_map_rgb(243,156,18)
#define COL_PHOENIX_Y    al_map_rgb(255,220,50)

typedef struct { char ewondo[64]; char francais[64]; } MotVocab;
typedef struct { char titre[80]; MotVocab mots[MAX_MOTS]; int nbMots; } Lecon;
typedef struct { char question[200]; char choix[NB_CHOIX][100]; int bonneReponse; } Question;
typedef struct { char code[20]; char nom[50]; char langue[20]; int niveau; int coeurs; int pieces; } Joueur;

typedef struct {
    ALLEGRO_FONT *fontHuge, *fontLarge, *fontMedium, *fontSmall, *fontTiny;
} UIContext;

typedef struct {
    float x, y, w, h;
    char  label[100];
    ALLEGRO_COLOR colNormal, colHover, colText;
    float radius;
    int   fontSize, hovered, pressed, disabled;
} Button;

typedef enum {
    STATE_LOGIN, STATE_NEW_USER, STATE_CONNECT_USER, STATE_MAIN_MENU,
    STATE_LESSON_SELECT, STATE_LESSON_VIEW, STATE_QUIZ, STATE_QUIZ_RESULT,
    STATE_PROGRESS, STATE_QUIT
} AppState;

typedef struct { float x,y,w,h; char buffer[80]; int cursor,active; char placeholder[80]; } TextField;
typedef struct { int questionIdx, nbBonnes, selectedChoice, answered; double answerTime; } QuizCtx;
typedef struct { int leconIdx, motIdx, flipped; } LessonCtx;

typedef struct {
    ALLEGRO_DISPLAY    *display;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_TIMER       *timer;
    UIContext           *ui;
    AppState   state;
    Joueur     joueur;
    int        estConnecte;
    double     animTick, timeNow;
    QuizCtx    quiz;
    LessonCtx  lesson;
    TextField  tfNom, tfCode;
    int        choixLangue;
    char          feedbackMsg[200];
    ALLEGRO_COLOR feedbackCol;
    double        feedbackEnd;
    Button btns[24];
    int    nbBtns;
    float  mouseX, mouseY;
    int    mouseClicked, keyDown, keyChar, needDraw, running;
} GameCtx;

static Lecon    lecons[NB_NIVEAUX][NB_LECONS];
static Question quiz[NB_NIVEAUX][NB_QUESTIONS];
static const char *LANGUES[] = {"Ewondo","Fulde","Duala","Bulu","Gomala"};
#define NB_LANGUES 5

/* ---------- PROTOTYPES ---------- */
static void gameSetState(GameCtx *ctx, AppState next);
static void updateAndDrawBtns(GameCtx *ctx);
static int  btnClicked(GameCtx *ctx, int idx);
static void drawTopBar(GameCtx *ctx);
static void drawFeedback(GameCtx *ctx);
static void setFeedback(GameCtx *ctx, const char *msg, ALLEGRO_COLOR col);

/* ================================================================
   DATA
   ================================================================ */

static void initLeconN(int niv, int idx, const char *titre,
                       const char mots[][2][64], int nb) {
    strncpy(lecons[niv][idx].titre, titre, 79);
    lecons[niv][idx].nbMots = nb;
    for (int i = 0; i < nb; i++) {
        strncpy(lecons[niv][idx].mots[i].ewondo,  mots[i][0], 63);
        strncpy(lecons[niv][idx].mots[i].francais, mots[i][1], 63);
    }
}

static void initDonnees(void) {
    { const char m[][2][64]={{"Bembekiri","Bonjour (formel)"},{"Mbolo","Bonjour (familier)"},
      {"Mbolo minkie","Bonsoir"},{"Ntem","Au revoir"},{"A ye ?","Comment ca va ?"},
      {"Ma ye","Ca va bien"},{"Akiba","Merci"}}; initLeconN(0,0,"Salutations",m,7); }
    { const char m[][2][64]={{"Tara","Pere"},{"Nna","Mere"},{"Ngon","Enfant / Fils"},
      {"Ndom","Frere"},{"Mebele","Soeur"},{"Nkukuma","Grand-pere"},
      {"Ya","Grand-mere"},{"Nda","Maison / Famille"}}; initLeconN(0,1,"La famille",m,8); }
    { const char m[][2][64]={{"Abe","Un"},{"Bele","Deux"},{"Lala","Trois"},
      {"Bini","Quatre"},{"Tan","Cinq"},{"Sambe","Six"},
      {"Zambe","Sept"}}; initLeconN(0,2,"Les chiffres",m,7); }
    { const char m[][2][64]={{"Okok","Feuilles de manioc"},{"Kpem","Manioc"},
      {"Mbanga","Noix de palme"},{"Nnam","Nourriture"},{"Mame","Poisson"},
      {"Soa","Viande"},{"Mame ndzom","Viande d'elephant"}}; initLeconN(0,3,"La nourriture",m,7); }
    { const char m[][2][64]={{"Owu","Tete"},{"Mesep","Yeux"},{"Ening","Nez"},
      {"Owomo","Bouche"},{"Mebaa","Mains"},{"Mekon","Pieds"},
      {"Nkol","Coeur"}}; initLeconN(0,4,"Le corps",m,7); }
    { const char m[][2][64]={{"Dze wa ?","Comment tu t'appelles ?"},{"Ma dze...","Je m'appelle..."},
      {"Fia wa ?","D'ou viens-tu ?"},{"Ma fi Yaounde","Je viens de Yaounde"},
      {"Abe wa ?","Quel age as-tu ?"}}; initLeconN(1,0,"Se presenter",m,5); }
    { const char m[][2][64]={{"Owondo","Noir"},{"Oton","Blanc"},{"Ngon owondo","Rouge"},
      {"Bikoe","Vert"},{"Osol","Bleu"},{"Ovi","Jaune"}}; initLeconN(1,1,"Les couleurs",m,6); }
    { const char m[][2][64]={{"Nnam","Poulet"},{"Ngoe","Singe"},{"Ngon mbu","Chien"},
      {"Koe","Chevre"},{"Ndzom","Elephant"},{"Mvog","Oiseau"}}; initLeconN(1,2,"Les animaux",m,6); }
    { const char m[][2][64]={{"Abaa","Porte"},{"Fene","Fenetre"},{"Nda minko","Cuisine"},
      {"Bele nda","Chambre"},{"Nda nnam","Salle a manger"}}; initLeconN(1,3,"La maison",m,5); }
    { const char m[][2][64]={{"Bikoe","Matin"},{"Awoeng","Midi"},{"Minkie","Soir"},
      {"Fong","Nuit"},{"Biko","Aujourd'hui"},{"Biko si","Demain"},
      {"Biko zam","Hier"}}; initLeconN(1,4,"Le temps",m,7); }

    #define SQ(n,i,q,a,b,c,d,r) strncpy(quiz[n][i].question,q,199); \
        strncpy(quiz[n][i].choix[0],a,99); strncpy(quiz[n][i].choix[1],b,99); \
        strncpy(quiz[n][i].choix[2],c,99); strncpy(quiz[n][i].choix[3],d,99); \
        quiz[n][i].bonneReponse=r;

    SQ(0,0,"Que signifie 'Bembekiri' ?","Au revoir","Bonjour","Merci","Comment ca va ?",1)
    SQ(0,1,"Comment dit-on 'Mere' en Ewondo ?","Tara","Ngon","Nna","Ya",2)
    SQ(0,2,"Que signifie 'Ma ye' ?","Ca va bien","Bonsoir","Au revoir","Je m'appelle",0)
    SQ(0,3,"Comment dit-on 'Deux' en Ewondo ?","Abe","Lala","Bele","Bini",2)
    SQ(0,4,"Que signifie 'Mame' ?","Viande","Poisson","Nourriture","Manioc",1)
    SQ(1,0,"Comment demande-t-on 'Comment tu t'appelles ?' ?","Fia wa ?","Dze wa ?","A ye ?","Abe wa ?",1)
    SQ(1,1,"Que signifie 'Owondo' ?","Blanc","Rouge","Vert","Noir",3)
    SQ(1,2,"Comment dit-on 'Maison' en Ewondo ?","Abaa","Fene","Nda","Bele nda",2)
    SQ(1,3,"Que signifie 'Minkie' ?","Matin","Midi","Soir","Nuit",2)
    SQ(1,4,"Comment dit-on 'Elephant' en Ewondo ?","Ngoe","Ndzom","Koe","Nnam",1)
    #undef SQ
}

static void genererCode(char out[20]) {
    FILE *f; char ligne[200], codeExist[20]; int unique=0;
    while (!unique) {
        sprintf(out,"U%04d",rand()%9000+1000); unique=1;
        f=fopen("users.txt","r"); if(!f) break;
        while(fgets(ligne,sizeof(ligne),f)){
            sscanf(ligne,"%19[^;]",codeExist);
            if(strcmp(codeExist,out)==0){unique=0;break;}
        } fclose(f);
    }
}

static int creerJoueur(const char *nom, const char *langue, Joueur *out) {
    FILE *f; char savefile[50];
    srand((unsigned)time(NULL));
    genererCode(out->code);
    strncpy(out->nom,nom,49); strncpy(out->langue,langue,19);
    out->niveau=1; out->coeurs=5; out->pieces=0;
    f=fopen("users.txt","a"); if(!f) return 0;
    fprintf(f,"%s;%s;%s\n",out->code,out->nom,out->langue); fclose(f);
    sprintf(savefile,"save_%s.txt",out->code);
    f=fopen(savefile,"w"); if(!f) return 0;
    fprintf(f,"niveau=%d\ncoeurs=%d\npieces=%d\n",out->niveau,out->coeurs,out->pieces);
    fclose(f); return 1;
}

static int connecterJoueur(const char *code, Joueur *out) {
    FILE *f; char ligne[200],codeF[20],nomF[50],langueF[20];
    f=fopen("users.txt","r"); if(!f) return 0;
    while(fgets(ligne,sizeof(ligne),f)){
        if(sscanf(ligne,"%19[^;];%49[^;];%19s",codeF,nomF,langueF)==3){
            if(strcmp(codeF,code)==0){
                strncpy(out->code,codeF,19); strncpy(out->nom,nomF,49);
                strncpy(out->langue,langueF,19); fclose(f);
                char savefile[50]; sprintf(savefile,"save_%s.txt",code);
                FILE *sf=fopen(savefile,"r");
                out->niveau=1; out->coeurs=5; out->pieces=0;
                if(sf){fscanf(sf,"niveau=%d\ncoeurs=%d\npieces=%d",&out->niveau,&out->coeurs,&out->pieces);fclose(sf);}
                return 1;
            }
        }
    } fclose(f); return 0;
}

static int sauvegarderJoueur(const Joueur *j) {
    char savefile[50]; sprintf(savefile,"save_%s.txt",j->code);
    FILE *f=fopen(savefile,"w"); if(!f) return 0;
    fprintf(f,"niveau=%d\ncoeurs=%d\npieces=%d\n",j->niveau,j->coeurs,j->pieces);
    fclose(f); return 1;
}

/* ================================================================
   UI — PRIMITIVES
   ================================================================ */

static UIContext* uiInit(const char *fontPath) {
    UIContext *ui=(UIContext*)calloc(1,sizeof(UIContext)); if(!ui) return NULL;
    ui->fontHuge   = al_load_ttf_font(fontPath,FONT_HUGE,0);
    ui->fontLarge  = al_load_ttf_font(fontPath,FONT_LARGE,0);
    ui->fontMedium = al_load_ttf_font(fontPath,FONT_MEDIUM,0);
    ui->fontSmall  = al_load_ttf_font(fontPath,FONT_SMALL,0);
    ui->fontTiny   = al_load_ttf_font(fontPath,FONT_TINY,0);
    if (!ui->fontHuge) {
        const char *fb[]={"C:/Windows/Fonts/arial.ttf","C:/Windows/Fonts/calibri.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",NULL};
        for(int i=0;fb[i]&&!ui->fontHuge;i++){
            ui->fontHuge  =al_load_ttf_font(fb[i],FONT_HUGE,0);
            ui->fontLarge =al_load_ttf_font(fb[i],FONT_LARGE,0);
            ui->fontMedium=al_load_ttf_font(fb[i],FONT_MEDIUM,0);
            ui->fontSmall =al_load_ttf_font(fb[i],FONT_SMALL,0);
            ui->fontTiny  =al_load_ttf_font(fb[i],FONT_TINY,0);
        }
    }
    if (!ui->fontHuge) {
        ALLEGRO_FONT *b=al_create_builtin_font();
        ui->fontHuge=ui->fontLarge=ui->fontMedium=ui->fontSmall=ui->fontTiny=b;
    }
    return ui;
}

static void uiDestroy(UIContext *ui) {
    if(!ui) return;
    if(ui->fontHuge   && ui->fontHuge  !=ui->fontMedium) al_destroy_font(ui->fontHuge);
    if(ui->fontLarge  && ui->fontLarge !=ui->fontMedium) al_destroy_font(ui->fontLarge);
    if(ui->fontSmall  && ui->fontSmall !=ui->fontMedium) al_destroy_font(ui->fontSmall);
    if(ui->fontTiny   && ui->fontTiny  !=ui->fontMedium) al_destroy_font(ui->fontTiny);
    if(ui->fontMedium) al_destroy_font(ui->fontMedium);
    free(ui);
}

static ALLEGRO_FONT* uiFont(UIContext *ui, int fs) {
    if(fs>=FONT_HUGE)   return ui->fontHuge;
    if(fs>=FONT_LARGE)  return ui->fontLarge;
    if(fs>=FONT_MEDIUM) return ui->fontMedium;
    if(fs>=FONT_SMALL)  return ui->fontSmall;
    return ui->fontTiny;
}

static void uiDrawBackground(void) {
    for(int y=0;y<WIN_H;y++){
        float t=(float)y/WIN_H;
        al_draw_line(0,(float)y,WIN_W,(float)y,
            al_map_rgb((int)(245-t*22),(int)(248-t*18),(int)(252-t*12)),1.f);
    }
    al_draw_filled_circle(-50.f,WIN_H+50.f,200.f,al_map_rgba(74,144,217,18));
    al_draw_filled_circle(WIN_W+50.f,-50.f,180.f,al_map_rgba(74,144,217,18));
    al_draw_filled_circle(WIN_W-80.f,WIN_H-80.f,120.f,al_map_rgba(74,144,217,15));
}

static void uiRoundRect(float x,float y,float w,float h,float r,ALLEGRO_COLOR fill) {
    al_draw_filled_rounded_rectangle(x,y,x+w,y+h,r,r,fill);
}

static void uiRoundBorder(float x,float y,float w,float h,float r,
                          ALLEGRO_COLOR fill,ALLEGRO_COLOR border,float bw) {
    al_draw_filled_rounded_rectangle(x,y,x+w,y+h,r,r,fill);
    al_draw_rounded_rectangle(x,y,x+w,y+h,r,r,border,bw);
}

static void uiShadow(float x,float y,float w,float h,float r) {
    for(int i=5;i>=1;i--)
        al_draw_filled_rounded_rectangle(x+i,y+i,x+w+i,y+h+i,r,r,
            al_map_rgba(60,80,120,(int)(8.f*i)));
}

static void uiText(UIContext *ui,const char *txt,int fs,ALLEGRO_COLOR col,float x,float y,int flags) {
    ALLEGRO_FONT *f=uiFont(ui,fs);
    if(f&&txt) al_draw_text(f,col,x,y,flags,txt);
}

static void uiTextCentered(UIContext *ui,const char *txt,int fs,ALLEGRO_COLOR col,float cx,float y) {
    ALLEGRO_FONT *f=uiFont(ui,fs);
    if(f&&txt) al_draw_text(f,col,cx,y,ALLEGRO_ALIGN_CENTRE,txt);
}

static void uiTextInRect(UIContext *ui,const char *txt,int fs,ALLEGRO_COLOR col,
                         float rx,float ry,float rw,float rh) {
    ALLEGRO_FONT *f=uiFont(ui,fs); if(!f||!txt) return;
    int tw=al_get_text_width(f,txt),th=al_get_font_line_height(f);
    al_draw_text(f,col,rx+(rw-tw)*0.5f,ry+(rh-th)*0.5f,0,txt);
}

static Button btnMake(float x,float y,float w,float h,const char *label,
                      ALLEGRO_COLOR cn,ALLEGRO_COLOR ch,ALLEGRO_COLOR ct,float radius,int fs) {
    Button b; memset(&b,0,sizeof(b));
    b.x=x;b.y=y;b.w=w;b.h=h; strncpy(b.label,label,99);
    b.colNormal=cn;b.colHover=ch;b.colText=ct;b.radius=radius;b.fontSize=fs;
    return b;
}

static void btnDraw(UIContext *ui,const Button *b) {
    float x=b->x,y=b->y,w=b->w,h=b->h;
    if(b->disabled){
        uiRoundRect(x,y,w,h,b->radius,al_map_rgb(190,190,198));
        uiTextInRect(ui,b->label,b->fontSize,al_map_rgb(220,220,225),x,y,w,h);
        return;
    }
    if(!b->pressed) uiShadow(x,y,w,h,b->radius);
    float ox=b->pressed?1.f:0.f, oy=b->pressed?3.f:0.f;
    ALLEGRO_COLOR col=b->hovered?b->colHover:b->colNormal;
    uiRoundRect(x+ox,y+oy,w,h,b->radius,col);
    if(!b->pressed){
        float r,g,bl,a; al_unmap_rgba_f(col,&r,&g,&bl,&a);
        ALLEGRO_COLOR shine=al_map_rgba_f(fminf(1.f,r+0.18f),fminf(1.f,g+0.18f),fminf(1.f,bl+0.18f),0.55f);
        al_draw_filled_rounded_rectangle(x+ox+b->radius,y+oy,x+ox+w-b->radius,y+oy+4.f,0,0,shine);
    }
    uiTextInRect(ui,b->label,b->fontSize,b->colText,x+ox,y+oy,w,h);
}

static int btnHit(const Button *b,float mx,float my) {
    return mx>=b->x&&mx<=b->x+b->w&&my>=b->y&&my<=b->y+b->h;
}

static void btnUpdate(Button *b,float mx,float my,int clicked) {
    b->hovered=btnHit(b,mx,my); b->pressed=b->hovered&&clicked;
}

static void uiDrawCard(float x,float y,float w,float h,float r,ALLEGRO_COLOR bg) {
    uiShadow(x,y,w,h,r);
    uiRoundBorder(x,y,w,h,r,bg,COL_CARD_BORDER,1.5f);
}

static void uiDrawHeart(float cx,float cy,float size,ALLEGRO_COLOR col) {
    float r=size*0.30f;
    al_draw_filled_circle(cx-r*0.9f,cy-r*0.4f,r,col);
    al_draw_filled_circle(cx+r*0.9f,cy-r*0.4f,r,col);
    al_draw_filled_triangle(cx-size*0.58f,cy-r*0.2f,cx+size*0.58f,cy-r*0.2f,cx,cy+size*0.55f,col);
}

static void uiDrawCoin(float cx,float cy,float size) {
    al_draw_filled_circle(cx,cy,size,COL_GOLD);
    al_draw_filled_circle(cx,cy,size*0.72f,COL_GOLD_DK);
    al_draw_filled_circle(cx-size*0.2f,cy-size*0.2f,size*0.32f,al_map_rgba(255,240,150,130));
}

static void uiDrawStar(float cx,float cy,float size,ALLEGRO_COLOR col) {
    ALLEGRO_VERTEX v[10];
    for(int i=0;i<10;i++){
        double ang=M_PI*i/5.0-M_PI/2.0;
        float r2=(i%2==0)?size:size*0.42f;
        v[i].x=cx+(float)(r2*cos(ang)); v[i].y=cy+(float)(r2*sin(ang));
        v[i].z=0; v[i].color=col; v[i].u=v[i].v=0;
    }
    for(int i=0;i<5;i++){
        al_draw_filled_triangle(cx,cy,v[i*2].x,v[i*2].y,v[(i*2+2)%10].x,v[(i*2+2)%10].y,col);
        al_draw_filled_triangle(cx,cy,v[i*2].x,v[i*2].y,v[i*2+1].x,v[i*2+1].y,col);
    }
}

static void uiProgressBar(float x,float y,float w,float h,float pct,
                          ALLEGRO_COLOR bg,ALLEGRO_COLOR fill,float radius) {
    uiRoundRect(x,y,w,h,radius,bg);
    if(pct>0.f){float fw=w*pct; if(fw<radius*2)fw=radius*2; uiRoundRect(x,y,fw,h,radius,fill);}
}

static void drawPhoenix(float cx,float cy,float size,float anim) {
    float sinA=sinf((float)(anim*2.0*M_PI));
    float cosA=cosf((float)(anim*2.0*M_PI));
    float bob=sinA*size*0.05f, wingY=sinA*size*0.10f;
    cy+=bob;
    ALLEGRO_COLOR cR=COL_PHOENIX_R, cO=COL_PHOENIX_O, cY=COL_PHOENIX_Y;
    ALLEGRO_COLOR cDark=al_map_rgb(60,18,8), cWhite=al_map_rgb(255,255,255), cEye=al_map_rgb(20,20,20);

    for(int i=-3;i<=3;i++){
        float ang=(float)M_PI*0.5f+i*0.20f;
        float px=cx+cosf(ang)*size*0.80f, py=cy+sinf(ang)*size*0.82f;
        ALLEGRO_COLOR qc=(abs(i)<=1)?cY:(abs(i)==2?cO:cR);
        al_draw_line(cx,cy+size*0.18f,px,py,qc,size*0.08f);
        al_draw_line(cx,cy+size*0.18f,px,py,al_map_rgba(255,255,255,40),size*0.03f);
        al_draw_filled_circle(px,py,size*0.07f,qc);
    }

    for(int layer=0;layer<3;layer++){
        float spread=0.52f+layer*0.16f;
        float wrx=size*0.48f+layer*size*0.08f, wry=size*0.26f;
        ALLEGRO_COLOR wc=(layer==0)?cR:(layer==1?cO:cY);
        int SEGS=10;

        float wx=cx-size*spread, wy=cy-size*0.22f+wingY-layer*size*0.08f;
        for(int s=0;s<SEGS;s++){
            float a1=(float)M_PI*0.4f+s*(float)M_PI*0.6f/SEGS;
            float a2=a1+(float)M_PI*0.6f/SEGS;
            al_draw_filled_triangle(wx,wy,wx+cosf(a1)*wrx,wy+sinf(a1)*wry,wx+cosf(a2)*wrx,wy+sinf(a2)*wry,wc);
        }
        for(int f=0;f<4;f++){
            float fa=(float)M_PI*0.5f+f*(float)M_PI*0.5f/3.f;
            al_draw_filled_circle(wx+cosf(fa)*wrx*0.85f,wy+sinf(fa)*wry+wingY*0.4f,size*0.065f,cY);
        }

        wx=cx+size*spread; wy=cy-size*0.22f+wingY-layer*size*0.08f;
        for(int s=0;s<SEGS;s++){
            float a1=-(float)M_PI*0.4f-s*(float)M_PI*0.6f/SEGS;
            float a2=a1-(float)M_PI*0.6f/SEGS;
            al_draw_filled_triangle(wx,wy,wx+cosf(a1)*wrx,wy+sinf(a1)*wry,wx+cosf(a2)*wrx,wy+sinf(a2)*wry,wc);
        }
        for(int f=0;f<4;f++){
            float fa=-(float)M_PI*0.5f-f*(float)M_PI*0.5f/3.f;
            al_draw_filled_circle(wx+cosf(fa)*wrx*0.85f,wy+sinf(fa)*wry+wingY*0.4f,size*0.065f,cY);
        }
    }

    al_draw_filled_ellipse(cx,cy,size*0.38f,size*0.46f,cR);
    al_draw_filled_ellipse(cx,cy,size*0.28f,size*0.36f,cO);
    for(int row=0;row<3;row++)
        for(int col=-1;col<=1;col++){
            float px2=cx+col*size*0.10f-(row%2)*size*0.05f, py2=cy-size*0.12f+row*size*0.10f;
            al_draw_filled_circle(px2,py2,size*0.075f,(row==0)?cY:(row==1?cO:cR));
        }

    float hx=cx, hy=cy-size*0.56f;
    al_draw_filled_circle(hx,hy,size*0.24f,cR);
    al_draw_filled_circle(hx,hy,size*0.17f,cO);
    for(int i=-2;i<=2;i++){
        float ca=(float)M_PI+i*0.22f;
        float cpx=hx+cosf(ca)*size*0.20f, cpy=hy+sinf(ca)*size*0.30f;
        ALLEGRO_COLOR cc=(i==0)?cY:(abs(i)==1?cO:cR);
        al_draw_filled_circle(cpx,cpy,size*0.065f,cc);
        al_draw_line(hx,hy-size*0.08f,cpx,cpy,cc,size*0.055f);
    }
    al_draw_filled_triangle(hx+size*0.10f,hy-size*0.04f,hx+size*0.32f,hy,hx+size*0.10f,hy+size*0.04f,cO);
    al_draw_filled_triangle(hx+size*0.10f,hy-size*0.015f,hx+size*0.30f,hy,hx+size*0.10f,hy+size*0.015f,cDark);
    al_draw_filled_circle(hx+size*0.12f,hy-size*0.08f,size*0.09f,cWhite);
    al_draw_filled_circle(hx+size*0.14f,hy-size*0.08f,size*0.055f,cEye);
    al_draw_filled_circle(hx+size*0.15f,hy-size*0.085f,size*0.022f,al_map_rgba(255,255,255,200));

    float glowAlpha=12.f+10.f*(0.5f+0.5f*sinA);
    for(int g=4;g>=1;g--)
        al_draw_filled_circle(cx,cy,size*0.40f+g*size*0.12f,al_map_rgba(255,140,40,(int)(glowAlpha*g*0.5f)));

    al_draw_line(cx-size*0.12f,cy+size*0.44f,cx-size*0.10f,cy+size*0.62f,cO,size*0.07f);
    al_draw_line(cx+size*0.12f,cy+size*0.44f,cx+size*0.10f,cy+size*0.62f,cO,size*0.07f);
    for(int t=-1;t<=1;t++){
        al_draw_line(cx-size*0.10f,cy+size*0.62f,cx-size*0.10f+t*size*0.08f,cy+size*0.70f,cDark,size*0.04f);
        al_draw_line(cx+size*0.10f,cy+size*0.62f,cx+size*0.10f+t*size*0.08f,cy+size*0.70f,cDark,size*0.04f);
    }
    (void)cosA;
}

/* ================================================================
   GAME — LOGIQUE
   ================================================================ */

static GameCtx* gameCreate(ALLEGRO_DISPLAY *disp, ALLEGRO_EVENT_QUEUE *q,
                            ALLEGRO_TIMER *timer, const char *fontPath) {
    GameCtx *ctx=(GameCtx*)calloc(1,sizeof(GameCtx)); if(!ctx) return NULL;
    ctx->display=disp; ctx->queue=q; ctx->timer=timer;
    ctx->ui=uiInit(fontPath); ctx->state=STATE_LOGIN;
    ctx->running=1; ctx->needDraw=1; ctx->choixLangue=0;
    ctx->quiz.selectedChoice=-1;
    ctx->tfNom.x=WIN_W/2.f-200; ctx->tfNom.y=258; ctx->tfNom.w=400; ctx->tfNom.h=52;
    strncpy(ctx->tfNom.placeholder,"Entrez votre prenom...",79);
    ctx->tfCode.x=WIN_W/2.f-200; ctx->tfCode.y=368; ctx->tfCode.w=400; ctx->tfCode.h=52;
    strncpy(ctx->tfCode.placeholder,"Votre code (ex: U1234)...",79);
    return ctx;
}

static void gameDestroy(GameCtx *ctx) {
    if(!ctx) return;
    if(ctx->estConnecte) sauvegarderJoueur(&ctx->joueur);
    uiDestroy(ctx->ui); free(ctx);
}

static void gameSetState(GameCtx *ctx, AppState next) {
    if(ctx->estConnecte) sauvegarderJoueur(&ctx->joueur);
    ctx->state=next; ctx->nbBtns=0; ctx->mouseClicked=0;
    if(next==STATE_QUIZ){ctx->quiz.questionIdx=0;ctx->quiz.nbBonnes=0;ctx->quiz.selectedChoice=-1;ctx->quiz.answered=0;}
    if(next==STATE_LESSON_VIEW){ctx->lesson.motIdx=0;ctx->lesson.flipped=0;}
    if(next==STATE_NEW_USER){memset(ctx->tfNom.buffer,0,80);ctx->tfNom.cursor=0;ctx->tfNom.active=1;ctx->choixLangue=0;}
    if(next==STATE_CONNECT_USER){memset(ctx->tfCode.buffer,0,80);ctx->tfCode.cursor=0;ctx->tfCode.active=1;}
    ctx->needDraw=1;
}

static void gameHandleEvent(GameCtx *ctx, ALLEGRO_EVENT *ev) {
    ctx->mouseClicked=0; ctx->keyChar=0;
    switch(ev->type){
    case ALLEGRO_EVENT_DISPLAY_CLOSE: ctx->running=0; break;
    case ALLEGRO_EVENT_TIMER: ctx->needDraw=1; break;
    case ALLEGRO_EVENT_MOUSE_AXES:
        ctx->mouseX=(float)ev->mouse.x; ctx->mouseY=(float)ev->mouse.y; ctx->needDraw=1; break;
    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        if(ev->mouse.button==1){
            ctx->mouseX=(float)ev->mouse.x; ctx->mouseY=(float)ev->mouse.y;
            ctx->mouseClicked=1; ctx->needDraw=1;
        } break;
    case ALLEGRO_EVENT_KEY_DOWN:
        ctx->keyDown=ev->keyboard.keycode; ctx->needDraw=1;
        if(ev->keyboard.keycode==ALLEGRO_KEY_BACKSPACE){
            if(ctx->tfNom.active&&ctx->tfNom.cursor>0)   ctx->tfNom.buffer[--ctx->tfNom.cursor]='\0';
            if(ctx->tfCode.active&&ctx->tfCode.cursor>0) ctx->tfCode.buffer[--ctx->tfCode.cursor]='\0';
        }
        if(ev->keyboard.keycode==ALLEGRO_KEY_ENTER) ctx->keyChar='\n';
        break;
    case ALLEGRO_EVENT_KEY_CHAR:
        ctx->keyChar=ev->keyboard.unichar; ctx->needDraw=1;
        if(ev->keyboard.unichar>=32&&ev->keyboard.unichar<127){
            char c[2]={(char)ev->keyboard.unichar,'\0'};
            if(ctx->tfNom.active&&ctx->tfNom.cursor<49){strcat(ctx->tfNom.buffer,c);ctx->tfNom.cursor++;}
            if(ctx->tfCode.active&&ctx->tfCode.cursor<19){strcat(ctx->tfCode.buffer,c);ctx->tfCode.cursor++;}
        } break;
    default: break;
    }
}

static void gameUpdate(GameCtx *ctx, double dt) {
    ctx->animTick+=dt*0.45; if(ctx->animTick>1.0) ctx->animTick-=1.0;
    ctx->timeNow=al_get_time();
    if(ctx->state==STATE_QUIZ&&ctx->quiz.answered){
        if(ctx->timeNow>=ctx->quiz.answerTime){
            ctx->quiz.answered=0; ctx->quiz.selectedChoice=-1; ctx->quiz.questionIdx++;
            if(ctx->quiz.questionIdx>=NB_QUESTIONS) gameSetState(ctx,STATE_QUIZ_RESULT);
        }
    }
}

static void setFeedback(GameCtx *ctx,const char *msg,ALLEGRO_COLOR col){
    strncpy(ctx->feedbackMsg,msg,199); ctx->feedbackCol=col; ctx->feedbackEnd=al_get_time()+2.8;
}

static Button* addBtn(GameCtx *ctx,float x,float y,float w,float h,const char *label,
                      ALLEGRO_COLOR cn,ALLEGRO_COLOR ch,ALLEGRO_COLOR ct,float radius,int fs){
    if(ctx->nbBtns>=24) return NULL;
    ctx->btns[ctx->nbBtns]=btnMake(x,y,w,h,label,cn,ch,ct,radius,fs);
    return &ctx->btns[ctx->nbBtns++];
}

static int btnClicked(GameCtx *ctx,int idx){
    if(idx<0||idx>=ctx->nbBtns) return 0;
    return ctx->btns[idx].hovered&&ctx->mouseClicked&&!ctx->btns[idx].disabled;
}

static void updateAndDrawBtns(GameCtx *ctx){
    for(int i=0;i<ctx->nbBtns;i++){
        btnUpdate(&ctx->btns[i],ctx->mouseX,ctx->mouseY,ctx->mouseClicked);
        btnDraw(ctx->ui,&ctx->btns[i]);
    }
}

static void drawTopBar(GameCtx *ctx){
    UIContext *ui=ctx->ui;
    uiRoundRect(0,0,WIN_W,62,0,COL_PRIMARY);
    uiText(ui,"CAMlex",FONT_LARGE,COL_WHITE,20,12,0);
    char lvl[20]; sprintf(lvl,"Niv. %d",ctx->joueur.niveau);
    uiText(ui,lvl,FONT_SMALL,al_map_rgba(255,255,255,180),210,22,0);
    if(ctx->estConnecte){
        for(int i=0;i<5;i++)
            uiDrawHeart(WIN_W-270.f+i*32,31,13,(i<ctx->joueur.coeurs)?al_map_rgb(255,255,255):al_map_rgba(255,255,255,55));
        uiDrawCoin(WIN_W-105.f,31,13);
        char coins[16]; sprintf(coins,"%d",ctx->joueur.pieces);
        uiText(ui,coins,FONT_SMALL,COL_WHITE,WIN_W-88,22,0);
        uiText(ui,ctx->joueur.nom,FONT_SMALL,al_map_rgba(255,255,255,210),WIN_W-235,22,0);
    }
}

static void drawFeedback(GameCtx *ctx){
    if(ctx->feedbackMsg[0]=='\0') return;
    if(ctx->timeNow>ctx->feedbackEnd){ctx->feedbackMsg[0]='\0';return;}
    float alpha=1.0f, rem=(float)(ctx->feedbackEnd-ctx->timeNow);
    if(rem<0.5f) alpha=rem/0.5f;
    float r,g,b,a; al_unmap_rgba_f(ctx->feedbackCol,&r,&g,&b,&a);
    uiRoundRect(WIN_W/2.f-260,WIN_H-78,520,50,12,al_map_rgba_f(r,g,b,alpha*0.88f));
    uiTextCentered(ctx->ui,ctx->feedbackMsg,FONT_SMALL,al_map_rgba_f(1,1,1,alpha),WIN_W/2.f,WIN_H-62);
}

static void tfDraw(UIContext *ui,const TextField *tf){
    uiShadow(tf->x,tf->y,tf->w,tf->h,10);
    uiRoundBorder(tf->x,tf->y,tf->w,tf->h,10,COL_WHITE,tf->active?COL_PRIMARY:COL_CARD_BORDER,2.f);
    const char *txt=(tf->buffer[0]!='\0')?tf->buffer:tf->placeholder;
    uiText(ui,txt,FONT_MEDIUM,(tf->buffer[0]!='\0')?COL_DARK:COL_TEXT_HINT,tf->x+14,tf->y+13,0);
    if(tf->active&&(int)(al_get_time()*2)%2==0){
        ALLEGRO_FONT *f=uiFont(ui,FONT_MEDIUM);
        int cw=f?al_get_text_width(f,tf->buffer):0;
        al_draw_line(tf->x+14+cw,tf->y+10,tf->x+14+cw,tf->y+tf->h-10,COL_PRIMARY,2.f);
    }
}

static int tfHit(const TextField *tf,float mx,float my){
    return mx>=tf->x&&mx<=tf->x+tf->w&&my>=tf->y&&my<=tf->y+tf->h;
}

/* ================================================================
   ÉCRANS
   ================================================================ */

static void drawLogin(GameCtx *ctx){
    ctx->nbBtns=0;
    uiDrawBackground();
    drawPhoenix(WIN_W/2.f,WIN_H/2.f-50,85,(float)ctx->animTick);
    uiTextCentered(ctx->ui,"CAMlex",FONT_HUGE,COL_PRIMARY,WIN_W/2.f,70);
    uiTextCentered(ctx->ui,"Apprenez les langues camerounaises",FONT_SMALL,COL_SECONDARY,WIN_W/2.f,142);
    uiTextCentered(ctx->ui,"Langue.  Culture.  Fierte.",FONT_TINY,COL_TEXT_HINT,WIN_W/2.f,166);
    addBtn(ctx,WIN_W/2.f-180,WIN_H-210,360,62,"Nouveau joueur",COL_PRIMARY,COL_PRIMARY_DK,COL_WHITE,RADIUS_BTN,FONT_MEDIUM);
    addBtn(ctx,WIN_W/2.f-180,WIN_H-132,360,62,"J'ai deja un compte",COL_SECONDARY,COL_SECONDARY_DK,COL_WHITE,RADIUS_BTN,FONT_MEDIUM);
    updateAndDrawBtns(ctx);
    if(btnClicked(ctx,0)) gameSetState(ctx,STATE_NEW_USER);
    if(btnClicked(ctx,1)) gameSetState(ctx,STATE_CONNECT_USER);
    drawFeedback(ctx);
}

static void drawNewUser(GameCtx *ctx){
    UIContext *ui=ctx->ui; ctx->nbBtns=0;
    uiDrawBackground(); drawTopBar(ctx);
    uiTextCentered(ui,"Creer un compte",FONT_LARGE,COL_PRIMARY,WIN_W/2.f,74);
    uiText(ui,"Votre prenom :",FONT_SMALL,COL_SECONDARY,WIN_W/2.f-200,230);
    ctx->tfNom.x=WIN_W/2.f-200; ctx->tfNom.y=258; tfDraw(ui,&ctx->tfNom);
    uiText(ui,"Langue :",FONT_SMALL,COL_SECONDARY,WIN_W/2.f-200,330);
    float lw=172,lh=44,lgap=10, lx=WIN_W/2.f-(3*lw+2*lgap)/2.f;
    for(int i=0;i<NB_LANGUES;i++){
        int row=i/3,col=i%3,sel=(i==ctx->choixLangue);
        addBtn(ctx,lx+col*(lw+lgap),362.f+row*(lh+lgap),lw,lh,LANGUES[i],
               sel?COL_PRIMARY:COL_CARD,sel?COL_PRIMARY_DK:COL_PRIMARY_LT,sel?COL_WHITE:COL_DARK,10,FONT_SMALL);
    }
    addBtn(ctx,WIN_W/2.f-200,510,188,52,"Retour",COL_SECONDARY,COL_SECONDARY_DK,COL_WHITE,RADIUS_BTN,FONT_SMALL);
    addBtn(ctx,WIN_W/2.f+12, 510,188,52,"Commencer !",COL_GREEN,COL_GREEN_DK,COL_WHITE,RADIUS_BTN,FONT_SMALL);
    updateAndDrawBtns(ctx);
    for(int i=0;i<NB_LANGUES;i++) if(btnClicked(ctx,i)) ctx->choixLangue=i;
    if(ctx->mouseClicked) ctx->tfNom.active=tfHit(&ctx->tfNom,ctx->mouseX,ctx->mouseY);
    if(btnClicked(ctx,NB_LANGUES)) gameSetState(ctx,STATE_LOGIN);
    if(btnClicked(ctx,NB_LANGUES+1)){
        if(strlen(ctx->tfNom.buffer)<2) setFeedback(ctx,"Entrez un prenom d'au moins 2 caracteres.",COL_RED);
        else if(creerJoueur(ctx->tfNom.buffer,LANGUES[ctx->choixLangue],&ctx->joueur)){
            ctx->estConnecte=1;
            char msg[100]; sprintf(msg,"Bienvenue %s ! Code : %s (NOTEZ-LE !)",ctx->joueur.nom,ctx->joueur.code);
            setFeedback(ctx,msg,COL_GREEN); gameSetState(ctx,STATE_MAIN_MENU);
        } else setFeedback(ctx,"Erreur lors de la creation.",COL_RED);
    }
    drawFeedback(ctx);
}

static void drawConnectUser(GameCtx *ctx){
    UIContext *ui=ctx->ui; ctx->nbBtns=0;
    uiDrawBackground();
    uiTextCentered(ui,"Connexion",FONT_LARGE,COL_PRIMARY,WIN_W/2.f,120);
    uiTextCentered(ui,"Entrez votre code personnel",FONT_SMALL,COL_SECONDARY,WIN_W/2.f,180);
    drawPhoenix(WIN_W/2.f,305,40,(float)ctx->animTick);
    ctx->tfCode.x=WIN_W/2.f-200; ctx->tfCode.y=368; tfDraw(ui,&ctx->tfCode);
    addBtn(ctx,WIN_W/2.f-200,444,188,52,"Retour",COL_SECONDARY,COL_SECONDARY_DK,COL_WHITE,RADIUS_BTN,FONT_SMALL);
    addBtn(ctx,WIN_W/2.f+12, 444,188,52,"Se connecter",COL_PRIMARY,COL_PRIMARY_DK,COL_WHITE,RADIUS_BTN,FONT_SMALL);
    updateAndDrawBtns(ctx);
    if(ctx->mouseClicked) ctx->tfCode.active=tfHit(&ctx->tfCode,ctx->mouseX,ctx->mouseY);
    if(btnClicked(ctx,0)) gameSetState(ctx,STATE_LOGIN);
    if(btnClicked(ctx,1)){
        if(strlen(ctx->tfCode.buffer)<5) setFeedback(ctx,"Code trop court.",COL_RED);
        else if(connecterJoueur(ctx->tfCode.buffer,&ctx->joueur)){
            ctx->estConnecte=1;
            char msg[80]; sprintf(msg,"Bon retour, %s !",ctx->joueur.nom);
            setFeedback(ctx,msg,COL_GREEN); gameSetState(ctx,STATE_MAIN_MENU);
        } else setFeedback(ctx,"Code introuvable. Verifiez et reessayez.",COL_RED);
    }
    drawFeedback(ctx);
}

static void drawMainMenu(GameCtx *ctx){
    UIContext *ui=ctx->ui; ctx->nbBtns=0;
    uiDrawBackground(); drawTopBar(ctx);
    char greet[80]; sprintf(greet,"Bonjour, %s !",ctx->joueur.nom);
    uiTextCentered(ui,greet,FONT_LARGE,COL_DARK,WIN_W/2.f,74);
    float pct=(ctx->joueur.pieces%100)/100.f;
    uiProgressBar(WIN_W/2.f-200,124,400,14,pct,COL_CARD_BORDER,COL_PRIMARY,7);
    char pxt[50]; sprintf(pxt,"%d pieces vers le niveau suivant",100-ctx->joueur.pieces%100);
    uiTextCentered(ui,pxt,FONT_TINY,COL_TEXT_HINT,WIN_W/2.f,142);
    drawPhoenix(WIN_W/2.f,270,70,(float)ctx->animTick);
    const char *phrases[]={"Le voyage de mille lecons commence ici.",
        "Chaque mot appris est une victoire.","Ta culture, ton tresor. Continue !",
        "Mbolo ! Tu es en feu aujourd'hui !"};
    uiTextCentered(ui,phrases[(int)(ctx->animTick*4)%4],FONT_TINY,COL_SECONDARY,WIN_W/2.f,370);
    float bw=290,bh=58,bx=WIN_W/2.f-bw/2.f;
    addBtn(ctx,bx,402,bw,bh,"Lecons",COL_PRIMARY,COL_PRIMARY_DK,COL_WHITE,RADIUS_BTN,FONT_MEDIUM);
    addBtn(ctx,bx,474,bw,bh,"Quiz",COL_GOLD,COL_GOLD_DK,COL_WHITE,RADIUS_BTN,FONT_MEDIUM);
    addBtn(ctx,bx,546,bw,bh,"Ma progression",COL_GREEN,COL_GREEN_DK,COL_WHITE,RADIUS_BTN,FONT_MEDIUM);
    addBtn(ctx,bx,612,bw,44,"Deconnexion",COL_SECONDARY,COL_SECONDARY_DK,COL_WHITE,RADIUS_BTN,FONT_SMALL);
    updateAndDrawBtns(ctx);
    if(btnClicked(ctx,0)) gameSetState(ctx,STATE_LESSON_SELECT);
    if(btnClicked(ctx,1)) gameSetState(ctx,STATE_QUIZ);
    if(btnClicked(ctx,2)) gameSetState(ctx,STATE_PROGRESS);
    if(btnClicked(ctx,3)){ctx->estConnecte=0; gameSetState(ctx,STATE_LOGIN);}
    drawFeedback(ctx);
}

static void drawLessonSelect(GameCtx *ctx){
    UIContext *ui=ctx->ui; ctx->nbBtns=0;
    int niv=ctx->joueur.niveau-1;
    uiDrawBackground(); drawTopBar(ctx);
    char titre[60]; sprintf(titre,"Lecons - Niveau %d",ctx->joueur.niveau);
    uiTextCentered(ui,titre,FONT_LARGE,COL_PRIMARY,WIN_W/2.f,74);
    float cw=380,ch=88,gapX=20,gapY=14;
    float gx=WIN_W/2.f-cw-gapX/2.f, gy=130;
    for(int i=0;i<NB_LECONS;i++){
        int col=i%2,row=i/2;
        float cx2=gx+col*(cw+gapX), cy2=gy+row*(ch+gapY);
        uiDrawCard(cx2,cy2,cw,ch,16,COL_CARD);
        al_draw_filled_circle(cx2+34,cy2+ch/2.f,22,COL_PRIMARY_LT);
        char num[4]; sprintf(num,"%d",i+1);
        uiTextCentered(ui,num,FONT_MEDIUM,COL_PRIMARY,cx2+34,cy2+ch/2.f-11);
        uiText(ui,lecons[niv][i].titre,FONT_MEDIUM,COL_DARK,cx2+68,cy2+16);
        char sub[30]; sprintf(sub,"%d mots a apprendre",lecons[niv][i].nbMots);
        uiText(ui,sub,FONT_TINY,COL_TEXT_HINT,cx2+68,cy2+52);
        uiText(ui,">",FONT_MEDIUM,COL_PRIMARY,cx2+cw-30,cy2+ch/2.f-12,0);
    }
    addBtn(ctx,20,WIN_H-68,130,44,"< Retour",COL_SECONDARY,COL_SECONDARY_DK,COL_WHITE,10,FONT_SMALL);
    updateAndDrawBtns(ctx);
    if(ctx->mouseClicked){
        for(int i=0;i<NB_LECONS;i++){
            int col=i%2,row=i/2;
            float cx2=gx+col*(cw+gapX), cy2=gy+row*(ch+gapY);
            if(ctx->mouseX>=cx2&&ctx->mouseX<=cx2+cw&&ctx->mouseY>=cy2&&ctx->mouseY<=cy2+ch){
                ctx->lesson.leconIdx=i; gameSetState(ctx,STATE_LESSON_VIEW); return;
            }
        }
    }
    if(btnClicked(ctx,0)) gameSetState(ctx,STATE_MAIN_MENU);
    drawFeedback(ctx);
}

static void drawLessonView(GameCtx *ctx){
    UIContext *ui=ctx->ui; ctx->nbBtns=0;
    int niv=ctx->joueur.niveau-1;
    Lecon *lecon=&lecons[niv][ctx->lesson.leconIdx];
    MotVocab *mot=&lecon->mots[ctx->lesson.motIdx];
    uiDrawBackground(); drawTopBar(ctx);
    char titre[100]; sprintf(titre,"Lecon %d : %s",ctx->lesson.leconIdx+1,lecon->titre);
    uiTextCentered(ui,titre,FONT_LARGE,COL_PRIMARY,WIN_W/2.f,74);
    float lpct=(float)(ctx->lesson.motIdx+1)/lecon->nbMots;
    uiProgressBar(WIN_W/2.f-300,112,600,12,lpct,COL_CARD_BORDER,COL_PRIMARY,6);
    char lp[30]; sprintf(lp,"%d / %d",ctx->lesson.motIdx+1,lecon->nbMots);
    uiTextCentered(ui,lp,FONT_TINY,COL_TEXT_HINT,WIN_W/2.f,130);
    float cx=WIN_W/2.f-280,cy=150,cw=560,ch=290;
    uiDrawCard(cx,cy,cw,ch,RADIUS_CARD,COL_CARD);
    uiTextCentered(ui,mot->ewondo,FONT_HUGE,COL_PRIMARY,WIN_W/2.f,cy+52);
    al_draw_line(WIN_W/2.f-80,cy+148,WIN_W/2.f+80,cy+148,COL_PRIMARY_LT,2.f);
    uiTextCentered(ui,mot->francais,FONT_LARGE,COL_DARK,WIN_W/2.f,cy+162);
    if(!ctx->lesson.flipped)
        uiTextCentered(ui,"Cliquez sur la carte pour la retourner",FONT_TINY,COL_TEXT_HINT,WIN_W/2.f,cy+260);
    else{
        al_draw_filled_rounded_rectangle(cx,cy,cx+cw,cy+ch,RADIUS_CARD,RADIUS_CARD,al_map_rgba(74,144,217,20));
        uiTextCentered(ui,"Prononcez lentement, syllabe par syllabe !",FONT_SMALL,COL_PRIMARY,WIN_W/2.f,cy+235);
    }
    int canPrev=(ctx->lesson.motIdx>0), isLast=(ctx->lesson.motIdx==lecon->nbMots-1);
    addBtn(ctx,WIN_W/2.f-170,WIN_H-85,155,50,"< Precedent",
           canPrev?COL_SECONDARY:al_map_rgb(200,200,200),
           canPrev?COL_SECONDARY_DK:al_map_rgb(200,200,200),COL_WHITE,RADIUS_BTN,FONT_SMALL);
    addBtn(ctx,WIN_W/2.f+15,WIN_H-85,155,50,isLast?"Terminer !":"Suivant >",
           isLast?COL_GREEN:COL_PRIMARY,isLast?COL_GREEN_DK:COL_PRIMARY_DK,COL_WHITE,RADIUS_BTN,FONT_SMALL);
    addBtn(ctx,20,WIN_H-68,120,44,"< Retour",COL_SECONDARY,COL_SECONDARY_DK,COL_WHITE,10,FONT_TINY);
    addBtn(ctx,WIN_W/2.f-65,WIN_H-140,130,36,"Retourner",COL_PRIMARY_LT,COL_PRIMARY,COL_PRIMARY,10,FONT_TINY);
    updateAndDrawBtns(ctx);
    if(ctx->mouseClicked&&ctx->mouseX>=cx&&ctx->mouseX<=cx+cw&&ctx->mouseY>=cy&&ctx->mouseY<=cy+ch)
        ctx->lesson.flipped=!ctx->lesson.flipped;
    if(canPrev&&btnClicked(ctx,0)){ctx->lesson.motIdx--;ctx->lesson.flipped=0;}
    if(btnClicked(ctx,1)){
        if(!isLast){ctx->lesson.motIdx++;ctx->lesson.flipped=0;}
        else{ctx->joueur.pieces+=20;setFeedback(ctx,"Lecon terminee ! +20 pieces gagnees",COL_GREEN);gameSetState(ctx,STATE_LESSON_SELECT);}
    }
    if(btnClicked(ctx,2)) gameSetState(ctx,STATE_LESSON_SELECT);
    if(btnClicked(ctx,3)) ctx->lesson.flipped=!ctx->lesson.flipped;
    drawFeedback(ctx);
}

static void drawQuiz(GameCtx *ctx){
    UIContext *ui=ctx->ui; ctx->nbBtns=0;
    int niv=ctx->joueur.niveau-1;
    Question *q=&quiz[niv][ctx->quiz.questionIdx];
    uiDrawBackground(); drawTopBar(ctx);
    char htxt[60]; sprintf(htxt,"Quiz - Question %d / %d",ctx->quiz.questionIdx+1,NB_QUESTIONS);
    uiTextCentered(ui,htxt,FONT_MEDIUM,COL_PRIMARY,WIN_W/2.f,72);
    uiProgressBar(WIN_W/2.f-280,102,560,12,(float)ctx->quiz.questionIdx/NB_QUESTIONS,COL_CARD_BORDER,COL_GOLD,6);
    uiDrawCard(WIN_W/2.f-340,124,680,104,16,COL_CARD);
    uiTextInRect(ui,q->question,FONT_SMALL,COL_DARK,WIN_W/2.f-330,124,670,104);
    drawPhoenix(WIN_W-82.f,WIN_H-85.f,ctx->quiz.answered?42:32,
                (float)(ctx->quiz.answered?ctx->animTick*2.5:ctx->animTick));
    float cw=318,ch=64,cgap=16;
    float bxs[4]={WIN_W/2.f-cw-cgap/2.f,WIN_W/2.f+cgap/2.f,WIN_W/2.f-cw-cgap/2.f,WIN_W/2.f+cgap/2.f};
    float bys[4]={248,248,248+ch+cgap,248+ch+cgap};
    for(int i=0;i<NB_CHOIX;i++){
        ALLEGRO_COLOR cn,ch_c,ct;
        if(!ctx->quiz.answered){cn=COL_CARD;ch_c=COL_PRIMARY_LT;ct=COL_DARK;}
        else if(i==q->bonneReponse){cn=COL_GREEN;ch_c=COL_GREEN_DK;ct=COL_WHITE;}
        else if(i==ctx->quiz.selectedChoice){cn=COL_RED;ch_c=COL_RED_DK;ct=COL_WHITE;}
        else{cn=COL_CARD;ch_c=COL_CARD;ct=COL_TEXT_HINT;}
        Button *b=addBtn(ctx,bxs[i],bys[i],cw,ch,q->choix[i],cn,ch_c,ct,12,FONT_SMALL);
        if(ctx->quiz.answered) b->disabled=1;
    }
    if(!ctx->quiz.answered)
        addBtn(ctx,WIN_W/2.f-80,WIN_H-68,160,42,"Passer ->",COL_TEXT_HINT,COL_SECONDARY,COL_WHITE,10,FONT_TINY);
    updateAndDrawBtns(ctx);
    if(!ctx->quiz.answered){
        for(int i=0;i<NB_CHOIX;i++){
            if(btnClicked(ctx,i)){
                ctx->quiz.selectedChoice=i; ctx->quiz.answered=1; ctx->quiz.answerTime=al_get_time()+1.4;
                if(i==q->bonneReponse){ctx->quiz.nbBonnes++;ctx->joueur.pieces+=15;setFeedback(ctx,"Excellent ! Bonne reponse ! +15 pieces",COL_GREEN);}
                else{if(ctx->joueur.coeurs>0)ctx->joueur.coeurs--;char err[140];sprintf(err,"Rate ! Bonne reponse : %s",q->choix[q->bonneReponse]);setFeedback(ctx,err,COL_RED);}
                break;
            }
        }
        if(ctx->nbBtns>NB_CHOIX&&btnClicked(ctx,NB_CHOIX)){
            ctx->quiz.answered=1;ctx->quiz.answerTime=al_get_time()+0.8;setFeedback(ctx,"Question passee.",COL_SECONDARY);
        }
    }
    drawFeedback(ctx);
}

static void drawQuizResult(GameCtx *ctx){
    UIContext *ui=ctx->ui; ctx->nbBtns=0;
    int bonnes=ctx->quiz.nbBonnes, reussi=(bonnes>=3);
    uiDrawBackground(); drawTopBar(ctx);
    drawPhoenix(WIN_W/2.f,WIN_H/2.f-100,reussi?95:65,(float)(ctx->animTick*(reussi?2.0:1.0)));
    uiTextCentered(ui,reussi?"Quiz reussi !":"Pas encore...",FONT_LARGE,reussi?COL_GREEN:COL_RED,WIN_W/2.f,WIN_H/2.f+16);
    char score[60]; sprintf(score,"%d / %d bonnes reponses",bonnes,NB_QUESTIONS);
    uiTextCentered(ui,score,FONT_MEDIUM,COL_DARK,WIN_W/2.f,WIN_H/2.f+62);
    int stars=(bonnes>=5)?3:(bonnes>=4)?2:(bonnes>=3)?1:0;
    for(int i=0;i<3;i++) uiDrawStar(WIN_W/2.f-60+i*60.f,WIN_H/2.f+108,(i<stars)?26:20,(i<stars)?COL_GOLD:al_map_rgb(200,200,200));
    char ptxt[40]; sprintf(ptxt,"+%d pieces",bonnes*15);
    uiTextCentered(ui,ptxt,FONT_SMALL,COL_GOLD,WIN_W/2.f,WIN_H/2.f+148);
    if(reussi&&ctx->joueur.niveau<NB_NIVEAUX){
        ctx->joueur.niveau++; ctx->joueur.pieces+=50;
        uiTextCentered(ui,"Niveau suivant debloque ! +50 pieces bonus",FONT_SMALL,COL_PRIMARY,WIN_W/2.f,WIN_H/2.f+178);
    }
    addBtn(ctx,WIN_W/2.f-200,WIN_H-90,185,52,"Recommencer",COL_SECONDARY,COL_SECONDARY_DK,COL_WHITE,RADIUS_BTN,FONT_SMALL);
    addBtn(ctx,WIN_W/2.f+15, WIN_H-90,185,52,"Menu principal",COL_PRIMARY,COL_PRIMARY_DK,COL_WHITE,RADIUS_BTN,FONT_SMALL);
    updateAndDrawBtns(ctx);
    if(btnClicked(ctx,0)) gameSetState(ctx,STATE_QUIZ);
    if(btnClicked(ctx,1)) gameSetState(ctx,STATE_MAIN_MENU);
    drawFeedback(ctx);
}

static void drawProgress(GameCtx *ctx){
    UIContext *ui=ctx->ui; ctx->nbBtns=0;
    uiDrawBackground(); drawTopBar(ctx);
    uiTextCentered(ui,"Ma Progression",FONT_LARGE,COL_PRIMARY,WIN_W/2.f,74);
    uiDrawCard(WIN_W/2.f-340,118,680,195,RADIUS_CARD,COL_CARD);
    drawPhoenix(WIN_W/2.f-240,218,58,(float)ctx->animTick);
    float sx=WIN_W/2.f-120,sy=138;
    uiText(ui,"Nom :",FONT_SMALL,COL_TEXT_HINT,sx,sy);
    uiText(ui,ctx->joueur.nom,FONT_MEDIUM,COL_DARK,sx+80,sy-4);
    uiText(ui,"Langue :",FONT_SMALL,COL_TEXT_HINT,sx,sy+44);
    uiText(ui,ctx->joueur.langue,FONT_MEDIUM,COL_PRIMARY,sx+80,sy+40);
    uiText(ui,"Code :",FONT_SMALL,COL_TEXT_HINT,sx,sy+88);
    uiText(ui,ctx->joueur.code,FONT_SMALL,COL_SECONDARY,sx+80,sy+90);
    char niv[30]; sprintf(niv,"Niveau %d / %d",ctx->joueur.niveau,NB_NIVEAUX);
    uiText(ui,niv,FONT_SMALL,COL_TEXT_HINT,sx,sy+130);
    uiProgressBar(sx+80,sy+134,230,14,(float)ctx->joueur.niveau/NB_NIVEAUX,COL_CARD_BORDER,COL_PRIMARY,7);
    struct{const char *lab;int val,type;}badges[]=
        {{"Coeurs",ctx->joueur.coeurs,0},{"Pieces",ctx->joueur.pieces,1},{"Niveau",ctx->joueur.niveau,2}};
    ALLEGRO_COLOR bcols[]={COL_RED,COL_GOLD,COL_PRIMARY};
    for(int i=0;i<3;i++){
        float bx=WIN_W/2.f-310+i*220.f,by=332;
        uiDrawCard(bx,by,200,96,14,COL_CARD);
        if(badges[i].type==0) uiDrawHeart(bx+30,by+36,17,bcols[i]);
        else if(badges[i].type==1) uiDrawCoin(bx+30,by+36,17);
        else uiDrawStar(bx+30,by+36,17,bcols[i]);
        char val[20]; sprintf(val,"%d",badges[i].val);
        uiText(ui,val,FONT_LARGE,bcols[i],bx+55,by+16);
        uiText(ui,badges[i].lab,FONT_TINY,COL_TEXT_HINT,bx+14,by+68);
    }
    uiDrawCard(WIN_W/2.f-340,448,680,76,14,al_map_rgb(245,250,255));
    uiTextInRect(ui,(ctx->joueur.coeurs<3)?"Attention ! Vos coeurs sont bas. Repassez les lecons.":
                 "Super ! Pratiquez chaque jour pour progresser rapidement.",
                 FONT_TINY,COL_SECONDARY,WIN_W/2.f-330,448,660,76);
    addBtn(ctx,WIN_W/2.f-130,WIN_H-76,260,50,"< Retour au menu",COL_PRIMARY,COL_PRIMARY_DK,COL_WHITE,RADIUS_BTN,FONT_SMALL);
    updateAndDrawBtns(ctx);
    if(btnClicked(ctx,0)) gameSetState(ctx,STATE_MAIN_MENU);
    drawFeedback(ctx);
}

static void gameDraw(GameCtx *ctx){
    al_clear_to_color(COL_BG);
    switch(ctx->state){
    case STATE_LOGIN:          drawLogin(ctx);         break;
    case STATE_NEW_USER:       drawNewUser(ctx);       break;
    case STATE_CONNECT_USER:   drawConnectUser(ctx);   break;
    case STATE_MAIN_MENU:      drawMainMenu(ctx);      break;
    case STATE_LESSON_SELECT:  drawLessonSelect(ctx);  break;
    case STATE_LESSON_VIEW:    drawLessonView(ctx);    break;
    case STATE_QUIZ:           drawQuiz(ctx);          break;
    case STATE_QUIZ_RESULT:    drawQuizResult(ctx);    break;
    case STATE_PROGRESS:       drawProgress(ctx);      break;
    default: break;
    }
    al_flip_display();
}

/* ================================================================
   MAIN
   ================================================================ */

int main(void) {
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS,1,ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES,4,ALLEGRO_SUGGEST);

    ALLEGRO_DISPLAY *display=al_create_display(WIN_W,WIN_H);
    al_set_window_title(display,"CAMlex - Langues camerounaises");

    ALLEGRO_TIMER *timer=al_create_timer(1.0/FPS);
    ALLEGRO_EVENT_QUEUE *queue=al_create_event_queue();
    al_register_event_source(queue,al_get_display_event_source(display));
    al_register_event_source(queue,al_get_keyboard_event_source());
    al_register_event_source(queue,al_get_mouse_event_source());
    al_register_event_source(queue,al_get_timer_event_source(timer));

    const char *fontPaths[]={"assets/font.ttf","assets/DejaVuSans.ttf",
        "C:/Windows/Fonts/arial.ttf","C:/Windows/Fonts/calibri.ttf",NULL};
    const char *fontPath=fontPaths[0];
    for(int i=0;fontPaths[i];i++){
        ALLEGRO_FONT *t=al_load_ttf_font(fontPaths[i],16,0);
        if(t){al_destroy_font(t);fontPath=fontPaths[i];break;}
    }

    initDonnees();

    GameCtx *ctx=gameCreate(display,queue,timer,fontPath);

    al_start_timer(timer);
    double prevTime=al_get_time();

    while(ctx->running&&ctx->state!=STATE_QUIT){
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue,&ev);
        gameHandleEvent(ctx,&ev);
        if(ev.type==ALLEGRO_EVENT_TIMER){
            double now=al_get_time(), dt=now-prevTime; prevTime=now;
            gameUpdate(ctx,dt);
            if(al_is_event_queue_empty(queue)){
                gameDraw(ctx);
                ctx->mouseClicked=0;
                ctx->needDraw=0;
            }
        }
    }

    gameDestroy(ctx);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;
}
