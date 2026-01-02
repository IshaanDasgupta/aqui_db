#pragma once
#include <vector>
#include <string>

namespace utils{
    inline static std::vector<std::string> split(const std::string& str, char delimitter){
        std::vector<std::string> segments;
        std::string currSegment = "";

        for (int i=0 ; i<str.size() ; i++){
            if (str[i] == delimitter){
                if (currSegment.size()){
                    segments.push_back(currSegment);
                }
                currSegment = "";
                continue;
            }

            currSegment.push_back(str[i]);
        }

        if (currSegment.size()){
            segments.push_back(currSegment);
        }

        return segments;
    };
};