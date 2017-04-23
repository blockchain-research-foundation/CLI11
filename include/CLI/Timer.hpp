#pragma once

// Distributed under the LGPL v2.1 license.  See accompanying
// file LICENSE or https://github.com/CLIUtils/CLI11 for details.

#include <string>
#include <iostream>
#include <chrono>
#include <functional>

namespace CLI {

class Timer {
protected:
    /// This is a typedef to make clocks easier to use
    typedef std::chrono::high_resolution_clock clock;

    /// This typedef is for points in time
    typedef std::chrono::time_point<clock> time_point;

    /// This is the type of a printing function, you can make your own
    typedef std::function<std::string(std::string, std::string)> time_print_t;

    /// This is the title of the timer
    std::string title_;

    /// This is the function that is used to format most of the timing message
    time_print_t time_print_;

    /// This is the starting point (when the timer was created)
    time_point start_;

public:

    /// Standard print function, this one is set by default
    static std::string Simple(std::string title, std::string time) {
        return title + ": " + time;
    }

    /// This is a fancy print function with --- headers
    static std::string Big(std::string title, std::string time) {
        return std::string("-----------------------------------------\n")
            + "| " + title + " | Time = " + time + "\n"
            + "-----------------------------------------";
    }

public:
    /// Standard constructor, can set title and print function
    Timer(std::string title="Timer", time_print_t time_print = Simple)
        : title_(title), time_print_(time_print), start_(clock::now()) {}

    /// Time a function by running it multiple times. Target time is the len to target.
    inline std::string time_it(std::function<void()> f, double target_time=1) {
        time_point start = start_;
        double total_time;

        start_ = clock::now();
        size_t n = 0;
        do {
            f();
            std::chrono::duration<double> elapsed = clock::now() - start_; 
            total_time = elapsed.count();
        } while (n++ < 100 && total_time < target_time);

        std::string out = make_time_str(total_time/n) + " for " + std::to_string(n) + " tries";
        start_ = start;
        return out;
    }
    /// This formats the numerical value for the time string

    std::string make_time_str() const {
        time_point stop = clock::now();
        std::chrono::duration<double> elapsed = stop - start_;
        double time = elapsed.count();
        return make_time_str(time);
    }
        
    // LCOV_EXCL_START
    std::string make_time_str(double time) const {
        auto print_it = [](double x, std::string unit){
            char buffer[50];
            std::snprintf(buffer, 50, "%.5g", x);
            return buffer + std::string(" ") + unit;
        };
        
        if(time < .000001)
            return print_it(time*1000000000, "ns");
        else if(time < .001)
            return print_it(time*1000000, "us");
        else if(time < 1)
            return print_it(time*1000, "ms");
        else
            return print_it(time, "s");
    }
    // LCOV_EXCL_END

    /// This is the main function, it creates a string
    std::string to_string() const {
        return time_print_(title_, make_time_str());
    }
};



/// This class prints out the time upon destruction
class AutoTimer : public Timer {
public:
    /// Reimplementing the constructor is required in GCC 4.7
    AutoTimer(std::string title="Timer", time_print_t time_print = Simple)
                 : Timer(title, time_print) {}
    // GCC 4.7 does not support using inheriting constructors.

    /// This desctructor prints the string
    ~AutoTimer () {
        std::cout << to_string() << std::endl;
    }
};

}

/// This prints out the time if shifted into a std::cout like stream.
inline std::ostream & operator<< (std::ostream& in, const CLI::Timer& timer) {
    return in << timer.to_string();
}