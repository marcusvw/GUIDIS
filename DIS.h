#pragma once
#include "../PAG/PAG.h"
#include "../../version.h"
#ifdef HW_M5PAPER
#include <M5EPD.h>
#endif
#ifdef HW_M5CORE2
#include <M5Core2.h>
#endif
#include "../../rpc/RPC.h"
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#define DIS_NUM_IMG 4
enum DIS_ValType_t{TYPE_FLOAT=0,TYPE_INT=1,TYPE_STRING=2};
class DisplayPage : public Page
{
private:
    String image[DIS_NUM_IMG];
    String itemId[DIS_NUM_IMG];
    String label[DIS_NUM_IMG];
    DIS_ValType_t types[DIS_NUM_IMG];
    String formatStr[DIS_NUM_IMG];
    String valStr[DIS_NUM_IMG];
    double valFloat[DIS_NUM_IMG]={0.0,0.0,0.0};
    int32_t valInt[DIS_NUM_IMG]={0,0,0};
    bool firstRun=true;
    String header;

    FS *fsHandler;
    JsonRPC rpc;
#ifdef HW_M5PAPER
    M5EPD_Canvas canvas = (&M5.EPD);
#endif
#ifdef HW_M5CORE2
    #define canvas M5.Lcd
#endif

    const uint16_t IMG_POS_X[DIS_NUM_IMG] = {10, 10, 10, 10};
    const uint16_t IMG_POS_Y[DIS_NUM_IMG] = {25, 80, 135, 190};
    const uint16_t TXT_L1_Y[DIS_NUM_IMG] = {25, 80, 135, 190};
    const uint16_t TXT_L2_Y[DIS_NUM_IMG] = {50, 105, 160, 215};
    const uint16_t IMG_WIDTH = 48;
    const uint16_t IMG_HEIGHT = 48;
    const uint16_t TXT_X_POS = 68;
    uint32_t lastUpdate = 0;
    void renderHeader(const char *string);

public:
    DisplayPage(JsonObject obj, PAG_pos_t cp, bool paramUseSdCard);
    void activate();
    void deActivate();
    void draw();
    void handleInput(PAG_pos_t pos);
    void middleButtonPushed();
    String getHeader();
};