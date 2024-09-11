#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>

void readDataAndCreateHistograms(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    std::string currentHybrid;
    std::vector<std::vector<double>> allData(8); // 8 separate histograms, 4 Top and 4 Bottom
    int hybridIndex = -1;

    // Create the histograms for each hybrid and a combined one
    std::vector<TH1F*> histograms;
    histograms.push_back(new TH1F("2SFEH40L-212000016_Top", "2SFEH40L-212000016 Top", 50, 7, 15));
    histograms.push_back(new TH1F("2SFEH40L-212000023_Top", "2SFEH40L-212000023 Top", 50, 7, 15));
    histograms.push_back(new TH1F("2SFEH18R-212000066_Top", "2SFEH18R-212000066 Top", 50, 7, 15));
    histograms.push_back(new TH1F("2SFEH18L-212000087_Top", "2SFEH18L-212000087 Top", 50, 7, 15));
    histograms.push_back(new TH1F("2SFEH40L-212000016_Bottom", "2SFEH40L-212000016 Bottom", 50, 7, 15));
    histograms.push_back(new TH1F("2SFEH40L-212000023_Bottom", "2SFEH40L-212000023 Bottom", 50, 7, 15));
    histograms.push_back(new TH1F("2SFEH18R-212000066_Bottom", "2SFEH18R-212000066 Bottom", 50, 7, 15));
    histograms.push_back(new TH1F("2SFEH18L-212000087_Bottom", "2SFEH18L-212000087 Bottom", 50, 7, 15));

    TH1F* combinedHistogram = new TH1F("All_Hybrids", "Combined Histogram for All Hybrids", 50, 7, 15);

    while (getline(file, line)) {
        std::istringstream iss(line);

        if (line.find("Top") != std::string::npos || line.find("Bottom") != std::string::npos) {
            // New hybrid section, determine index
            if (line.find("2SFEH40L-212000016 Top") != std::string::npos) {
                hybridIndex = 0;
            } else if (line.find("2SFEH40L-212000023 Top") != std::string::npos) {
                hybridIndex = 1;
            } else if (line.find("2SFEH18R-212000066 Top") != std::string::npos) {
                hybridIndex = 2;
            } else if (line.find("2SFEH18L-212000087 Top") != std::string::npos) {
                hybridIndex = 3;
            } else if (line.find("2SFEH40L-212000016 Bottom") != std::string::npos) {
                hybridIndex = 4;
            } else if (line.find("2SFEH40L-212000023 Bottom") != std::string::npos) {
                hybridIndex = 5;
            } else if (line.find("2SFEH18R-212000066 Bottom") != std::string::npos) {
                hybridIndex = 6;
            } else if (line.find("2SFEH18L-212000087 Bottom") != std::string::npos) {
                hybridIndex = 7;
            }
        } else {
            // Parse the pull force data, remove the 'g' and convert to double
            if (!line.empty() && hybridIndex != -1) {
                try {
                    // Remove 'g' from the string and convert to double
                    line = line.substr(0, line.find('g'));
                    double value = std::stod(line);

                    // Fill the corresponding histogram
                    float correction_factor = 1.7;
                    histograms[hybridIndex]->Fill(value*correction_factor);
                    combinedHistogram->Fill(value*correction_factor);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Warning: Invalid number format in line: " << line << std::endl;
                }
            }
        }
    }

    file.close();

    // Draw each histogram in a separate canvas
    std::vector<TCanvas*> canvases;
    for (int i = 0; i < histograms.size(); i++) {
        std::string canvasName = "Canvas_" + std::to_string(i + 1);
        canvases.push_back(new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 400));
        canvases[i]->cd();
        histograms[i]->Draw();
        
        // Calculate and display mean, standard deviation, and ratio for individual histograms
        double mean = histograms[i]->GetMean();
        double stddev = histograms[i]->GetStdDev();
        double ratio = stddev / mean;
        std::ostringstream stats;
        stats << std::fixed << std::setprecision(3) << "s/#mu: " << ratio;

        TPaveText *pt = new TPaveText(0.1, 0.8, 0.3, 0.9, "NDC");
        // Set the font and font size
        pt->SetTextFont(42);  // Use Helvetica font (normal)
        pt->SetTextSize(0.04); // Set font size
        // Set the panel to white and remove the shadow
        pt->SetFillColor(10);   // Set background color to white
        pt->SetShadowColor(0);  // Remove the shadow
        pt->SetBorderSize(1);   // Optional: set the border size

        pt->AddText(stats.str().c_str());
        pt->Draw();
        
        std::string outputFileName = canvasName + ".png";
        canvases[i]->SaveAs(outputFileName.c_str());
    }

    // Draw the combined histogram in its own canvas
    TCanvas* combinedCanvas = new TCanvas("CombinedCanvas", "Combined Histogram", 600, 400);
    combinedCanvas->cd();
    combinedHistogram->Draw();
    combinedCanvas->SaveAs("Combined_Histogram.png");
}

int main() {
    readDataAndCreateHistograms("2sfeh_august24.txt");
    return 0;
}

