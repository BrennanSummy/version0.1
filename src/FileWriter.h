#pragma once
#include "constants.h"
#include <iostream>
#include <fstream>
#include <string>

class FileWriter
{
    private:
    std::string m_fileName;
    const std::string m_subdir = "logs/bulk/";
    public:
    FileWriter(std::string fileName)
    {
        m_fileName = fileName;
        createFile();
    }
    void createFile()
    {
        std::ofstream f(dir + m_subdir + m_fileName + ".csv");
        f.close();
        
    }

    void insertLine(std::string line)
    {
        std::ofstream f(dir + m_subdir + m_fileName + ".csv",std::ios::app);
        f << line << "\n";
        f.close();
    }

    void setNewFileName(std::string fileName)
    {
        m_fileName = fileName;
    }

    std::string getName()
    {
        return m_fileName;
    }
};