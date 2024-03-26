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

#include "DaytimeDisplay.h"
#include "DisplayClock.h"
#include "DisplayWeather.h"
#include "DisplaySolaX.h"
#include "DisplaySystemStatus.h"
#include "DisplayAirQuality.h"
#include "DisplayBitcoinPrice.h"
#include "DisplayTideData.h"
#include "Temperature.h"

DaytimeDisplay::DaytimeDisplay(const std::string &pPath,eui::Graphics* pGraphics,int pBigFont,int pNormalFont,int pMiniFont,int pBitcoinFont,int pLargeFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS)
{

    this->SetID("mainScreen");
    this->SetGrid(1,2);

    mInfoRoot = new eui::Element;
    mInfoRoot->SetID("InfoScreen");
    mInfoRoot->SetPos(0,0);
    mInfoRoot->SetGrid(3,4);

    this->Attach(mInfoRoot);
    this->GetStyle().mTexture = pGraphics->TextureLoadPNG(pPath + "images/bg-fire-01.png");


    mInfoRoot->SetFont(pNormalFont);
    mInfoRoot->GetStyle().mBackground = eui::COLOUR_NONE;

    eui::ElementPtr BottomPannel = new eui::Element;
        BottomPannel->SetPos(0,2);
        BottomPannel->SetGrid(3,2);
        BottomPannel->SetSpan(3,1);
        mBTC = new DisplayBitcoinPrice(pBitcoinFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS);
        BottomPannel->Attach(mBTC);

        DisplayTideData* tide = new DisplayTideData(pLargeFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS);
            tide->SetPos(0,1);
            tide->SetSpan(3,1);
            BottomPannel->Attach(tide);

        mSolar = new DisplaySolaX(pGraphics,pPath,pLargeFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS);
        BottomPannel->Attach(mSolar);
    mInfoRoot->Attach(BottomPannel);

    eui::ElementPtr clock = new DisplayClock(pBigFont,pNormalFont,pMiniFont,CELL_PADDING,BORDER_SIZE,eui::COLOUR_WHITE);
    clock->SetPos(0,0);
    mInfoRoot->Attach(clock);


    mInfoRoot->Attach(new DisplayWeather(pGraphics,pPath,pBigFont,pNormalFont,pMiniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));

    eui::ElementPtr status = new eui::Element;
    status->SetGrid(1,2);
    status->SetPos(2,0);
    mInfoRoot->Attach(status);
    status->Attach(new DisplaySystemStatus(pBigFont,pNormalFont,pMiniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));
    status->Attach(new DisplayAirQuality(pLargeFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));

    eui::ElementPtr topCentre = new eui::Element;
        topCentre->SetPos(1,0);
        topCentre->SetGrid(1,2);
	
	// Leave 0,0 empty for now, obscured by fireplace.

        // My bitcoin investment.
        eui::ElementPtr MyInvestment = new eui::Element(mBTC->GetUpStyle());
            MyInvestment->SetPadding(0.05f);
            MyInvestment->SetText("£XXXXXX");
            MyInvestment->SetPadding(CELL_PADDING);
            MyInvestment->SetPos(0,1);
            MyInvestment->SetFont(pBitcoinFont);
            MyInvestment->SetOnUpdate([this](eui::ElementPtr pElement,const eui::Rectangle& pContentRect)
            {
                pElement->SetText(myBTC);
                return true;
            });
        topCentre->Attach(MyInvestment);
        
    mInfoRoot->Attach(topCentre);
    
    //
    // Bottom 4th line.
    eui::ElementPtr line4 = new eui::Element;
        line4->SetPos(0,3);
        line4->SetGrid(3,2);
        line4->SetSpan(3,1);    
            // The temperature outside
        mOutSideTemp = new Temperature(pLargeFont   ,mBTC->GetUpStyle(),CELL_PADDING);
            mOutSideTemp->SetPos(0,0);
        line4->Attach(mOutSideTemp);
    mInfoRoot->Attach(line4);
}

void DaytimeDisplay::OnMQTT(const std::string &pTopic,const std::string &pData)
{
    // Record when we last seen a change, if we don't see one for a while something is wrong.
    // I send an 'hartbeat' with new data that is just a value incrementing.
    // This means we get an update even if the tempareture does not change.
    if( tinytools::string::CompareNoCase(pTopic,"/outside/temperature") && mOutSideTemp )
    {
        mOutSideTemp->NewShedOutSide(pData);
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/shed/temperature") && mOutSideTemp)
    {
        mOutSideTemp->NewShedTemperature(pData);
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/btc/gb") )
    {
        mBTC->UpdateGBP(pData);
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/btc/usd") )
    {
        mBTC->UpdateUSD(pData);
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/btc/change") )
    {
        mBTC->UpdateChange(pData);
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/btc/mine") )
    {
        myBTC = pData;
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/solar/",7) )
    {
        mSolar->UpdateData(pTopic,pData);
    }
}

