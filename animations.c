/*
 * Includes all animations for the RGB-LED-Cube
 *
 */

 #include "animations.h"
 #include "tlc5940.h"
 #include "system.h"
 #include "transformation.h"
 #include "uart.h"
 #include <avr/interrupt.h>
 #include <stdlib.h>

 #define FRAMECOUNT_VALUE_SLOW          50
 #define FRAMECOUNT_VALUE_MEDIUM        25
 #define FRAMECOUNT_VALUE_FAST          10
 #define FRAMECOUNT_VALUE_VERY_FAST     5

volatile uint8_t frameReady = 0;
volatile uint8_t frameCnt = 0;

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
uint8_t ledValue_Array1[4][4][4];
uint8_t ledValue_Array2[4][4][4];

volatile uint8_t *currentFrame = &ledValue_Array1[0][0][0];
volatile uint8_t *nextFrame = &ledValue_Array2[0][0][0];
//volatile uint8_t *

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

static void waitForNextFrame(uint8_t counter) {
	frameReady = 1;
	frameCnt = counter;

//	USART_puts("load next Frame...\t");
//	USART_puts("counter = ");
//	USART_putc(counter/10+48);
//	USART_putc(counter%10+48);
//	USART_putc('\n');

	while(frameReady){
	}
}

void everyLED(void)
{
    clearLEDCube();
    waitForNextFrame(FRAMECOUNT_VALUE_FAST);

    for(uint8_t lednumber = 0; lednumber < 64; lednumber++)
    {
        *(nextFrame+lednumber) = 1;
        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
        copyFrame();
    }
}

// Regenähnliche Animation
// Auf der obersten Ebene werden zufällige LEDs aktiviert, die sich dann nach unten bewegen
void rainfall(uint16_t frameCt){
    uint8_t activeLED = 0;
     uint8_t lednumber = 0;
	// Animation-Start
	clearLEDCube();
	waitForNextFrame(FRAMECOUNT_VALUE_VERY_FAST);

	// Animation-Main
	for(; frameCt>=1; frameCt--){
		shiftDownward();

		for(uint8_t ct = rand() % 2 + 1; ct>=1; ct--){
            do{
                lednumber = rand() % 16;
                for(uint8_t z = 0; z<4; z++){
                    if(*(nextFrame+lednumber+z*16) == 0){
                        activeLED = 0;
                    }
                    else{
                        activeLED = 1;
                        break;
                    }
                }
            }
            while(activeLED);
//            *(nextFrame+lednumber+TOP*16) = 1;
            *(nextFrame+lednumber+TOP*16) = 1;
		}
        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
	}

	// Animation-End
	for(uint8_t ct=0; ct<4; ct++){
		shiftDownward();
		waitForNextFrame(FRAMECOUNT_VALUE_FAST);
	}
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
            waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
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
//                waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
//            }
//
//            for(i = 63; i > 0; i--)
//            {
//                fillLEDCube(color[k], pwmtable_10[i]);
//                waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
//            }
//        }
//    }
}

void fillCubeDiagonal(uint8_t replays)
{
    //Animation-Start
	clearLEDCube();

    for(;replays > 0; replays--)
    {
        *(nextFrame+0+0*4+0*16) = 1;
        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

        shiftUpward();
        *(nextFrame+0+1*4+0*16) = 1;
        *(nextFrame+1+0*4+0*16) = 1;
        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

        shiftUpward();
        *(nextFrame+1+1*4+0*16) = 1;
        *(nextFrame+0+2*4+0*16) = 1;
        *(nextFrame+2+0*4+0*16) = 1;
        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

        shiftUpward();
        *(nextFrame+2+1*4+0*16) = 1;
        *(nextFrame+1+2*4+0*16) = 1;
        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

        shiftUpward();
        *(nextFrame+2+2*4+0*16) = 1;
        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

        shiftUpward();
        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

        shiftUpward();
        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

        clearLEDCube();
    }
}

void floatingZLayer(uint8_t replay)
{
    uint8_t i;
    //Animation-Start
	clearLEDCube();

    fillLayer(Z_LAYER,BOTTOM);
    waitForNextFrame(FRAMECOUNT_VALUE_FAST);

    for(;replay > 0; replay--)
    {
        for(i = 0; i < 3; i++)
        {
            shiftUpward();
            waitForNextFrame(FRAMECOUNT_VALUE_FAST);
        }

        for(i = 0; i < 3; i++)
        {
            shiftDownward();
            waitForNextFrame(FRAMECOUNT_VALUE_FAST);
        }
    }
}

void floatingYLayer(uint8_t replay)
{
    uint8_t i;
    //Animation-Start
	clearLEDCube();

    fillLayer(Y_LAYER,BACK);
    waitForNextFrame(FRAMECOUNT_VALUE_FAST);

    for(;replay > 0; replay--)
    {
        for(i = 0; i < 3; i++)
        {
            shiftForward();
            waitForNextFrame(FRAMECOUNT_VALUE_FAST);
        }

        for(i = 0; i < 3; i++)
        {
            shiftBackward();
            waitForNextFrame(FRAMECOUNT_VALUE_FAST);
        }
    }
}

void floatingXLayer(uint8_t replay)
{
    uint8_t i;
    //Animation-Start
	clearLEDCube();

    fillLayer(X_LAYER,RIGHT);
    waitForNextFrame(FRAMECOUNT_VALUE_FAST);

    for(;replay > 0; replay--)
    {
        for(i = 0; i < 3; i++)
        {
            shiftLeft();
            waitForNextFrame(FRAMECOUNT_VALUE_FAST);
        }

        for(i = 0; i < 3; i++)
        {
            shiftRight();
            waitForNextFrame(FRAMECOUNT_VALUE_FAST);
        }
    }
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
                if(*(nextFrame+x+y*4+z*16) != 0)
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
        *(nextFrame+x+y*4+TOP*16) = 1;
        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
    }

    /* Regentropfen auslaufen lassen */
    for(uint8_t i = 0; i < 4; i++)
    {
        moveLayerDown(1);
        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
    }
}

/*
 * Zufallsgenerator bestimmt LED die aktiviert werden soll
 */
void activateRandomLED(uint8_t replay)
{
    uint8_t ledNumber = 0;

    /* Intializes random number generator */
    srand(5);

    clearLEDCube();

    for(;replay > 0; replay--)
    {
        ledNumber = rand() % 64;

        *(nextFrame+ledNumber) = 1;
        waitForNextFrame(FRAMECOUNT_VALUE_VERY_FAST);
        copyFrame();
        *(nextFrame+ledNumber) = 0;
        waitForNextFrame(FRAMECOUNT_VALUE_VERY_FAST);
        copyFrame();
    }
}

void fillCube_randomly(uint8_t replay)
{
    uint8_t ledNumber = 0;

    /* Intializes random number generator */
    srand(9);

    for(;replay > 0; replay--)
    {
        clearLEDCube();
        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
        for(uint8_t i = 0; i < 64; i++)
        {
            copyFrame();
            ledNumber = rand() % 64;

            if(*(nextFrame+ledNumber) != 1)
            {
                *(nextFrame+ledNumber) = 1;
                waitForNextFrame(FRAMECOUNT_VALUE_FAST);
            }
            else
            {
                i--;
            }
        }
    }
}

void clearCube_randomly(uint8_t replay)
{
    uint8_t ledNumber = 0;

    /* Intializes random number generator */
    srand(5);

    for(;replay > 0; replay--)
    {
        fillLEDCube();
        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
        for(uint8_t i = 0; i < 64; i++)
        {
            copyFrame();
            ledNumber = rand() % 64;

            if(*(nextFrame+ledNumber) != 0)
            {
                *(nextFrame+ledNumber) = 0;
                waitForNextFrame(FRAMECOUNT_VALUE_FAST);
            }
            else
            {
                i--;
            }
        }
    }
}

// Zufälliges füllen und leeren des Cubes
void randomFill(uint16_t frameCt){
	clearLEDCube();
	waitForNextFrame(FRAMECOUNT_VALUE_FAST);

	for(; frameCt>=1; frameCt--){

		for(uint8_t mode=0; mode <=1; mode++){
			// mode=0: Cube füllen
			// mode=1: Cube leeren

			for(uint8_t ct=0; ct < 256; ct++){
				copyFrame();
                uint8_t xpos = rand() % 4;
                uint8_t ypos = rand() % 4;
                uint8_t zpos = rand() % 4;
                if (mode) {
                    *(nextFrame+xpos+ypos*4+zpos*16) = 0;
                }else{
                    *(nextFrame+xpos+ypos*4+zpos*16) = 1;
                }
				waitForNextFrame(FRAMECOUNT_VALUE_FAST);
			}
		}
	}
}

void dropLedTopDown(uint8_t replay)
{
    uint8_t ledNumber = 0;
    uint8_t lastLED = 0;
    uint8_t ledCounter = 0;

    /* Intializes random number generator */
    srand(32);

    clearLEDCube();

    /* Obere Ebene anschalten */
    fillLayer(Z_LAYER, TOP);
    waitForNextFrame(FRAMECOUNT_VALUE_FAST);
    copyFrame();

    /* LEDs wandern lassen */
    do
    {
        /* zufällige LED bestimmen die wandern soll */
        ledNumber = rand() % 16;

        if(ledNumber != lastLED)
        {
            /* Erst abwarten, bis genügend LEDs in die untere Ebene gewandert sind
             * Dann Zufall entscheiden lassen, ob LED hoch oder runter wandern soll
             */
            if((ledCounter > 9) && (rand() % 2))
            {
                /* Falls LED ganz unten ist, dann hochschieben */
                if(*(nextFrame+ledNumber+BOTTOM*16) != 0)
                {
                    ledCounter++;
                    for(uint8_t z = 0; z < 3; z++)
                    {
                        *(nextFrame+ledNumber+z*16) = 0;
                        *(nextFrame+ledNumber+(z+1)*16) = 1;
                        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
                        copyFrame();
                    }
                }
            }
            else
            {
                /* Falls LED ganz oben ist, dann runterfallen lassen */
                if(*(nextFrame+ledNumber+TOP*16) != 0)
                {
                    ledCounter++;
                    for(uint8_t z = TOP; z > 0; z--)
                    {
                        *(nextFrame+ledNumber+z*16) = 0;
                        *(nextFrame+ledNumber+(z-1)*16) = 1;
                        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
                        copyFrame();
                    }
                }
            }
        }
        lastLED = ledNumber;
    }
    while(ledCounter < replay + 5);
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
//            waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
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
//        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
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
//        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
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
//        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
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
//        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
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
//                    waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
//                    break;
//                }
//                waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
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
//            waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
//            k = 0;
//        }
//    }
//}

ISR(TIMER0_COMP_vect)
{
    static uint8_t layer = 4;

    //Update data for all columns
    for(uint8_t i = 0; i < 16; i++)
    {
        Tlc5940_set(ledChannel_Array[i].r,(*(currentFrame+i+layer*16) * color[RGB_COLOR_YELLOW].r * 255));
        Tlc5940_set(ledChannel_Array[i].g,(*(currentFrame+i+layer*16) * color[RGB_COLOR_YELLOW].g * 255));
        Tlc5940_set(ledChannel_Array[i].b,(*(currentFrame+i+layer*16) * color[RGB_COLOR_YELLOW].b * 255));

//        Tlc5940_set(ledChannel_Array[i].r,1024); // color[RGB_COLOR_RED].r
//        Tlc5940_set(ledChannel_Array[i].g,0); // color[RGB_COLOR_RED].g
//        Tlc5940_set(ledChannel_Array[i].b,0); // color[RGB_COLOR_RED].b
    }

    //shift new data into tlc
    Tlc5940_update();

    //Update layer
    if(layer == 0)
    {
        PORTC = (1 << LAYER_1_PIN);
    }
    else if(layer == 1)
    {
        PORTC = (1 << LAYER_2_PIN);
    }
    else if(layer == 2)
    {
        PORTC = (1 << LAYER_3_PIN);
    }
    else if(layer == 3)
    {
        PORTC = (1 << LAYER_4_PIN);
    }

    layer++;

    if(layer >= 4)
    {
        static uint8_t cntDown = 0;
        if((frameReady) && (cntDown == 0)) {
            volatile uint8_t *ptr = currentFrame;
            currentFrame = nextFrame;
            nextFrame = ptr;

            frameReady = 0;
            cntDown = frameCnt;
        }
        layer = 0;

        if(cntDown > 0) {
            cntDown--;
        }
    }
}
