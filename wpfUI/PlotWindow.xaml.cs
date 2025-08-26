// mohsenm22zz/proj-mz-rjb/proj-mz-rjb-1b949d5aa204b9f590a1c5f0644f3424cf2a70ce/wpfUI/PlotWindow.xaml.cs

using System;
using System.Collections.Generic;
using System.Windows;
using System.Linq;

namespace wpfUI
{
    public partial class PlotWindow : Window
    {
        public PlotWindow()
        {
            InitializeComponent();
        }

        public void LoadData(CircuitSimulatorService simulator, string[] itemsToPlot)
        {
            WpfPlot1.Plot.Clear();
            bool hasVoltageData = false;
            bool hasCurrentData = false;

            foreach (var item in itemsToPlot)
            {
                if (item.StartsWith("I(")) // It's a current probe
                {
                    // NOTE: This requires GetComponentCurrentHistory to be implemented in the C++ core
                    // For now, this part will not plot anything until the backend is updated.
                    string componentName = item.Substring(2, item.Length - 3);
                    // Tuple<double[], double[]> history = simulator.GetComponentCurrentHistory(componentName);
                    // if (history.Item1.Length > 1) { ... plotting logic ... }
                    hasCurrentData = true; 
                }
                else // It's a voltage probe
                {
                    Tuple<double[], double[]> history = simulator.GetNodeVoltageHistory(item);
                    if (history.Item1.Length > 1)
                    {
                        double sampleRate = 1.0 / (history.Item1[1] - history.Item1[0]);
                        var signalPlot = WpfPlot1.Plot.Add.Signal(history.Item2, sampleRate);
                        signalPlot.Label = $"V({item})";
                        hasVoltageData = true;
                    }
                }
            }

            WpfPlot1.Plot.Title("Simulation Results");
            WpfPlot1.Plot.XLabel("Time (s)");
            
            if (hasVoltageData && hasCurrentData)
                WpfPlot1.Plot.YLabel("Voltage (V) / Current (A)");
            else if (hasVoltageData)
                WpfPlot1.Plot.YLabel("Voltage (V)");
            else if (hasCurrentData)
                WpfPlot1.Plot.YLabel("Current (A)");

            if (itemsToPlot.Any())
                WpfPlot1.Plot.ShowLegend();
                
            WpfPlot1.Refresh();
        }
    }
}
