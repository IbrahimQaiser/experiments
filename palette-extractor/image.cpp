#include "image.hpp"
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <sstream>
#include <vector>

unsigned char *create_buffer(size_t sz)
{
    unsigned char *buffer = static_cast<unsigned char *>(malloc(sz));
    if (!buffer)
    {
        throw image_error("Could not allocate memory");
    }
    return buffer;
}

void free_buffer(unsigned char *ptr)
{
    if (ptr)
    {
        free(ptr);
    }
}

Image::Image()
    : width_(0), height_(0), num_components_(0), buffer_(nullptr), is_buffer_created_by_stbi(false)
{
}

Image Image::from_file(const std::string &path)
{
    Image image;
    image.file_path_ = path;
    image.buffer_ =
        stbi_load(path.c_str(), &image.width_, &image.height_, &image.num_components_, 0);
    image.is_buffer_created_by_stbi = true;
    if (!image.buffer_)
    {
        std::ostringstream oss;
        oss << "Error while loading image \"" << path << "\": ";
        oss << stbi_failure_reason();
        throw image_error(oss.str());
    }
    return image;
}

auto Image::width() const -> int { return width_; }

auto Image::height() const -> int { return height_; }

auto Image::file_path() const -> std::string { return file_path_; }

auto Image::channels() const -> int { return num_components_; }

auto Image::buffer() -> unsigned char * { return buffer_; }

auto Image::buffer() const -> const unsigned char * { return buffer_; }

auto Image::size() const -> size_t { return width_ * height_ * num_components_; }

Image::~Image()
{
    if (buffer_)
    {
        if (is_buffer_created_by_stbi)
        {
            stbi_image_free(buffer_);
        }
        else
        {
            free_buffer(buffer_);
        }
        buffer_ = nullptr;
    }
}

Image::Image(const Image &other)
{
    file_path_ = other.file_path_;
    width_ = other.width_;
    height_ = other.height_;
    num_components_ = other.num_components_;
    is_buffer_created_by_stbi = false;
    buffer_ = nullptr;
    if (other.size() != 0)
    {
        buffer_ = create_buffer(size());
        memcpy(buffer_, other.buffer_, other.size());
    }
}

Image &Image::operator=(Image other)
{
    swap(*this, other);
    return *this;
}

Image::Image(Image &&other) noexcept : Image() { swap(*this, other); }

Image &Image::operator=(Image &&other) noexcept
{
    swap(*this, other);
    return *this;
}

Image Image::from_buffer(unsigned char *buffer, int width, int height, int channels)
{
    Image img;
    img.width_ = width;
    img.height_ = height;
    img.num_components_ = channels;
    img.buffer_ = buffer;
    img.is_buffer_created_by_stbi = false;
    return img;
}

Image Image::create(int width, int height, int channels)
{
    Image img;
    img.width_ = width;
    img.height_ = height;
    img.num_components_ = channels;
    img.buffer_ = create_buffer(img.size());
    img.is_buffer_created_by_stbi = false;
    return img;
}

auto Image::at_gray(int row, int col) const -> Color
{
    unsigned char *ptr = buffer_ + (num_components_ * (row * width_ + col));
    Color c;
    c.r = ptr[0];
    c.g = ptr[0];
    c.b = ptr[0];
    return c;
}

auto Image::at_gray_alpha(int row, int col) const -> Color
{
    unsigned char *ptr = buffer_ + (num_components_ * (row * width_ + col));
    Color c;
    c.has_alpha = true;
    c.r = ptr[0];
    c.g = ptr[0];
    c.b = ptr[0];
    c.a = ptr[1];
    return c;
}

auto Image::at_rgb(int row, int col) const -> Color
{
    unsigned char *ptr = buffer_ + (num_components_ * (row * width_ + col));
    Color c;
    c.r = ptr[0];
    c.g = ptr[1];
    c.b = ptr[2];
    return c;
}

auto Image::at_rgba(int row, int col) const -> Color
{
    unsigned char *ptr = buffer_ + (num_components_ * (row * width_ + col));
    Color c;
    c.has_alpha = true;
    c.r = ptr[0];
    c.g = ptr[1];
    c.b = ptr[2];
    c.a = ptr[3];
    return c;
}

auto Image::get_colors() const -> std::vector<Color>
{
    std::vector<Color> colors;
    colors.resize(width_ * height_);
    for (size_t i = 0; i < width_ * height_; ++i)
    {
        unsigned char *ptr = buffer_ + (num_components_ * i);
        colors[i].r = ptr[0];
        colors[i].g = ptr[1];
        colors[i].b = ptr[2];
        if (num_components_ == 4)
        {
            colors[i].a = ptr[3];
            colors[i].has_alpha = true;
        }
    }

    return colors;
}