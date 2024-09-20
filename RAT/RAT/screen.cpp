#include "screen.h"

using namespace Gdiplus;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;
    UINT size = 0;

    ImageCodecInfo* pImage = NULL;

    GetImageEncodersSize(&num, &size);

    if (size == 0) return -1;

    pImage = (ImageCodecInfo*)malloc(size);
    if (pImage == NULL) return -1;

    GetImageEncoders(num, size, pImage);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImage[j].MimeType, format) == 0) {
            *pClsid = pImage[j].Clsid;
            free(pImage);
            return j;
        }
    }
    free(pImage);
    return -1;
}

int screen::Screenshot(char* folder)
{
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMemDC = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmScreen = NULL;

    int Height = GetSystemMetrics(SM_CYSCREEN);
    int Width = GetSystemMetrics(SM_CXSCREEN);

    Gdiplus::GdiplusStartupInput gdip;
    ULONG_PTR gdipToken;
    Gdiplus::GdiplusStartup(&gdipToken, &gdip, NULL);

    hbmScreen = CreateCompatibleBitmap(hdcScreen, Width, Height);
    SelectObject(hdcMemDC, hbmScreen);

    BitBlt(hdcMemDC, 0, 0, Width, Height, hdcScreen, 0, 0, SRCCOPY);

    CLSID encoderID;

    GetEncoderClsid(L"image/png", &encoderID);

    Bitmap* bmp = new Bitmap(hbmScreen, (HPALETTE)0);
    strcat(folder, "\\random.png");
    // to wchar_t
    size_t length = strlen(folder) + 1;
    wchar_t* folder_w = new wchar_t[length];
    mbstowcs(folder_w, folder, length);

    bmp->Save(folder_w, (CLSID*)&encoderID, NULL);

    Gdiplus::GdiplusShutdown(gdipToken);

    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    ReleaseDC(NULL, hdcScreen);
    return 0;
}


