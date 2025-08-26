// mohsenm22zz/proj-mz-rjb/proj-mz-rjb-e850e6c0f7d11e5661819e4f80ff5ef06a6db456/wpfUI/PlotWindow.xaml.cs

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

        // --- FIX: Added the missing LoadData method ---
        public void LoadData(CircuitSimulatorService simulator, string[] itemsToPlot)
        {
            WpfPlot1.Plot.Clear();
            bool hasVoltageData = false;
            bool hasCurrentData = false;

            if (itemsToPlot == null || !itemsToPlot.Any())
            {
                 WpfPlot1.Plot.Title("No items selected for plotting.");
                 WpfPlot1.Refresh();
                 return;
            }

            foreach (var item in itemsToPlot)
            {
                if (item.StartsWith("I("))
                {
                    string componentName = item.Substring(2, item.Length - 3);
                    Tuple<double[], double[]> history = simulator.GetComponentCurrentHistory(componentName);
                    if (history.Item1.Length > 1)
                    {
                        double sampleRate = 1.0 / (history.Item1[1] - history.Item1[0]);
                        var signalPlot = WpfPlot1.Plot.Add.Signal(history.Item2, sampleRate);
                        signalPlot.LegendText = $"I({componentName})";
                        hasCurrentData = true;
                    }
                }
                else
                {
                    Tuple<double[], double[]> history = simulator.GetNodeVoltageHistory(item);
                    if (history.Item1.Length > 1)
                    {
                        double sampleRate = 1.0 / (history.Item1[1] - history.Item1[0]);
                        var signalPlot = WpfPlot1.Plot.Add.Signal(history.Item2, sampleRate);
                        signalPlot.LegendText = $"V({item})";
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
