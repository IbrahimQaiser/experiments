#pragma once
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

class Color
{
  public:
    unsigned char r, g, b, a;
    bool has_alpha;

    Color() : r(0), g(0), b(0), a(0), has_alpha(false) {}

    Color(unsigned char value) : r(value), g(value), b(value), a(0), has_alpha(false) {}

    Color(unsigned char r, unsigned char g, unsigned char b)
        : r(r), g(g), b(b), a(0), has_alpha(false)
    {
    }

    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        : r(r), g(g), b(b), a(a), has_alpha(true)
    {
    }

    bool operator==(Color other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    bool operator!=(Color other) const { return !operator==(other); }

    auto format_hex() const -> std::string
    {
        std::ostringstream oss;
        oss << "#";
        oss << std::hex << std::setfill('0');
        oss << std::setw(2) << +r;
        oss << std::setw(2) << +g;
        oss << std::setw(2) << +b;
        if (has_alpha)
            oss << std::setw(2) << +a;
        return oss.str();
    }

    auto format_tuple() const -> std::string
    {
        std::ostringstream oss;
        if (has_alpha)
            oss << "rgba(";
        else
            oss << "rgb(";
        oss << +r;
        oss << ", " << +g;
        oss << ", " << +b;
        if (has_alpha)
            oss << ", " << +a;
        oss << ")";
        return oss.str();
    }

    auto distance_squared(Color other) -> int
    {
        int rdelta = r - other.r;
        int gdelta = g - other.g;
        int bdelta = b - other.b;
        int adelta = a - other.a;
        return (rdelta * rdelta) + (gdelta * gdelta) + (bdelta * bdelta) + (adelta * adelta);
    }
};

class image_error : public std::runtime_error
{
  public:
    image_error(const std::string &message) : std::runtime_error(message) {}
};

class Image
{
  private:
    std::string file_path_;
    int width_;
    int height_;
    int num_components_;
    // Pixels are stored row wise in RGB(A) format
    unsigned char *buffer_;
    // Was the buffer created by the library or did we create it
    bool is_buffer_created_by_stbi;
    Image();

  public:
    static Image from_file(const std::string &path);
    /// Creates an image from an already existing buffer
    /// @note It does not take ownership of the buffer, and it is the responsibility of the caller
    /// to free the buffer
    static Image from_buffer(unsigned char *buffer, int width, int height, int channels);

    static Image create(int width, int height, int channels);

    auto width() const -> int;
    auto height() const -> int;
    auto file_path() const -> std::string;
    auto channels() const -> int;
    // Returns the size of pixel data in bytes
    auto size() const -> size_t;
    // Returns pointer to underlying data buffer
    auto buffer() -> unsigned char *;
    auto buffer() const -> const unsigned char *;

    auto at_gray(int row, int col) const -> Color;
    auto at_gray_alpha(int row, int col) const -> Color;
    auto at_rgb(int row, int col) const -> Color;
    auto at_rgba(int row, int col) const -> Color;
    auto get_colors() const -> std::vector<Color>;

    ~Image();
    Image(const Image &other);
    Image &operator=(Image other);
    Image(Image &&other) noexcept;
    Image &operator=(Image &&other) noexcept;

    friend void swap(Image &first, Image &second)
    {
        using std::swap;
        swap(first.file_path_, second.file_path_);
        swap(first.width_, second.width_);
        swap(first.height_, second.height_);
        swap(first.num_components_, second.num_components_);
        swap(first.buffer_, second.buffer_);
        swap(first.is_buffer_created_by_stbi, second.is_buffer_created_by_stbi);
    }
};
