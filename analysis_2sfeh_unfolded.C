#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <TH1F.h>
#include <TFile.h>
#include <TCanvas.h>

using namespace std;

// Function to apply the correction factor based on the hybrid name
double applyCorrectionFactor(const string& hybridName, double value) {
    if (hybridName == "2SFEH-L-40-CYNR-465004-1-Top" || hybridName == "2SFEH-L-40-CYNR-465004-1-Bottom") {
        return value * 1.7;
    } else {
        return value * 1.53;
    }
}

// Function to read and process the data
void processHybridData(const string& hybridName, const vector<double>& data, map<string, vector<double>>& hybridData, vector<double>& allData) {
    for (double value : data) {
        double correctedValue = applyCorrectionFactor(hybridName, value);
        hybridData[hybridName].push_back(correctedValue);
        allData.push_back(correctedValue);
    }
}

int main() {
    // The hybrid data
    map<string, vector<double>> hybridData;
    vector<double> allData;

    // Hybrid names and their associated data
    map<string, vector<double>> inputData = {
        {"2SFEH-L-40-CYNR-465004-1-Top", {7.163, 6.157, 6.308, 5.593, 5.325, 5.467, 5.253, 5.631, 5.658, 5.315, 5.615, 6.058, 5.535, 5.441, 5.440, 5.466, 5.385, 5.658, 5.132, 5.629, 5.200, 5.452, 5.523, 5.684, 6.185, 5.778, 5.841, 5.354}},
        {"2SFEH-L-40-CYNR-465004-1-Bottom", {5.724, 5.227, 5.641, 5.798, 5.951, 6.370, 5.111, 5.516, 5.370, 5.646, 5.901, 6.168, 5.339, 5.454, 5.901, 5.741, 5.289, 5.608, 6.167, 6.061, 5.820, 5.557, 6.181, 6.139, 5.605, 6.178, 5.744, 5.996}},
        {"2SFEH-R-18-CYNV-465005-1-Top", {5.205, 6.877, 7.184, 7.378, 6.699, 7.073, 6.575, 6.343, 7.200, 6.835, 6.343, 6.814, 7.185, 7.694, 7.482, 6.491, 7.127, 7.315, 7.357, 6.268, 6.874, 6.927, 7.314, 7.081, 6.689, 7.113, 7.081, 6.181}},
        {"2SFEH-R-18-CYNV-465005-1-Bottom", {7.227, 7.065, 7.330, 7.292, 7.195, 7.067, 7.259, 6.887, 7.529, 7.106, 7.038, 7.046, 7.354, 7.266, 7.197, 7.197, 7.266, 7.330, 7.477, 7.098, 7.505, 6.736, 7.297, 7.310, 7.822, 7.641, 7.328, 7.302}},
        {"2SFEH-R-40-CYNT-465006-1-Top", {7.219, 7.481, 6.774, 7.147, 7.250, 7.098, 7.040, 7.844, 7.338, 6.849, 7.259, 7.179, 6.892, 7.068, 7.227, 7.065, 8.035, 7.681, 5.874, 6.747, 6.916, 6.878, 7.383, 7.551, 6.805, 7.525, 7.197, 7.197}},
        {"2SFEH-R-40-CYNT-465006-1-Bottom", {7.296, 7.152, 7.676, 6.983, 7.575, 7.579, 7.179, 7.412, 7.181, 7.191, 6.927, 7.233, 6.911, 6.756, 7.423, 6.883, 7.111, 7.341, 7.077, 7.421, 7.094, 6.650, 6.484, 6.728, 6.807, 6.572, 6.386, 7.143}},
        {"2SFEH-L-18-CXVQ-465003-2-Top", {7.411, 7.510, 7.799, 7.425, 7.195, 7.549, 6.625, 7.693, 6.764, 7.311, 7.110, 7.548, 7.580, 7.694, 7.580, 7.800, 7.657, 7.326, 7.040, 7.484, 7.194, 7.722, 7.071, 6.739, 6.932, 7.197, 6.779, 7.019}},
        {"2SFEH-L-18-CXVQ-465003-2-Bottom", {7.411, 7.612, 7.337, 7.718, 7.291, 7.255, 7.477, 7.535, 7.300, 7.424, 6.927, 7.617, 7.065, 6.765, 7.677, 6.885, 7.798, 7.559, 7.913, 7.683, 7.533, 7.757, 7.690, 7.602, 7.155, 7.517, 7.148, 6.887}},
        {"2SFEH-R-18-CYNV-465005-2-Top", {7.342, 6.739, 6.930, 7.416, 6.855, 7.076, 7.486, 6.933, 7.117, 7.347, 7.127, 6.814, 7.312, 7.307, 6.810, 7.202, 6.690, 7.560, 7.199, 7.075, 6.861, 7.317, 7.040, 6.538, 6.745, 6.730, 7.580, 7.694}},
        {"2SFEH-R-18-CYNV-465005-2-Bottom", {6.767, 7.010, 6.924, 7.575, 7.333, 7.433, 7.188, 6.932, 7.355, 6.934, 7.651, 7.233, 6.434, 6.017, 5.998, 5.928, 7.274, 7.389, 7.657, 7.573, 7.038, 7.074, 7.191, 7.272, 7.164, 7.305, 7.111, 7.226}},
        {"2SFEH-R-40-CYNT-465006-2-Top", {6.844, 7.763, 8.033, 7.843, 6.522, 6.671, 7.498, 7.591, 7.159, 6.658, 6.975, 7.342, 7.119, 6.931, 7.333, 7.262, 7.277, 7.554, 7.763, 6.894, 7.047, 6.756, 6.831, 7.280, 7.250, 6.993, 6.883, 6.746}},
      
        {"2SFEH-R-40-CYNT-465006-2-Bottom", {6.434, 7.612, 7.191, 7.226, 6.698, 7.085, 7.287, 7.439, 7.206, 7.488, 7.073, 7.187, 7.353, 7.420, 6.763, 7.875, 7.378, 7.265, 6.739, 7.508, 6.664, 7.724, 7.085, 6.773, 7.150, 7.044, 6.808, 7.507}},
        {"2SFEH-L-40-CYNR-465004-2-Top", {7.110, 7.150, 7.581, 7.265, 7.046, 7.739, 7.695, 6.992, 7.548, 7.271, 7.347, 6.595, 7.281, 7.687, 7.354, 7.793, 6.515, 7.282, 7.060, 7.120, 7.493, 6.775, 7.042, 7.168, 7.386, 7.360, 7.268, 7.077}},
        {"2SFEH-L-40-CYNR-465004-2-Bottom", {7.738, 8.083, 7.339, 7.491, 6.922, 7.191, 7.153, 7.421, 7.134, 7.631, 7.078, 7.433, 7.856, 7.387, 7.119, 7.510, 7.612, 7.779, 7.083, 7.348, 7.349, 7.563, 6.730, 7.381, 7.923, 7.626, 7.540, 7.514}},
        {"2SFEH-L-18-CXVQ-465003-1-Top", {7.159, 7.235, 7.348, 7.704, 6.893, 7.528, 7.000, 7.311, 7.767, 7.630, 7.358, 7.348, 7.311, 7.848, 7.895, 7.781, 7.275, 7.170, 6.684, 7.603, 7.358, 7.707, 7.645, 7.059, 7.705, 7.437, 7.054, 8.068}},
        {"2SFEH-L-18-CXVQ-465003-1-Bottom", {6.581, 7.491, 7.434, 7.996, 7.571, 7.397, 7.588, 7.270, 7.816, 7.271, 7.208, 7.081, 7.653, 7.684, 7.428, 7.769, 7.381, 7.490, 7.382, 7.659, 7.041, 7.891, 7.588, 7.770, 8.124, 7.856, 7.798, 8.036}},
    };

    // Process each hybrid's data
    for (const auto& entry : inputData) {
        processHybridData(entry.first, entry.second, hybridData, allData);
    }

    // Create output ROOT file
    TFile* outFile = new TFile("output_histograms.root", "RECREATE");

    // Save histograms for each hybrid
    for (const auto& entry : hybridData) {
        const string& hybridName = entry.first;
        const vector<double>& data = entry.second;

        // Create histogram for this hybrid
        string histName = "hist_" + hybridName;
        TH1F* hist = new TH1F(histName.c_str(), ("Histogram for " + hybridName).c_str(), 40, 7, 15);

        // Set axis labels
        hist->GetXaxis()->SetTitle("pull-force (g)");
        hist->GetYaxis()->SetTitle("Number of WBs");
        for (double value : data) {
            hist->Fill(value);
        }

        // Draw the histogram and save it as a PNG file
        TCanvas* canvas = new TCanvas();
        hist->Draw();
        string canvasName = hybridName + ".png";
        canvas->SaveAs(canvasName.c_str());

        // Write the histogram to the output file
        hist->Write();
    }

    // Create and fill a histogram for all hybrids combined
    TH1F* combinedHist = new TH1F("combinedHist", "Combined Histogram", 40, 7, 15);
    combinedHist->GetXaxis()->SetTitle("pull-force (g)");
    combinedHist->GetYaxis()->SetTitle("Number of WBs");
    
    
    
    for (double value : allData) {
        combinedHist->Fill(value);
    }

    // Draw the combined histogram and save it as a PNG file
    TCanvas* combinedCanvas = new TCanvas();
    combinedHist->Draw();
    combinedCanvas->SaveAs("combined_histogram.png");

    // Write the combined histogram to the output file
    combinedHist->Write();

    // Close the output file
    outFile->Close();

    cout << "Histograms saved to output_histograms.root and PNG files." << endl;

    return 0;
}
