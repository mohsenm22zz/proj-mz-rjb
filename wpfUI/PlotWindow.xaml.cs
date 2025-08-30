using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using ScottPlot;

namespace wpfUI
{
    public partial class PlotWindow : Window
    {
        public PlotWindow()
        {
            InitializeComponent();
        }

        public void LoadTransientData(CircuitSimulatorService simulator, string[] itemsToPlot)
        {
            WpfPlot1.Plot.Clear();
            bool hasData = false;

            if (itemsToPlot == null || !itemsToPlot.Any())
            {
                WpfPlot1.Plot.Title("No items selected for plotting.");
                WpfPlot1.Refresh();
                return;
            }

            foreach (var item in itemsToPlot)
            {
                // Note: This assumes you have a way to differentiate voltage vs. current probes.
                // For simplicity, we'll assume everything is a voltage for now.
                Tuple<double[], double[]> history = simulator.GetNodeVoltageHistory(item);
                if (history?.Item1 != null && history.Item1.Length > 1)
                {
                    // Use AddSignal for transient analysis as it's more performant for evenly spaced time data.
                    double sampleRate = 1.0 / (history.Item1[1] - history.Item1[0]);
                    var signalPlot = WpfPlot1.Plot.AddSignal(history.Item2, sampleRate);
                    signalPlot.Label = $"V({item})";
                    hasData = true;
                }
            }

            WpfPlot1.Plot.Title("Transient Analysis Results");
            WpfPlot1.Plot.XLabel("Time (s)");
            WpfPlot1.Plot.YLabel("Voltage (V)");
            
            // Ensure the X-axis is linear for transient plots using ScottPlot 4 API
            WpfPlot1.Plot.XAxis.LogScale = false;

            if (hasData)
                WpfPlot1.Plot.Legend();

            WpfPlot1.Refresh();
        }

        public void LoadACData(CircuitSimulatorService simulator, string[] nodesToPlot)
        {
            WpfPlot1.Plot.Clear();
            bool hasData = false;

            if (nodesToPlot == null || !nodesToPlot.Any())
            {
                WpfPlot1.Plot.Title("No nodes selected for plotting.");
                WpfPlot1.Refresh();
                return;
            }

            foreach (var nodeName in nodesToPlot)
            {
                Tuple<double[], double[]> history = simulator.GetNodeSweepHistory(nodeName);
                if (history?.Item1 != null && history.Item1.Length > 0)
                {
                    var scatter = WpfPlot1.Plot.AddScatter(history.Item1, history.Item2);
                    scatter.Label = $"V({nodeName})";
                    hasData = true;
                }
            }

            WpfPlot1.Plot.Title("AC Sweep Results");
            WpfPlot1.Plot.XLabel("Frequency (Hz)");
            WpfPlot1.Plot.YLabel("Magnitude (V)");

            // This is the correct API for setting a logarithmic scale on an axis in ScottPlot 4.
            WpfPlot1.Plot.XAxis.LogScale = true;

            if (hasData)
                WpfPlot1.Plot.Legend();

            WpfPlot1.Refresh();
        }

        public void LoadPhaseData(CircuitSimulatorService simulator, string[] nodesToPlot)
        {
            WpfPlot1.Plot.Clear();
            bool hasData = false;
            
            if (nodesToPlot == null || !nodesToPlot.Any())
            {
                WpfPlot1.Plot.Title("No nodes selected for plotting.");
                WpfPlot1.Refresh();
                return;
            }

            foreach (var nodeName in nodesToPlot)
            {
                Tuple<double[], double[]> history = simulator.GetNodePhaseSweepHistory(nodeName);
                if (history?.Item1 != null && history.Item1.Length > 0)
                {
                    var scatter = WpfPlot1.Plot.AddScatter(history.Item1, history.Item2);
                    scatter.Label = $"V({nodeName})";
                    hasData = true;
                }
            }

            WpfPlot1.Plot.Title("Phase Sweep Results");
            WpfPlot1.Plot.XLabel("Phase (degrees)");
            WpfPlot1.Plot.YLabel("Magnitude (V)");
            
            // // Ensure the X-axis is linear for phase plots using ScottPlot 4 API
            WpfPlot1.Plot.XAxis.LogScale = false;
            if (hasData)
            WpfPlot1.Plot.Legend();

            WpfPlot1.Refresh();
        }
    }
}

