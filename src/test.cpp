#include "Bitmap.h"

using namespace std;


int main(void)
{
    Bitmap* bitmap = Bitmap::LoadFile("output.bmp");

    auto height =(float) bitmap->GetH();
    auto width =(float) bitmap->GetW();
    // 遍历像素
    for (float j = 0; j < height; j+=1.0f){
        for (float i = 0; i < width; i+=1.0f) {
            auto color = bitmap->Sample2D(i / width  ,j/height );
            //auto color = bitmap->GetPixel(i,j);
            //cout<< color;
            bitmap->SetPixel(i,j, color);
        }
    }

    auto success =  bitmap->SaveFile("output2.bmp",true);

    // 用画板显示图片
#if defined(_WIN32) || defined(WIN32)
    system("mspaint.exe output2.bmp");
#endif

    return 0;
}


