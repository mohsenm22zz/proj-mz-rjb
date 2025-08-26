using System;
using System.Windows;

namespace wpfUI
{
    public partial class PlotWindow : Window
    {
        public PlotWindow()
        {
            InitializeComponent();
            WpfPlot1.Plot.Title("Simulation Results");
            WpfPlot1.Plot.XLabel("Time (s)");
            WpfPlot1.Plot.YLabel("Voltage (V)");
        }

        // --- NEW: Implemented the missing method ---
        public void LoadInitialData(CircuitSimulatorService simulator, string[] nodeNames)
        {
            WpfPlot1.Plot.Clear();
            WpfPlot1.Plot.ShowLegend();
            WpfPlot1.Refresh();
        }

        // --- NEW: Implemented the missing method ---
        public void AddNodeData(CircuitSimulatorService simulator, string nodeName)
        {
            if (string.IsNullOrEmpty(nodeName)) return;

            Tuple<double[], double[]> history = simulator.GetNodeVoltageHistory(nodeName);
            if (history.Item1.Length > 0)
            {
                var scatterPlot = WpfPlot1.Plot.Add.Scatter(history.Item1, history.Item2);
                scatterPlot.Label = $"V({nodeName})";
                WpfPlot1.Plot.ShowLegend();
                WpfPlot1.Refresh();
            }
        }
    }
}
