/*
 * CSVReader.hpp
 *
 *  Created on: 25 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_CSVREADER_HPP_
#define INCLUDE_INPUTS_CSVREADER_HPP_

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>

// Trim whitespace from a string
std::string trim(const std::string &s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) start++;
    auto end = s.end();
    do { end--; } while (std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
}

// Split a string by a delimiter
std::vector<std::string> split(const std::string &s, char delimiter = ';') {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

class CSVStreamReader {
public:
    CSVStreamReader(const std::string &filename, char delimiter = ';')
        : delimiter_(delimiter), file_(filename) {
        if (!file_.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        readHeader();
    }

    // Returns true if a row was read, false on EOF
    bool nextRow(std::vector<std::string> &row) {
        std::string line;
        if (!std::getline(file_, line)) {
            return false;
        }
        if (line.empty()) {
            return nextRow(row); // Skip empty lines
        }
        row = split(line, delimiter_);
        if (row.size() != header_.size()) {
            std::cerr << "Warning: Row has " << row.size()
                      << " columns, expected " << header_.size() << "\n";
            // Pad or truncate to match header size (optional)
            row.resize(header_.size());
        }
        return true;
    }

    // Get the index of a column by name (throws if not found)
    size_t getColumnIndex(const std::string &name) const {
        auto it = headerIndex_.find(name);
        if (it == headerIndex_.end()) {
            throw std::runtime_error("Column not found: " + name);
        }
        return it->second;
    }

    // Check if a column exists
    bool hasColumn(const std::string &name) const {
        return headerIndex_.find(name) != headerIndex_.end();
    }

    // Get the header names
    const std::vector<std::string> &header() const { return header_; }

private:
    void readHeader() {
        std::string line;
        if (!std::getline(file_, line)) {
            throw std::runtime_error("Empty file or missing header");
        }
        header_ = split(line, delimiter_);
        for (size_t i = 0; i < header_.size(); ++i) {
            headerIndex_[header_[i]] = i;
        }
    }

    char delimiter_;
    std::ifstream file_;
    std::vector<std::string> header_;
    std::unordered_map<std::string, size_t> headerIndex_;
};

// CSV Reader class
class CSVReader {
public:
    CSVReader(char delimiter = ';') : delimiter_(delimiter) {}

    // Read a CSV file with a header row
    std::vector<std::unordered_map<std::string, std::string>> read(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        std::vector<std::unordered_map<std::string, std::string>> rows;
        std::string line;

        // Read header
        if (!std::getline(file, line)) {
            throw std::runtime_error("Empty file or missing header: " + filename);
        }
        auto headers = split(line, delimiter_);
        if (headers.empty()) {
            throw std::runtime_error("Empty header in file: " + filename);
        }

        // Read data rows
        while (std::getline(file, line)) {
            if (line.empty()) continue; // Skip empty lines
            auto values = split(line, delimiter_);
            if (values.size() != headers.size()) {
                std::cerr << "Warning: Row has " << values.size()
                          << " columns, expected " << headers.size() << ". Skipping.\n";
                continue;
            }

            std::unordered_map<std::string, std::string> row;
            for (size_t i = 0; i < headers.size(); ++i) {
                row[headers[i]] = values[i];
            }
            rows.push_back(row);
        }

        return rows;
    }

    static auto getDouble (const std::unordered_map<std::string, std::string> &row, const std::string & key) {
        return row.count(key) ? std::stod(row.at(key)) : 0.0;
    };

private:
    char delimiter_;
};

#endif /* INCLUDE_INPUTS_CSVREADER_HPP_ */
