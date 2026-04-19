#ifndef SCANLOGGER_HPP
#define SCANLOGGER_HPP

#include <fstream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <sstream>
#include <cstdlib>

class ScanLogger {
private:
    std::string scan_dir = "scan_data";
    std::string log_dir = "scan_logs";
    std::ofstream scan_file, txt_file;
    std::vector<double> scan_times;
    int scan_count = 0;
    std::chrono::high_resolution_clock::time_point program_start;
public:
    ScanLogger() : program_start(std::chrono::high_resolution_clock::now()) {
        char* scan_env = getenv("SCAN_DATA_DIR");
        if (scan_env) {
            scan_dir = scan_env;
        }
        
        char* log_env = getenv("SCAN_LOG_DIR");
        if (log_env) {
            log_dir = log_env;
        }

        // Make sure the folders have been created
        system(("mkdir -p " + scan_dir).c_str());
        system(("mkdir -p " + log_dir).c_str());

        // Gets current date so each csv name is unique
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);

        // Stepper running scan count csv
        std::ostringstream scanname;
        scanname << scan_dir << "/stepper_test_history__" << std::put_time(&tm, "%Y-%m-%d__%H-%M") << ".csv";

        scan_file.open(scanname.str());
        scan_file << "scan_count,timestamp_s,duration_s\n";
        scan_file.flush();

        std::cout << "Logging raster scan counts to: " << scanname.str() << std::endl;


        // Log Text file
        std::ostringstream txtname;
        txtname << log_dir << "/wallops_operation_log__" << std::put_time(&tm, "%Y-%m-%d__%H-%M") << ".txt";
        txt_file.open(txtname.str());
        txt_file << "[GOOD MORNING, WALLOPS!!!] " << std::put_time(&tm, "%Y-%m-%d__%H-%M") << "\n";
        std::cout << "Text logging to: " << txtname.str() << std::endl; 
    }

    ~ScanLogger() {
        scan_file.close();
        txt_file.close();
    }

    std::chrono::high_resolution_clock::time_point start_scan() {
        return std::chrono::high_resolution_clock::now();
    }

    void log_scan(std::chrono::high_resolution_clock::time_point scan_start_time) {
        auto scan_end = std::chrono::high_resolution_clock::now();
        
        double program_elapsed_s = std::chrono::duration<double>(scan_end - program_start).count();
        
        double scan_duration_s = std::chrono::duration<double>(scan_end - scan_start_time).count();

        scan_times.push_back(scan_duration_s);
        scan_count++;

        scan_file << scan_count << "," << program_elapsed_s << "," << scan_duration_s << "\n";
        scan_file.flush();

        std::cout << "[LOG] Scan # " << scan_count << "  | Duration: " << scan_duration_s << " s | Total time: " << program_elapsed_s << "s\n";

        log("Scan # " + std::to_string(scan_count) + " complete");
    }

    void log(const std::string& message) {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed_s = std::chrono::duration<double>(now - program_start).count();
        std::string timestamp = "[" + std::to_string(elapsed_s).substr(0,8) + "s] " + message;

        std::cout << timestamp << "\n";
        txt_file << timestamp << "\n";
        txt_file.flush();
    }

    void print_summary () {
        if (scan_times.empty()) {
            std::cout << "No scans completed." << std::endl;
            return;
        }

        double mean_time = 0;
        double min_time = scan_times[0];
        double max_time = scan_times[0];

        for (double t : scan_times) {
            mean_time += t;

            if (t < min_time) {
                min_time = t;
            }
            if (t > max_time) {
                max_time = t;
            }
        }

        mean_time /= scan_times.size();

        auto total_runtime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - program_start).count();

        std::cout << "\n\n[RASTER TEST SUMMARY]\n";
        
        std::cout << "Total scans: " << scan_count << std::endl;
        std::cout << "Min scan time: " << min_time << "s" << std::endl;
        std::cout << "Mean scan time: " << mean_time << "s" << std::endl;
        std::cout << "Max scan time: " << max_time << "s" << std::endl;
        std::cout << "Total runtime: " << total_runtime << "s" << std::endl;
        std::cout << "Scans per min: " << (scan_count * 60.0 / total_runtime) << std::endl;
    }
};

#endif