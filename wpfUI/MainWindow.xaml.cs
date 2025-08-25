using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;

namespace wpfUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly CircuitSimulatorService _simulatorService;
        // Dictionaries to keep track of component counts for naming
        private readonly Dictionary<string, int> _componentCounts = new Dictionary<string, int>
        {
            {"R", 1}, {"C", 1}, {"L", 1}, {"D", 1}, {"V", 1}, {"ACV", 1}, {"I", 1}
        };


        public MainWindow()
        {
            InitializeComponent();
            _simulatorService = new CircuitSimulatorService();
        }

        private void AddComponent_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Button button && button.Tag is string type)
            {
                // Get the current count for this component type
                int count = _componentCounts[type];

                // Create a new component control
                var componentControl = new ComponentControl
                {
                    ComponentName = $"{type}{count}",
                    Width = 100,
                    Height = 40
                };

                // Position it on the canvas
                Canvas.SetLeft(componentControl, 100);
                Canvas.SetTop(componentControl, 100);

                // Add it to the canvas
                SchematicCanvas.Children.Add(componentControl);

                // Increment the count for the next component of this type
                _componentCounts[type]++;
            }
        }

        private void PlaceComponent_Click(object sender, RoutedEventArgs e)
        {
            // This would open a more complex component library window.
            // For now, we can just show a message.
            MessageBox.Show("Component Library window would open here.", "Place Component");
        }

        private void EditSimulationCmd_Click(object sender, RoutedEventArgs e)
        {
            // This is where you would open a new dialog window to get simulation parameters
            // (e.g., for AC Sweep: start freq, stop freq, etc.)
            MessageBox.Show("Simulation Settings dialog would open here.", "Edit Simulation Command");
        }

        /// <summary>
        /// Handle "Run" menu item click event. This can be a central point for all analyses.
        /// </summary>
        private void RunAnalysis_Click(object sender, RoutedEventArgs e)
        {
            // In a real application, you would check which simulation type is currently selected.
            // For this example, let's just run a simple DC analysis on a hardcoded circuit.
            try
            {
                // For demonstration, we create a simple circuit here.
                // In the final app, this would be built from the components on the canvas.
                using (var sim = new CircuitSimulatorService())
                {
                    sim.AddNode("N1");
                    sim.AddNode("GND");
                    sim.SetGroundNode("GND");
                    sim.AddVoltageSource("V1", "N1", "GND", 5.0);
                    sim.AddResistor("R1", "N1", "GND", 1000.0);

                    bool success = sim.RunDCAnalysis();
                    if (success)
                    {
                        double voltage = sim.GetNodeVoltage("N1");
                        MessageBox.Show($"DC Analysis Complete.\n\nVoltage at N1: {voltage:F3} V", "Analysis Results");
                    }
                    else
                    {
                        MessageBox.Show("Analysis failed.", "Error");
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error running analysis: {ex.Message}", "Error");
            }
        }
    }
}
