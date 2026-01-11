// Homepage: https://github.com/ananthvk/cpp-raytracer
#pragma once
#include <iomanip>
#include <iostream>
#include <stdio.h>

// A class which represents an ASCII progressbar
class ProgressBar
{
  private:
    int value;
    int max_value;
    int width;
    bool display_percent;

  public:
    /// @brief Constructor for progressbar
    /// @param max_value
    /// @param width
    /// @param display_percent
    ProgressBar(int max_value, int width, bool display_percent = true)
        : value(0), max_value(max_value), width(width), display_percent(display_percent)
    {
    }

    /// @brief Updates the value of progressbar by given amount
    /// @param dt the amount by which the progressbar value has to be increased
    void tick(int dt = 1) { value += dt; }

    /// @brief Display the progressbar on the passed stream
    void display(std::ostream &os)
    {
        char fill = '=';
        char head = '>';
        char left_end = '[';
        char right_end = ']';

        os << "\r" << left_end;
        double percent = (double)value / max_value;
        os << std::string(percent * width, fill) << head << std::string((1 - percent) * width, ' ')
           << right_end;
        if (display_percent)
        {
            printf("( %0.03f%% )", percent * 100);
        }
        os << std::flush;
    }

    /// @brief Hide the cursor to prevent flickering when updating progressbar
    void hide_cursor(std::ostream &os) { os << "\033[?25l"; }

    /// @brief Show the cursor
    void show_cursor(std::ostream &os) { os << "\033[?25h"; }
};