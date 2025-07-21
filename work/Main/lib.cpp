#include "/home/vboxuser/work/include/lib.h"
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <filesystem>

Logger::Logger(const std::string& filename, Levels level)
    : filename(filename), currentLevel(level) {
        std::filesystem::path p(filename);
        if(p.has_parent_path()){
            std::filesystem::create_directories(p.parent_path());  
        }

        std::cout << "Log file path: " << std::filesystem::absolute(filename) << std::endl;
        logFile.open(filename, std::ios::app);
        if (!logFile.is_open())
        {
            throw std::runtime_error("Cannot open log file: " + filename);
        }
        log("Logger initialized", Levels::INFO);
    }
    std::string getCurrTime(){
        auto currTime = std::chrono::system_clock::now();
        auto inTimeT = std::chrono::system_clock::to_time_t(currTime);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&inTimeT), "%Y-%m-%d %X");
        return ss.str();
    }

    Logger::~Logger(){
        log("Logger shuting down", Levels::INFO);
        if (logFile.is_open())
        {
            logFile.close();
        }
    
    }

    void Logger::setLogLvl(Levels level){
        std::lock_guard<std::mutex> lock(writeMutex);
        currentLevel = level;
        log("Log level change to " + levelToString(level), Levels::DEBUG);
    }

    void Logger::log(const std::string& message, Levels level){
        std::ofstream tmp("test.log", std::ios::app);
        tmp << "TEST" << std::endl;
        tmp.close();
        if (level < currentLevel) return;

        std::lock_guard<std::mutex> lock(writeMutex);
        
        std::cout << "Attemping to log: " << message << std::endl;

        if(!logFile.is_open()){
            try
            {
                logFile.open(filename, std::ios::app);
                if (!logFile.is_open())
                {
                    throw std::runtime_error("Can't reopen log file");
                }
                
            }
            catch(const std::exception& e)
            {
                std::cerr << "Failed to reopen log file: " << e.what() << std::endl;
                return;
            }
        }

        try {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);

            struct tm time_info;
            localtime_r(&in_time_t, &time_info);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &time_info);

            logFile << "[" << buffer << "] "
                    << "[" << levelToString(level) << "] "
                    << message << std::endl;

                    logFile.flush();
        } catch(const std::exception& e){
            std::cerr << "Logging failed: " << e.what() << std::endl;
            logFile.close();
        }
    }