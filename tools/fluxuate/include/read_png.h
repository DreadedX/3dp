#ifndef READ_PNG_H
#define READ_PNG_H

struct Image {

    int width = 0;
    int height = 0;
    int size = 0;
    unsigned char *pixels = nullptr;
};

Image pngLoad(char *name);

#endif

