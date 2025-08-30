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
                        // Use Add.Scatter instead of Add.Signal to properly handle X and Y data
                        var scatterPlot = WpfPlot1.Plot.Add.ScatterLine(history.Item1, history.Item2);
                        scatterPlot.MarkerSize = 0; // Hide markers, only show lines
                        scatterPlot.LineWidth = 2;
                        scatterPlot.LegendText = $"I({componentName})";
                        hasCurrentData = true;
                    }
                }
                else
                {
                    Tuple<double[], double[]> history = simulator.GetNodeVoltageHistory(item);
                    if (history.Item1.Length > 1)
                    {
                        // Use Add.Scatter instead of Add.Signal to properly handle X and Y data
                        var scatterPlot = WpfPlot1.Plot.Add.ScatterLine(history.Item1, history.Item2);
                        scatterPlot.MarkerSize = 0; // Hide markers, only show lines
                        scatterPlot.LineWidth = 2;
                        scatterPlot.LegendText = $"V({item})";
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