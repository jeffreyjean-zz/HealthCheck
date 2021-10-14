// HealthCheck.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>


int main()
{
    // BusIdleTimeoutOverride
    HKEY hKey;
    const wchar_t *keyPath = L"SYSTEM\\CurrentControlSet\\Services\\pci\\Parameters";
    LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ, &hKey);
    bool bExistsAndSuccess = (lRes == ERROR_SUCCESS);
    LONG nError = ERROR_INVALID_FUNCTION;

    if (bExistsAndSuccess)
        printf("Registry key %ls found\n", keyPath);
    else
        printf("Registry key %ls does not exit\n", keyPath);

    if (bExistsAndSuccess)
    {
        // Read the specific registry value under key path
        LPBYTE lpData = (LPBYTE) malloc(16);
        DWORD nValue = 0;
        DWORD dwBufferSize = 8;
        ZeroMemory((PVOID *)lpData, 16);
        nError = RegQueryValueExW(hKey,
            L"BusIdleTimeoutOverride",
            0,
            NULL,
            reinterpret_cast<LPBYTE>(lpData),
            &dwBufferSize);
        if (ERROR_SUCCESS == nError)
        {
            printf("BusIdleTimeoutOverride : %d data size %d\n", *(unsigned int *)lpData, dwBufferSize);
        }
        else
        {
            printf("BusIdleTimeoutOverride : error %d expected data size %d\n", nError, dwBufferSize);
            nError = RegQueryValueExW(hKey,
                L"DmaRemappingCompatible",
                0,
                NULL,
                reinterpret_cast<LPBYTE>(lpData),
                &dwBufferSize);
            printf("DmaRemappingCompatible : %d data size %d\n", *(unsigned int*)lpData, dwBufferSize);
        }
    }

    RegCloseKey(hKey);

    BOOL            FoundSecondaryDisp = FALSE;
    DWORD           DispNum = 0;
    DISPLAY_DEVICE  DisplayDevice;
    LONG            Result;
    TCHAR           szTemp[200];
    int             i = 0;
    DEVMODE   defaultMode;

    // initialize DisplayDevice
    ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
    DisplayDevice.cb = sizeof(DisplayDevice);

    // get all display devices
    while (EnumDisplayDevices(NULL, DispNum, &DisplayDevice, 0))
    {
        ZeroMemory(&defaultMode, sizeof(DEVMODE));
        defaultMode.dmSize = sizeof(DEVMODE);
        if (!EnumDisplaySettings((LPCWSTR)DisplayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &defaultMode))
            OutputDebugString(L"EnumDisplaySettings failed\n");

        printf("DeviceName %ls DeviceString %ls DeviceID %ls DeviceKey %ls\n", DisplayDevice.DeviceName, DisplayDevice.DeviceString, DisplayDevice.DeviceID, DisplayDevice.DeviceKey);
        printf("StateFlags 0x%.8x\n", DisplayDevice.StateFlags);
        //if ((DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
        //    !(DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
        if (DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
        {
            printf("Mode %.04dx%.04d %d bpp @ %d Hz\n", defaultMode.dmPelsWidth, defaultMode.dmPelsHeight, defaultMode.dmBitsPerPel, defaultMode.dmDisplayFrequency);
            /*
            DEVMODE    DevMode;
            ZeroMemory(&DevMode, sizeof(DevMode));
            DevMode.dmSize = sizeof(DevMode);
            DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_POSITION
                | DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS;
            Result = ChangeDisplaySettingsEx((LPCWSTR)DisplayDevice.DeviceName,
                &DevMode,
                NULL,
                CDS_UPDATEREGISTRY,
                NULL);
            */
            
            //The code below shows how to re-attach the secondary displays to the desktop

            //ChangeDisplaySettingsEx((LPSTR)DisplayDevice.DeviceName,
            //                       &defaultMode,
            //                       NULL,
            //                       CDS_UPDATEREGISTRY,
            //                       NULL);
        }

        // Reinit DisplayDevice just to be extra clean

        ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
        DisplayDevice.cb = sizeof(DisplayDevice);
        DispNum++;
    } // end while for all display devices

    return nError;


}