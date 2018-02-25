#ifdef __APPLE__
#include <openGL/gl.h>
#include <openGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>


/************* CONSTANTES **************/


/* Dimensions de la fenêtre */
static unsigned int WINDOW_WIDTH = 400;
static unsigned int WINDOW_HEIGHT = 400;

/* Nombre de bits par pixel de la fenêtre */
static const unsigned int BIT_PER_PIXEL = 8;

/* Couleurs RVB (à savoir qu'un unsigned char est relatif à un int) */
static const unsigned char COLORS[] = {
     255, 255, 255 ,
     0, 0, 0 ,
     255, 0, 0 ,
     0, 255, 0 ,
     0, 0, 255 ,
     255, 255, 0 ,
     0, 255, 255 ,
     255, 0, 255
};

/* Pour avoir les couleurs RVB du points, on prend les 3 valeurs correspondantes à ce point */
static const unsigned int NB_COLORS = sizeof(COLORS) / (3 * sizeof(unsigned char));


/* Nombre minimal de millisecondes separant le rendu de deux images */
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;


/************** STRUCTURES **************/


typedef struct Point{
    float x, y; // Position 2D du point
    unsigned char r, g, b; // Couleur du point
    struct Point* next; // Point suivant à dessiner
} Point, *PointList;

typedef struct Primitive{
    GLenum primitiveType;
    PointList points;
    struct Primitive* next;
} Primitive, *PrimitiveList;


/************** FONCTIONS ***************/


/* Création du point, placement dans la liste, dessin et libération de mémoire */
Point* allocPoint(float x, float y, unsigned char r, unsigned char g, unsigned char b) {
    Point* point = (Point*)malloc(sizeof(Point));

    if(!point) {
        printf("Error at point malloc\n");
        exit(1);
    }

    point->x = x;
    point->y = y;
    point->r = r;
    point->g = g;
    point->b = b;
    point->next = NULL;

    return point;
}

void addPointToList(Point* point, PointList* list) {

    /* Si ma liste est nulle, j'y place mon point simplement */
    if(*list == NULL) {
        *list = point;
    }
    /* Sinon j'applique une récusivité pour que mon point se place en fin de liste */
    else {
        addPointToList(point, &(*list)->next);
    }

    return;

}

void drawPoints(PointList list) {

    /* Si ma liste n'est pas vide */
    while(list) {
        /* Je colorie le pixel aux coordonnées du point avec la couleur spécifique du point */
        glColor3ub(list->r, list->g, list->b);
        glVertex2f(list->x, list->y);
        list = list->next;
    }
    return;

}
 
void deletePoints(PointList* list) {

    /* Si ma liste n'est pas vide */
    while(*list != NULL) {
        /* Je créée un point* pour enregistrer la valeur suivant à celle que je veux free, je free celle-ci, puis ma liste commence maintenant à ce point* */
        Point* next = (*list)->next;
        free(*list);
        *list = next;
    }

    return;
}

/* Création de la primitive, placement dans la liste, dessin et libération de mémoire */
Primitive* allocPrimitive(GLenum primitiveType) {
    Primitive* primitive = (Primitive*)malloc(sizeof(Primitive));

    if (!primitive) {
        printf("Error at primitive malloc\n");
        exit(1);
    }

    primitive->primitiveType = primitiveType;
    primitive->points = NULL;
    primitive->next = NULL;

    return primitive;
}

void addPrimitive(Primitive* primitive, PrimitiveList* list){

    /* J'ajoute ma primitive au début de la liste */ 
    primitive->next = *list;
    *list = primitive;

    return;
}

void drawPrimitives(PrimitiveList list){

    while(list) {
        glBegin(list->primitiveType);
        drawPoints(list->points);
        list = list->next;
        glEnd();
    }

    return;
}

/* Je vide d'abord les champs de la primitive puis la primitive de la liste */
void deletePrimitive(PrimitiveList* list) {

    /* Si ma liste n'est pas vide */
    while(*list) {
        Primitive* next = (*list)->next;
        deletePoints(&(*list)->points);
        free(*list);
        *list = next;
    }

    return;
}

/* Fonctions qui resize la fenêtre */
void resize(int w, int h) {

    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1., 1., -1., 1.);

}

/* Fonction qui affiche la palette par rapport aux colonnes de width */
void affichePalette() {
    int i, r, g, b;
    int nb_color = 8;
    float column_width = 1.0/4.0; // Largeur d'une colonne width

    for(i = 0 ; i < nb_color ; i++){
        switch (i){
            case 0:
                r = 255;
                g = 255;
                b = 255;
                break;
            case 1:
                r = 0;
                g = 0;
                b = 0;
                break;
            case 2:
                r = 255;
                g = 0;
                b = 0;
                break;
            case 3:
                r = 0;
                g = 255;
                b = 0;
                break;
            case 4:
                r = 0;
                g = 0;
                b = 255;
                break;
            case 5:
                r = 255;
                g = 255;
                b = 0;
                break;
            case 6:
                r = 0;
                g = 255;
                b = 255;
                break;
            case 7:
                r = 255;
                g = 0;
                b = 255;
                break;
             default:
                break;
        }
        glColor3ub(r, g, b);
        glBegin(GL_QUADS);
            glVertex2f(-1 + (column_width * i), 1);
            glVertex2f(-1 + ((i+1) * column_width), 1);
            glVertex2f(-1 + ((i+1) * column_width), -1);
            glVertex2f(-1 + (i * column_width), -1);
        glEnd();
    }
}

/* Fonction qui affiche ma liste passée en paramètre */
void afficheListe(PrimitiveList list) {
    PrimitiveList tmp = list;

    while (tmp->next != NULL) {
        printf("Élément dans liste de %u : r : %d  v : %d b : %d : x : %f y : %f \n", tmp->primitiveType, tmp->points->r, tmp->points->g, tmp->points->b, tmp->points->x, tmp->points->y);
        tmp = tmp->next;
    }

}


/**************** MAIN ***************/


int main(int argc, char** argv) {
    unsigned char color = 0; /* color par défaut : blanc */
    int mode = 0; /* mode dessin par défaut */

    /* Initialisation de la SDL */
    if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
        return EXIT_FAILURE;
    }

    /* Initialisation d'une liste de primitives */
    PrimitiveList primList = NULL;
    addPrimitive(allocPrimitive(GL_POINTS), &primList);

    /* Ouverture d'une fenêtre et création d'un contexte OpenGL */
    if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE)) {
        fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        return EXIT_FAILURE;
    }

    resize(WINDOW_WIDTH, WINDOW_HEIGHT);  

    /* Titre de la fenêtre */
    SDL_WM_SetCaption("L'OpenGL du Spoula", NULL);

    /* Boucle d'affichage */
    int loop = 1;
    while(loop) {

        /* Récupération du temps au début de la boucle */
        Uint32 startTime = SDL_GetTicks();

        glClear(GL_COLOR_BUFFER_BIT);
        
        /* Choix du mode pour le dessin, 1 pour palette et 0 pour dessin */
        if (mode == 1) {
            affichePalette();
        }
        else {
            /* Mode dessin */
            drawPrimitives(primList);
        }
        
        SDL_GL_SwapBuffers();

        /* Boucle traitant les evenements */
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            /* L'utilisateur ferme la fenêtre : */
            if (e.type == SDL_QUIT) {
                loop = 0;
                break;
            }

            /* Quelques exemples de traitement d'evenements : */
            switch (e.type) {

                /* Clic souris */
                case SDL_MOUSEBUTTONUP:

                    if (mode == 1) {
                        /* On modifie la couleur par défaut */
                        color = e.button.x * NB_COLORS / WINDOW_WIDTH;
                    }
                    else {
                        /* En mode dessin on ajoute un point dans la liste de la primitive courante */
                        addPointToList(allocPoint(-1 + 2. * e.button.x / WINDOW_WIDTH, - (-1 + 2. * e.button.y / WINDOW_HEIGHT), COLORS[color * 3], COLORS[color * 3 + 1], COLORS[color * 3 + 2]), &primList->points);
                    }
                    break;

                /* Touche clavier */
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym) {
                        case SDLK_a:
                            afficheListe(primList);
                            break;
                        case SDLK_l:
                            addPrimitive(allocPrimitive(GL_LINES), &primList);
                            break;
                        case SDLK_p:
                            addPrimitive(allocPrimitive(GL_POINTS), &primList);
                            break; 
                        case SDLK_t:
                            addPrimitive(allocPrimitive(GL_TRIANGLES), &primList);
                            break;
                        case SDLK_q:
                            loop = 0;
                            break;
                        case SDLK_SPACE:
                            mode = 1;
                            break;
                        /* Reset le dessin (vide les listes puis réalloue) */
                        case SDLK_r:
                            deletePrimitive(&primList);
                            addPrimitive(allocPrimitive(GL_POINTS), &primList);
                            break; 
                        /* Comme un ctrl + z mais possible qu'une fois pour le dernier élément */
                        case SDLK_z:
                            deletePoints(&primList->points);
                            break;  
                        default:
                            mode = 0;
                        break;
                    }
                    //printf("touche pressée (code = %d)\n", e.key.keysym.sym);
                    break;

                case SDL_KEYUP:
                    if(e.key.keysym.sym == SDLK_SPACE) {
                        mode = 0;
                    }
                    break;

                /*case SDL_MOUSEMOTION:
                    printf("mouvement en (%d, %d)\n", e.motion.x, e.motion.y);
                    float rouge,vert,bleu;
                    rouge = e.motion.x/(float)WINDOW_WIDTH;
                    vert = e.motion.y/(float)WINDOW_HEIGHT;
                    bleu = rouge + vert;
                    glClearColor(rouge, vert, bleu, 1);
                    break;*/

                case SDL_VIDEORESIZE:
                    resize(e.resize.w, e.resize.h);
                    break;

                default:
                    break;
            }
        }
        /* Calcul du temps écoulé */
        Uint32 elapsedTime = SDL_GetTicks() - startTime;

        /* Si trop peu de temps s'est écoulé, on met en pause le programme */
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }
    /* Libération de la mémoire */
    deletePrimitive(&primList);

    /* Liberation des ressources associées à la SDL */ 
    SDL_Quit();

    return EXIT_SUCCESS;
}
