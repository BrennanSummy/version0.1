#include "Texture.h"
// This is a C++ feature where you can define object values before even entering the constructor body
Texture::Texture(const std::string& path): texture_ID(0), filePath(path),localBuffer(nullptr),width(0),height(0),bytesPerPixel(0)
{
    stbi_set_flip_vertically_on_load(1);
    // Load the image assuming 4 channels (RGBA)
    localBuffer = stbi_load(path.c_str(),&width,&height,&bytesPerPixel,4);
    // Only run the GL stuff if the texture actually loads
    if (localBuffer)
    {
    // Prime GL for handling the texture
    glGenTextures(1, &texture_ID);
    glBindTexture(GL_TEXTURE_2D, texture_ID);
    
    // Params for texture sampling
    // Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Hand the image data to GL
    int border = 0;
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,width,height,border,GL_RGBA,GL_UNSIGNED_BYTE,localBuffer);

    // Free the image memory
    stbi_image_free(localBuffer);
    }
    else
    {
        std::cout << "Texture failed to load" << std::endl;
    }

}

Texture::~Texture()
{
    glDeleteTextures(1,&texture_ID);
}

void Texture::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture_ID);

}