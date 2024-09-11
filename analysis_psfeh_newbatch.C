#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>  // For std::setprecision
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TPaveText.h>  // Include for TPaveText

void readDataAndCreateHistograms(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    int hybridIndex = -1;

    // Create the histograms for each hybrid and a combined one
    std::vector<TH1F*> histograms;
    histograms.push_back(new TH1F("PSFEH40L-212000030", "PSFEH40L-212000030", 40, 7, 15));
    histograms.push_back(new TH1F("PSFEH40L-212000100", "PSFEH40L-212000100", 40, 7, 15));
    histograms.push_back(new TH1F("PSFEH40L-212000102", "PSFEH40L-212000102", 40, 7, 15));
    histograms.push_back(new TH1F("PSFEH40L-212000185", "PSFEH40L-212000185", 40, 7, 15));

    TH1F* combinedHistogram = new TH1F("All_Hybrids", "Combined Histogram for All Hybrids", 40, 7, 15);

    while (getline(file, line)) {
        // Print line for debugging
        std::cout << "Read line: " << line << std::endl;

        if (line.find("PSFEH40L-") != std::string::npos) {
            // New hybrid section, determine index
            if (line.find("PSFEH40L-212000030") != std::string::npos) {
                hybridIndex = 0;
            } else if (line.find("PSFEH40L-212000100") != std::string::npos) {
                hybridIndex = 1;
            } else if (line.find("PSFEH40L-212000102") != std::string::npos) {
                hybridIndex = 2;
            } else if (line.find("PSFEH40L-212000185") != std::string::npos) {
                hybridIndex = 3;
            } else {
                hybridIndex = -1; // Reset index if hybrid not found
            }
        } else if (hybridIndex != -1 && !line.empty()) {
            // Parse the pull force data, remove the 'g' and convert to double
            try {
                // Remove 'g' from the string and convert to double
                size_t pos = line.find('g');
                if (pos != std::string::npos) {
                    line = line.substr(0, pos);
                }
                double value = std::stod(line);

                // Print value and index for debugging
                std::cout << "Filling histogram " << hybridIndex << " with value " << value << std::endl;

                // Fill the corresponding histogram
                float correction_factor = 1.7;
                histograms[hybridIndex]->Fill(value * correction_factor);
                combinedHistogram->Fill(value * correction_factor);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Warning: Invalid number format in line: " << line << std::endl;
            }
        }
    }

    file.close();

    // Debug: Check histogram contents
    for (int i = 0; i < histograms.size(); ++i) {
        std::cout << "Histogram " << i << " entries: " << histograms[i]->GetEntries() << std::endl;
    }
    std::cout << "Combined histogram entries: " << combinedHistogram->GetEntries() << std::endl;

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
        pt->SetTextFont(42);  // Use Helvetica font (normal)
        pt->SetTextSize(0.04); // Set font size
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
    readDataAndCreateHistograms("psfeh_august24.txt");
    return 0;
}

