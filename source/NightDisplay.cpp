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

#include "NightDisplay.h"
#include "DisplayClock.h"
#include "DisplayWeather.h"
#include "DisplaySolaX.h"
#include "DisplaySystemStatus.h"
#include "DisplayAirQuality.h"

#include "DisplayTideData.h"
#include "Temperature.h"

NightDisplay::NightDisplay(const std::string &pPath,eui::Graphics* pGraphics,int pBigFont,int pNormalFont,int pMiniFont,int pBitcoinFont,int pLargeFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS)
{

    this->SetID("mainScreen");
    this->SetGrid(1,2);

    mInfoRoot = new eui::Element;
    mInfoRoot->SetID("InfoScreen");
    mInfoRoot->SetPos(0,0);
    mInfoRoot->SetGrid(3,4);

    this->Attach(mInfoRoot);
    this->GetStyle().mBackground = eui::COLOUR_BLACK;

    mInfoRoot->SetFont(pNormalFont);

    eui::ElementPtr clock = new DisplayClock(pBigFont,pNormalFont,pMiniFont,CELL_PADDING,0,eui::COLOUR_DARK_GREY);
    clock->SetPos(1,1);
    mInfoRoot->Attach(clock);

    eui::ElementPtr status = new eui::Element;
    status->SetGrid(1,2);
    status->SetPos(2,0);
    mInfoRoot->Attach(status);

    eui::ElementPtr topCentre = new eui::Element;
        topCentre->SetPos(1,0);
        topCentre->SetGrid(1,2);
	
	// Leave 0,0 empty for now, obscured by fireplace.

        // My bitcoin investment.
        eui::ElementPtr MyInvestment = new eui::Element();
            MyInvestment->SetPadding(0.05f);
            MyInvestment->SetText("£XXXXXX");
            MyInvestment->SetPadding(CELL_PADDING);
            MyInvestment->SetPos(0,1);
            MyInvestment->SetFont(pBitcoinFont);
            MyInvestment->GetStyle().mForeground = eui::COLOUR_DARK_GREY;        

            MyInvestment->SetOnUpdate([this](eui::ElementPtr pElement,const eui::Rectangle& pContentRect)
            {
                pElement->SetText(myBTC);
                return true;
            });
        topCentre->Attach(MyInvestment);
                
    mInfoRoot->Attach(topCentre);

    eui::Style temp;
    temp.mBackground = eui::COLOUR_NONE;
    temp.mForeground = eui::COLOUR_DARK_GREY;

    mOutSideTemp = new Temperature(pLargeFont,temp,CELL_PADDING);
        mOutSideTemp->SetPos(0,2);
        mOutSideTemp->SetSpan(3,1);    
        mOutSideTemp->NewOutSideTemperature("--");
        mOutSideTemp->NewShedTemperature("--");
    mInfoRoot->Attach(mOutSideTemp);

    eui::Style SOCStyle;
    SOCStyle.mForeground = eui::COLOUR_DARK_GREY;

    mBatterySOC = new eui::Element;
        mBatterySOC->SetPadding(0.05f);
        mBatterySOC->SetText("Fetching");
        mBatterySOC->SetPadding(CELL_PADDING);
        mBatterySOC->SetPos(1,3);
        mBatterySOC->SetStyle(SOCStyle);
    mInfoRoot->Attach(mBatterySOC);

    mInverter = new eui::Element;
        mInverter->SetPadding(0.05f);
        mInverter->SetText("Fetching");
        mInverter->SetPadding(CELL_PADDING);
        mInverter->SetPos(1,4);
        mInverter->SetStyle(SOCStyle);
    mInfoRoot->Attach(mInverter);

}

void NightDisplay::OnMQTT(const std::string &pTopic,const std::string &pData)
{
    // Record when we last seen a change, if we don't see one for a while something is wrong.
    // I send an 'hartbeat' with new data that is just a value incrementing.
    // This means we get an update even if the tempareture does not change.
    if( tinytools::string::CompareNoCase(pTopic,"/loft/temperature") && mOutSideTemp )
    {
        mOutSideTemp->NewLoftTemperature(pData);
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/outside/temperature") && mOutSideTemp )
    {
        mOutSideTemp->NewOutSideTemperature(pData);
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/shed/temperature") && mOutSideTemp)
    {
        mOutSideTemp->NewShedTemperature(pData);
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/btc/mine") )
    {
        myBTC = pData;
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/solar/battery/total") )
    {
        mBatterySOC->SetTextF("%d%%",std::stoi(pData));
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/solar/inverter/total") )
    {
        mInverter->SetTextF("%d",std::stoi(pData));
    }

}

