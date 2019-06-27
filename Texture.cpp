#include "SolarSystem.h"
#include "Texture.h"


FIBITMAP* Texture::LoadTextureFile(const GLchar *filename, int& width, int& height) {

    // Determine the format of the image.
    // Note: The second paramter ('size') is currently unused, and we should use 0 for it.
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename , 0);

    // Check format
    //TODO: add return state and cleanup
    if (format == FIF_UNKNOWN) {
        cout << "Could not find image: " << filename << " - Aborting." << endl;
        exit(-1);
    }

    // If we're here we have a known image format, so load the image into a bitap
    FIBITMAP* bitmap = FreeImage_Load(format, filename);

    // Flip image vertical
    FreeImage_FlipVertical(bitmap);

    // How many bits-per-pixel is the source image?
    int bitsPerPixel =  FreeImage_GetBPP(bitmap);

    // Convert our image up to 32 bits (8 bits per channel, Red/Green/Blue/Alpha) -
    // but only if the image is not already 32 bits (i.e. 8 bits per channel).
    // Note: ConvertTo32Bits returns a CLONE of the image data - so if we
    // allocate this back to itself without using our bitmap32 intermediate
    // we will LEAK the original bitmap data, and valgrind will show things like this:
    //
    // LEAK SUMMARY:
    //  definitely lost: 24 bytes in 2 blocks
    //  indirectly lost: 1,024,874 bytes in 14 blocks    <--- Ouch.
    //
    // Using our intermediate and cleaning up the initial bitmap data we get:
    //
    // LEAK SUMMARY:
    //  definitely lost: 16 bytes in 1 blocks
    //  indirectly lost: 176 bytes in 4 blocks
    //
    // All above leaks (192 bytes) are caused by XGetDefault (in /usr/lib/libX11.so.6.3.0) - we have no control over this.
    //
    FIBITMAP* bitmap32;
    if (bitsPerPixel == 32) {
        bitmap32 = bitmap;
    }
    else {
        // conv image to 32bpp
        bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
        // Free original(non 32bpp) image
        FreeImage_Unload(bitmap);
    }

    // Some basic image info - strip it out if you don't care
    width  = FreeImage_GetWidth(bitmap32);
    height = FreeImage_GetHeight(bitmap32);
    cout << "TEXTURE: file: " << filename << " is size: " << width << "x" << height << "x" << bitsPerPixel << "." << endl;

    // Return pointer on converted image
    return bitmap32;
}


GLuint Texture::LoadCubeMap(vector<string> faces) {

    int width, height;
    FIBITMAP* bitmap;
    GLubyte* textureData;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    //for(GLuint i = 0; i < faces.size(); i++)
    for(GLuint i = 0; i < 6; i++)
    {

        bitmap = LoadTextureFile(faces[i].c_str(), width, height);
        textureData = FreeImage_GetBits(bitmap);

        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, textureData
        );


        // Unload the bitmap
        FreeImage_Unload(bitmap);

    }
    // No Mipmapping, just linear
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // Clamping
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);





    return textureID;

}

// Method to load an image into a texture using the freeimageplus library.
// Returns the texture ID or dies trying.
// Default values for filtering are GL_LINEAR
GLuint Texture::LoadTexture(const string &filenameString, GLenum minificationFilter, GLenum magnificationFilter) {

    // Get the filename as a pointer to a const char array to play nice with FreeImage
    const char* filename = filenameString.c_str();

    // Some image data
    int width, height;
    FIBITMAP* bitmap;

    // Load and convert(32bpp) texture file, get width and height
    bitmap = LoadTextureFile(filename, width, height);

    // Get a pointer to the texture data as an array of unsigned bytes.
    // Note: At this point bitmap32 ALWAYS holds a 32-bit colour version of our image - so we get our data from that.
    // Also, we don't need to delete or delete[] this textureData because it's not on the heap (so attempting to do
    // so will cause a crash) - just let it go out of scope and the memory will be returned to the stack.
    GLubyte* textureData = FreeImage_GetBits(bitmap);

    // Generate a texture ID and bind to it
    GLuint tempTextureID;
    glGenTextures(1, &tempTextureID);
    glBindTexture(GL_TEXTURE_2D, tempTextureID);

    // Construct the texture.
    // Note: The 'Data format' is the format of the image data as provided by the image library. FreeImage decodes images into
    // BGR/BGRA format, but we want to work with it in the more common RGBA format, so we specify the 'Internal format' as such.
    glTexImage2D(GL_TEXTURE_2D,    // Type of texture
                 0,                // Mipmap level (0 being the top level i.e. full size)
                 GL_RGBA,          // Internal format
                 width,            // Width of the texture
                 height,           // Height of the texture,
                 0,                // Border in pixels
                 GL_BGRA,          // Data format
                 GL_UNSIGNED_BYTE, // Type of texture data
                 textureData);     // The image data to use for this texture


    // Specify our minification and magnification filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minificationFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magnificationFilter);

    // If we're using MipMaps, then we'll generate them here.
    // Note: The glGenerateMipmap call requires OpenGL 3.0 as a minimum.
    if (minificationFilter == GL_LINEAR_MIPMAP_LINEAR   ||
        minificationFilter == GL_LINEAR_MIPMAP_NEAREST  ||
        minificationFilter == GL_NEAREST_MIPMAP_LINEAR  ||
        minificationFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Specify Texture Wrapping (Repeat)
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    // Unbind Texture
    glBindTexture( GL_TEXTURE_2D, 0 );

    // Unload the 32-bit colour bitmap
    FreeImage_Unload(bitmap);

    // Return pointer to the final GL texture
    return tempTextureID;
}

GLuint Texture::LoadSkybox(const string& path, const string& ext) {

    vector <string> faces;
    faces.push_back(path + "right" + ext);
    faces.push_back(path + "left" + ext);
    faces.push_back(path + "top" + ext);
    faces.push_back(path + "bottom" + ext);
    faces.push_back(path + "back" + ext);
    faces.push_back(path + "front" + ext);

    return LoadCubeMap(faces);

};

