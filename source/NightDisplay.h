/*
   Copyright (C) 2021, Richard e Collins.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */
   
#ifndef NightDisplay_H
#define NightDisplay_H

#include "Graphics.h"
#include "Element.h"

#include <string>

class NightDisplay : public eui::Element
{
public:

    NightDisplay(const std::string &pPath,eui::Graphics* pGraphics,int pBigFont,int pNormalFont,int pMiniFont,int pBitcoinFont,int pLargeFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS);
    void OnMQTT(const std::string &pTopic,const std::string &pData);

private:
    eui::ElementPtr mInfoRoot = nullptr;
    eui::ElementPtr mBatterySOC,mInverter;

    class Temperature *mOutSideTemp;

    std::string myBTC = "n/a";

};


#endif //#ifndef NightDisplay_H
