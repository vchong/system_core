/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <map>
#include <iostream>
#include <fstream>
#include <vector>

#include <android-base/strings.h>

std::vector<std::vector<std::string>> readBlocks(const std::string& path){
    std::ifstream file(path);
    std::string line;
    std::vector<std::string> block;
    std::vector<std::vector<std::string>> blocks;

    if (!file) {
        std::cout << "unable to open file " << path << "\n";
        return blocks;
    }

    while (std::getline(file, line)){
        if (line.empty()){
            blocks.push_back(block);
            block.resize(0);
        }else{
            block.push_back(line);
        }
    }
    file.close();

    return blocks;
}
int main(int argc, char** argv) {

    std::string log_procs("/data/bootchart/proc_ps.log");
    auto blocks = readBlocks(log_procs);
    if (blocks.size() == 0 ) return 1;

    std::map<std::string, std::string> proc_map;
    std::map<std::string, std::map<std::string, std::string>> procs_map;
    for (auto &b: blocks ){
        //long long uptime = stoll(b[0]);
        for (std::size_t i = 1; i < b.size(); i++ ){
            std::vector<std::string> components = android::base::Split(b[i], " ");
            //  0: pid
            //  1: process name
            // 17: priority
            // 18: nice
            // 21: creation time
            std::string proc_name = components[1];
            proc_name.erase(std::remove(proc_name.begin(), proc_name.end(), '('), proc_name.end());
            proc_name.erase(std::remove(proc_name.begin(), proc_name.end(), ')'), proc_name.end());

            std::string start_time = components[21];
            auto proc_map = procs_map.find(proc_name);
            if (proc_map != procs_map.end()){
                proc_map->second.find("last_uptime")->second = b[0];
            }else{
                std::map<std::string, std::string> proc_map;
                proc_map.insert(std::make_pair("proc_name", proc_name));
                proc_map.insert(std::make_pair("start_time", start_time));
                proc_map.insert(std::make_pair("last_uptime", b[0]));
                procs_map.insert(std::make_pair(proc_name, proc_map));
            }
        }
    }

    std::vector<std::string> processes_of_interest = {
        // the start time for init is not correct, it's not the point that the userspace /init get started
        // "/init",
        "/system/bin/surfaceflinger",
        "/system/bin/bootanimation",
        "zygote64",
        "zygote",
        "system_server"
    };

    for (auto& proc_name: processes_of_interest){
        auto proc_map = procs_map.find(proc_name);
        if (proc_map != procs_map.end()) {
            auto start_time = (proc_map->second)["start_time"];
            auto last_uptime = (proc_map->second)["last_uptime"];
            std::cout << proc_name << "," << strtol(start_time.c_str(), NULL, 10) * 10 << "," << strtol(last_uptime.c_str(), NULL, 10) * 10 << std::endl;
        }
    }
    return 0;
}
