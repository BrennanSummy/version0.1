#pragma once

#include "RenderWindow.h"
#include <iostream>

#include "external/stb_image.h"

class Texture
{
    private:

        std::string filePath;
        unsigned int texture_ID;
        unsigned char* localBuffer;
        int width, height, bytesPerPixel;
    
    public:
    Texture(const std::string& path);
    ~Texture();

    void bind(unsigned int slot = 0) const;
    void unBind() const;

};