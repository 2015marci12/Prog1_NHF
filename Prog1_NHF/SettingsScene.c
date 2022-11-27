#include "SettingsScene.h"

bool globalSettings_Init = false;
GlobalSettings globalSettings;

GlobalSettings* GetGlobalSettings()
{
    //Load or init settings if necessary.
    if (!globalSettings_Init) 
    {
        FILE* file = NULL;
        fopen_s(&file, "Resources\\GlobalSettings.set", "r");
        if(file)
            fread(&globalSettings, sizeof(GlobalSettings), 1, file);
        else 
        {
            globalSettings.masterVolume = 1.f;
            globalSettings.musicVolume = 1.f;
            globalSettings.effectsVolume = 1.f;
            globalSettings.ResolutionVariation = 0;
            globalSettings.FullScreen = false;

            SaveGlobalSettings();
        }

        if (file) fclose(file);

        globalSettings_Init = true;
    }

    return &globalSettings;
}

void SaveGlobalSettings()
{
    FILE* file = NULL;
    fopen_s(&file, "Resources\\GlobalSettings.set", "w");

    ASSERT(file, "Failed to create or open file!");

    fwrite(&globalSettings, sizeof(GlobalSettings), 1, file);

    fclose(file);
}

//Basic resolutions.
//800 x 600
//1280 x 720
//1920 x 1080

uint32_t resolutionsX[] =
{
    800, 1280, 1920
};
uint32_t resolutionsY[] =
{
    600, 720, 1080
};

uvec2 GetResolutionVariation(uint32_t variation)
{
    return new_uvec2(resolutionsX[variation], resolutionsY[variation]);
}

uint32_t GetResolutionVariationCount()
{
    return sizeof(resolutionsX) / sizeof(uint32_t);
}


