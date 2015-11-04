// image.cpp

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "image.h"

/***************************************************************************\
 * Image::Image                                                            *
\***************************************************************************/
Image::Image(const std::string &filename)
{
  std::string ext;
  size_t length;
  std::vector<unsigned char> data;

  // Extract file extension
  ext.assign(filename, filename.find_last_of ('.') + 1, std::string::npos);

  if (ext.compare("pcx") != 0)
  {
    // problemo
  }
  // Open file
  std::ifstream ifs(filename);

  if (ifs.fail())
  {
    std::cerr << "Couldn't open file: " << filename << std::endl;
    exit(-1);
  }

  // Get file length
  ifs.seekg(0, std::ios::end);
  length = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  data.resize(length);

  // Read whole file data
  ifs.read(reinterpret_cast<char*>(data.data()), length);
  ifs.close();

  const unsigned char *data_ptr = data.data();

  // Read PCX header
  header = reinterpret_cast<const PCX_Header *>(data_ptr);
  data_ptr += sizeof(PCX_Header);

  // Check if is valid PCX file
  if (header->manufacturer != 0x0a)
  {
    std::cerr << "Bad version number: " << filename << std::endl;
    exit(-1);
  }

  // Initialize image variables
  width  = header->xmax - header->xmin + 1;
  height = header->ymax - header->ymin + 1;
  pixels.resize(width * height * 3);

  int palette_pos = data.size() - 768;
  readPCX8bits(data_ptr, data.data() + palette_pos);
}

int Image::rgbTable[3] = { 0, 1, 2 };

/*-------------------------------------------------------------------------*\
 * Image::readPCX8bits                                                     *
 * Read 8 bits PCX image.                                                  *
\*-------------------------------------------------------------------------*/
void Image::readPCX8bits(const unsigned char *data, const unsigned char *palette)
{
  const unsigned char *pData = data;
  int rle_count = 0, rle_value = 0;
  unsigned char *ptr;
  int *compTable = rgbTable;

  // Palette should be preceded by a value of 0x0c (12)...
  unsigned char magic = palette[-1];
  if (magic != 0x0c)
  {
    // ... but sometimes it is not
    std::cerr << "Warning: PCX palette should start with a value of 0x0c (12)!" << std::endl;
  }

  // Read pixel data
  for (unsigned y = 0; y < height; y++)
  {
    ptr = &pixels[(height - (y + 1)) * width * 3];
    int bytes = header->bytesPerScanLine;

    // Decode line number y
    while (bytes--)
    {
      if (rle_count == 0)
      {
        if( (rle_value = *(pData++)) < 0xc0)
          rle_count = 1;
        else
        {
          rle_count = rle_value - 0xc0;
          rle_value = *(pData++);
        }
      }

      rle_count--;

      ptr[0] = palette[rle_value * 3 + compTable[0]];
      ptr[1] = palette[rle_value * 3 + compTable[1]];
      ptr[2] = palette[rle_value * 3 + compTable[2]];
      ptr += 3;
    }
  }
}
