using System;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace wpfUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly CircuitSimulatorService _simulatorService;

        public MainWindow()
        {
            InitializeComponent();
            _simulatorService = new CircuitSimulatorService();
            
            // Create a simple circuit for testing
            CreateSampleCircuit();
        }

        /// <summary>
        /// Creates a sample circuit for testing
        /// </summary>
        private void CreateSampleCircuit()
        {
            // Create nodes
            _simulatorService.AddNode("N1");
            _simulatorService.AddNode("N2");
            _simulatorService.AddNode("GND");
            
            // Set ground
            _simulatorService.SetGroundNode("GND");
            
            // Add components
            _simulatorService.AddVoltageSource("V1", "N1", "GND", 5.0);
            _simulatorService.AddResistor("R1", "N1", "N2", 1000.0);
            _simulatorService.AddResistor("R2", "N2", "GND", 2000.0);
        }

        /// <summary>
        /// Handle "Run Analysis" menu item click event
        /// </summary>
        private void RunAnalysis_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // Run DC analysis
                bool success = _simulatorService.RunDCAnalysis();
                
                if (success)
                {
                    // Get node voltages
                    var nodeNames = _simulatorService.GetNodeNames();
                    StringBuilder result = new StringBuilder();
                    result.AppendLine("Analysis Results:");
                    
                    foreach (var nodeName in nodeNames)
                    {
                        double voltage = _simulatorService.GetNodeVoltage(nodeName);
                        result.AppendLine($"{nodeName}: {voltage:F3} V");
                    }
                    
                    MessageBox.Show(result.ToString(), "Analysis Results");
                }
                else
                {
                    MessageBox.Show("Analysis failed.", "Error");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error running analysis: {ex.Message}", "Error");
            }
        }

        /// <summary>
        /// Handle "Transient Analysis" menu item click event
        /// </summary>
        private void RunTransientAnalysis_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // Run transient analysis (1ms step, 10ms stop)
                bool success = _simulatorService.RunTransientAnalysis(0.001, 0.01);
                
                if (success)
                {
                    // Get voltage history for a node
                    var history = _simulatorService.GetNodeVoltageHistory("N2");
                    double[] timePoints = history.Item1;
                    double[] voltages = history.Item2;
                    
                    StringBuilder result = new StringBuilder();
                    result.AppendLine("Transient Analysis Results:");
                    result.AppendLine("Time (s)\tVoltage (V)");
                    
                    for (int i = 0; i < Math.Min(timePoints.Length, 10); i++)
                    {
                        result.AppendLine($"{timePoints[i]:F4}\t{voltages[i]:F4}");
                    }
                    
                    MessageBox.Show(result.ToString(), "Transient Analysis Results");
                }
                else
                {
                    MessageBox.Show("Transient analysis failed.", "Error");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error running transient analysis: {ex.Message}", "Error");
            }
        }
        
        // Context menu event handlers
        private void ShowAddElementContextMenu(object sender, RoutedEventArgs e)
        {
            ContextMenu contextMenu = (ContextMenu)FindResource("AddElementContextMenu");
            RadioButton radioButton = sender as RadioButton;
            if (radioButton != null && contextMenu != null)
            {
                contextMenu.PlacementTarget = radioButton;
                contextMenu.IsOpen = true;
            }
        }
        
        private void ShowRunContextMenu(object sender, RoutedEventArgs e)
        {
            ContextMenu contextMenu = (ContextMenu)FindResource("RunContextMenu");
            RadioButton radioButton = sender as RadioButton;
            if (radioButton != null && contextMenu != null)
            {
                contextMenu.PlacementTarget = radioButton;
                contextMenu.IsOpen = true;
            }
        }
        
        // Add Element menu item handlers
        private void AddResistor_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Add Resistor functionality would be implemented here", "Add Element");
        }
        
        private void AddCapacitor_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Add Capacitor functionality would be implemented here", "Add Element");
        }
        
        private void AddInductor_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Add Inductor functionality would be implemented here", "Add Element");
        }
        
        private void AddVoltageSource_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Add Voltage Source functionality would be implemented here", "Add Element");
        }
        
        private void AddCurrentSource_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Add Current Source functionality would be implemented here", "Add Element");
        }
        
        private void AddDiode_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Add Diode functionality would be implemented here", "Add Element");
        }
        
        // Run Analysis menu item handlers
        private void RunDCAnalysis_Click(object sender, RoutedEventArgs e)
        {
            RunAnalysis_Click(sender, e); // Reuse existing method
        }
        
        private void RunACAnalysis_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("AC Analysis functionality would be implemented here", "Run Analysis");
        }
    }
}