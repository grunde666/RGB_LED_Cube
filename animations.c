#include "animations.h"

/*
 *  y-Achse
 *
 *  2   O   O   O
 *      |   |   |
 *  1   O   O   O
 *      |   |   |
 *  0   O   O   O
 *
 *      2   1   0   x-Achse
 *
 * z-Achse unten 0, mitte 1, oben 2
 */


/* Ausstehende Animationen
 * Regen - fertig
 * Zufällige Aktivierung einer LED - fertig
 * Zufällige Befüllung des Würfels LED-weise - fertig
 * rotierende Ebene auf z-Achse
 * rotierende Ebene auf x-Achse
 * Explosion innen nach außen
 * Zufällig wandernde LEDs von oben nach unten und zurück - fertig
 * Außenkanten von Quader zeichnen
 * Ebenen von unten nach oben und wieder runter füllen - fertig
 * Ebenen von hinten nach vorne und zurück füllen - fertig
 * Ganzen Cube an und ausschalten - fertig
 * Regen - fertig
 * Zufällige Aktivierung einer LED - fertig
 * Zufällige Befüllung des Würfels LED-weise - fertig
 * rotierende Ebene auf z-Achse - fertig
 * Würfelrahmen aufbau - fertig
 * Explosion innen nach außen - fertig
 * Zufällig wandernde LEDs von oben nach unten und zurück - fertig
 * LED-Ebene in z-Richtung auf und ab - fertig
 * LED-Ebene in y-Richtung auf und ab - fertig
 * LED-Ebene in x-Richtung auf und ab - fertig
 * LED-Würfel diagonal befüllen - fertig
 * blinkender Würfel - fertig
 *
 */

uint8_t cube_array[3][3] = {0};

extern volatile uint8_t waitFlag;
extern volatile uint16_t frameCntMax;

void waitForFrame(uint16_t maxNumber)
{
    waitFlag = 1;

    frameCntMax = maxNumber;

    while(waitFlag)
    {
        ;
    }
}

void blinkingCube(uint8_t replays)
{
    for(;replays > 0; replays--)
    {
        fillLEDCube();
        waitForFrame(30);
        clearLEDCube();
        waitForFrame(30);
    }
}

void fillCubeDiagonal(uint8_t replays)
{
    for(;replays > 0; replays--)
    {
        set_pixelStatus(0,0,0,1);
        waitForFrame(40);

        moveLayerUp(0);
        set_pixelStatus(0,1,0,1);
        set_pixelStatus(1,0,0,1);
        waitForFrame(40);

        moveLayerUp(0);
        set_pixelStatus(1,1,0,1);
        set_pixelStatus(0,2,0,1);
        set_pixelStatus(2,0,0,1);
        waitForFrame(40);

        moveLayerUp(0);
        set_pixelStatus(2,1,0,1);
        set_pixelStatus(1,2,0,1);
        waitForFrame(40);

        moveLayerUp(0);
        set_pixelStatus(2,2,0,1);
        waitForFrame(40);

        moveLayerUp(0);
        waitForFrame(40);

        moveLayerUp(0);
        waitForFrame(40);

        clearLEDCube();
    }
}

void set_pixelStatus(uint8_t x, uint8_t y, uint8_t z, uint8_t status)
{
    if(status)
    {
        cube_array[y][z] |= (1 << x);
    }
    else
    {
        cube_array[y][z] &= ~(1 << x);
    }

}

uint8_t get_pixelStatus(uint8_t x, uint8_t y, uint8_t z)
{
    uint8_t status = 0;

    if(cube_array[y][z] & (1 << x))
    {
        status = 1;
    }
    else
    {
        status = 0;
    }

    return status;
}

void floatingZLayer(uint8_t replay)
{
    fillLayer(Z_LAYER,UNTEN);
    waitForFrame(40);

    for(;replay > 0; replay--)
    {
        moveLayerUp(1);
        waitForFrame(40);

        moveLayerUp(1);
        waitForFrame(40);

        moveLayerDown(1);
        waitForFrame(40);

        moveLayerDown(1);
        waitForFrame(40);
    }

    clearLayer(Z_LAYER, UNTEN);
}

void floatingYLayer(uint8_t replay)
{
    fillLayer(Y_LAYER,HINTEN);
    waitForFrame(40);

    for(;replay > 0; replay--)
    {
        moveLayerForward(1);
        waitForFrame(40);

        moveLayerForward(1);
        waitForFrame(40);

        moveLayerBackward(1);
        waitForFrame(40);

        moveLayerBackward(1);
        waitForFrame(40);
    }

    clearLayer(Y_LAYER, HINTEN);
}

void floatingXLayer(uint8_t replay)
{
    fillLayer(X_LAYER,RECHTS);
    waitForFrame(40);

    for(;replay > 0; replay--)
    {
        moveLayerLeft(1);
        waitForFrame(40);

        moveLayerLeft(1);
        waitForFrame(40);

        moveLayerRight(1);
        waitForFrame(40);

        moveLayerRight(1);
        waitForFrame(40);
    }

    clearLayer(X_LAYER, RECHTS);
}

void rain(uint8_t replay)
{
    uint8_t x,y;
    uint8_t ledNumber = 0;
    uint8_t activeLED = 1;

    /* Intializes random number generator */
    srand(20);

    for(;replay > 0; replay--)
    {
        /* Solange eine LED-Säule raussuchen bis eine gefunden wurde,
         * die nicht aktiv ist
         */
        do
        {
            ledNumber = rand() % 9;
            y = ledNumber / 3;     // Bestimmen der y-Ebene
            x = ledNumber % 3;     // Bestimmen der x-Ebene

            /* Prüfen, ob LED-Säule schon in einer Ebene aktiv ist
             * Falls ja, dann erneut Zufallsgenerator starten
             */
            for(uint8_t z = 0; z < 3; z++)
            {
                if(get_pixelStatus(x,y,z) != 0)
                {
                    activeLED = 1;
                    break;
                }
                else
                {
                    activeLED = 0;
                }
            }
        }
        while(activeLED == 1);

        /* Zuerst vorherige Ebenen verschieben */
        moveLayerDown(1);
        /* Neue LED in oberster Ebene aktivieren */
        set_pixelStatus(x,y,2,1);
        waitForFrame(50);
    }

    /* Regentropfen auslaufen lassen */
    moveLayerDown(1);
    waitForFrame(50);
    moveLayerDown(1);
    waitForFrame(50);
    moveLayerDown(1);
    waitForFrame(10);
}

/*
 * Zufallsgenerator bestimmt LED die aktiviert werden soll
 */
void activateRandomLED(uint8_t replay)
{
    uint8_t x,y,z;
    uint8_t ledNumber = 0;

    /* Intializes random number generator */
    srand(5);

    for(;replay > 0; replay--)
    {
        ledNumber = rand() % 27;
        z = ledNumber / 9;         // Bestimmen der z-Ebene (unten, mitte, oben)
        y = ledNumber % 9 / 3;     // Bestimmen der y-Ebene
        x = ledNumber % 9 % 3;     // Bestimmen der x-Ebene

        set_pixelStatus(x,y,z,1);
        waitForFrame(30);
        set_pixelStatus(x,y,z,0);
        waitForFrame(30);
    }
}

void fillCube_randomly(uint8_t replay)
{
    uint8_t x,y,z;
    uint8_t ledNumber = 0;

    /* Intializes random number generator */
    srand(9);

    for(;replay > 0; replay--)
    {
        for(uint8_t i = 0; i < 27; i++)
        {
            ledNumber = rand() % 27;
            z = ledNumber / 9;         // Bestimmen der z-Ebene (unten, mitte, oben)
            y = ledNumber % 9 / 3;     // Bestimmen der y-Ebene
            x = ledNumber % 9 % 3;     // Bestimmen der x-Ebene

            if(get_pixelStatus(x,y,z) != 1)
            {
                set_pixelStatus(x,y,z,1);
                if(i < 26)
                {
                    waitForFrame(20);
                }
                else
                {
                    waitForFrame(40);
                }
            }
            else
            {
                i--;
            }
        }
        clearLEDCube();
    }
}

void clearCube_randomly(uint8_t replay)
{
    uint8_t x,y,z;
    uint8_t ledNumber = 0;

    /* Intializes random number generator */
    srand(5);

    for(;replay > 0; replay--)
    {
        fillLEDCube();
        waitForFrame(20);
        for(uint8_t i = 0; i < 27; i++)
        {
            ledNumber = rand() % 27;
            z = ledNumber / 9;         // Bestimmen der z-Ebene (unten, mitte, oben)
            y = ledNumber % 9 / 3;     // Bestimmen der y-Ebene
            x = ledNumber % 9 % 3;     // Bestimmen der x-Ebene

            if(get_pixelStatus(x,y,z) != 0)
            {
                set_pixelStatus(x,y,z,0);
                waitForFrame(20);
            }
            else
            {
                i--;
            }
        }
    }
}

void dropLedTopDown(uint8_t replay)
{
    uint8_t x,y,z;
    uint8_t ledNumber = 0;
    uint8_t lastLED = 0;
    uint8_t ledCounter = 0;

    /* Intializes random number generator */
    srand(32);

    /* Obere Ebene anschalten */
    fillLayer(Z_LAYER, OBEN);
    waitForFrame(20);

    /* LEDs wandern lassen */
    do
    {
        /* zufällige LED bestimmen die wandern soll */
        ledNumber = rand() % 9;
        y = ledNumber / 3;     // Bestimmen der y-Ebene
        x = ledNumber % 3;     // Bestimmen der x-Ebene

        if(ledNumber != lastLED)
        {
            /* Erst abwarten, bis genügend LEDs in die untere Ebene gewandert sind
             * Dann Zufall entscheiden lassen, ob LED hoch oder runter wandern soll
             */
            if((ledCounter > 5) && (rand() % 2))
            {
                /* Falls LED ganz unten ist, dann hochschieben */
                if(get_pixelStatus(x,y,0) != 0)
                {
                    ledCounter++;
                    for(z = 0; z < 2; z++)
                    {
                        set_pixelStatus(x,y,z,0);
                        set_pixelStatus(x,y,z+1,1);
                        waitForFrame(25);
                    }
                }
            }
            else
            {
                /* Falls LED ganz oben ist, dann runterfallen lassen */
                if(get_pixelStatus(x,y,2) != 0)
                {
                    ledCounter++;
                    for(z = 2; z > 0; z--)
                    {
                        set_pixelStatus(x,y,z,0);
                        set_pixelStatus(x,y,z-1,1);
                        waitForFrame(25);
                    }
                }
            }
        }
        lastLED = ledNumber;
    }
    while(ledCounter < replay + 5);

    clearLEDCube();
}

void explosion(uint8_t replay)
{
    for(;replay > 0; replay--)
    {
        for(uint8_t i = 0; i < 5; i++)
        {
            copyLayer(Z_LAYER,MITTE,OBEN);
            copyLayer(Z_LAYER,MITTE,UNTEN);
            clearLayer(Z_LAYER, MITTE);

            switch(i)
            {
                case 0:
                    set_pixelStatus(1,1,1,1);
                    break;
                case 1:
                    set_pixelStatus(1,0,1,1);
                    set_pixelStatus(1,2,1,1);
                    set_pixelStatus(2,1,1,1);
                    set_pixelStatus(0,1,1,1);
                    break;
                case 2:
                    set_pixelStatus(0,0,1,1);
                    set_pixelStatus(2,0,1,1);
                    set_pixelStatus(0,2,1,1);
                    set_pixelStatus(2,2,1,1);
                    break;
                default:
                    break;
            }
            waitForFrame(50);
        }
    }
    clearLEDCube();
}

void rotateLayer(uint8_t replay)
{
    replay *= 2;

    for(; replay > 0; replay--)
    {
        for(uint8_t z = 0; z < 3; z++)
        {
            for(uint8_t x = 0; x < 3; x++)
            {
               set_pixelStatus(x,1,z,1);
            }
        }
        waitForFrame(30);

        for(uint8_t z = 0; z < 3; z++)
        {
            for(uint8_t y = 0; y < 3; y++)
            {
                if(y == 0)
                {
                    set_pixelStatus(0,y,z,1);
                }
                else if(y == 1)
                {
                    set_pixelStatus(0,y,z,0);
                    set_pixelStatus(2,y,z,0);
                }
                else if(y == 2)
                {
                    set_pixelStatus(2,y,z,1);
                }
            }
        }
        waitForFrame(30);

        for(uint8_t z = 0; z < 3; z++)
        {
            for(uint8_t y = 0; y < 3; y++)
            {
                if(y == 0)
                {
                    set_pixelStatus(0,y,z,0);
                    set_pixelStatus(1,y,z,1);
                }
                else if(y == 2)
                {
                    set_pixelStatus(2,y,z,0);
                    set_pixelStatus(1,y,z,1);
                }
            }
        }
        waitForFrame(30);

        for(uint8_t z = 0; z < 3; z++)
        {
            for(uint8_t y = 0; y < 3; y++)
            {
                if(y == 0)
                {
                    set_pixelStatus(1,y,z,0);
                    set_pixelStatus(2,y,z,1);
                }
                else if(y == 2)
                {
                    set_pixelStatus(1,y,z,0);
                    set_pixelStatus(0,y,z,1);
                }
            }
        }
        waitForFrame(30);

        clearLEDCube();
    }
}

void cubeFraming(uint8_t replay)
{
    uint8_t k = 0;
    uint8_t x_off = 0;
    uint8_t y_off = 0;

    for(; replay > 0; replay--)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            if(j == 0)
            {
                x_off = 0;
                y_off = 0;
            }
            else if(j == 1)
            {
                x_off = 2;
                y_off = 0;
            }
            else if(j == 2)
            {
                x_off = 2;
                y_off = 2;
            }
            else if(j == 3)
            {
                x_off = 0;
                y_off = 2;
            }

            for(uint8_t i = 0; i < 5; i++)
            {
                clearLEDCube();
                switch(k)
                {
                /* Erste LED in einer Ecke aktivieren */
                case 0:
                    set_pixelStatus(x_off,y_off,0,1);
                    break;
                /* kleinen Würfel mit 4 LEDs aktivieren */
                case 1:
                    for(uint8_t y = (0 + y_off / 2); y < (2 + y_off / 2); y++)
                    {
                        for(uint8_t x = (0 + x_off / 2); x < (2 + x_off / 2); x++)
                        {
                            set_pixelStatus(x,y,0,1);
                        }
                    }
                    copyLayer(Z_LAYER,UNTEN,MITTE);
                    break;
                /* Würfelkanten aktivieren */
                case 2:
                    for(uint8_t y = 0; y < 3; y++)
                    {
                        for(uint8_t x = 0; x < 3; x++)
                        {
                            set_pixelStatus(x,y,0,1);
                        }
                    }
                    set_pixelStatus(1,1,0,0);
                    copyLayer(Z_LAYER,UNTEN,OBEN);
                    for(uint8_t y = 0; y < 3; y++)
                    {
                        for(uint8_t x = 0; x < 3; x++)
                        {
                            set_pixelStatus(x,y,1,1);
                            x++;
                        }
                        y++;
                    }
                    waitForFrame(100);
                    break;
                }
                waitForFrame(40);

                /* Wenn Hälfte der Animation abgearbeitet, dann Rückwärts ablaufen */
                if(i >= 2)
                {
                    k--;
                }
                else
                {
                    k++;
                }
            }
            clearLEDCube();
            waitForFrame(60);
            k = 0;
        }
    }
}

/* Schiebt LEDs eine Ebene nach rechts
 * LEDs in der rechtesten Ebene werden überschrieben und die linkeste Ebene wird nach verschieben gelöscht
 */
void moveLayerRight(uint8_t clearLEDs)
{
    copyLayer(X_LAYER,MITTE,RECHTS);
    copyLayer(X_LAYER,LINKS,MITTE);

    if(clearLEDs)
    {
        clearLayer(X_LAYER,LINKS);
    }
}

/* Schiebt LEDs eine Ebene nach links
 * LEDs in der linkesten Ebene werden überschrieben und die rechteste Ebene wird nach verschieben gelöscht
 */
void moveLayerLeft(uint8_t clearLEDs)
{
    copyLayer(X_LAYER,MITTE,LINKS);
    copyLayer(X_LAYER,RECHTS,MITTE);

    if(clearLEDs)
    {
        clearLayer(X_LAYER,RECHTS);
    }
}

/* Schiebt LEDs eine Ebene nach vorn
 * LEDs in der vordersten Ebene werden überschrieben und die hinterste Ebene wird nach verschieben gelöscht
 */
void moveLayerForward(uint8_t clearLEDs)
{
    copyLayer(Y_LAYER,MITTE,VORNE);
    copyLayer(Y_LAYER,HINTEN,MITTE);

    if(clearLEDs)
    {
        clearLayer(Y_LAYER,HINTEN);
    }
}

/* Schiebt LEDs eine Ebene nach hinten
 * LEDs in der hintersten Ebene werden überschrieben und die vorderste Ebene wird nach verschieben gelöscht
 */
void moveLayerBackward(uint8_t clearLEDs)
{
    copyLayer(Y_LAYER,MITTE,HINTEN);
    copyLayer(Y_LAYER,VORNE,MITTE);

    if(clearLEDs)
    {
        clearLayer(Y_LAYER,VORNE);
    }
}

/* Schiebt LEDs eine Ebene tiefer
 * LEDs in der untersten Ebene werden überschrieben und die oberste Ebene wird nach verschieben gelöscht
 */
void moveLayerDown(uint8_t clearLEDs)
{
    copyLayer(Z_LAYER,MITTE,UNTEN);
    copyLayer(Z_LAYER,OBEN,MITTE);

    if(clearLEDs)
    {
        clearLayer(Z_LAYER,OBEN);
    }
}

/* Schiebt LEDs eine Ebene höher
 * LEDs in der obersten Ebene werden überschrieben und die unterste Ebene wird nach verschieben gelöscvoid moveLayerForward(uint8_t clearLEDs)ht
 */
void moveLayerUp(uint8_t clearLEDs)
{
    copyLayer(Z_LAYER,MITTE,OBEN);
    copyLayer(Z_LAYER,UNTEN,MITTE);

    if(clearLEDs)
    {
        clearLayer(Z_LAYER,UNTEN);
    }
}

void clearLEDCube(void)
{
    for(uint8_t z = 0; z < 3; z++)
    {
        for(uint8_t y = 0; y < 3; y++)
        {
            for(uint8_t x = 0; x < 3; x++)
            {
                set_pixelStatus(x,y,z,0);
            }
        }
    }
}

void fillLEDCube(void)
{
    for(uint8_t z = 0; z < 3; z++)
    {
        for(uint8_t y = 0; y < 3; y++)
        {
            for(uint8_t x = 0; x < 3; x++)
            {
                set_pixelStatus(x,y,z,1);
            }
        }
    }
}

void copyLayer(uint8_t layerType, uint8_t originLayer, uint8_t destinationLayer)
{
    if(layerType == X_LAYER)
    {
        for(uint8_t z = 0; z < 3; z++)
        {
            for(uint8_t y = 0; y < 3; y++)
            {
                set_pixelStatus(destinationLayer,y,z,get_pixelStatus(originLayer,y,z));
            }
        }
    }
    else if(layerType == Y_LAYER)
    {
        for(uint8_t z = 0; z < 3; z++)
        {
            for(uint8_t x = 0; x < 3; x++)
            {
                set_pixelStatus(x,destinationLayer,z,get_pixelStatus(x,originLayer,z));
            }
        }
    }
    else if(layerType == Z_LAYER)
    {
        for(uint8_t y = 0; y < 3; y++)
        {
            for(uint8_t x = 0; x < 3; x++)
            {
                set_pixelStatus(x,y,destinationLayer,get_pixelStatus(x,y,originLayer));
            }
        }
    }
}

void fillLayer(uint8_t layerType, uint8_t layerNumber)
{
    if(layerType == X_LAYER)
    {
        // x-Ebene wird gefüllt
        for(uint8_t z = 0; z < 3; z++)
        {
            for(uint8_t y = 0; y < 3; y++)
            {
                set_pixelStatus(layerNumber,y,z,1);
            }
        }
    }
    else if(layerType == Y_LAYER)
    {
        // y-Ebene wird gefüllt
        for(uint8_t z = 0; z < 3; z++)
        {
            for(uint8_t x = 0; x < 3; x++)
            {
                set_pixelStatus(x,layerNumber,z,1);
            }
        }
    }
    else if(layerType == Z_LAYER)
    {
        //z-Ebene wird gefüllt
        for(uint8_t y = 0; y < 3; y++)
        {
            for(uint8_t x = 0; x < 3; x++)
            {
                set_pixelStatus(x,y,layerNumber,1);
            }
        }
    }

}

void clearLayer(uint8_t layerType, uint8_t layerNumber)
{
    if(layerType == X_LAYER)
    {
        // x-Ebene wird geleert
        for(uint8_t z = 0; z < 3; z++)
        {
            for(uint8_t y = 0; y < 3; y++)
            {
                set_pixelStatus(layerNumber,y,z,0);
            }
        }
    }
    else if(layerType == Y_LAYER)
    {
        // y-Ebene wird geleert
        for(uint8_t z = 0; z < 3; z++)
        {
            for(uint8_t x = 0; x < 3; x++)
            {
                set_pixelStatus(x,layerNumber,z,0);
            }
        }
    }
    else if(layerType == Z_LAYER)
    {
        //z-Ebene wird geleert
        for(uint8_t y = 0; y < 3; y++)
        {
            for(uint8_t x = 0; x < 3; x++)
            {
                set_pixelStatus(x,y,layerNumber,0);
            }
        }
    }
}
