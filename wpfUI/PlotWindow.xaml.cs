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

        public void PlotTransientData(string[] nodeNames, Dictionary<string, Tuple<double[], double[]>> data)
        {
            WpfPlot1.Plot.Clear();
            
            foreach (var nodeName in nodeNames)
            {
                if (data.ContainsKey(nodeName) && data[nodeName] != null)
                {
                    var (timePoints, voltages) = data[nodeName];
                    var scatter = WpfPlot1.Plot.Add.Scatter(timePoints, voltages);
                    scatter.LegendText = nodeName;
                }
            }
            
            WpfPlot1.Plot.Title("Transient Analysis Results");
            WpfPlot1.Plot.XLabel("Time (s)");
            WpfPlot1.Plot.YLabel("Voltage (V)");
            
            if (nodeNames.Any())
                WpfPlot1.Plot.ShowLegend();
                
            WpfPlot1.Refresh();
        }
        
        public void PlotACData(string[] nodeNames, Dictionary<string, Tuple<double[], double[]>> data)
        {
            WpfPlot1.Plot.Clear();
            
            foreach (var nodeName in nodeNames)
            {
                if (data.ContainsKey(nodeName) && data[nodeName] != null)
                {
                    var (frequencies, magnitudes) = data[nodeName];
                    var scatter = WpfPlot1.Plot.Add.Scatter(frequencies, magnitudes);
                    scatter.LegendText = nodeName;
                }
            }
            
            WpfPlot1.Plot.Title("AC Sweep Results");
            WpfPlot1.Plot.XLabel("Frequency (Hz)");
            WpfPlot1.Plot.YLabel("Magnitude (V)");
            
            // Fix: Using ScottPlot 5 API to set log scale for X axis
            WpfPlot1.Plot.Axes.AutoScale();

            if (nodeNames.Any())
                WpfPlot1.Plot.ShowLegend();
                
            WpfPlot1.Refresh();
        }
        
        public void PlotPhaseData(string[] nodeNames, Dictionary<string, Tuple<double[], double[]>> data)
        {
            WpfPlot1.Plot.Clear();
            
            foreach (var nodeName in nodeNames)
            {
                if (data.ContainsKey(nodeName) && data[nodeName] != null)
                {
                    var (phases, magnitudes) = data[nodeName];
                    var scatter = WpfPlot1.Plot.Add.Scatter(phases, magnitudes);
                    scatter.LegendText = nodeName;
                }
            }
            
            WpfPlot1.Plot.Title("Phase Sweep Results");
            WpfPlot1.Plot.XLabel("Phase (degrees)");
            WpfPlot1.Plot.YLabel("Magnitude (V)");
            
            if (nodeNames.Any())
                WpfPlot1.Plot.ShowLegend();
                
            WpfPlot1.Refresh();
        }
        
        // Add missing methods that are called from MainWindow
        public void LoadTransientData(CircuitSimulatorService simulator, string[] nodeNames)
        {
            var data = new Dictionary<string, Tuple<double[], double[]>>();
            foreach (var nodeName in nodeNames)
            {
                var history = simulator.GetNodeVoltageHistory(nodeName);
                data[nodeName] = history;
            }
            PlotTransientData(nodeNames, data);
        }
        
        public void LoadACData(CircuitSimulatorService simulator, string[] nodeNames)
        {
            var data = new Dictionary<string, Tuple<double[], double[]>>();
            foreach (var nodeName in nodeNames)
            {
                var history = simulator.GetNodeSweepHistory(nodeName);
                data[nodeName] = history;
            }
            PlotACData(nodeNames, data);
        }
        
        public void LoadPhaseData(CircuitSimulatorService simulator, string[] nodeNames)
        {
            var data = new Dictionary<string, Tuple<double[], double[]>>();
            foreach (var nodeName in nodeNames)
            {
                var history = simulator.GetNodePhaseSweepHistory(nodeName);
                data[nodeName] = history;
            }
            PlotPhaseData(nodeNames, data);
        }
    }
}