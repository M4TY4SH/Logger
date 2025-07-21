#include "/home/vboxuser/work/include/lib.h"
#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <sstream>

struct LogMessage{
    std::string text;
    Levels level;
};

std::queue<LogMessage> messageQueue;
std::mutex queueMutex;
std::condition_variable conVar;
std::atomic<bool> exitFlag(false);

void writerThread(Logger& Logger){
    while(!exitFlag || messageQueue.empty()){
        std::unique_lock<std::mutex> lock(queueMutex);
        conVar.wait(lock, []{return !messageQueue.empty() || exitFlag;});
        
        while (!messageQueue.empty())
        {
            auto msg = messageQueue.front();
            messageQueue.pop();
            lock.unlock();

            try
            {
                Logger.log(msg.text, msg.level);
            }
            catch(const std::exception& e)
            {
                std::cerr << "Logging error" << e.what() << std::endl;
            }
            

            lock.lock();
        }
    }
}

Levels parseLvl(const std::string& levelStr){
    if (levelStr == "DEBUG") return Levels::DEBUG;
    if (levelStr == "INFO") return Levels::INFO;
    if (levelStr == "ERROR") return Levels::ERROR;
    throw std::invalid_argument("Unknown log level " + levelStr);
}

int main(int argc, char* argv[]) {

    if (argc < 3){
        std::cerr << "Usage: " << argv[0] << " <logfile> <loglevel> [DEBUG|INFO|ERROR]" << std::endl;
        return 1;
    }



    std::string logfile = argv[1];
    Levels logLevel;

    try {
        logLevel = parseLvl(argv[2]);
    }catch (const std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    try {
        Logger logger(logfile, logLevel);
        std::thread writer(writerThread, std::ref(logger));

        std::cout << "Logger started. Enter messages in format: [LEVEL] message" << std::endl;
        std::cout << "Example: ERROR Something went wrong" << std::endl;
        std::cout << "Or just message for default level (INFO)" << std::endl;
        std::cout << "Type 'exit' to quit" << std::endl;
        std::string input;
        while (true)
        {
            std::getline(std::cin, input);
            if(input == "exit") return false;

            std::istringstream iss(input);
            std::string firstWord;
            iss >> firstWord;

            LogMessage msg;
            try{
                msg.level = parseLvl(firstWord);
                std::string message;
                std::getline(iss, message);
                msg.text = message.empty() ? message : message.substr(1);
            } catch(...){
                msg.level = Levels::INFO;
                msg.text = input;
            }

            conVar.notify_one();
        }
        
        exitFlag = true;
        conVar.notify_one();
        writer.join();
    }catch (const std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}