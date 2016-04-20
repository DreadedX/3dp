#ifndef PNG_READER_H
#define PNG_READER_H

namespace png {

    struct Data {

	int width = 0;
	int height = 0;
	int size = 0;
	unsigned char bytesPerPixel = 0;
	unsigned char *pixels = nullptr;
    };

    Data read(const char *name);
}

#endif

