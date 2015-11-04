#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>

class Image
{
  unsigned width;
  unsigned height;
  std::vector<unsigned char> pixels;
public:
  Image(const std::string &filename);
  unsigned get_width()  const { return width; }
  unsigned get_height() const { return height; }
  const unsigned char *get_pixels() const { return pixels.data(); }

private:
  // Internal functions
  void readPCX8bits (const unsigned char *data,
                     const unsigned char *palette);

private:
#pragma pack(push, 1)
  // pcx header
  struct PCX_Header
  {
    unsigned char manufacturer;
    unsigned char version;
    unsigned char encoding;
    unsigned char bitsPerPixel;

    unsigned short xmin, ymin;
    unsigned short xmax, ymax;
    unsigned short horzRes, vertRes;

    unsigned char palette[48];
    unsigned char reserved;
    unsigned char numColorPlanes;

    unsigned short bytesPerScanLine;
    unsigned short paletteType;
    unsigned short horzSize, vertSize;

    unsigned char padding[54];
  };
#pragma pack(pop)

  const PCX_Header *header;

  // RGBA/BGRA component table access -- usefull for
  // switching from bgra to rgba at load time.
  static int rgbTable[3]; // bgra to rgba: 0, 1, 2
};

#endif
