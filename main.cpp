#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

vector<float> readWavFile(string filename) {
    ifstream file(filename, ios::binary | ios::in);
    file.seekg(44);

    vector<float> samples;
    while (!file.eof()) {
        float sample;
        file.read(reinterpret_cast<char*>(&sample), sizeof(float));
        samples.push_back(sample);
    }

    file.close();
    return samples;
}

string decodeMorseCode(vector<float> samples) {
    string morseCode;
    int dotCount = 0;
    for (int i = 0; i < samples.size(); i++) {
        if (samples[i] > 0) {
            dotCount++;
        }
    }
    int sizeOfSymbol = 0;
    vector<float> durations;
    for (int i = 0; i < samples.size(); i++) {
        float sample = samples[i];
        int duration;
        if (sample > 0) {
            duration = 1;
            while (i < samples.size() && samples[i + 1] > 0) {
                duration++;
                i++;
            }
        } else {
            duration = 0;
            while (i < samples.size() - 1 && samples[i + 1] <= 0) {
                duration++;
                i++;
            }
        }
        durations.push_back(duration);
    }
    int space = durations[0];
    int nextSymbol = durations[1];

    for (int i = 2; i < durations.size()-1; i++) {
        if (durations[i] > space) {
            nextSymbol = space;
            space = durations[i];
        } else if (durations[i] > nextSymbol && durations[i] != space) {
            nextSymbol = durations[i];
        }
    }
    durations[durations.size()-1] = durations[durations.size()-1]*2;
    if(nextSymbol < space/5) {
        nextSymbol = space;
        space = space * 5;
    }
    vector<float> dots;
    for (int i = 0; i < durations.size(); i++) {
        if (durations[i] >= nextSymbol) {
            dots.push_back(sizeOfSymbol);
            sizeOfSymbol = 0;
        } else { sizeOfSymbol += 1; }
    }
    int  dash = dots[1];
    int  dot = dots[2];

    for (int i = 2; i < dots.size(); i++) {
        if (dots[i] > dash) {
            dot = dash;
            dash = dots[i];
        } else if (dots[i] > dot && dots[i] != dash) {
            dot = dots[i];
        }
    }
    int countOfSymbols = 0;
    for (int i = 0; i < durations.size(); i++) {
        if (durations[i] >= nextSymbol) {
            if (dots[countOfSymbols] >= dash - 5) {
                morseCode += "-";
                if (durations[i] == space) {
                    morseCode += " ";
                }
            } else if (dots[countOfSymbols] >= dot - 5 && dots[countOfSymbols] < dash - 5) {
                morseCode += ".";
                if (durations[i] == space) {
                    morseCode += " ";
                }
            }
            countOfSymbols++;
        }
    }

    return morseCode;
}

string decodeMorseToText(string morseCode) {
    string morseAlphabet[36] = {
            ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
            "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
            "..-", "...-", ".--", "-..-", "-.--", "--..",
            ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.", "-----"
    };

    string text;

    size_t start = 0;
    size_t end = 0;
    while ((end = morseCode.find(' ', start)) != string::npos) {
        string symbol = morseCode.substr(start, end - start);
        for (int i = 0; i < 36; i++) {
            if (morseAlphabet[i] == symbol) {
                text += static_cast<char>(i + 65);
                break;
            }
        }
        start = end + 1;
    }
    string symbol = morseCode.substr(start);
    for (int i = 0; i < 36; i++) {
        if (morseAlphabet[i] == symbol) {
            text += static_cast<char>(i + 65);
            break;
        }
    }

    return text;
}

int main() {
    string filename = "Feel.wav";
    vector<float> samples = readWavFile(filename);

    string morseCode = decodeMorseCode(samples);
    string text = decodeMorseToText(morseCode);

    cout << "Code: " << morseCode << endl;
    cout << "Text: " << text << endl;

    return 0;
}

