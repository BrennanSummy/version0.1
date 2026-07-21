#include "Texture.h"
// This is a C++ feature where you can define object values before even entering the constructor body
Texture::Texture(const std::string& path): m_texture_ID(0), m_filePath(path),m_localBuffer(nullptr),m_width(0),m_height(0),m_bytesPerPixel(0)
{
    stbi_set_flip_vertically_on_load(1);
    // Load the image assuming 4 channels (RGBA)
    m_localBuffer = stbi_load(path.c_str(),&m_width,&m_height,&m_bytesPerPixel,4);
    // Only run the GL stuff if the texture actually loads
    if (m_localBuffer)
    {
    // Prime GL for handling the texture
    glGenTextures(1, &m_texture_ID);
    glBindTexture(GL_TEXTURE_2D, m_texture_ID);
    
    // Params for texture sampling
    // Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Hand the image data to GL
    int border = 0;
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,m_width,m_height,border,GL_RGBA,GL_UNSIGNED_BYTE,m_localBuffer);

    // Free the image memory
    stbi_image_free(m_localBuffer);
    }
    else
    {
        std::cout << "Texture failed to load" << std::endl;
    }

}

Texture::~Texture()
{
    glDeleteTextures(1,&m_texture_ID);
}

void Texture::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_texture_ID);

}