using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;

namespace wpfUI
{
    public partial class MainWindow : Window
    {
        private readonly CircuitSimulatorService _simulatorService;
        private readonly Dictionary<string, int> _componentCounts = new Dictionary<string, int>
        {
            {"R", 1}, {"C", 1}, {"L", 1}, {"D", 1}, {"V", 1}, {"ACV", 1}, {"I", 1}
        };

        // --- NEW: Wiring State Variables ---
        private bool _isWiringMode = false;
        private Line _previewWire = null;
        private Point _wireStartPoint;

        public MainWindow()
        {
            InitializeComponent();
            _simulatorService = new CircuitSimulatorService();
            this.Loaded += (s, e) => DrawGrid(); // Draw the grid when the window loads
        }

        // --- NEW: Method to draw the background grid ---
        private void DrawGrid()
        {
            double gridSize = 20.0;
            SchematicCanvas.Children.Clear(); // Clear existing grid lines if any

            for (double x = 0; x < SchematicCanvas.ActualWidth; x += gridSize)
            {
                var line = new Line
                {
                    X1 = x, Y1 = 0,
                    X2 = x, Y2 = SchematicCanvas.ActualHeight,
                    Stroke = new SolidColorBrush(Color.FromArgb(50, 80, 80, 80)),
                    StrokeThickness = 1
                };
                SchematicCanvas.Children.Add(line);
            }

            for (double y = 0; y < SchematicCanvas.ActualHeight; y += gridSize)
            {
                var line = new Line
                {
                    X1 = 0, Y1 = y,
                    X2 = SchematicCanvas.ActualWidth, Y2 = y,
                    Stroke = new SolidColorBrush(Color.FromArgb(50, 80, 80, 80)),
                    StrokeThickness = 1
                };
                SchematicCanvas.Children.Add(line);
            }
        }


        private void AddComponent_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Button button && button.Tag is string type)
            {
                int count = _componentCounts[type];
                var componentControl = new ComponentControl
                {
                    ComponentName = $"{type}{count}",
                    Width = 100,
                    Height = 40
                };
                Canvas.SetLeft(componentControl, 100);
                Canvas.SetTop(componentControl, 100);
                SchematicCanvas.Children.Add(componentControl);
                _componentCounts[type]++;
            }
        }

        // --- NEW: Event handler for the "Wire" menu item ---
        private void PlaceWire_Click(object sender, RoutedEventArgs e)
        {
            _isWiringMode = true;
            SchematicCanvas.Cursor = Cursors.Cross;
            // Add canvas mouse handlers for wiring
            SchematicCanvas.MouseLeftButtonDown += Canvas_Wiring_MouseDown;
            SchematicCanvas.MouseMove += Canvas_Wiring_MouseMove;
        }

        // --- NEW: Mouse down handler for starting a wire ---
        private void Canvas_Wiring_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (!_isWiringMode) return;

            if (_previewWire == null) // Start a new wire
            {
                _wireStartPoint = e.GetPosition(SchematicCanvas);
                _previewWire = new Line
                {
                    X1 = _wireStartPoint.X,
                    Y1 = _wireStartPoint.Y,
                    X2 = _wireStartPoint.X,
                    Y2 = _wireStartPoint.Y,
                    Stroke = Brushes.Cyan,
                    StrokeThickness = 2
                };
                SchematicCanvas.Children.Add(_previewWire);
            }
            else // Finish the current wire
            {
                // Finalize the wire's position
                Point endPoint = e.GetPosition(SchematicCanvas);
                _previewWire.X2 = endPoint.X;
                _previewWire.Y2 = endPoint.Y;

                // Reset for the next wire
                _previewWire = null;
                
                // You can add logic here to check if it connected to a valid point
            }
        }

        // --- NEW: Mouse move handler to preview the wire ---
        private void Canvas_Wiring_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isWiringMode && _previewWire != null)
            {
                Point currentPoint = e.GetPosition(SchematicCanvas);
                // This creates the "LTspice-style" right-angle wiring preview
                double deltaX = Math.Abs(currentPoint.X - _wireStartPoint.X);
                double deltaY = Math.Abs(currentPoint.Y - _wireStartPoint.Y);

                if (deltaX > deltaY) // Prefer horizontal segment first
                {
                    _previewWire.X2 = currentPoint.X;
                    _previewWire.Y2 = _wireStartPoint.Y;
                }
                else // Prefer vertical segment first
                {
                    _previewWire.X2 = _wireStartPoint.X;
                    _previewWire.Y2 = currentPoint.Y;
                }
            }
        }
        
        // --- MODIFIED: Update the Place menu item in MainWindow.xaml to call the new handler ---
        // In MainWindow.xaml, change: <MenuItem Header="_Wire"/>
        // To: <MenuItem Header="_Wire" Click="PlaceWire_Click"/>


        private void PlaceComponent_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Component Library window would open here.", "Place Component");
        }

        private void EditSimulationCmd_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Simulation Settings dialog would open here.", "Edit Simulation Command");
        }

        private void RunAnalysis_Click(object sender, RoutedEventArgs e)
        {
            try
            {
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
