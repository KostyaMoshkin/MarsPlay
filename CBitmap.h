#pragma once

#include <windows.h>

namespace bmp
{

    class CBitmap
    {
    public:
        static void SaveBitmapToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, LPCTSTR lpszFileName)
        {
            RGBQUAD palette[256];
            for (int i = 0; i < 256; ++i)
            {
                palette[i].rgbBlue = (byte)i;
                palette[i].rgbGreen = (byte)i;
                palette[i].rgbRed = (byte)i;
            }

            BITMAPINFOHEADER bmpInfoHeader = { 0 };

            bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);

            bmpInfoHeader.biBitCount = wBitsPerPixel;

            bmpInfoHeader.biClrImportant = 0;

            bmpInfoHeader.biClrUsed = 0;

            bmpInfoHeader.biCompression = BI_RGB;

            bmpInfoHeader.biHeight = lHeight;

            bmpInfoHeader.biWidth = lWidth;

            bmpInfoHeader.biPlanes = 1;

            bmpInfoHeader.biSizeImage = lWidth * lHeight * (wBitsPerPixel / 8);

            BITMAPFILEHEADER bfh = { 0 };

            bfh.bfType = 'B' + ('M' << 8);

            bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + sizeof(RGBQUAD) * 256;

            bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSizeImage;

            HANDLE hFile = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL,
                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

            if (!hFile)
            {
                return;
            }

            DWORD dwWritten = 0;

            WriteFile(hFile, &bfh, sizeof(bfh), &dwWritten, NULL);

            WriteFile(hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &dwWritten, NULL);

            WriteFile(hFile, &palette[0], sizeof(RGBQUAD) * 256, &dwWritten, NULL);

            if (lWidth % 4 == 0)
            {
                WriteFile(hFile, pBitmapBits, bmpInfoHeader.biSizeImage, &dwWritten, NULL);
            }
            else
            {
                char* empty = new char[4 - lWidth % 4];
                for (int i = 0; i < lHeight; ++i)
                {
                    WriteFile(hFile, &pBitmapBits[i * lWidth], lWidth, &dwWritten, NULL);
                    WriteFile(hFile, empty, 4 - lWidth % 4, &dwWritten, NULL);
                }

                delete[] empty;
            }
            CloseHandle(hFile);
        }
    };

}