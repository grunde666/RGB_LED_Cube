#include "transformation.h"
#include "tlc5940.h"

static void fillFrame(const struct hsv *newColor){
	for (uint8_t z=0; z<4; z++){
		for (uint8_t y=0; y<4; y++){
            for (uint8_t x=0; x<4; x++){
                setLedColor((nextFrame+x+y*4+z*16), newColor);
            }
		}
	}
}

void fillLEDCube(struct hsv *newColor)
{
    fillFrame(newColor);
}

void clearLEDCube(void)
{
   fillFrame(&color_table[HSV_COLOR_BLACK]);
}

void copyFrame(){
	for (uint8_t z=0; z<4; z++){
		for (uint8_t y=0; y<4; y++){
            for(uint8_t x=0; x<4; x++) {
                (nextFrame+x+y*4+z*16)->r = (currentFrame+x+y*4+z*16)->r;
                (nextFrame+x+y*4+z*16)->g = (currentFrame+x+y*4+z*16)->g;
                (nextFrame+x+y*4+z*16)->b = (currentFrame+x+y*4+z*16)->b;
            }
		}
	}
}

// Schiebt den Inhalt um eine Position nach vorne, Einf�gen von Nullen hinten
void shiftForward(){
	for (uint8_t z=0; z<4; z++){
        for(uint8_t x=0; x<4; x++){
            for (uint8_t y=1; y<4; y++){
                (nextFrame+x+y*4+z*16)->r = (currentFrame+x+(y-1)*4+z*16)->r;
                (nextFrame+x+y*4+z*16)->g = (currentFrame+x+(y-1)*4+z*16)->g;
                (nextFrame+x+y*4+z*16)->b = (currentFrame+x+(y-1)*4+z*16)->b;
            }
            (nextFrame+x+0*4+z*16)->r = 0;
            (nextFrame+x+0*4+z*16)->g = 0;
            (nextFrame+x+0*4+z*16)->b = 0;
		}
	}
}

// Schiebt den Inhalt um eine Position nach hinten, Einf�gen von Nullen vorne
void shiftBackward(){
	for (uint8_t z=0; z<4; z++){
        for(uint8_t x=0; x<4; x++){
            for (uint8_t y=0; y<3; y++){
                (nextFrame+x+y*4+z*16)->r = (currentFrame+x+(y+1)*4+z*16)->r;
                (nextFrame+x+y*4+z*16)->g = (currentFrame+x+(y+1)*4+z*16)->g;
                (nextFrame+x+y*4+z*16)->b = (currentFrame+x+(y+1)*4+z*16)->b;
            }
            (nextFrame+x+3*4+z*16)->r = 0;
            (nextFrame+x+3*4+z*16)->g = 0;
            (nextFrame+x+3*4+z*16)->b = 0;
		}
	}
}

// Schiebt den Inhalt um eine Position nach unten, Einf�gen von Nullen oben
void shiftDownward(){
	for(uint8_t y=0; y<4; y++){
	    for(uint8_t x=0; x<4; x++){
            for(uint8_t z=0; z<3; z++){
                (nextFrame+x+y*4+z*16)->r = (currentFrame+x+y*4+(z+1)*16)->r;
                (nextFrame+x+y*4+z*16)->g = (currentFrame+x+y*4+(z+1)*16)->g;
                (nextFrame+x+y*4+z*16)->b = (currentFrame+x+y*4+(z+1)*16)->b;
            }
            (nextFrame+x+y*4+3*16)->r = 0;
            (nextFrame+x+y*4+3*16)->g = 0;
            (nextFrame+x+y*4+3*16)->b = 0;
	    }
	}
}

// Schiebt den Inhalt um eine Position nach oben, Einf�gen von Nullen unten
void shiftUpward(){
	for(uint8_t y=0; y<4; y++){
        for(uint8_t x=0; x<4; x++){
            for(uint8_t z=0; z<3; z++){
                (nextFrame+x+y*4+(z+1)*16)->r = (currentFrame+x+y*4+z*16)->r;
                (nextFrame+x+y*4+(z+1)*16)->g = (currentFrame+x+y*4+z*16)->g;
                (nextFrame+x+y*4+(z+1)*16)->b = (currentFrame+x+y*4+z*16)->b;
            }
            (nextFrame+x+y*4+0*16)->r = 0;
            (nextFrame+x+y*4+0*16)->g = 0;
            (nextFrame+x+y*4+0*16)->b = 0;
        }
	}
}

// Schiebt den Inhalt um eine Position nach links
void shiftLeft(){
	for (uint8_t z=0; z<4; z++){
		for (uint8_t y=0; y<4; y++){
            for (uint8_t x=0; x<3; x++){
                (nextFrame+x+y*4+z*16)->r = (currentFrame+x+1+y*4+z*16)->r;
                (nextFrame+x+y*4+z*16)->g = (currentFrame+x+1+y*4+z*16)->g;
                (nextFrame+x+y*4+z*16)->b = (currentFrame+x+1+y*4+z*16)->b;
            }
            (nextFrame+3+y*4+z*16)->r = 0;
            (nextFrame+3+y*4+z*16)->g = 0;
            (nextFrame+3+y*4+z*16)->b = 0;
		}
	}
}

// Schiebt den Inhalt um eine Position nach rechts
void shiftRight(){
	for (uint8_t z=0; z<4; z++){
		for (uint8_t y=0; y<4; y++){
            for (uint8_t x=1; x<4; x++){
                (nextFrame+x+y*4+z*16)->r = (currentFrame+x-1+y*4+z*16)->r;
                (nextFrame+x+y*4+z*16)->g = (currentFrame+x-1+y*4+z*16)->g;
                (nextFrame+x+y*4+z*16)->b = (currentFrame+x-1+y*4+z*16)->b;
            }
            (nextFrame+0+y*4+z*16)->r = 0;
            (nextFrame+0+y*4+z*16)->g = 0;
            (nextFrame+0+y*4+z*16)->b = 0;
		}
	}
}

void copyLayer(uint8_t layerType, uint8_t originLayer, uint8_t destinationLayer)
{
    if(layerType == X_LAYER)
    {
        for(uint8_t z = 0; z < 4; z++)
        {
            for(uint8_t y = 0; y < 4; y++)
            {
                (nextFrame+destinationLayer+y*4+z*16)->r = (nextFrame+originLayer+y*4+z*16)->r;
                (nextFrame+destinationLayer+y*4+z*16)->g = (nextFrame+originLayer+y*4+z*16)->g;
                (nextFrame+destinationLayer+y*4+z*16)->b = (nextFrame+originLayer+y*4+z*16)->b;
            }
        }
    }
    else if(layerType == Y_LAYER)
    {
        for(uint8_t z = 0; z < 4; z++)
        {
            for(uint8_t x = 0; x < 4; x++)
            {
                (nextFrame+x+destinationLayer*4+z*16)->r = (nextFrame+x+originLayer*4+z*16)->r;
                (nextFrame+x+destinationLayer*4+z*16)->g = (nextFrame+x+originLayer*4+z*16)->g;
                (nextFrame+x+destinationLayer*4+z*16)->b = (nextFrame+x+originLayer*4+z*16)->b;
            }
        }
    }
    else if(layerType == Z_LAYER)
    {
        for(uint8_t y = 0; y < 4; y++)
        {
            for(uint8_t x = 0; x < 4; x++)
            {
                (nextFrame+x+y*4+destinationLayer*16)->r = (nextFrame+x+y*4+originLayer*16)->r;
                (nextFrame+x+y*4+destinationLayer*16)->g = (nextFrame+x+y*4+originLayer*16)->g;
                (nextFrame+x+y*4+destinationLayer*16)->b = (nextFrame+x+y*4+originLayer*16)->b;
            }
        }
    }
}

void fillLayer(uint8_t layerType, uint8_t layerNumber, struct hsv *newColor)
{
    if(layerType == X_LAYER)
    {
        // x-Ebene wird gef�llt
        for(uint8_t z = 0; z < 4; z++)
        {
            for(uint8_t y = 0; y < 4; y++)
            {
                setLedColor((nextFrame+layerNumber+y*4+z*16), newColor);
            }
        }
    }
    else if(layerType == Y_LAYER)
    {
        // y-Ebene wird gef�llt
        for(uint8_t z = 0; z < 4; z++)
        {
            for(uint8_t x = 0; x < 4; x++)
            {
                setLedColor((nextFrame+x+layerNumber*4+z*16), newColor);
            }
        }
    }
    else if(layerType == Z_LAYER)
    {
        //z-Ebene wird gef�llt
        for(uint8_t y = 0; y < 4; y++)
        {
            for(uint8_t x = 0; x < 4; x++)
            {
                setLedColor((nextFrame+x+y*4+layerNumber*16), newColor);
            }
        }
    }
}

void clearLayer(uint8_t layerType, uint8_t layerNumber)
{
    if(layerType == X_LAYER)
    {
        // x-Ebene wird geleert
        for(uint8_t z = 0; z < 4; z++)
        {
            for(uint8_t y = 0; y < 4; y++)
            {
                setLedColor((nextFrame+layerNumber+y*4+z*16), &color_table[HSV_COLOR_BLACK]);
            }
        }
    }
    else if(layerType == Y_LAYER)
    {
        // y-Ebene wird geleert
        for(uint8_t z = 0; z < 4; z++)
        {
            for(uint8_t x = 0; x < 4; x++)
            {
                setLedColor((nextFrame+x+layerNumber*4+z*16), &color_table[HSV_COLOR_BLACK]);
            }
        }
    }
    else if(layerType == Z_LAYER)
    {
        //z-Ebene wird geleert
        for(uint8_t y = 0; y < 4; y++)
        {
            for(uint8_t x = 0; x < 4; x++)
            {
                setLedColor((nextFrame+x+y*4+layerNumber*16), &color_table[HSV_COLOR_BLACK]);
            }
        }
    }
}

/* Schiebt LEDs eine Ebene nach rechts
 * LEDs in der rechtesten Ebene werden �berschrieben und die linkeste Ebene wird nach verschieben gel�scht
 */
void moveLayerRight(uint8_t clearLEDs)
{
    for(uint8_t i = 3; i > 0; i--)
    {
        copyLayer(X_LAYER,i-1,i);
    }

    if(clearLEDs)
    {
        clearLayer(X_LAYER,0);
    }
}

/* Schiebt LEDs eine Ebene nach links
 * LEDs in der linkesten Ebene werden �berschrieben und die rechteste Ebene wird nach verschieben gel�scht
 */
void moveLayerLeft(uint8_t clearLEDs)
{
    for(uint8_t i = 0; i < 3; i++)
    {
        copyLayer(X_LAYER,i+1,i);
    }

    if(clearLEDs)
    {
        clearLayer(X_LAYER,3);
    }
}

/* Schiebt LEDs eine Ebene nach vorn
 * LEDs in der vordersten Ebene werden �berschrieben und die hinterste Ebene wird nach verschieben gel�scht
 */
void moveLayerForward(uint8_t clearLEDs)
{
    for(uint8_t i = 3; i > 0; i--)
    {
        copyLayer(Y_LAYER,i-1,i);
    }

    if(clearLEDs)
    {
        clearLayer(Y_LAYER,0);
    }
}

/* Schiebt LEDs eine Ebene nach hinten
 * LEDs in der hintersten Ebene werden �berschrieben und die vorderste Ebene wird nach verschieben gel�scht
 */
void moveLayerBackward(uint8_t clearLEDs)
{
    for(uint8_t i = 0; i < 3; i++)
    {
        copyLayer(Y_LAYER,i+1,i);
    }

    if(clearLEDs)
    {
        clearLayer(Y_LAYER,3);
    }
}

/* Schiebt LEDs eine Ebene h�her
 * LEDs in der obersten Ebene werden �berschrieben und die unterste Ebene wird nach verschieben gel�scvoid moveLayerForward(uint8_t clearLEDs)ht
 */
void moveLayerUp(uint8_t clearLEDs)
{
    for(uint8_t i = 3; i > 0; i--)
    {
        copyLayer(Z_LAYER,i-1,i);
    }

    if(clearLEDs)
    {
        clearLayer(Z_LAYER,0);
    }
}

/* Schiebt LEDs eine Ebene tiefer
 * LEDs in der untersten Ebene werden �berschrieben und die oberste Ebene wird nach verschieben gel�scht
 */
void moveLayerDown(uint8_t clearLEDs)
{
    for(uint8_t i = 0; i < 3; i++)
    {
        copyLayer(Z_LAYER,i+1,i);
    }

    if(clearLEDs)
    {
        clearLayer(Z_LAYER,3);
    }
}
