#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <TH1D.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TLegend.h>

struct Measurement {
    std::string sensorID;
    std::vector<double> values;
};

std::vector<Measurement> readData(const std::string& filename) {
    std::vector<Measurement> data;
    std::ifstream infile(filename);
    std::string line;
    Measurement currentMeasurement;

    while (std::getline(infile, line)) {
        if (line.empty()) {
            continue;
        }
        if (line.find("2SFEH40R") != std::string::npos) {
            if (!currentMeasurement.sensorID.empty()) {
                data.push_back(currentMeasurement);
                currentMeasurement = Measurement();
            }
            currentMeasurement.sensorID = line;
        } else {
            line.erase(line.find(" g"), 2); // Remove the ' g' suffix
            double value = std::stod(line);
            float correction_factor = 1.6;
            double corrected_value = correction_factor*value;
            currentMeasurement.values.push_back(corrected_value);
        }
    }
    if (!currentMeasurement.sensorID.empty()) {
        data.push_back(currentMeasurement);
    }

    return data;
}

void createHistograms(const std::vector<Measurement>& data) {
    TCanvas* canvas = new TCanvas("canvas", "Histograms", 1600, 1200);
    canvas->Divide(2, 2); // Divide canvas into a 2x2 grid

    int pad = 1;
    TH1D* combinedHist = new TH1D("combinedHist", "Pull Force Histogram;Value (g);Number of WB", 50, 8, 13);
    std::vector<TH1D*> histograms;

    for (const auto& measurement : data) {
        std::string histName = "hist_" + measurement.sensorID;
        TH1D* hist = new TH1D(histName.c_str(), (measurement.sensorID + ";Value (g);Number of WB").c_str(), 50, 8, 13);

        for (const auto& value : measurement.values) {
            hist->Fill(value);
            combinedHist->Fill(value);
        }

        histograms.push_back(hist);
        canvas->cd(pad);
        hist->Draw();

        // Calculate and display mean, standard deviation, and ratio
        double mean = hist->GetMean();
        double stddev = hist->GetStdDev();
        double ratio = stddev / mean;
        std::ostringstream stats;
        stats  << std::fixed << std::setprecision(3)<< "s/#mu: " << ratio;

        TPaveText *pt1 = new TPaveText(0.1, 0.8, 0.3, 0.9, "NDC");
        // Set the font and font size
         pt1->SetTextFont(42);  // Use Helvetica font (normal)
         pt1->SetTextSize(0.04); // Set font size
        // Set the panel to white and remove the shadow
         pt1->SetFillColor(10);   // Set background color to white
         pt1->SetShadowColor(0);  // Remove the shadow
         pt1->SetBorderSize(1);   // Optional: set the border size

        pad++;
    }

    TCanvas* combinedCanvas = new TCanvas("combinedCanvas", "Pull force for all 2SFEHs", 800, 600);
    combinedHist->Draw();
    combinedCanvas->SaveAs("all2SFEH.png");

    /*
    // Save individual histograms
    for (size_t i = 0; i < histograms.size(); ++i) {
        std::string filename = "histogram_" + data[i].sensorID + ".png";
        canvas->cd(i+1);
        histograms[i]->Draw();

        // Calculate and display mean, standard deviation, and ratio for individual histograms
        double mean = histograms[i]->GetMean();
        double stddev = histograms[i]->GetStdDev();
        double ratio = stddev / mean;
        std::ostringstream stats;
        stats  << std::fixed << std::setprecision(3)<< "s/#mu: " << ratio;

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

        canvas->SaveAs(filename.c_str());
    }
     */
    
    for (size_t i = 0; i < histograms.size(); ++i) {
        // Create a new canvas for each histogram
        TCanvas *canvas = new TCanvas(("canvas_" + std::to_string(i)).c_str(), ("Canvas " + std::to_string(i)).c_str(), 800, 600);

        std::string filename = "histogram_" + data[i].sensorID + ".png";
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

        canvas->SaveAs(filename.c_str());

        // Clean up
        delete canvas;
    }

    

    canvas->SaveAs("all_2SFEH.png");
}

int main() {
    std::string inputFilename = "data_2sfeh_july24.txt";  // Adjust filename as necessary

    auto data = readData(inputFilename);
    createHistograms(data);

    return 0;
}

