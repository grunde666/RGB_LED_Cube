#include "transformation.h"
#include "animations.h"
#include "tlc5940.h"

//void fillLEDCube(struct rgbLed color, uint16_t dimm)
//{
//    for(uint8_t z = 0; z < 4; z++)
//    {
//        for(uint8_t y = 0; y < 4; y++)
//        {
//            for(uint8_t x = 0; x < 4; x++)
//            {
//                ledValue_Array[x][y][z].r = color.r * dimm;
//                ledValue_Array[x][y][z].g = color.g * dimm;
//                ledValue_Array[x][y][z].b = color.b * dimm;
//            }
//        }
//    }
//}

void fillLEDCube(void)
{
    for(uint8_t z = 0; z < 4; z++)
    {
        for(uint8_t y = 0; y < 4; y++)
        {
            for(uint8_t x = 0; x < 4; x++)
            {
                ledValue_Array[x][y][z] = 1;
            }
        }
    }
}

void clearLEDCube(void)
{
    for(uint8_t z = 0; z < 4; z++)
    {
        for(uint8_t y = 0; y < 4; y++)
        {
            for(uint8_t x = 0; x < 4; x++)
            {
                ledValue_Array[x][y][z] = 0;
            }
        }
    }
    Tlc5940_setAll(0);
}

void copyLayer(uint8_t layerType, uint8_t originLayer, uint8_t destinationLayer)
{
    if(layerType == X_LAYER)
    {
        for(uint8_t z = 0; z < 4; z++)
        {
            for(uint8_t y = 0; y < 4; y++)
            {
                ledValue_Array[destinationLayer][y][z] = ledValue_Array[originLayer][y][z];
            }
        }
    }
    else if(layerType == Y_LAYER)
    {
        for(uint8_t z = 0; z < 4; z++)
        {
            for(uint8_t x = 0; x < 4; x++)
            {
                ledValue_Array[x][destinationLayer][z] = ledValue_Array[x][originLayer][z];
            }
        }
    }
    else if(layerType == Z_LAYER)
    {
        for(uint8_t y = 0; y < 4; y++)
        {
            for(uint8_t x = 0; x < 4; x++)
            {
                ledValue_Array[x][y][destinationLayer] = ledValue_Array[x][y][originLayer];
            }
        }
    }
}

void fillLayer(uint8_t layerType, uint8_t layerNumber)
{
    if(layerType == X_LAYER)
    {
        // x-Ebene wird gef�llt
        for(uint8_t z = 0; z < 4; z++)
        {
            for(uint8_t y = 0; y < 4; y++)
            {
                ledValue_Array[layerNumber][y][z] = 1;
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
                ledValue_Array[x][layerNumber][z] = 1;
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
                ledValue_Array[x][y][layerNumber] = 1;
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
                ledValue_Array[layerNumber][y][z] = 0;
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
                ledValue_Array[x][layerNumber][z] = 0;
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
                ledValue_Array[x][y][layerNumber] = 0;
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
