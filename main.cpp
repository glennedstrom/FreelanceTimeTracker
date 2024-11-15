#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <algorithm>

using json = nlohmann::json;

// Function to load the JSON data from a file
json loadData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return json({});
    }
    json data;
    file >> data;
    file.close();
    return data;
}

// Function to save the JSON data to a file
void saveData(const std::string& filename, const json& data) {
    std::ofstream file(filename);
    file << std::setw(4) << data << std::endl;
    file.close();
}

// Function to display the list of jobs
void displayJobs(const json& data) {
    std::cout << "Available Jobs:" << std::endl;
    for (size_t i = 0; i < data["freelancer"]["companies"].size(); ++i) {
        std::cout << i + 1 << ". " << data["freelancer"]["companies"][i]["companyName"] << std::endl;
    }
}

// Function to add a new job
void addJob(json& data) {
    std::string companyName;
    std::cout << "Enter the company name: ";
    std::cin.ignore();
    std::getline(std::cin, companyName);

    json newCompany = {
        {"companyName", companyName},
        {"sessions", json::array()}
    };

    data["freelancer"]["companies"].push_back(newCompany);
}

// Function to display the current time
void displayCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::cout << "Current Time: " << std::ctime(&currentTime);
}

// Function to check if there is an active session
void checkActiveSession(const json& data) {
    int activeCompanyIndex = data["freelancer"]["activeCompanyIndex"];
    int activeSessionIndex = data["freelancer"]["activeSessionIndex"];

    if (activeCompanyIndex >= 0 && activeSessionIndex >= 0) {
        std::string companyName = data["freelancer"]["companies"][activeCompanyIndex]["companyName"];
        std::string startTimestamp = data["freelancer"]["companies"][activeCompanyIndex]["sessions"][activeSessionIndex]["startTimestamp"];

        std::cout << "Active Session: " << companyName << std::endl;
        std::cout << "Started at: " << startTimestamp << std::endl;
    } else {
        std::cout << "No active session." << std::endl;
    }
}

// Function to start a new session
void startSession(json& data) {
    displayJobs(data);
    checkActiveSession(data); // Display if there's an active session

    size_t companyIndex;
    std::cout << "Select a job (1-" << data["freelancer"]["companies"].size() << ") or enter 0 to create a new job: ";
    std::cin >> companyIndex;

    if (companyIndex > 0) {
        companyIndex -= 1;
    } else {
        addJob(data); // Add new job if 0 is selected
        companyIndex = data["freelancer"]["companies"].size() - 1;
    }

    std::string description = "";
    double pay;
    std::cout << "Enter the pay rate per hour: ";
    std::cin >> pay;

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::string startTimestamp = std::ctime(&currentTime);
    startTimestamp.pop_back();
    std::cout << "START TIME STAMP: " << startTimestamp << std::endl;

    json newSession = {
        {"sessionId", std::to_string(data["freelancer"]["companies"][companyIndex]["sessions"].size() + 1)},
        {"description", description},
        {"pay", pay},
        {"startTimestamp", startTimestamp},
        {"endTimestamp", ""},
        {"billingStatus", "Pending"}
    };

    data["freelancer"]["companies"][companyIndex]["sessions"].push_back(newSession);
    data["freelancer"]["activeCompanyIndex"] = companyIndex;
    data["freelancer"]["activeSessionIndex"] = data["freelancer"]["companies"][companyIndex]["sessions"].size() - 1;

    std::cout << "Session started.\n";
    saveData("freelancer_data.json", data);
}

// Function to clock out
void clockOut(json& data) {
    int activeCompanyIndex = data["freelancer"]["activeCompanyIndex"];
    int activeSessionIndex = data["freelancer"]["activeSessionIndex"];

    if (activeCompanyIndex == -1 || activeSessionIndex == -1) {
        std::cout << "No active sessions.\n";
        return;
    }

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::string endTimestamp = std::ctime(&currentTime);
    endTimestamp.pop_back();

    std::string description;
    std::cout << "Enter a brief description of the work done: ";
    std::cin.ignore();
    std::getline(std::cin, description);

    data["freelancer"]["companies"][activeCompanyIndex]["sessions"][activeSessionIndex]["description"] = description;
    data["freelancer"]["companies"][activeCompanyIndex]["sessions"][activeSessionIndex]["endTimestamp"] = endTimestamp;

    data["freelancer"]["activeCompanyIndex"] = -1; // Reset active session
    data["freelancer"]["activeSessionIndex"] = -1;

    std::cout << "Session ended.\n";
    saveData("freelancer_data.json", data);
}

// Function to show the last X sessions
void showLastSessions(const json& data, int numSessions) {
    std::vector<json> allSessions;
    for (auto& company : data["freelancer"]["companies"]) {
        std::string companyName = company["companyName"];  // Store company name

        for (auto& session : company["sessions"]) {
            // Create a new session object that includes the company name
            json sessionWithCompanyName = session;
            sessionWithCompanyName["companyName"] = companyName;  // Add company name to session

            allSessions.push_back(sessionWithCompanyName);
        }
    }

    // Sort in reverse chronological order based on start timestamp
    std::sort(allSessions.begin(), allSessions.end(), [](const json& a, const json& b) {
        return a["startTimestamp"] < b["startTimestamp"];
    });

    int count = 0;
    for (auto& session : allSessions) {
        if (count >= numSessions) {
            break;
        }
        std::cout << "Company: " << session["companyName"] << std::endl; 
        std::cout << "Description: " << session["description"] << std::endl;
        std::cout << "Start Time: " << session["startTimestamp"] << std::endl;
        std::cout << "End Time: " << session["endTimestamp"] << std::endl;
        std::cout << "Pay: " << session["pay"] << std::endl;
        std::cout << "---------------------------------" << std::endl;
        count++;
    }
}

int main() {
    const std::string filename = "freelancer_data.json";
    json data = loadData(filename);

    if (data.empty()) {
        data["freelancer"] = {
            {"name", "John Doe"},
            {"companies", json::array()},
            {"totalEarnings", 0.0},
            {"currency", "USD"},
            {"activeCompanyIndex", -1},
            {"activeSessionIndex", -1}
        };
    }

    bool running = true;
    while (running) {
        displayCurrentTime(); // Show current time at the start of the loop
        checkActiveSession(data); // Check and display active session info

        std::cout << "1. Start a new session\n2. Clock out\n3. Display Sessions\n4. Exit\n";
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                startSession(data);
                break;
            case 2:
                clockOut(data);
                break;
            case 3:
                std::cout << "How many sessions: " << std::endl;
                int sessions;
                std::cin >> sessions;
                showLastSessions(data, sessions);
                break;
            case 4:
                running = false;
                break;
            default:
                std::cout << "Invalid choice.\n";
                break;
        }
    }

    saveData(filename, data);
    return 0;
}

