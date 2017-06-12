#include <iostream>
#include <windows.h>
#include <gdiplus.h>
#include <memory>

#pragma comment (lib,"Gdiplus.lib")
using namespace Gdiplus;
using namespace std;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;          // number of image encoders
    UINT size = 0;         // size of the image encoder array in bytes

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if(size == 0)
    {
        return -1;  // Failure
    }

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL)
    {
        return -1;  // Failure
    }

    GetImageEncoders(num, size, pImageCodecInfo);

    for(UINT j = 0; j < num; ++j)
    {
        if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }    
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

void BitmapToJpg(HBITMAP hbmpImage, int width, int height)
{
    Bitmap *p_bmp = Bitmap::FromHBITMAP(hbmpImage, NULL);
    //Bitmap *p_bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	ULONG quality = 10;
	encoderParameters.Parameter[0].Value = &quality;
    CLSID pngClsid;
    int result = GetEncoderClsid(L"image/jpeg", &pngClsid);
    if(result != -1)
        std::cout << "Encoder succeeded" << std::endl;
    else
        std::cout << "Encoder failed" << std::endl;
    p_bmp->Save(L"screen.jpg", &pngClsid, &encoderParameters);
	
    delete p_bmp;
}

bool ScreenCapture(int x, int y, int width, int height, char *filename)
{
    HDC hDc = CreateCompatibleDC(0);
    HBITMAP hBmp = CreateCompatibleBitmap(GetDC(0), width, height);
    SelectObject(hDc, hBmp);
    BitBlt(hDc, 0, 0, width, height, GetDC(0), x, y, SRCCOPY);
    BitmapToJpg(hBmp, width, height);
    DeleteObject(hBmp);
    return true;
}

int main() {
    // Initialize GDI+.
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    
    int x1 = 0;
    int y1 = 0;
    int x2 = GetSystemMetrics(SM_CXSCREEN);
    int y2 = GetSystemMetrics(SM_CYSCREEN);
    ScreenCapture(x1, y1, x2 - x1, y2 - y1, "screen.jpg");
    
    //Shutdown GDI+
    GdiplusShutdown(gdiplusToken);
	getchar();
    return 0;
}