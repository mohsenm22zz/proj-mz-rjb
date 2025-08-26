using System;
using System.Collections.Generic;
using System.Windows;

namespace wpfUI
{
    public partial class PlotWindow : Window
    {
        public PlotWindow()
        {
            InitializeComponent();
        }

        // --- NEW: Method to load and display simulation data ---
        public void LoadData(CircuitSimulatorService simulator, string[] nodeNames)
        {
            WpfPlot1.Plot.Clear();

            foreach (var nodeName in nodeNames)
            {
                // Retrieve the history for each node
                Tuple<double[], double[]> history = simulator.GetNodeVoltageHistory(nodeName);
                if (history.Item1.Length > 0)
                {
                    // Add the data to the plot
                    var scatterPlot = WpfPlot1.Plot.Add.Scatter(history.Item1, history.Item2);
                    scatterPlot.Label = $"V({nodeName})";
                }
            }

            // Customize the plot's appearance
            WpfPlot1.Plot.Title("Simulation Results");
            WpfPlot1.Plot.XLabel("Time (s)");
            WpfPlot1.Plot.YLabel("Voltage (V)");
            WpfPlot1.Plot.ShowLegend();
            WpfPlot1.Refresh();
        }
    }
}