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

#include "Style.h"
#include "Graphics.h"
#include "Element.h"
#include "Application.h"


#include "TinyTools.h"
#include "DaytimeDisplay.h"

#include "MQTTData.h"

#include <unistd.h>
#include <filesystem>
#include <curl/curl.h> // libcurl4-openssl-dev

// https://ffmpeg.org/doxygen/0.6/api-example_8c-source.html
// for video to texture 

class MyUI : public eui::Application
{
public:
    MyUI(const std::string& path);
    virtual ~MyUI();

    virtual void OnOpen(eui::Graphics* pGraphics);
    virtual void OnClose();
    virtual void OnUpdate()
    {
        MQTT->Tick();
    }

    virtual eui::ElementPtr GetRootElement(){return daytimeDisplay;}
    virtual uint32_t GetUpdateInterval()const{return 1000;}

    virtual int GetEmulatedWidth()const{return 1080;}
    virtual int GetEmulatedHeight()const{return 1920;}

private:
    const float CELL_PADDING = 0.02f;
    const float RECT_RADIUS = 0.2f;
    const float BORDER_SIZE = 3.0f;
    const std::string mPath;

    DaytimeDisplay* daytimeDisplay = nullptr;


    MQTTData* MQTT = nullptr;
    std::map<std::string,std::string> mMQTTData;

    int n = 0;


    void StartMQTT();

};

MyUI::MyUI(const std::string& path):mPath(path)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

MyUI::~MyUI()
{
    delete MQTT;
	curl_global_cleanup();
}

void MyUI::OnOpen(eui::Graphics* pGraphics)
{
    std::cout << "mPath = " << mPath << "\n";

    pGraphics->SetDisplayRotation(eui::Graphics::ROTATE_FRAME_PORTRAIT);
    
    StartMQTT();    

    int miniFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Regular.ttf",25);
    int normalFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Regular.ttf",40);

    int largeFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Bold.ttf",55);
    int bigFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Bold.ttf",130);
    int bitcoinFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Bold.ttf",70);

    daytimeDisplay = new DaytimeDisplay(mPath,pGraphics,bigFont,normalFont,miniFont,bitcoinFont,largeFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS);


    std::cout << "UI started\n";
}


void MyUI::OnClose()
{
    delete daytimeDisplay;
    daytimeDisplay = nullptr;
}

void MyUI::StartMQTT()
{
    // MQTT data
    const std::vector<std::string> topics =
    {
        "/outside/temperature","/outside/hartbeat",
        "/shed/temperature","/shed/hartbeat",
        "/btc/gb",
        "/btc/usd",
        "/btc/change",
        "/btc/diskused",
        "/btc/verificationprogress",
        "/btc/blockdate",
        "/btc/mine",
        "/solar/battery/total",
        "/solar/inverter/total",
        "/solar/grid/total",
        "/solar/yeld",
        "/solar/panel/front",
        "/solar/panel/rear"        
    };

    for( auto t : topics )
    {
        mMQTTData[t] = "N/A";
    }

    // Make sure there is data.
    mMQTTData["/outside/temperature"] = "--.-C";
    mMQTTData["/shed/temperature"] = "--.-C";
    mMQTTData["/shed/temperature"] = "--.-C";

    MQTT = new MQTTData("MQTT",1883,topics,
        [this](const std::string &pTopic,const std::string &pData)
        {
//            std::cout << "MQTTData " << pTopic << " " << pData << "\n";
            mMQTTData[pTopic] = pData;
            daytimeDisplay->OnMQTT(pTopic,pData);
        });

}

int main(const int argc,const char *argv[])
{
#ifdef NDEBUG
    std::cout << "Release\n";
#else
    std::cout << "Debug\n";
#endif


// Crude argument list handling.
    std::string path = "./";
    if( argc == 2 && std::filesystem::directory_entry(argv[1]).exists() )
    {
        path = argv[1];
        if( path.back() != '/' )
            path += '/';
    }

    MyUI* theUI = new MyUI(path); // MyUI is your derived application class.
    eui::Application::MainLoop(theUI);
    delete theUI;

    return EXIT_SUCCESS;
}
