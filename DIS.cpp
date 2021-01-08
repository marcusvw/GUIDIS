#include "DIS.h"
#include "../../version.h"
#ifdef HW_M5PAPER
#include <M5EPD.h>
#endif
#ifdef HW_M5CORE2
#include <M5Core2.h>
#endif
#include <ArduinoJson.h>
#include "../GUI.h"
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
DisplayPage::DisplayPage(JsonObject obj, PAG_pos_t cp, bool paramUseSdCard)
{
    canvas_pos.x = 0;
    canvas_pos.y = 0;
    canvas_pos = cp;
    for (uint32_t x = 0; x < DIS_NUM_IMG; x++)
    {
        Serial.printf("DIS INF Loading Image: %d\r\n", x);
        image[x] = obj["images"][x].as<String>();
        itemId[x] = obj["ids"][x].as<String>();
        label[x] = obj["label"][x].as<String>();
        formatStr[x] = obj["format"][x].as<String>();
        types[x] = (DIS_ValType_t)obj["types"][x].as<uint32_t>();
        if (paramUseSdCard)
        {
            fsHandler = &SD;
        }
        else
        {
            fsHandler = &SPIFFS;
        }

        GUI_CheckImage(image[x]);
        GUI_CheckImage(image[x]);
    }
    header = obj["head"].as<String>();
#ifdef HW_M5PAPER
    canvas.createCanvas(320, 240);
#endif
    Serial.printf("DIS INF Display page loaded\r\n");
}

/**
 * Header Render function
 * */
void DisplayPage::renderHeader(const char *string)
{
    canvas.setTextSize(1);
    canvas.fillRect(3, 4, 316, 20, PAG_FOREGND);
    canvas.setTextColor(PAG_BACKGND);
    canvas.setTextDatum(TC_DATUM);
    canvas.drawString(string, 160, 3, 4);
}

void DisplayPage::activate()
{
    active = true;
#ifdef HW_M5PAPER
    canvas.fillCanvas(BLACK);
#endif
#ifdef HW_M5CORE2
    canvas.fillScreen(BLACK);
#endif
    renderHeader(header.c_str());
    for (uint32_t x; x < DIS_NUM_IMG; x++)
    {
        canvas.drawBmpFile(*fsHandler, image[x].c_str(), IMG_POS_X[x], IMG_POS_Y[x]);
        canvas.setTextSize(1);
        canvas.setTextDatum(TL_DATUM);
        canvas.setTextColor(PAG_FOREGND);
        canvas.drawString(label[x], TXT_X_POS, TXT_L1_Y[x], 4);
    }
    PAG_pos_t pos;
    pos.y = -1;
    pos.x = -1;
    firstRun = true;
    //handleInput(pos);
#ifdef HW_M5PAPER
    if (GUI_cachedUpdate())
    {
        canvas.pushCanvas(canvas_pos.x, canvas_pos.y, UPDATE_MODE_NONE);
    }
    else
    {
        canvas.pushCanvas(canvas_pos.x, canvas_pos.y, UPDATE_MODE_GC16);
    }
#endif
}
void DisplayPage::deActivate()
{
    active = false;
}

String DisplayPage::getHeader()
{
    return (header);
}
void DisplayPage::draw()
{

#ifdef HW_M5PAPER
    if (GUI_cachedUpdate())
    {
       // canvas.pushCanvas(canvas_pos.x, canvas_pos.y, UPDATE_MODE_NONE);
    }
    else
    {
        canvas.pushCanvas(canvas_pos.x, canvas_pos.y, UPDATE_MODE_DU4);
    }
#endif
}

void DisplayPage::middleButtonPushed()
{
}

void DisplayPage::handleInput(PAG_pos_t pos)
{
    char cstr[16];
    bool redraw = false;
    if (active)
    {

        if ((millis() - lastUpdate) > 1300)
        {
            for (uint32_t x = 0; x < DIS_NUM_IMG; x++)
            {
                if (types[x] == TYPE_FLOAT)
                {
                    double uVal = 0.0;
                    uVal = JsonRPC::execute_float("GetValue", String(itemId[x]));
                    if ((uVal != valFloat[x]) || firstRun)
                    {
                        char buffer[100];
                        canvas.fillRect(TXT_X_POS, TXT_L2_Y[x], 388, 20, PAG_BACKGND);
                        canvas.setCursor(TXT_X_POS, TXT_L2_Y[x]);
                        canvas.setTextSize(1);
                        canvas.setTextDatum(TL_DATUM);
                        canvas.setTextColor(PAG_FOREGND);

                        sprintf(buffer, formatStr[x].c_str(), uVal);
                        ;
                        canvas.drawString(buffer, TXT_X_POS, TXT_L2_Y[x], 4);
                        valFloat[x] = uVal;
                        redraw = true;
                    }
                }
                else if (types[x] == TYPE_INT)
                {
                    uint32_t uVal = 0;
                    uVal = JsonRPC::execute_int("GetValue", String(itemId[x]));
                    if ((uVal != valInt[x]) || firstRun)
                    {
                        char buffer[100];
                        canvas.fillRect(TXT_X_POS, TXT_L2_Y[x], 388, 20, PAG_BACKGND);
                        canvas.setCursor(TXT_X_POS, TXT_L2_Y[x]);
                        canvas.setTextSize(1);
                        canvas.setTextDatum(TL_DATUM);
                        canvas.setTextColor(PAG_FOREGND);

                        sprintf(buffer, formatStr[x].c_str(), uVal);

                        canvas.drawString(buffer, TXT_X_POS, TXT_L2_Y[x], 4);
                        valInt[x] = uVal;
                        redraw = true;
                    }
                }
                else if (types[x] == TYPE_STRING)
                {
                    String uVal;
                    uVal = JsonRPC::execute_string("GetValue", String(itemId[x]));
                    if ((uVal != valStr[x]) || firstRun)
                    {
                        char buffer[100];
                        canvas.fillRect(TXT_X_POS, TXT_L2_Y[x], 388, 20, PAG_BACKGND);
                        canvas.setCursor(TXT_X_POS, TXT_L2_Y[x]);
                        canvas.setTextSize(1);
                        canvas.setTextDatum(TL_DATUM);
                        canvas.setTextColor(PAG_FOREGND);

                        sprintf(buffer, formatStr[x].c_str(), uVal);
                        ;
                        canvas.drawString(buffer, TXT_X_POS, TXT_L2_Y[x], 4);
                        valStr[x] = uVal;
                        redraw = true;
                    }
                }
            }
            lastUpdate = millis();
        }
    }
    if (redraw)
    {
        draw();
    }
    firstRun = false;
}
