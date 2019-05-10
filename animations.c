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
 * Timer0 updates tlc5940 led driver every 512 us --> display whole frame takes 2.048 ms
 * Update function needs led array with pwm value of every color part (rgb)
 * Color can be set by user via remote control
 *
 */

#define FRAMECOUNT_VALUE_VERY_SLOW     80
#define FRAMECOUNT_VALUE_SLOW          40
#define FRAMECOUNT_VALUE_MEDIUM        20
#define FRAMECOUNT_VALUE_FAST          10
#define FRAMECOUNT_VALUE_VERY_FAST     5

volatile uint8_t frameReady = 0;
volatile uint8_t frameCnt = 0;

struct rgbLed ledValue_Array1[LED_ROWS_NUMBER][LED_COLUMNS_NUMBER][LED_LAYERS_NUMBER];
struct rgbLed ledValue_Array2[LED_ROWS_NUMBER][LED_COLUMNS_NUMBER][LED_LAYERS_NUMBER];

struct hsv globalHSV;
static struct hsv localHSV;

static uint8_t counter = 0;

volatile struct rgbLed *currentFrame = &ledValue_Array1[0][0][0];
volatile struct rgbLed *nextFrame = &ledValue_Array2[0][0][0];

static uint8_t animationState = 0;

const struct hsv color_table[COLOR_MAX_NUMBER] =
{
    {0,255,255}, {21,255,255}, {42,255,255}, {192,255,255},
    {171,255,255}, {0,0,255}, {85,255,255}, {128,255,255},
    {213,255,255}, {107,255,255}, {149,255,255}, {64,255,255},
    {235,255,255}, {0,0,0}
};

// struct of led pinning
const struct rgbLed ledChannel_Array[LED_NUMBER_LAYER] =
{
    {5,8,2}, {10,9,11}, {40,44,39}, {36,35,37},
    {3,0,1}, {16,13,17}, {38,24,25}, {47,46,45},
    {6,4,7}, {20,18,19}, {30,31,29}, {41,43,42},
    {14,12,15}, {23,22,21}, {28,26,27}, {34,33,32}
};

const uint16_t gamma_table[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,
    2,  2,  2,  3,  3,  4,  4,  5,  5,  6,  7,  8,  8,  9, 10, 11,
   12, 13, 15, 16, 17, 18, 20, 21, 23, 25, 26, 28, 30, 32, 34, 36,
   38, 40, 43, 45, 48, 50, 53, 56, 59, 62, 65, 68, 71, 75, 78, 82,
   85, 89, 93, 97,101,105,110,114,119,123,128,133,138,143,149,154,
  159,165,171,177,183,189,195,202,208,215,222,229,236,243,250,258,
  266,273,281,290,298,306,315,324,332,341,351,360,369,379,389,399,
  409,419,430,440,451,462,473,485,496,508,520,532,544,556,569,582,
  594,608,621,634,648,662,676,690,704,719,734,749,764,779,795,811,
  827,843,859,876,893,910,927,944,962,980,998,1016,1034,1053,1072,1091,
  1110,1130,1150,1170,1190,1210,1231,1252,1273,1294,1316,1338,1360,1382,1404,1427,
  1450,1473,1497,1520,1544,1568,1593,1617,1642,1667,1693,1718,1744,1770,1797,1823,
  1850,1877,1905,1932,1960,1988,2017,2045,2074,2103,2133,2162,2192,2223,2253,2284,
  2315,2346,2378,2410,2442,2474,2507,2540,2573,2606,2640,2674,2708,2743,2778,2813,
  2849,2884,2920,2957,2993,3030,3067,3105,3143,3181,3219,3258,3297,3336,3376,3416,
  3456,3496,3537,3578,3619,3661,3703,3745,3788,3831,3874,3918,3962,4006,4050,4095
  };



static void waitForNextFrame(uint8_t counter) {
    cli();
	frameReady = 1;
	frameCnt = (counter-1);
	sei();
}

uint8_t getLedColor(volatile struct rgbLed *led)
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

void setLedColor(volatile struct rgbLed *led, const struct hsv *newColor)
{
    unsigned char r = 0,g = 0,b = 0, i = 0, f = 0;
    unsigned int p = 0, q = 0, t = 0;

    if( newColor->s == 0 )
    {
        r = g = b = newColor->v;
    }
    else
    {
        i=newColor->h/43;
        f=newColor->h%43;
        p = (newColor->v * (255 - newColor->s))/256;
        q = (newColor->v * ((10710 - (newColor->s * f))/42))/256;
        t = (newColor->v * ((10710 - (newColor->s * (42 - f)))/42))/256;

        switch(i)
        {
        case 0:
            r = newColor->v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = newColor->v;
            b = p;
            break;
        case 2:
            r = p;
            g = newColor->v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = newColor->v;
          break;
        case 4:
            r = t;
            g = p;
            b = newColor->v;
            break;
        case 5:
            r = newColor->v;
            g = p;
            b = q;
            break;
        }
    }

    //Gamma correction
    led->r = gamma_table[r];
    led->g = gamma_table[g];
    led->b = gamma_table[b];
}

// Activates every led one by one
uint8_t everyLED(void)
{
    static uint8_t lednumber = 0;   //counter for active leds
    // Check if new frame is displayed
    switch (animationState)
    {
    case 0:
    //First step: clear whole cube
        clearLEDCube();
        lednumber = 0;
        animationState = 1;
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;
        break;
    case 1:
    //Second step: copy current frame and add one led
        copyFrame();
        if(lednumber < LED_NUMBER_MAX) {
            //Check if color/dimming level has changed and rebuild cube with new color/dimming level
            if((globalHSV.h != localHSV.h) || (globalHSV.s != localHSV.s) || (globalHSV.v != localHSV.v)) {

                localHSV.h = globalHSV.h;
                localHSV.s = globalHSV.s;
                localHSV.v = globalHSV.v;

                for(uint8_t i = 0; i < lednumber; i++) {
                    setLedColor((nextFrame + i), &localHSV);
                }
            }
            else {
                setLedColor((nextFrame + lednumber), &localHSV);
            }

            lednumber++;

            //If all leds are active, end animation
            if (lednumber == LED_NUMBER_MAX) {
                animationState = 0;
            }
        }
        break;
    }
    waitForNextFrame(FRAMECOUNT_VALUE_FAST);

    return animationState;
}

// Regenähnliche Animation
// Auf der obersten Ebene werden zufällige LEDs aktiviert, die sich dann nach unten bewegen
uint8_t rainfall(uint8_t replay){
    uint8_t activeLED = 1;
    uint8_t lednumber = 0;

    switch(animationState) {
    case 0:
        InitRandomGenerator();
        clearLEDCube();
        counter = replay;
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;
        animationState = 1;
        break;
    case 1:
        shiftDownward();
        //Maximum of 4 Leds at the same time
        for(uint8_t ct = rand() % 2 + 1; ct > 0; ct--){
            do{
                lednumber = rand() % LED_NUMBER_LAYER;
                //Look for active led in column
                for(uint8_t z = 0; z < LED_LAYERS_NUMBER; z++){
                    if(getLedColor(nextFrame+lednumber+z*16) == RGB_COLOR_BLACK){
                        activeLED = 0;
                    }
                    else{
                        activeLED = 1;
                        break;
                    }
                }
            }
            while(activeLED);

            setLedColor((nextFrame+lednumber+TOP*16), &localHSV);
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

    if((globalHSV.h != localHSV.h) || (globalHSV.s != localHSV.s) || (globalHSV.v != localHSV.v)) {
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;

        for(uint8_t i = 0; i < LED_NUMBER_MAX; i++) {
            if(getLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                setLedColor((nextFrame + i), &localHSV);
            }
        }
    }

    waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

    return animationState;
}

uint8_t fadeColorCube(uint8_t replay)
{
    if(animationState == 0) {
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;
        counter = replay;
        animationState = 1;
    }

    switch(animationState) {
    case 1:
        localHSV.h++;
        fillLEDCube(&localHSV);
        if(localHSV.h == globalHSV.h)
        {
            if(counter == 0) {
                animationState = 0;
            }
            else {
                counter--;
            }
        }
        break;
    }

    waitForNextFrame(FRAMECOUNT_VALUE_FAST);

    return animationState;
}

uint8_t randomLedColorCube(uint8_t replay) {
    if(animationState == 0) {
        counter = replay;
        animationState = 1;
    }

    switch(animationState) {
    case 1:
        for(uint8_t i = 0; i < LED_NUMBER_MAX; i++)
        {
            uint8_t color = 0;
            color = rand() % (COLOR_MAX_NUMBER-1);

            localHSV.h = color_table[color].h;
            localHSV.s = color_table[color].s;
            localHSV.v = color_table[color].v;

            setLedColor((nextFrame + i), &localHSV);
        }

        if(counter == 0) {
            animationState = 0;
        }
        else {
            counter--;
        }
        break;
    }

    waitForNextFrame(FRAMECOUNT_VALUE_SLOW);

    return animationState;
}

//TODO 4. Layer muss mit einbezogen werden --> editiert
uint8_t fillCubeDiagonal(uint8_t replays)
{
    if(animationState == 0) {
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;
        counter = replays;
        animationState = 1;
    }

    switch(animationState) {
    case 1:
        clearLEDCube();
        setLedColor((nextFrame+0+0*4+0*16), &localHSV);
        animationState = 2;
        break;
    case 2:
        shiftUpward();
        setLedColor((nextFrame+1+0*4+0*16), &localHSV);
        setLedColor((nextFrame+0+1*4+0*16), &localHSV);
        animationState = 3;
        break;
    case 3:
        shiftUpward();
        setLedColor((nextFrame+1+1*4+0*16), &localHSV);
        setLedColor((nextFrame+0+2*4+0*16), &localHSV);
        setLedColor((nextFrame+2+0*4+0*16), &localHSV);
        animationState = 4;
        break;
    case 4:
        shiftUpward();
        setLedColor((nextFrame+2+1*4+0*16), &localHSV);
        setLedColor((nextFrame+1+2*4+0*16), &localHSV);
        setLedColor((nextFrame+0+3*4+0*16), &localHSV);
        setLedColor((nextFrame+3+0*4+0*16), &localHSV);
        animationState = 5;
        break;
    case 5:
        shiftUpward();
        setLedColor((nextFrame+2+2*4+0*16), &localHSV);
        setLedColor((nextFrame+1+3*4+0*16), &localHSV);
        setLedColor((nextFrame+3+1*4+0*16), &localHSV);
        animationState = 6;
        break;
    case 6:
        shiftUpward();
        setLedColor((nextFrame+3+2*4+0*16), &localHSV);
        setLedColor((nextFrame+2+3*4+0*16), &localHSV);
        animationState = 7;
        break;
    case 7:
        shiftUpward();
        setLedColor((nextFrame+3+3*4+0*16), &localHSV);
        animationState = 8;
        break;
    case 8:
        shiftUpward();
        animationState = 9;
        break;
    case 9:
        shiftUpward();
        animationState = 10;
        break;
    case 10:
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

    if((globalHSV.h != localHSV.h) || (globalHSV.s != localHSV.s) || (globalHSV.v != localHSV.v)) {
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;

        for(uint8_t i = 0; i < LED_NUMBER_MAX; i++) {
            if(getLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                setLedColor((nextFrame + i), &localHSV);
            }
        }
    }

    waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

    return animationState;
}

uint8_t floatingZLayer(uint8_t replay)
{
    switch(animationState) {
    case 0:
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;
        counter = replay;
        clearLEDCube();
        fillLayer(Z_LAYER,BOTTOM,&localHSV);
        animationState = 1;
        break;
    case 1:
    case 2:
    case 3:
        shiftUpward();
        animationState++;
        break;
    case 4:
    case 5:
        shiftDownward();
        animationState++;
        break;
    case 6:
        shiftDownward();
        if(counter == 0) {
            animationState = 0;
        }
        else {
            counter--;
            animationState = 1;
        }
        break;
    }

    if((globalHSV.h != localHSV.h) || (globalHSV.s != localHSV.s) || (globalHSV.v != localHSV.v)) {
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;

        for(uint8_t i = 0; i < LED_NUMBER_MAX; i++) {
            if(getLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                setLedColor((nextFrame + i), &localHSV);
            }
        }
    }

    waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

    return animationState;
}

uint8_t floatingYLayer(uint8_t replay)
{
    switch(animationState) {
    case 0:
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;
        counter = replay;
        clearLEDCube();
        fillLayer(Y_LAYER,BACK,&localHSV);
        animationState = 1;
        break;
    case 1:
    case 2:
    case 3:
        shiftForward();
        animationState++;
        break;
    case 4:
    case 5:
        shiftBackward();
        animationState++;
        break;
    case 6:
        shiftBackward();
        if(counter == 0) {
            animationState = 0;
        }
        else {
            counter--;
            animationState = 1;
        }
        break;
    }

    if((globalHSV.h != localHSV.h) || (globalHSV.s != localHSV.s) || (globalHSV.v != localHSV.v)) {
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;

        for(uint8_t i = 0; i < LED_NUMBER_MAX; i++) {
            if(getLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                setLedColor((nextFrame + i), &localHSV);
            }
        }
    }

    waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

    return animationState;
}

uint8_t floatingXLayer(uint8_t replay)
{
    switch(animationState) {
    case 0:
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;
        counter = replay;
        clearLEDCube();
        fillLayer(X_LAYER,RIGHT,&localHSV);
        animationState = 1;
        break;
    case 1:
    case 2:
    case 3:
        shiftLeft();
        animationState++;
        break;
    case 4:
    case 5:
        shiftRight();
        animationState++;
        break;
    case 6:
        shiftRight();
        if(counter == 0) {
            animationState = 0;
        }
        else {
            counter--;
            animationState = 1;
        }
        break;
    }

    if((globalHSV.h != localHSV.h) || (globalHSV.s != localHSV.s) || (globalHSV.v != localHSV.v)) {
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;

        for(uint8_t i = 0; i < LED_NUMBER_MAX; i++) {
            if(getLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                setLedColor((nextFrame + i), &localHSV);
            }
        }
    }

    waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

    return animationState;
}

/*
 * Zufallsgenerator bestimmt LED die aktiviert werden soll
 */
uint8_t activateRandomLED(uint8_t replay)
{
    static uint8_t ledNumber = 0;

    if(animationState == 0) {
        /* Intializes random number generator */
        InitRandomGenerator();
        counter = replay;
        animationState = 1;
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;
    }

    switch(animationState) {
    case 1:
        clearLEDCube();
        ledNumber = rand() % LED_NUMBER_MAX;
        setLedColor((nextFrame+ledNumber), &localHSV);
        animationState = 2;
        break;
    case 2:
        copyFrame();
        setLedColor((nextFrame+ledNumber), &color_table[HSV_COLOR_BLACK]);
        if(counter == 0) {
            animationState = 0;
        }
        else {
            counter--;
            animationState = 1;
        }
        break;
    }

    if((globalHSV.h != localHSV.h) || (globalHSV.s != localHSV.s) || (globalHSV.v != localHSV.v)) {
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;

        for(uint8_t i = 0; i < LED_NUMBER_MAX; i++) {
            if(getLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                setLedColor((nextFrame + i), &localHSV);
            }
        }
    }

    waitForNextFrame(FRAMECOUNT_VALUE_VERY_FAST);

    return animationState;
}

uint8_t fillCube_randomly(uint8_t replay)
{
    uint8_t ledNumber = 0;
    static uint8_t activeLEDs = 0;

    if(animationState == 0) {
        /* Intializes random number generator */
        InitRandomGenerator();
        counter = replay;
        animationState = 1;
    }

    switch(animationState) {
    case 1:
        clearLEDCube();
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;
        activeLEDs = 0;
        animationState = 2;
        break;
    case 2:
        copyFrame();
        do{
            ledNumber = rand() % LED_NUMBER_MAX;

            if(getLedColor(nextFrame+ledNumber) == RGB_COLOR_BLACK)
            {
                setLedColor((nextFrame+ledNumber), &localHSV);
                activeLEDs++;
                if(activeLEDs == LED_NUMBER_MAX) {
                    if(counter == 0) {
                        animationState = 0;
                    }
                    else {
                        counter--;
                        animationState = 1;
                    }
                }
                break;
            }
        }while(1);
        break;
    }

    if((globalHSV.h != localHSV.h) || (globalHSV.s != localHSV.s) || (globalHSV.v != localHSV.v)) {
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;

        for(uint8_t i = 0; i < LED_NUMBER_MAX; i++) {
            if(getLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                setLedColor((nextFrame + i), &localHSV);
            }
        }
    }

    waitForNextFrame(FRAMECOUNT_VALUE_FAST);

    return animationState;
}

uint8_t clearCube_randomly(uint8_t replay)
{
    uint8_t ledNumber = 0;
    static uint8_t activeLEDs = 0;

    if(animationState == 0) {
        /* Intializes random number generator */
        InitRandomGenerator();
        counter = replay;
        animationState = 1;
    }

    switch(animationState) {
    case 1:
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;
        fillLEDCube(&localHSV);
        activeLEDs = 0;
        animationState = 2;
        break;
    case 2:
        copyFrame();
        do{
            ledNumber = rand() % LED_NUMBER_MAX;

            if(getLedColor(nextFrame+ledNumber) != RGB_COLOR_BLACK)
            {
                setLedColor((nextFrame+ledNumber), &color_table[HSV_COLOR_BLACK]);
                activeLEDs++;
                if(activeLEDs == LED_NUMBER_MAX) {
                    if(counter == 0) {
                        animationState = 0;
                    }
                    else {
                        counter--;
                        animationState = 1;
                    }
                }
                break;
            }
        }while(1);
        break;
    }

    if((globalHSV.h != localHSV.h) || (globalHSV.s != localHSV.s) || (globalHSV.v != localHSV.v)) {
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;

        for(uint8_t i = 0; i < LED_NUMBER_MAX; i++) {
            if(getLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                setLedColor((nextFrame + i), &localHSV);
            }
        }
    }

    waitForNextFrame(FRAMECOUNT_VALUE_FAST);

    return animationState;
}

//TODO nur eine Säule geht durch anstatt mehrere --> gelöst
uint8_t dropLedTopDown(uint8_t replay)
{
    static uint8_t ledNumber = 0;
    static uint8_t lastLED = 16;

    if(animationState == 0) {
        /* Intializes random number generator */
        InitRandomGenerator();
        counter = replay;
        animationState = 1;
    }

    switch(animationState) {
    case 1:
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;
        clearLEDCube();
        lastLED = 16;
        ledNumber = 0;
        /* Obere Ebene anschalten */
        fillLayer(Z_LAYER,TOP,&localHSV);
        animationState = 2;
        break;
    case 2:
        copyFrame();
        do{
            /* zufällige LED bestimmen die wandern soll */
            ledNumber = rand() % LED_NUMBER_LAYER;

            if(ledNumber != lastLED)
            {
                /* Zufall entscheiden lassen, ob LED hoch oder runter wandern soll */
                if(rand() % 2)
                {
                    /* Falls LED ganz unten ist, dann hochschieben */
                    if(getLedColor(nextFrame+ledNumber+BOTTOM*16) != RGB_COLOR_BLACK)
                    {
                        setLedColor((nextFrame+ledNumber+BOTTOM*16), &color_table[HSV_COLOR_BLACK]);
                        setLedColor((nextFrame+ledNumber+(BOTTOM+1)*16), &localHSV);
                        animationState = 3;
                    }
                }
                else
                {
                    /* Falls LED ganz oben ist, dann runterfallen lassen */
                    if(getLedColor(nextFrame+ledNumber+TOP*16) != RGB_COLOR_BLACK)
                    {
                        setLedColor((nextFrame+ledNumber+TOP*16), &color_table[HSV_COLOR_BLACK]);
                        setLedColor((nextFrame+ledNumber+(TOP-1)*16), &localHSV);
                        animationState = 5;
                    }
                }
            }
        }while(animationState == 2);
        lastLED = ledNumber;
        break;
    case 3:
        copyFrame();
        setLedColor((nextFrame+ledNumber+(BOTTOM+1)*16), &color_table[HSV_COLOR_BLACK]);
        setLedColor((nextFrame+ledNumber+(BOTTOM+2)*16), &localHSV);
        animationState = 4;
        break;
    case 4:
        copyFrame();
        setLedColor((nextFrame+ledNumber+(BOTTOM+2)*16), &color_table[HSV_COLOR_BLACK]);
        setLedColor((nextFrame+ledNumber+(BOTTOM+3)*16), &localHSV);

        if(counter == 0) {
            animationState = 0;
        }
        else {
            animationState = 2;
            counter--;
        }
        break;
    case 5:
        copyFrame();
        setLedColor((nextFrame+ledNumber+(TOP-1)*16), &color_table[HSV_COLOR_BLACK]);
        setLedColor((nextFrame+ledNumber+(TOP-2)*16), &localHSV);
        animationState = 6;
        break;
    case 6:
        copyFrame();
        setLedColor((nextFrame+ledNumber+(TOP-2)*16), &color_table[HSV_COLOR_BLACK]);
        setLedColor((nextFrame+ledNumber+(TOP-3)*16), &localHSV);

        if(counter == 0) {
            animationState = 0;
        }
        else {
            animationState = 2;
            counter--;
        }
        break;
    }

    if((globalHSV.h != localHSV.h) || (globalHSV.s != localHSV.s) || (globalHSV.v != localHSV.v)) {
        localHSV.h = globalHSV.h;
        localHSV.s = globalHSV.s;
        localHSV.v = globalHSV.v;

        for(uint8_t i = 0; i < LED_NUMBER_MAX; i++) {
            if(getLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
                setLedColor((nextFrame + i), &localHSV);
            }
        }
    }

    waitForNextFrame(FRAMECOUNT_VALUE_MEDIUM);

    return animationState;
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
//    static uint8_t led_start = 0;
//
//    if(frameReady == 0) {
//        if(animationState == 0) {
//            counter = replay;
//            clearLEDCube();
//            animationState = 1;
//            localHSV.h = globalHSV.h;
//            localHSV.s = globalHSV.s;
//            localHSV.v = globalHSV.v;
//        }
//
//        switch(animationState) {
//        case 1:
//            /* Erste LED in einer Ecke aktivieren */
//            setLedColor((nextFrame+led_start+0*16), &localHSV);
//            animationState = 2;
//            break;
//        case 2:
//            clearLEDCube();
//            /* Würfel mit 8 LEDs aktivieren */
//            for(uint8_t x = (0+(led_start%4)/2); x < (2+(led_start%4)/2); x++)
//            {
//                for(uint8_t y = (0+(led_start/4)/2); y < (2+(led_start/4)/2); y++)
//                {
//                    for(uint8_t z = 0; z < 2; z++)
//                    {
//                        setLedColor((nextFrame+x+y*4+z*16), &localHSV);
//                    }
//                }
//            }
//            animationState = 3;
//            break;
//        case 3:
//            clearLEDCube();
//            for(uint8_t y = 0; y < 3; y++)
//            {
//                for(uint8_t x = 0; x < 3; x++)
//                {
//                    setLedColor((nextFrame+x+y*4+0*16), &localHSV);
//                }
//            }
//            setLedColor((nextFrame+1+1*4+z*16), &color_table[HSV_COLOR_BLACK]);
//            copyLayer(Z_LAYER,BOTTOM,TOP-1);
//            for(uint8_t y = 0; y < 3; y++)
//            {
//                for(uint8_t x = 0; x < 3; x++)
//                {
//                    setLedColor((nextFrame+x+y*4+1*16), &localHSV);
//                    x++;
//                }
//                y++;
//            }
//            animationState = 4;
//            break;
//        }
//
//        if((globalHSV.h != localHSV.h) || (globalHSV.s != localHSV.s) || (globalHSV.v != localHSV.v)) {
//            localHSV.h = globalHSV.h;
//            localHSV.s = globalHSV.s;
//            localHSV.v = globalHSV.v;
//
//            for(uint8_t i = 0; i < LED_NUMBER_MAX; i++) {
//                if(getLedColor(nextFrame + i) != RGB_COLOR_BLACK) {
//                    setLedColor((nextFrame + i), &localHSV);
//                }
//            }
//        }
//
//        waitForNextFrame(FRAMECOUNT_VALUE_VERY_FAST);
//    }
//
//    uint8_t k = 0;
//
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
    static uint8_t layer = 0;
    static uint8_t ledDriverUpdate_cnt = 0;



    if(ledDriverUpdate_cnt == 0) {
        ledDriverUpdate_cnt = 2;

        //Update data for all columns
        for(uint8_t i = 0; i < LED_NUMBER_LAYER; i++)
        {
            Tlc5940_set(ledChannel_Array[i].r,(currentFrame+i+layer*16)->r / 16);
            Tlc5940_set(ledChannel_Array[i].g,(currentFrame+i+layer*16)->g / 16);
            Tlc5940_set(ledChannel_Array[i].b,(currentFrame+i+layer*16)->b / 16);
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

        if(layer >= LED_LAYERS_NUMBER)
        {
            //Check if time for actual frame is exceeded and load next frame
            static uint8_t cntDown = 0;
            if((frameReady != 0) && (cntDown == 0)) {
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

    if(ledDriverUpdate_cnt > 0) {
        ledDriverUpdate_cnt--;
    }
}
