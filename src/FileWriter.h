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
    FileWriter(std::string fileName = "default")
    {
        m_fileName = fileName;
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

    void vectorToLine(std::string preamble, std::vector<int> vec)
    {
        std::ofstream f(dir + m_subdir + m_fileName + ".csv",std::ios::app);
        f << preamble;
        for (auto iter = vec.begin(); iter != vec.end(); iter++)
        {
            f << *iter << ",";
        }
        f<<"\n";

        f.close();
    }

    void saveBoardArray(const char (&board_ref)[2][bHeight][bWidth])
    {
        std::ofstream f(dir + m_subdir + m_fileName + ".csv",std::ios::app);

        for (int top = 0; top < 2; top++)
        {
            for (int j = 0; j < bHeight; j++)
            {
                for (int i = 0; i < bWidth; i++)
                {
                    // Write this element to the file
                    f << board_ref[top][j][i] << ",";
                }
                f << "\n";
            }
            f << "-\n";
        }
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