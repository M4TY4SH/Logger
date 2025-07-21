#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>


enum class Levels{
    DEBUG,
    INFO,
    ERROR
};

class Logger {
    std::ofstream logFile;
    Levels currentLevel;
    std::mutex writeMutex;
    std::string filename;

    
    std::string levelToString(Levels level){
        switch (level)
        {
        case Levels::DEBUG: return "DEBUG";
        case Levels::INFO: return "INFO";
        case Levels::ERROR: return "ERROR";
        default: return "UNKNOWN";
        }
    };
    std::string getCurrTime();

    public:
        Logger(const std::string& filename, Levels level = Levels::INFO);
        ~Logger();
        void setLogLvl(Levels level);
        void log(const std::string& message, Levels level = Levels::INFO);
};