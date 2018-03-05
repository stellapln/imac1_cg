#ifdef __APPLE__
#include <openGL/gl.h>
#include <openGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <SDL_image/SDL_image.h>
#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/********** VARIABLES & CONSTANTES **********/

static unsigned int WINDOW_WIDTH = 800;
static unsigned int WINDOW_HEIGHT = 800;
static const unsigned int BIT_PER_PIXEL = 32;
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;
const char* filename = "images.bmp";

/********** FONCTIONS **********/

void resizeViewport() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1., 1., -1., 1.);
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE);
}

/********** MAIN **********/

int main(int argc, char** argv) {

    /* Initialisation de la SDL */
    if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
        return EXIT_FAILURE;
    }

    /* Ouverture d'une fenêtre et création d'un contexte OpenGL */
    if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE)) {
        fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    SDL_WM_SetCaption("L'OpenGL de Stella", NULL);
    resizeViewport();

    SDL_Surface* surface = SDL_LoadBMP(filename);

    if(surface == NULL){
        printf("Error surface\n");
        exit(EXIT_FAILURE);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID); /* GL_TEXTURE_2D = point de bind, la fonction glBindTexture lui attache une texture */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); /* GL_TEXTURE_MIN_FILTER = filtre de minification */

    /* Chargement et traitement de la texture sur la GPU */
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        surface->w,
        surface->h,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        surface->pixels);

    // TODO: Libération des données CPU
    // ...

    /* Boucle de dessin (à décommenter pour l'exercice 3) */
    int loop = 1;
    glClearColor(0.1, 0.1, 0.1 ,1.0);
    while(loop) {

        Uint32 startTime = SDL_GetTicks();

        /* Code de dessin */

        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D); /* On précise qu’on veut activer la fonctionnalité detexturing */
        glBindTexture(GL_TEXTURE_2D, textureID); /* On bind la texture pour pouvoir l’utiliser */

        glPushMatrix();
        glScalef(0.5,1,1);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(-0.5,0.5);
            glTexCoord2f(1, 0);
            glVertex2f(0.5,0.5);
            glTexCoord2f(1, 1);
            glVertex2f(0.5,-0.5);
            glTexCoord2f(0, 1);
            glVertex2f(-0.5,-0.5);
        glEnd();
        glPopMatrix();

        // Fin du code de dessin
         glDisable(GL_TEXTURE_2D); /* On désactive le sampling de texture */
         glBindTexture(GL_TEXTURE_2D, 0); /* On débinde la texture */

        SDL_Event e;
        while(SDL_PollEvent(&e)) {

            switch(e.type) {

                case SDL_QUIT:
                    loop = 0;
                    break;

                case SDL_VIDEORESIZE:
                    WINDOW_WIDTH = e.resize.w;
                    WINDOW_HEIGHT = e.resize.h;
                    resizeViewport();

                default:
                    break;
            }
        }

        SDL_GL_SwapBuffers();
        Uint32 elapsedTime = SDL_GetTicks() - startTime;
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }
    

    // TODO: Libération des données GPU
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Liberation des ressources associées à la SDL */
    SDL_FreeSurface(surface);
    SDL_Quit();

    return EXIT_SUCCESS;
}
