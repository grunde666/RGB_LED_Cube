/*
 * Includes all animations for the RGB-LED-Cube
 *
 */

 #include "animations.h"
 #include "definitions.h"
 #include "tlc5940.h"
 #include "system.h"
 #include "transformation.h"
 #include "uart.h"
 #include <avr/interrupt.h>
 #include <stdlib.h>


/*
 * Timer0 updates tlc5940 led driver every 512 µs --> display whole frame takes 2.048 ms
 * Update function needs led array with pwm value of every color part (rgb)
 * Color can be set by user via remote control
 *
 */

 #define FRAMECOUNT_VALUE_SLOW          50
 #define FRAMECOUNT_VALUE_MEDIUM        25
 #define FRAMECOUNT_VALUE_FAST          10
 #define FRAMECOUNT_VALUE_VERY_FAST     5

volatile uint8_t frameReady = 0;
volatile uint8_t frameCnt = 0;

struct rgbLed ledValue_Array1[4][4][4];
struct rgbLed ledValue_Array2[4][4][4];

volatile struct rgbLed *currentFrame = &ledValue_Array1[0][0][0];
volatile struct rgbLed *nextFrame = &ledValue_Array2[0][0][0];

static uint8_t animationState = 0;

uint8_t globalColor = RGB_COLOR_RED;
uint8_t globalDimmingLevel = 64;

const struct rgbLed color[8] =
{
    {0,0,0}, {0,0,1}, {0,1,0}, {0,1,1},
    {1,0,0}, {1,0,1}, {1,1,0}, {1,1,1}
};

// struct of led pining
const struct rgbLed ledChannel_Array[16] =
{
    {5,8,2}, {10,9,11}, {40,44,39}, {36,35,37},
    {3,0,1}, {16,13,17}, {38,24,25}, {47,46,45},
    {6,4,7}, {20,18,19}, {30,31,29}, {41,43,42},
    {14,12,15}, {23,22,21}, {28,26,27}, {34,33,32}
};

// 10 Bit pwm table with 64 steps
const uint16_t pwmtable_10[64] =
{
    0, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 8, 9, 10,
    11, 12, 13, 15, 17, 19, 21, 23, 26, 29, 32, 36, 40, 44, 49, 55,
    61, 68, 76, 85, 94, 105, 117, 131, 146, 162, 181, 202, 225, 250,
    279, 311, 346, 386, 430, 479, 534, 595, 663, 739, 824, 918, 1023
};

// 12 Bit pwm table with 128 steps
const uint16_t pwmtable_12[128] =
{
    0,2,2,2,2,2,2,2,2,3,3,3,3,3,3,4,4,4,4,5,5,5,6,6,6,7,7,8,8,9,9,
    10,11,11,12,13,14,15,16,17,18,19,20,21,23,24,26,27,29,31,33,35,
    37,40,42,45,48,51,54,58,62,66,70,74,79,84,90,95,102,108,115,123,
    131,139,148,158,168,179,190,202,216,229,244,260,277,295,314,334,
    356,379,403,429,457,487,518,552,587,625,666,709,754,803,855,910,
    969,1032,1099,1170,1245,1326,1412,1503,1600,1704,1814,1931,2056,
    2189,2330,2481,2641,2812,2994,3187,3393,3613,3846,4095
};


static void waitForNextFrame(uint8_t counter) {
	frameReady = 1;
	frameCnt = counter;

#ifdef DEBUG
	USART_puts("load next Frame...\t");
	USART_puts("counter = ");
	USART_putc(counter/10+48);
	USART_putc(counter%10+48);
	USART_putc('\n');
#endif
}

uint8_t getRGBLedColor(volatile struct rgbLed *led)
{
    uint8_t res = 0;

    if((led->r == 0) && (led->g == 0) && (led->b == 0)) {
        res = RGB_COLOR_BLACK;
    }
    else if((led->r == 0) && (led->g == 0) && (led->b != 0)) {
        res = RGB_COLOR_BLUE;
    }
    else if((led->r == 0) && (led->g != 0) && (led->b == 0)) {
        res = RGB_COLOR_GREEN;
    }
    else if((led->r == 0) && (led->g != 0) && (led->b != 0)) {
        res = RGB_COLOR_CYAN;
    }
    else if((led->r != 0) && (led->g == 0) && (led->b == 0)) {
        res = RGB_COLOR_RED;
    }
    else if((led->r != 0) && (led->g == 0) && (led->b != 0)) {
        res = RGB_COLOR_MAGENTA;
    }
    else if((led->r != 0) && (led->g != 0) && (led->b == 0)) {
        res = RGB_COLOR_YELLOW;
    }
    else if((led->r != 0) && (led->g != 0) && (led->b != 0)) {
        res = RGB_COLOR_WHITE;
    }

    return res;
}

// Activates every led one by one
// TODO: Implement different colors and dimming step
void everyLED(void)
{
    static uint8_t lednumber = 0;   //counter for active leds
    static uint8_t localColor = 0;
    static uint8_t localDimmingLevel = 0;

    // Check if new frame is displayed
    if (frameReady == 0)
    {
        switch (animationState)
        {
        case 0:
        //First step: clear whole cube
            clearLEDCube();
            animationState = 1;
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;
            break;
        case 1:
        //Second step: copy current frame and add one led
            copyFrame();
            if(lednumber < 64) {
                //Check if color/dimming level has changed and rebuild cube with new color/dimming level
                if((globalColor != localColor) || (globalDimmingLevel != localDimmingLevel)) {

                    localColor = globalColor;
                    localDimmingLevel = globalDimmingLevel;

                    for(uint8_t i = 0; i < lednumber; i++) {
                        setRGBLed((nextFrame + i), localColor, localDimmingLevel);
                    }
                }
                else {
                    setRGBLed((nextFrame + lednumber), localColor, localDimmingLevel);
                }

                lednumber++;

                //If all leds are active, end animation
                if (lednumber == 64) {
                    animationState = 0;
                    lednumber = 0;
                }
            }
            break;
        }
        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
    }
}

// Regenähnliche Animation
// Auf der obersten Ebene werden zufällige LEDs aktiviert, die sich dann nach unten bewegen
void rainfall(uint16_t frameCt){
    uint8_t activeLED = 0;
    uint8_t lednumber = 0;
    static uint8_t counter = 0;
    static uint8_t localColor = 0;
    static uint8_t localDimmingLevel = 0;

    if(frameReady == 0) {
        switch(animationState) {
        case 0:
            clearLEDCube();
            counter = frameCt;
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;
            animationState = 1;
            break;
        case 1:
            shiftDownward();
            for(uint8_t ct = rand() % 2 + 1; ct >= 1; ct--){
                do{
                    lednumber = rand() % 16;
                    for(uint8_t z = 0; z < 4; z++){
                        if(getRGBLedColor(nextFrame+lednumber+z*16) == RGB_COLOR_BLACK){
                            activeLED = 0;
                        }
                        else{
                            activeLED = 1;
                            break;
                        }
                    }
                }
                while(activeLED);

                setRGBLed((nextFrame+lednumber+TOP*16), localColor, localDimmingLevel);
            }

            if(counter == 0) {
                animationState = 2;
            }
            else {
                counter--;
            }
            break;
        case 2:
        case 3:
        case 4:
            shiftDownward();
            animationState++;
            break;
        case 5:
            shiftDownward();
            animationState = 0;
            break;
        }

        if((localColor != globalColor) || (localDimmingLevel != globalDimmingLevel)) {
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;

            for(uint8_t i = 0; i < 64; i++) {
                if(getRGBLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                    setRGBLed((nextFrame + i), localColor, localDimmingLevel);
                }
            }
        }

        waitForNextFrame(FRAMECOUNT_VALUE_VERY_FAST);
    }
}

void blinkingCube(uint8_t replays)
{
//    uint8_t k = 0;
//
//    fillLEDCube();
//
//    while(k < 6)
//    {
//        for(uint8_t i = 0; i < 64; i++)
//        {
//            switch(k)
//            {
//                case 0:
//                    color_new.g = pwmtable_10[i];
//                    break;
//                case 1:
//                    color_new.r = pwmtable_10[63-i];
//                    break;
//                case 2:
//                    color_new.b = pwmtable_10[i];
//                    break;
//                case 3:
//                    color_new.g = pwmtable_10[63-i];
//                    break;
//                case 4:
//                    color_new.r = pwmtable_10[i];
//                    break;
//                case 5:
//                    color_new.b = pwmtable_10[63-i];
//                    break;
//            }
//            waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
//        }
//        k++;
//    }
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
    static uint8_t localColor = 0;
    static uint8_t localDimmingLevel = 0;
    static uint8_t counter = 0;

    if(frameReady == 0) {

        if(animationState == 0) {
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;
            counter = replays;
            animationState = 1;
        }

        switch(animationState) {
        case 1:
            clearLEDCube();
            setRGBLed((nextFrame+0+0*4+0*16), localColor, localDimmingLevel);
            animationState = 2;
            break;
        case 2:
            shiftUpward();
            setRGBLed((nextFrame+0+1*4+0*16), localColor, localDimmingLevel);
            setRGBLed((nextFrame+0+1*4+0*16), localColor, localDimmingLevel);
            animationState = 3;
            break;
        case 3:
            shiftUpward();
            setRGBLed((nextFrame+1+1*4+0*16), localColor, localDimmingLevel);
            setRGBLed((nextFrame+0+2*4+0*16), localColor, localDimmingLevel);
            setRGBLed((nextFrame+2+0*4+0*16), localColor, localDimmingLevel);
            animationState = 4;
            break;
        case 4:
            shiftUpward();
            setRGBLed((nextFrame+2+1*4+0*16), localColor, localDimmingLevel);
            setRGBLed((nextFrame+1+2*4+0*16), localColor, localDimmingLevel);
            animationState = 5;
            break;
        case 5:
            shiftUpward();
            setRGBLed((nextFrame+2+2*4+0*16), localColor, localDimmingLevel);
            animationState = 6;
            break;
        case 6:
            shiftUpward();
            animationState = 7;
            break;
        case 7:
            shiftUpward();

            if(counter == 0) {
                animationState = 0;
            }
            else {
                counter--;
                animationState = 1;
            }
            break;
        }

        if((localColor != globalColor) || (localDimmingLevel != globalDimmingLevel)) {
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;

            for(uint8_t i = 0; i < 64; i++) {
                if(getRGBLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                    setRGBLed((nextFrame + i), localColor, localDimmingLevel);
                }
            }
        }

        waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);
    }
}

void floatingZLayer(uint8_t replay)
{
    uint8_t i;
    //Animation-Start
	clearLEDCube();

    fillLayer(Z_LAYER,BOTTOM,RGB_COLOR_RED, 2);
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

    fillLayer(Y_LAYER,BACK,RGB_COLOR_RED, 2);
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
    uint8_t ledNumber = 0;
    static uint8_t localColor = 0;
    static uint8_t localDimmingLevel = 0;
    static uint8_t counter = 0;

    if(frameReady == 0) {
        if(animationState == 0) {
            /* Intializes random number generator */
            srand(5);
            counter = replay;
            clearLEDCube();
            animationState = 1;
        }

        switch(animationState) {
        case 1:
            ledNumber = rand() % 64;
            setRGBLed((nextFrame+ledNumber), localColor, localDimmingLevel);
            animationState = 2;
            break;
        case 2:
            copyFrame();
            setRGBLed((nextFrame+ledNumber), RGB_COLOR_BLACK, localDimmingLevel);
            animationState = 3;
            break;
        case 3:
            copyFrame();
            if(counter == 0) {
                animationState = 0;
            }
            else {
                counter--;
                animationState = 1;
            }
            break;
        }

        if((localColor != globalColor) || (localDimmingLevel != globalDimmingLevel)) {
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;

            for(uint8_t i = 0; i < 64; i++) {
                if(getRGBLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                    setRGBLed((nextFrame + i), localColor, localDimmingLevel);
                }
            }
        }

        waitForNextFrame(FRAMECOUNT_VALUE_VERY_FAST);
    }

    uint8_t i;
    //Animation-Start
	clearLEDCube();

    fillLayer(X_LAYER,RIGHT,RGB_COLOR_RED, 2);
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
    uint8_t activeLED = 1;
    static uint8_t counter = 0;
    static uint8_t localColor = 0;
    static uint8_t localDimmingLevel = 0;

    if(frameReady == 0) {

        if(animationState == 0) {
            /* Intializes random number generator */
            srand(20);
            counter = replay;
            animationState = 1;
        }

        switch(animationState) {
        case 1:
            /* Solange eine LED-Säule raussuchen bis eine gefunden wurde,
             * die nicht aktiv ist
             */
            do
            {
                uint8_t ledNumber = 0;

                ledNumber = rand() % 16;
                y = ledNumber / 4;     // Bestimmen der y-Ebene
                x = ledNumber % 4;     // Bestimmen der x-Ebene

                /* Prüfen, ob LED-Säule schon in einer Ebene aktiv ist
                 * Falls ja, dann erneut Zufallsgenerator starten
                 */
                for(uint8_t z = 0; z < 4; z++)
                {
                    if(getRGBLedColor(nextFrame+x+y*4+z*16) != RGB_COLOR_BLACK)
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
            setRGBLed((nextFrame+x+y*4+TOP*16), localColor, localDimmingLevel);

            if(counter == 0) {
                animationState = 2;
            }
            else {
                counter--;
            }
            break;
        case 2:
        case 3:
        case 4:
            /* Regentropfen auslaufen lassen */
            moveLayerDown(1);
            animationState++;
        case 5:
            moveLayerDown(1);
            animationState = 0;
            break;
        }

        if((localColor != globalColor) || (localDimmingLevel != globalDimmingLevel)) {
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;

            for(uint8_t i = 0; i < 64; i++) {
                if(getRGBLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                    setRGBLed((nextFrame + i), localColor, localDimmingLevel);
                }
            }
        }

        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
    }
}

/*
 * Zufallsgenerator bestimmt LED die aktiviert werden soll
 */
void activateRandomLED(uint8_t replay)
{
    uint8_t ledNumber = 0;
    static uint8_t localColor = 0;
    static uint8_t localDimmingLevel = 0;
    static uint8_t counter = 0;

    if(frameReady == 0) {
        if(animationState == 0) {
            /* Intializes random number generator */
            srand(5);
            counter = replay;
            clearLEDCube();
            animationState = 1;
        }

        switch(animationState) {
        case 1:
            ledNumber = rand() % 64;
            setRGBLed((nextFrame+ledNumber), localColor, localDimmingLevel);
            animationState = 2;
            break;
        case 2:
            copyFrame();
            setRGBLed((nextFrame+ledNumber), RGB_COLOR_BLACK, localDimmingLevel);
            animationState = 3;
            break;
        case 3:
            copyFrame();
            if(counter == 0) {
                animationState = 0;
            }
            else {
                counter--;
                animationState = 1;
            }
            break;
        }

        if((localColor != globalColor) || (localDimmingLevel != globalDimmingLevel)) {
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;

            for(uint8_t i = 0; i < 64; i++) {
                if(getRGBLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                    setRGBLed((nextFrame + i), localColor, localDimmingLevel);
                }
            }
        }

        waitForNextFrame(FRAMECOUNT_VALUE_VERY_FAST);
    }
}

void fillCube_randomly(uint8_t replay)
{
    uint8_t ledNumber = 0;
    static uint8_t activeLEDs = 0;
    static uint8_t localColor = 0;
    static uint8_t localDimmingLevel = 0;
    static uint8_t counter = 0;

    if(frameReady == 0) {
        if(animationState == 0) {
            /* Intializes random number generator */
            srand(9);
            counter = replay;
            animationState = 1;
        }

        switch(animationState) {
        case 1:
            clearLEDCube();
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;
            animationState = 2;
            break;
        case 2:
            do{
                ledNumber = rand() % 64;

                if(getRGBLedColor(nextFrame+ledNumber) == RGB_COLOR_BLACK)
                {
                    copyFrame();
                    setRGBLed((nextFrame+ledNumber), localColor, localDimmingLevel);
                    activeLEDs++;
                    if(activeLEDs == 64) {
                        animationState = 3;
                    }
                    break;
                }
            }while(1);
            break;
        case 3:
            if(counter == 0) {
                animationState = 0;
            }
            else {
                counter--;
                animationState = 1;
            }
            break;
        }

        if((localColor != globalColor) || (localDimmingLevel != globalDimmingLevel)) {
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;

            for(uint8_t i = 0; i < 64; i++) {
                if(getRGBLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                    setRGBLed((nextFrame + i), localColor, localDimmingLevel);
                }
            }
        }

        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
    }
}

void clearCube_randomly(uint8_t replay)
{
    uint8_t ledNumber = 0;
    static uint8_t activeLEDs = 0;
    static uint8_t localColor = 0;
    static uint8_t localDimmingLevel = 0;
    static uint8_t counter = 0;

    if(frameReady == 0) {
        if(animationState == 0) {
            /* Intializes random number generator */
            srand(5);
            counter = replay;
            animationState = 1;
        }

        switch(animationState) {
        case 1:
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;
            fillLEDCube(localColor, localDimmingLevel);
            animationState = 2;
            break;
        case 2:
            do{
                ledNumber = rand() % 64;

                if(getRGBLedColor(nextFrame+ledNumber) != RGB_COLOR_BLACK)
                {
                    copyFrame();
                    setRGBLed((nextFrame+ledNumber), RGB_COLOR_BLACK, 0);
                    activeLEDs++;
                    if(activeLEDs == 64) {
                        animationState = 3;
                    }
                    break;
                }
            }while(1);
            break;
        case 3:
            if(counter == 0) {
                animationState = 0;
            }
            else {
                counter--;
                animationState = 1;
            }
            break;
        }

        if((localColor != globalColor) || (localDimmingLevel != globalDimmingLevel)) {
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;

            for(uint8_t i = 0; i < 64; i++) {
                if(getRGBLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                    setRGBLed((nextFrame + i), localColor, localDimmingLevel);
                }
            }
        }

        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
    }
}

void dropLedTopDown(uint8_t replay)
{
    uint8_t ledNumber = 0;
    static uint8_t localColor = 0;
    static uint8_t localDimmingLevel = 0;

    static uint8_t lastLED = 0;
    static uint8_t ledCounter = 0;

    if(frameReady == 0) {
        if(animationState == 0) {
            /* Intializes random number generator */
            srand(32);
            ledCounter = replay * 3;
            animationState = 1;
        }

        switch(animationState) {
        case 1:
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;
            clearLEDCube();
            /* Obere Ebene anschalten */
            fillLayer(Z_LAYER,TOP,localColor,localDimmingLevel);
            animationState = 2;
            break;
        case 2:
            copyFrame();
            do{
                /* zufällige LED bestimmen die wandern soll */
                ledNumber = rand() % 16;

               if(ledNumber != lastLED)
                {
                    /* Erst abwarten, bis genügend LEDs in die untere Ebene gewandert sind
                     * Bevor LEDs von unten nach oben wandern
                     * Zufall entscheiden lassen, ob LED hoch oder runter wandern soll
                     */
                    if((ledCounter > 9) && (rand() % 2))
                    {
                        /* Falls LED ganz unten ist, dann hochschieben */
                        if(getRGBLedColor(nextFrame+ledNumber+BOTTOM*16) != RGB_COLOR_BLACK)
                        {
                            ledCounter--;
                            setRGBLed((nextFrame+ledNumber+BOTTOM*16), RGB_COLOR_BLACK, 0);
                            setRGBLed((nextFrame+ledNumber+(BOTTOM+1)*16), localColor, localDimmingLevel);
                            animationState = 3;
                        }
                    }
                    else
                    {
                        /* Falls LED ganz oben ist, dann runterfallen lassen */
                        if(getRGBLedColor(nextFrame+ledNumber+TOP*16) != RGB_COLOR_BLACK)
                        {
                            ledCounter--;
                            setRGBLed((nextFrame+ledNumber+TOP*16), RGB_COLOR_BLACK, 0);
                            setRGBLed((nextFrame+ledNumber+(TOP-1)*16), localColor, localDimmingLevel);
                            animationState = 5;
                        }
                    }
                }
                lastLED = ledNumber;
            }while(1);
            break;
        case 3:
            copyFrame();
            setRGBLed((nextFrame+ledNumber+(BOTTOM+1)*16), RGB_COLOR_BLACK, 0);
            setRGBLed((nextFrame+ledNumber+(BOTTOM+2)*16), localColor, localDimmingLevel);
            animationState = 4;
            break;
        case 4:
            copyFrame();
            setRGBLed((nextFrame+ledNumber+(BOTTOM+2)*16), RGB_COLOR_BLACK, 0);
            setRGBLed((nextFrame+ledNumber+(BOTTOM+3)*16), localColor, localDimmingLevel);

            if(ledCounter == 0) {
                animationState = 2;
            }
            else {
                animationState = 0;
            }
            break;
        case 5:
            copyFrame();
            setRGBLed((nextFrame+ledNumber+(TOP-1)*16), RGB_COLOR_BLACK, 0);
            setRGBLed((nextFrame+ledNumber+(TOP-2)*16), localColor, localDimmingLevel);
            animationState = 6;
            break;
        case 6:
            copyFrame();
            setRGBLed((nextFrame+ledNumber+(TOP-2)*16), RGB_COLOR_BLACK, 0);
            setRGBLed((nextFrame+ledNumber+(TOP-3)*16), localColor, localDimmingLevel);

            if(ledCounter == 0) {
                animationState = 2;
            }
            else {
                animationState = 0;
            }
            break;
        }

        if((localColor != globalColor) || (localDimmingLevel != globalDimmingLevel)) {
            localColor = globalColor;
            localDimmingLevel = globalDimmingLevel;

            for(uint8_t i = 0; i < 64; i++) {
                if(getRGBLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                    setRGBLed((nextFrame + i), localColor, localDimmingLevel);
                }
            }
        }

        waitForNextFrame(FRAMECOUNT_VALUE_FAST);
    }
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
        Tlc5940_set(ledChannel_Array[i].r,((currentFrame+i+layer*16)->r * 255));
        Tlc5940_set(ledChannel_Array[i].g,((currentFrame+i+layer*16)->g * 255));
        Tlc5940_set(ledChannel_Array[i].b,((currentFrame+i+layer*16)->b * 255));

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
        //Check if time for actual frame is exceeded and load next frame
        static uint8_t cntDown = 0;
        if((frameReady) && (cntDown == 0)) {
            volatile struct rgbLed *ptr = currentFrame;
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
