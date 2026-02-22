#include "pch.h"
#include "MyCppLib.h"
#include <iostream>
#include <cmath> // jeœli nie masz




extern "C" {

  __declspec(dllexport)
void ComputeHistogram(const uint8_t* data, int totalPixels, int channel, int channels, int* outTable)
{
     // std::cout << "wywo³anie dll budowa tablica histogram c++" << std::endl;
    for (int i = 0; i < 256; i++)
        outTable[i] = 0;

    for (int i = 0; i < totalPixels; i++)
    {
        int value = data[i * channels + channel];
        outTable[value]++;
    }
}


} // koniec extern "C"


extern "C" {

    __declspec(dllexport)
        uint8_t ByteBoundCpp(int v)
    {
        if (v < 0) return 0;
        if (v > 255) return 255;
        return (uint8_t)v;
    }

}



extern "C" __declspec(dllexport)
void NormalizeKernelCpp(double* kernel, int count, double sum)
{
    for (int i = 0; i < count; ++i)
    {
        kernel[i] /= sum;
    }
}


extern "C" {

    __declspec(dllexport)
        double ConvolvePixelCpp(const uint8_t* data,//tablica pikseli obrazu
            int w, int h, //szerokosc i wysokosc
            int channels, int channel, //liczba kana³ów i kana³ który przetwarzanmy
            const double* kernel, //tablea kernela
            int ker_size, int ker_center, //rozmiar i srodek kernela (1dla 3x3)
            int pos_x, int pos_y) //pozycja piksela wiersz kolumna
    {
        double A = 0.0;

        for (int i = -ker_center; i <= ker_center; ++i)
        {
            int position_x_current = pos_x + i;
            if (position_x_current < 0 || position_x_current >= h)
                continue; //do nastêpnej iteraji

            for (int j = -ker_center; j <= ker_center; ++j)
            {
                int position_y_current = pos_y + j;
                if (position_y_current < 0 || position_y_current >= w)
                    continue;//do nastêpnej iteracji

                int One_dim_position_kernel =
                    ker_size * (i + ker_center) + (j + ker_center);

                int One_dim_position =
                    (position_x_current * w + position_y_current) * channels + channel;

                A += kernel[One_dim_position_kernel] * data[One_dim_position];
            }
        }

        return A;
    }

} // extern "C"
