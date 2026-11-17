/******************************
* Автор: Чеботников Александр *
* Задание: Работа с файлами   *
* Вариант: 30                 *
******************************/

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Bicycle {
  string model;
  string type;
  int availableCount;
  double hourlyPrice;
};

struct BicycleStatistics {
  long long totalAvailable;
  double hourlyRevenue;
  bool hasMatching;
  bool hasCheapest;
  string cheapestModel;
  double cheapestPrice;
};

vector<string> splitFields(const string& line) {
  vector<string> fields;
  size_t start;
  size_t separator;

  start = 0;

  while (true) {
    separator = line.find(';', start);

    if (separator == string::npos) {
      fields.push_back(line.substr(start));
      break;
    }

    fields.push_back(line.substr(start, separator - start));
    start = separator + 1;
  }

  return fields;
}

bool readInteger(const string& text, int& value) {
  istringstream parser(text);
  char extra;

  if (!(parser >> value)) {
    return false;
  }

  return !(parser >> extra);
}

bool readDouble(const string& text, double& value) {
  istringstream parser(text);
  char extra;

  if (!(parser >> value) || !isfinite(value)) {
    return false;
  }

  return !(parser >> extra);
}

bool readBicycles(const string& inputPath, vector<Bicycle>& bicycles, string& error) {
  ifstream input(inputPath);
  string line;
  int lineNumber;

  if (!input) {
    error = "ERROR input";
    return false;
  }

  lineNumber = 0;

  while (getline(input, line)) {
    ++lineNumber;

    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    vector<string> fields;

    fields = splitFields(line);

    if (fields.size() != 4 ||
      fields[0].find_first_not_of(" \t") == string::npos ||
      fields[1].find_first_not_of(" \t") == string::npos) {
      error = "ERROR line " + to_string(lineNumber);
      return false;
    }

    Bicycle bicycle;

    if (!readInteger(fields[2], bicycle.availableCount) ||
      !readDouble(fields[3], bicycle.hourlyPrice) ||
      bicycle.availableCount < 0 || bicycle.hourlyPrice < 0 ||
      bicycles.size() >= 100) {
      error = "ERROR line " + to_string(lineNumber);
      return false;
    }

    bicycle.model = fields[0];
    bicycle.type = fields[1];

    bicycles.push_back(bicycle);
  }

  if (input.bad()) {
    error = "ERROR read";
    return false;
  }

  return true;
}

BicycleStatistics calculateStatistics(const vector<Bicycle>& bicycles, const string& requestedType) {
  BicycleStatistics statistics;

  statistics.totalAvailable = 0;
  statistics.hourlyRevenue = 0.0;
  statistics.hasMatching = false;
  statistics.hasCheapest = false;
  statistics.cheapestPrice = 0.0;

  for (size_t bicycleIndex = 0; bicycleIndex < bicycles.size(); ++bicycleIndex) {
    if (bicycles[bicycleIndex].type == requestedType) {
      statistics.hasMatching = true;
      statistics.totalAvailable += bicycles[bicycleIndex].availableCount;
      statistics.hourlyRevenue += bicycles[bicycleIndex].availableCount * bicycles[bicycleIndex].hourlyPrice;
    }

    if (!statistics.hasCheapest || bicycles[bicycleIndex].hourlyPrice < statistics.cheapestPrice) {
      statistics.hasCheapest = true;
      statistics.cheapestModel = bicycles[bicycleIndex].model;
      statistics.cheapestPrice = bicycles[bicycleIndex].hourlyPrice;
    }
  }

  return statistics;
}

string createReport(const vector<Bicycle>& bicycles, const string& requestedType,
  const BicycleStatistics& statistics) {
  ostringstream report;

  report << "Type: " << requestedType << '\n'
    << "Bicycles:\n";

  if (!statistics.hasMatching) {
    report << "EMPTY\n";
  }
  else {
    for (size_t bicycleIndex = 0; bicycleIndex < bicycles.size(); ++bicycleIndex) {
      if (bicycles[bicycleIndex].type == requestedType) {
        report << bicycles[bicycleIndex].model << ";"
               << bicycles[bicycleIndex].type << ";"
               << bicycles[bicycleIndex].availableCount << ";"
               << bicycles[bicycleIndex].hourlyPrice << '\n';
      }
    }
  }

  report << "Available: " << statistics.totalAvailable << '\n'
    << "Revenue per hour: " << statistics.hourlyRevenue << '\n';

  if (statistics.hasCheapest) {
    report << "Cheapest: " << statistics.cheapestModel
      << "; " << statistics.cheapestPrice << '\n';
  }
  else {
    report << "Cheapest: NONE\n";
  }

  return report.str();
}

bool writeReport(const string& outputPath, const string& reportText, string& error) {
  ofstream output(outputPath);

  if (!output) {
    error = "ERROR output";
    return false;
  }

  output << reportText;
  output.flush();

  if (!output) {
    error = "ERROR write";
    return false;
  }

  return true;
}

int main() {
  string inputPath;
  string outputPath;
  string requestedType;
  string error;
  string reportText;
  vector<Bicycle> bicycles;
  BicycleStatistics statistics;

  getline(cin, inputPath);
  getline(cin, outputPath);
  getline(cin, requestedType);

  if (!cin || inputPath.empty() || outputPath.empty() ||
    requestedType.find_first_not_of(" \t") == string::npos ||
    inputPath == outputPath) {
    cout << "INVALID\n";
    return 0;
  }

  if (!readBicycles(inputPath, bicycles, error)) {
    cout << error << '\n';
    return 1;
  }

  statistics = calculateStatistics(bicycles, requestedType);
  reportText = createReport(bicycles, requestedType, statistics);

  if (!writeReport(outputPath, reportText, error)) {
    cout << error << '\n';
    return 1;
  }

  cout << reportText;

  return 0;
}