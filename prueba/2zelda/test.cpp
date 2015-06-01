    /*********************************************************
     *
     *  main.cpp
     *
     *  Código fuente para juego de plataformas básico
     *  Autor: Juan Orozco (EliasYFGM)
     *
     *********************************************************/
     
    #include <allegro5/allegro.h>
    #include <allegro5/allegro_image.h>
    #include <allegro5/allegro_primitives.h>
     

    #define SPRITESHEET "Custom Edited - Legend of Zelda Series - Link.png" 
    // "bbox_collision" para detectar colisiones entre dos espacios
    #define bbox_collision(x1,y1,w1,h1,x2,y2,w2,h2) (!( ((x1)>=(x2)+(w2)) || ((x2)>=(x1)+(w1)) || \
                                                        ((y1)>=(y2)+(h2)) || ((y2)>=(y1)+(h1)) ))
     
    // El mapa del juego
    // Cada tile ocupa un espacio de 32x32 píxeles
    char tilemap[15][20] =
    {
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1 },
        { 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1 },
        { 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    };
     
    // Las teclas que usaremos en el juego
    enum GAME_KEYS
    {
        KEY_LEFT,
        KEY_RIGHT,
        KEY_UP,
        KEY_DOWN,
        KEY_X
    };
     
    // El 'array' para checar qué teclas están siendo presionadas
    int key[] = { 0, 0, 0, 0, 0 };
     
    // Ancho y alto de la pantalla
    const int SCREEN_W = 640;
    const int SCREEN_H = 480;
     
    // Si usaremos pantalla completa (1) o no (0)
    const int FULLSCREEN = 0;
     
    // Si queremos mostrar el cursor (1) o no (0)
    const int SHOW_CURSOR = 1;
     
    // Cuadros por segundo (FPS)
    const float FRAMERATE = 60;
     
    // Título de la ventana
    char WINDOW_TITLE[] = "Allegro 5 application";
     
    // Variables del juego
    int game_is_running;
    int redraw;
     
    // Variables de Allegro
    ALLEGRO_DISPLAY     *display;
    ALLEGRO_TIMER       *timer;
    ALLEGRO_EVENT_QUEUE *event_queue;
     
    // La clase principal del jugador
    class Player
    {
        public:
            Player(float _x, float _y, int _w, int _h);
            ~Player();
     
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
     
    Player::Player(float _x, float _y, int _w, int _h)
    {
        sprite = al_create_bitmap(_w, _h);
     
        al_set_target_bitmap(sprite);
     
        al_clear_to_color(al_map_rgb(255, 255, 255));
     
        al_set_target_bitmap(al_get_backbuffer(display));
     
        w = _w;
        h = _h;
        x = _x;
        y = _y;
        vspeed = 0;
        gravity = 0.5;
    }
     
    Player::~Player()
    {
    }
     
    void Player::Update()
    {
        int i, j;
     
        // Gravedad
        vspeed += gravity;
        y += vspeed;
     
        // Checamos el mapa para saber si el jugador ha tocado un tile sólido (1)
        // al caer o al saltar.
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
     
        // Movemos el jugador con la tecla <Izquierda>
        if (key[KEY_LEFT] && PlaceFree(x-4, y))
            x -= 4;
     
        // Movemos el jugador con la tecla <Derecha>
        if (key[KEY_RIGHT] && PlaceFree(x+4, y))
            x += 4;
     
        // El jugador salta con la tecla <X> mientras esté en el piso
        if (key[KEY_X])
        {
            if (!PlaceFree(x, y+1))
                vspeed = -12;
     
            key[KEY_X] = 0;
        }
    }
     
    void Player::Draw()
    {
        // Se dibuja el jugador
        al_draw_bitmap(sprite, x, y, 0);
    }
     
    bool Player::PlaceFree(int _x, int _y)
    {
        int i, j;
     
        // Checamos si no hay colisión en la posición especificada
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
     
    // El jugador
    Player *player;
     
    //------------------------------------------------------------
    // Funciones del juego
    //------------------------------------------------------------
    void game_setup()
    {
        // Se crea el jugador (24x24)
        player = new Player(64, 10, 24, 35);
    }
     
    void game_update()
    {
        // Actualizamos el jugador
        player->Update();
    }
     
    void game_render()
    {
        int i, j;
     
        // Dibujamos el mapa
        for (i=0; i<15; i++)
        {
            for (j=0; j<20; j++)
            {
                if (tilemap[i][j] == 1)
                    al_draw_filled_rectangle(j*32, i*32, j*32+32, i*32+32, al_map_rgb(80, 200, 80));
            }
        }
     
        // Se dibuja el jugador
        player->Draw();
    }
     
    void game_shutdown()
    {
        delete player;
    }
     
    //------------------------------------------------------------
    // La función principal main()
    //------------------------------------------------------------
    int main(int argc, char **argv)
    {
        // Iniciar Allegro (y añadidos)
        al_init();
        al_init_image_addon();
        al_init_primitives_addon();
     
        // Instalar el mouse, teclado, etc.
        al_install_keyboard();
        al_install_mouse();
     
        if (FULLSCREEN)
            al_set_new_display_flags(ALLEGRO_FULLSCREEN);
     
        // Crear el "display"
        display = al_create_display(SCREEN_W, SCREEN_H);
     
        if (!SHOW_CURSOR)
            al_hide_mouse_cursor(display);
     
        // Poner el título de la ventana
        al_set_window_title(display, WINDOW_TITLE);
     
        // Creamos el timer (controlador de FPS)
        timer = al_create_timer(1.0 / FRAMERATE);
     
        // Creamos la 'pila' de eventos
        event_queue = al_create_event_queue();
     
        // Preparamos el juego
        game_setup();
     
        // Los eventos que usaremos
        al_register_event_source(event_queue, al_get_keyboard_event_source());
        al_register_event_source(event_queue, al_get_mouse_event_source());
        al_register_event_source(event_queue, al_get_display_event_source(display));
        al_register_event_source(event_queue, al_get_timer_event_source(timer));
     
        al_start_timer(timer);
     
        // Si esta variable se pone a 0, el juego terminará de inmediato
        game_is_running = 1;
     
        ALLEGRO_EVENT event;
     
        // El 'loop' principal del juego
        while (game_is_running)
        {
            al_wait_for_event(event_queue, &event);
     
            // Si el botón para cerrar fue presionado...
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                break;
     
            // Actualizamos las teclas
            if (event.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                // Al presionar <Esc> el juego terminará
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    game_is_running = 0;
     
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
     
            // Actualizamos las teclas
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
            }
        }
     
        al_destroy_display(display);
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
     
        game_shutdown();
     
        return 0;
    }
