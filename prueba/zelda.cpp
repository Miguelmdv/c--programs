#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define bbox_collision(x1,y1,w1,h1,x2,y2,w2,h2) (!( ((x1)>=(x2)+(w2)) || ((x2)>=(x1)+(w1)) || \
 ((y1)>=(y2)+(h2)) || ((y2)>=(y1)+(h1)) ))


char tilemap[15][20] =
{
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
};

enum GAME_KEYS
{
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,
    KEY_X
};

int key[] = { 0, 0, 0, 0, 0 };

const int SCREEN_W = 640;
const int SCREEN_H = 480;
const int FULLSCREEN = 0;
const int SHOW_CURSOR = 1;
const float FPS = 60;

char titulo_ventana[] = "The legend of Zelda, allegro of time";

int juego_corriendo;
int redraw;


ALLEGRO_DISPLAY *ventana;
ALLEGRO_TIMER *temporizador;
ALLEGRO_EVENT_QUEUE *cola_de_eventos;

class Jugador
{
public:
    Jugador(float _x, float _y, int _w, int _h);
    ~Jugador();

    void Update();
    void Draw();
    bool PlaceFree(int _x, int _y);

    ALLEGRO_BITMAP *sprite;

    int w;
    int h;
    float x;
    float y;
    float vspeed;
    float gravity;
};

Jugador::Jugador(float _x, float _y, int _w, int _h)
{
    sprite = al_create_bitmap(_w, _h);

    al_set_target_bitmap(sprite);

    al_clear_to_color(al_map_rgb(255, 255, 255));

    al_set_target_bitmap(al_get_backbuffer(ventana));

    w = _w;
    h = _h;
    x = _x;
    y = _y;
    vspeed = 0;
    gravity = 0.5;
}

Jugador::~Jugador()
{
}

void Jugador::Update()
{
    int i, j;

    vspeed += gravity;
    y += vspeed;

    for (i=0; i<15; i++)
    {
        for (j=0; j<20; j++)
        {
            if (tilemap[i][j] == 1)
            {
                if (bbox_collision(x, y, w, h, j*32, i*32, 32, 32))
                {
                    if (vspeed < 0)
                        y = i*32 + 32;
                    else
                        y = i*32 - h;

                    vspeed = 0;
                }
            }
        }
    }

    if (key[KEY_LEFT] && PlaceFree(x-4, y))
        x -= 4;

    if (key[KEY_RIGHT] && PlaceFree(x+4, y))
        x += 4;

    if (key[KEY_X])
    {
        if (!PlaceFree(x, y+1))
            vspeed = -12;

        key[KEY_X] = 0;
    }
}

void Jugador::Draw()
{
    al_draw_bitmap(sprite, x, y, 0);
}

bool Jugador::PlaceFree(int _x, int _y)
{
    int i, j;

    for (i=0; i<15; i++)
    {
        for (j=0; j<20; j++)
        {
            if (tilemap[i][j] == 1 && bbox_collision(_x, _y, w, h, j*32, i*32, 32, 32))
                return false;
        }
    }

    return true;
}

Jugador *jugador;


//------------------------------------------------------------
// Funciones del juego
//------------------------------------------------------------


void game_setup()
{
    jugador = new Jugador(64, 10, 24, 24);
}

void game_update()
{
    jugador->Update(); 
}

void game_render()
{
    int i, j;

    for (i=0; i<15; i++)
    {
        for (j=0; j<20; j++)
        {
            if (tilemap[i][j] == 1)
                al_draw_filled_rectangle(j*32, i*32, j*32+32, i*32+32, al_map_rgb(80, 80, 80));
        }
    }

    jugador->Draw();
}

void game_shutdown()
{
    delete jugador;
}


//------------------------------------------------------------
// La función principal main()
//------------------------------------------------------------


int
main (int argc, char **argv)
{

    //ALLEGRO_DISPLAY *ventana = NULL;
    //ALLEGRO_EVENT_QUEUE *cola_de_eventos = NULL;
    //ALLEGRO_TIMER *temporizador =  NULL;

    if (!al_init())
    {
        fprintf(stderr, "No se ha podido inicar allegro.\n");
        exit(EXIT_FAILURE);
    }

    if (!al_init_image_addon())
    {
        fprintf(stderr, "No se ha creado el addon de imagenes.\n");
        exit(EXIT_FAILURE);
    }

    if (!al_init_primitives_addon())
    {
        fprintf(stderr, "No se ha creado la primitiva del addon de imagenes.\n");
        exit(EXIT_FAILURE);
    }

    temporizador = al_create_timer (1.0 / FPS);
    if (!temporizador)
    {
      fprintf (stderr, "No se ha podido crear un temporizador.");
      exit(EXIT_FAILURE);
  }

  if (!al_install_keyboard())
  {
    fprintf(stderr, "No hay teclado.\n");
    exit(EXIT_FAILURE);
}

if (!al_install_mouse())
{
    fprintf(stderr, "No hay raton.\n");
    exit(EXIT_FAILURE);
}

al_init();
al_init_image_addon();
al_init_primitives_addon();
al_install_keyboard();
al_install_mouse();

if (FULLSCREEN)
   al_set_new_display_flags(ALLEGRO_FULLSCREEN);

ventana = al_create_display(SCREEN_W, SCREEN_H);

if (!SHOW_CURSOR)
    al_hide_mouse_cursor(ventana);

al_set_window_title(ventana, titulo_ventana);

temporizador = al_create_timer(1.0 / FPS);

cola_de_eventos = al_create_event_queue();

game_setup();

al_register_event_source(cola_de_eventos, al_get_keyboard_event_source());
al_register_event_source(cola_de_eventos, al_get_mouse_event_source());
al_register_event_source(cola_de_eventos, al_get_display_event_source(ventana));
al_register_event_source(cola_de_eventos, al_get_timer_event_source(temporizador));

al_start_timer (temporizador);

juego_corriendo = 1;

ALLEGRO_EVENT event;

ALLEGRO_TIMEOUT cuenta_atras;
al_init_timeout(&cuenta_atras, 0.06);
bool hay_un_evento = false;

while(juego_corriendo)
{
    al_wait_for_event(cola_de_eventos, &event);

    hay_un_evento = al_wait_for_event_until(cola_de_eventos, &event, &cuenta_atras);

    if (hay_un_evento)
    {
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
    }
}

if (event.type == ALLEGRO_EVENT_KEY_DOWN)
{
    if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
        juego_corriendo = 0;

    if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
        key[KEY_LEFT] = 1;

    if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
        key[KEY_RIGHT] = 1;

    if (event.keyboard.keycode == ALLEGRO_KEY_UP)
        key[KEY_UP] = 1;

    if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
        key[KEY_DOWN] = 1;

    if (event.keyboard.keycode == ALLEGRO_KEY_X)
        key[KEY_X] = 1;
}

if (event.type == ALLEGRO_EVENT_KEY_UP)
{
    if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
        key[KEY_LEFT] = 0;

    if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
        key[KEY_RIGHT] = 0;

    if (event.keyboard.keycode == ALLEGRO_KEY_UP)
        key[KEY_UP] = 0;

    if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
        key[KEY_DOWN] = 0;

    if (event.keyboard.keycode == ALLEGRO_KEY_X)
        key[KEY_X] = 0;
}

if (event.type == ALLEGRO_EVENT_TIMER)
{
    game_update();
    redraw = 1;
}

if (redraw)
{
    redraw = 0;

    al_clear_to_color(al_map_rgb(0, 0, 0));

    game_render();

    al_flip_display();


    al_destroy_display(ventana);
    al_destroy_timer(temporizador);
    al_destroy_event_queue(cola_de_eventos);

    game_shutdown();

    return EXIT_SUCCESS;
}
}
