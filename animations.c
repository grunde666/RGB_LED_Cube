/*
 * Includes all animations for the RGB-LED-Cube
 *
 */

 #include "animations.h"
 #include "tlc5940.h"
 #include "system.h"
 #include "transformation.h"
 #include <avr/interrupt.h>
 #include <stdlib.h>

static volatile uint8_t tlc_data_ready = 0;

enum color_type
{
    RGB_COLOR_RED,
    RGB_COLOR_YELLOW,
    RGB_COLOR_GREEN,
    RGB_COLOR_CYAN,
    RGB_COLOR_BLUE,
    RGB_COLOR_MAGENTA,
    RGB_COLOR_WHITE
};

//struct rgbLed ledValue_Array[4][4][4];
uint8_t ledValue_Array[4][4][4];

struct rgbLed color_new;

const struct rgbLed color[7] =
{
    {1,0,0}, {1,1,0}, {0,1,0}, {0,1,1},
    {0,0,1}, {1,0,1}, {1,1,1}
};

const struct rgbLed ledChannel_Array[16] =
{
    {5,8,2}, {10,9,11}, {40,44,39}, {36,35,37},
    {3,0,1}, {16,13,17}, {38,24,25}, {47,46,45},
    {6,4,7}, {20,18,19}, {30,31,29}, {41,43,42},
    {14,12,15}, {23,22,21}, {28,26,27}, {34,33,32}
};

const uint16_t pwmtable_10[64] =
{
    0, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 8, 9, 10,
    11, 12, 13, 15, 17, 19, 21, 23, 26, 29, 32, 36, 40, 44, 49, 55,
    61, 68, 76, 85, 94, 105, 117, 131, 146, 162, 181, 202, 225, 250,
    279, 311, 346, 386, 430, 479, 534, 595, 663, 739, 824, 918, 1023
};

static void waitForFrame(uint16_t maxNumber)
{
    uint8_t i = 0;
    uint16_t frameCnt = 0;
    static uint8_t layer = 0;
    frameCnt = maxNumber;

    while(frameCnt)
    {
        //new data ready for tlc
        if(tlc_data_ready)
        {
            PORTC = 0;
            //Update data for all columns
            for(i = 0; i < 16; i++)
            {
                Tlc5940_set(ledChannel_Array[i].r,(ledValue_Array[i%4][i/4][layer] * color_new.r)); // color[RGB_COLOR_RED].r
                Tlc5940_set(ledChannel_Array[i].g,(ledValue_Array[i%4][i/4][layer] * color_new.g)); // color[RGB_COLOR_RED].g
                Tlc5940_set(ledChannel_Array[i].b,(ledValue_Array[i%4][i/4][layer] * color_new.b)); // color[RGB_COLOR_RED].b
            }
            //wait until data is shifted in tlc
            while(Tlc5940_update());

            //Update layer
            if(layer == 0)
            {
                PORTC |= (1 << LAYER_1_PIN);
            }
            else if(layer == 1)
            {
                PORTC |= (1 << LAYER_2_PIN);
            }
            else if(layer == 2)
            {
                PORTC |= (1 << LAYER_3_PIN);
            }
            else if(layer == 3)
            {
                PORTC |= (1 << LAYER_4_PIN);
            }

            layer++;

            if(layer >= 4)
            {
                layer = 0;

                frameCnt--;
            }
            tlc_data_ready = 0;
        }
    }
}

void everyLED(void)
{
    for(uint8_t z = 0; z < 4; z++)
    {
        for(uint8_t y = 0; y < 4; y++)
        {
            for(uint8_t x = 0; x < 4; x++)
            {
                ledValue_Array[x][y][z] = 1;
                waitForFrame(5);
            }
        }
    }
    clearLEDCube();
}

void blinkingCube(uint8_t replays)
{
    uint8_t k = 0;

    fillLEDCube();

    color_new.r = 1023;
    color_new.g = 0;
    color_new.b = 0;

    while(k < 6)
    {
        for(uint8_t i = 0; i < 64; i++)
        {
            switch(k)
            {
                case 0:
                    color_new.g = pwmtable_10[i];
                    break;
                case 1:
                    color_new.r = pwmtable_10[63-i];
                    break;
                case 2:
                    color_new.b = pwmtable_10[i];
                    break;
                case 3:
                    color_new.g = pwmtable_10[63-i];
                    break;
                case 4:
                    color_new.r = pwmtable_10[i];
                    break;
                case 5:
                    color_new.b = pwmtable_10[63-i];
                    break;
            }
            waitForFrame(5);
        }
        k++;
    }
//    uint8_t i,k;
//    for(;replays > 0; replays--)
//    {
//        for(k = 0; k < 6; k++)
//        {
//            for(i = 0; i < 64; i++)
//            {
//                fillLEDCube(color[k], pwmtable_10[i]);
//                waitForFrame(4);
//            }
//
//            for(i = 63; i > 0; i--)
//            {
//                fillLEDCube(color[k], pwmtable_10[i]);
//                waitForFrame(4);
//            }
//        }
//    }
}

void fillCubeDiagonal(uint8_t replays)
{
    for(;replays > 0; replays--)
    {
        ledValue_Array[0][0][0] = 1;
        waitForFrame(40);

        moveLayerUp(0);
        ledValue_Array[0][1][0] = 1;
        ledValue_Array[1][0][0] = 1;
        waitForFrame(40);

        moveLayerUp(0);
        ledValue_Array[1][1][0] = 1;
        ledValue_Array[0][2][0] = 1;
        ledValue_Array[2][0][0] = 1;
        waitForFrame(40);

        moveLayerUp(0);
        ledValue_Array[2][1][0] = 1;
        ledValue_Array[1][2][0] = 1;
        waitForFrame(40);

        moveLayerUp(0);
        ledValue_Array[2][2][0] = 1;
        waitForFrame(40);

        moveLayerUp(0);
        waitForFrame(40);

        moveLayerUp(0);
        waitForFrame(40);

        clearLEDCube();
    }
}

void floatingZLayer(uint8_t replay)
{
    uint8_t i;
    fillLayer(Z_LAYER,BOTTOM);
    waitForFrame(5);

    for(;replay > 0; replay--)
    {
        for(i = 0; i < 3; i++)
        {
            moveLayerUp(1);
            waitForFrame(5);
        }

        for(i = 0; i < 3; i++)
        {
            moveLayerDown(1);
            waitForFrame(5);
        }
    }

    clearLayer(Z_LAYER, BOTTOM);
}

void floatingYLayer(uint8_t replay)
{
    uint8_t i;
    fillLayer(Y_LAYER,BACK);
    waitForFrame(5);

    for(;replay > 0; replay--)
    {
        for(i = 0; i < 3; i++)
        {
            moveLayerForward(1);
            waitForFrame(5);
        }

        for(i = 0; i < 3; i++)
        {
            moveLayerBackward(1);
            waitForFrame(5);
        }
    }

    clearLayer(Y_LAYER, BACK);
}

void floatingXLayer(uint8_t replay)
{
    uint8_t i;
    fillLayer(X_LAYER,RIGHT);
    waitForFrame(5);

    for(;replay > 0; replay--)
    {
        for(i = 0; i < 3; i++)
        {
            moveLayerLeft(1);
            waitForFrame(5);
        }

        for(i = 0; i < 3; i++)
        {
            moveLayerRight(1);
            waitForFrame(5);
        }
    }

    clearLayer(X_LAYER, RIGHT);
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
            ledNumber = rand() % 16;
            y = ledNumber / 4;     // Bestimmen der y-Ebene
            x = ledNumber % 4;     // Bestimmen der x-Ebene

            /* Prüfen, ob LED-Säule schon in einer Ebene aktiv ist
             * Falls ja, dann erneut Zufallsgenerator starten
             */
            for(uint8_t z = 0; z < 4; z++)
            {
                if(ledValue_Array[x][y][z] != 0)
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
        ledValue_Array[x][y][TOP] = 1;
        waitForFrame(15);
    }

    /* Regentropfen auslaufen lassen */
    for(uint8_t i = 0; i < 4; i++)
    {
        moveLayerDown(1);
        waitForFrame(15);
    }
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
        ledNumber = rand() % 64;
        z = ledNumber / 16;         // Bestimmen der z-Ebene (unten, mitte, oben)
        y = (ledNumber % 16) / 4;     // Bestimmen der y-Ebene
        x = (ledNumber % 16) % 4;     // Bestimmen der x-Ebene

        ledValue_Array[x][y][z] = 1;
        waitForFrame(5);
        ledValue_Array[x][y][z] = 0;
        waitForFrame(5);
    }
}

void fillCube_randomly(uint8_t replay)
{
    uint8_t x,y,z;
    uint8_t refresh_cnt = 0;
    uint8_t ledNumber = 0;

    /* Intializes random number generator */
    srand(9);

    for(;replay > 0; replay--)
    {
        for(uint8_t i = 0; i < 64; i++)
        {
            ledNumber = rand() % 64;
            z = ledNumber / 16;         // Bestimmen der z-Ebene
            y = (ledNumber % 16) / 4;     // Bestimmen der y-Ebene
            x = (ledNumber % 16) % 4;     // Bestimmen der x-Ebene

            if(ledValue_Array[x][y][z] != 1)
            {
                ledValue_Array[x][y][z] = 1;
                refresh_cnt = 0;
                waitForFrame(5);
            }
            else
            {
                i--;
                refresh_cnt++;
                if(refresh_cnt > 10)
                {
                    refresh_cnt = 0;
                    waitForFrame(1);
                }
            }
        }
        clearLEDCube();
    }
}

void clearCube_randomly(uint8_t replay)
{
    uint8_t x,y,z;
    uint8_t refresh_cnt = 0;
    uint8_t ledNumber = 0;

    /* Intializes random number generator */
    srand(5);

    for(;replay > 0; replay--)
    {
        fillLEDCube();
        waitForFrame(5);
        for(uint8_t i = 0; i < 64; i++)
        {
            ledNumber = rand() % 64;
            z = ledNumber / 16;         // Bestimmen der z-Ebene (unten, mitte, oben)
            y = ledNumber % 16 / 4;     // Bestimmen der y-Ebene
            x = ledNumber % 16 % 4;     // Bestimmen der x-Ebene

            if(ledValue_Array[x][y][z] != 0)
            {
                ledValue_Array[x][y][z] = 0;
                refresh_cnt = 0;
                waitForFrame(5);
            }
            else
            {
                i--;
                refresh_cnt++;
                if(refresh_cnt > 10)
                {
                    refresh_cnt = 0;
                    waitForFrame(1);
                }
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
    fillLayer(Z_LAYER, TOP);
    waitForFrame(5);

    /* LEDs wandern lassen */
    do
    {
        /* zufällige LED bestimmen die wandern soll */
        ledNumber = rand() % 16;
        y = ledNumber / 4;     // Bestimmen der y-Ebene
        x = ledNumber % 4;     // Bestimmen der x-Ebene

        if(ledNumber != lastLED)
        {
            /* Erst abwarten, bis genügend LEDs in die untere Ebene gewandert sind
             * Dann Zufall entscheiden lassen, ob LED hoch oder runter wandern soll
             */
            if((ledCounter > 9) && (rand() % 2))
            {
                /* Falls LED ganz unten ist, dann hochschieben */
                if(ledValue_Array[x][y][BOTTOM] != 0)
                {
                    ledCounter++;
                    for(z = 0; z < 3; z++)
                    {
                        ledValue_Array[x][y][z] = 0;
                        ledValue_Array[x][y][z+1] = 1;
                        waitForFrame(5);
                    }
                }
            }
            else
            {
                /* Falls LED ganz oben ist, dann runterfallen lassen */
                if(ledValue_Array[x][y][TOP] != 0)
                {
                    ledCounter++;
                    for(z = TOP; z > 0; z--)
                    {
                        ledValue_Array[x][y][z] = 0;
                        ledValue_Array[x][y][z-1] = 1;
                        waitForFrame(5);
                    }
                }
            }
        }
        lastLED = ledNumber;
    }
    while(ledCounter < replay + 5);

    clearLEDCube();
}

//void explosion(uint8_t replay)
//{
//    for(;replay > 0; replay--)
//    {
//        for(uint8_t i = 0; i < 5; i++)
//        {
//            copyLayer(Z_LAYER,MITTE,TOP);
//            copyLayer(Z_LAYER,MITTE,BOTTOM);
//            clearLayer(Z_LAYER, MITTE);
//
//            switch(i)
//            {
//                case 0:
//                    ledValue_Array[x][y][z](1,1,1,1);
//                    break;
//                case 1:
//                    ledValue_Array[x][y][z](1,0,1,1);
//                    ledValue_Array[x][y][z](1,2,1,1);
//                    ledValue_Array[x][y][z](2,1,1,1);
//                    ledValue_Array[x][y][z](0,1,1,1);
//                    break;
//                case 2:
//                    ledValue_Array[x][y][z](0,0,1,1);
//                    ledValue_Array[x][y][z](2,0,1,1);
//                    ledValue_Array[x][y][z](0,2,1,1);
//                    ledValue_Array[x][y][z](2,2,1,1);
//                    break;
//                default:
//                    break;
//            }
//            waitForFrame(50);
//        }
//    }
//    clearLEDCube();
//}

//void rotateLayer(uint8_t replay)
//{
//    replay *= 2;
//
//    for(; replay > 0; replay--)
//    {
//        for(uint8_t z = 0; z < 4; z++)
//        {
//            for(uint8_t x = 0; x < 4; x++)
//            {
//               ledValue_Array[x][y][z](x,1,z,1);
//            }
//        }
//        waitForFrame(30);
//
//        for(uint8_t z = 0; z < 3; z++)
//        {
//            for(uint8_t y = 0; y < 3; y++)
//            {
//                if(y == 0)
//                {
//                    ledValue_Array[x][y][z](0,y,z,1);
//                }
//                else if(y == 1)
//                {
//                    ledValue_Array[x][y][z](0,y,z,0);
//                    ledValue_Array[x][y][z](2,y,z,0);
//                }
//                else if(y == 2)
//                {
//                    ledValue_Array[x][y][z](2,y,z,1);
//                }
//            }
//        }
//        waitForFrame(30);
//
//        for(uint8_t z = 0; z < 3; z++)
//        {
//            for(uint8_t y = 0; y < 3; y++)
//            {
//                if(y == 0)
//                {
//                    ledValue_Array[x][y][z](0,y,z,0);
//                    ledValue_Array[x][y][z](1,y,z,1);
//                }
//                else if(y == 2)
//                {
//                    ledValue_Array[x][y][z](2,y,z,0);
//                    ledValue_Array[x][y][z](1,y,z,1);
//                }
//            }
//        }
//        waitForFrame(30);
//
//        for(uint8_t z = 0; z < 3; z++)
//        {
//            for(uint8_t y = 0; y < 3; y++)
//            {
//                if(y == 0)
//                {
//                    ledValue_Array[x][y][z](1,y,z,0);
//                    ledValue_Array[x][y][z](2,y,z,1);
//                }
//                else if(y == 2)
//                {
//                    ledValue_Array[x][y][z](1,y,z,0);
//                    ledValue_Array[x][y][z](0,y,z,1);
//                }
//            }
//        }
//        waitForFrame(30);
//
//        clearLEDCube();
//    }
//}

//void cubeFraming(uint8_t replay)
//{
//    uint8_t k = 0;
//    uint8_t x_off = 0;
//    uint8_t y_off = 0;
//
//    for(; replay > 0; replay--)
//    {
//        for(uint8_t j = 0; j < 4; j++)
//        {
//            if(j == 0)
//            {
//                x_off = 0;
//                y_off = 0;
//            }
//            else if(j == 1)
//            {
//                x_off = 2;
//                y_off = 0;
//            }
//            else if(j == 2)
//            {
//                x_off = 2;
//                y_off = 2;
//            }
//            else if(j == 3)
//            {
//                x_off = 0;
//                y_off = 2;
//            }
//
//            for(uint8_t i = 0; i < 5; i++)
//            {
//                clearLEDCube();
//                switch(k)
//                {
//                /* Erste LED in einer Ecke aktivieren */
//                case 0:
//                    ledValue_Array[x][y][z](x_off,y_off,0,1);
//                    break;
//                /* kleinen Würfel mit 4 LEDs aktivieren */
//                case 1:
//                    for(uint8_t y = (0 + y_off / 2); y < (2 + y_off / 2); y++)
//                    {
//                        for(uint8_t x = (0 + x_off / 2); x < (2 + x_off / 2); x++)
//                        {
//                            ledValue_Array[x][y][z](x,y,0,1);
//                        }
//                    }
//                    copyLayer(Z_LAYER,BOTTOM,MITTE);
//                    break;
//                /* Würfelkanten aktivieren */
//                case 2:
//                    for(uint8_t y = 0; y < 3; y++)
//                    {
//                        for(uint8_t x = 0; x < 3; x++)
//                        {
//                            ledValue_Array[x][y][z](x,y,0,1);
//                        }
//                    }
//                    ledValue_Array[x][y][z](1,1,0,0);
//                    copyLayer(Z_LAYER,BOTTOM,TOP);
//                    for(uint8_t y = 0; y < 3; y++)
//                    {
//                        for(uint8_t x = 0; x < 3; x++)
//                        {
//                            ledValue_Array[x][y][z](x,y,1,1);
//                            x++;
//                        }
//                        y++;
//                    }
//                    waitForFrame(100);
//                    break;
//                }
//                waitForFrame(40);
//
//                /* Wenn Hälfte der Animation abgearbeitet, dann Rückwärts ablaufen */
//                if(i >= 2)
//                {
//                    k--;
//                }
//                else
//                {
//                    k++;
//                }
//            }
//            clearLEDCube();
//            waitForFrame(60);
//            k = 0;
//        }
//    }
//}

ISR(TIMER0_COMP_vect)
{
    tlc_data_ready = 1;
}
