using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Text;

namespace wpfUI
{
    public partial class MainWindow : Window
    {
        private readonly Dictionary<string, int> _componentCounts = new Dictionary<string, int>
        {
            {"R", 1}, {"C", 1}, {"L", 1}, {"D", 1}, {"V", 1}, {"ACV", 1}, {"I", 1}
        };

        private bool _isWiringMode = false;
        private bool _isCircuitLocked = false;
        private Wire _currentWire = null;
        private SimulationParameters _simulationParameters;

        public MainWindow()
        {
            InitializeComponent();
            _simulationParameters = new SimulationParameters(); // Initialize with default parameters
            this.Loaded += (s, e) => DrawGrid();
            this.KeyDown += MainWindow_KeyDown;
        }

        private void MainWindow_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Escape)
            {
                ExitWiringMode();
                DeselectAll();
            }
        }

        private void DrawGrid()
        {
            double gridSize = 20.0;
            var existingChildren = SchematicCanvas.Children.OfType<UIElement>().ToList();

            SchematicCanvas.Children.Clear();

            for (double x = 0; x < SchematicCanvas.ActualWidth; x += gridSize)
            {
                var line = new Line { X1 = x, Y1 = 0, X2 = x, Y2 = SchematicCanvas.ActualHeight, Stroke = new SolidColorBrush(Color.FromArgb(50, 80, 80, 80)), StrokeThickness = 1 };
                SchematicCanvas.Children.Add(line);
            }

            for (double y = 0; y < SchematicCanvas.ActualHeight; y += gridSize)
            {
                var line = new Line { X1 = 0, Y1 = y, X2 = SchematicCanvas.ActualWidth, Y2 = y, Stroke = new SolidColorBrush(Color.FromArgb(50, 80, 80, 80)), StrokeThickness = 1 };
                SchematicCanvas.Children.Add(line);
            }

            foreach (var child in existingChildren.Where(c => !(c is Line && ((Line)c).StrokeThickness == 1)))
            {
                SchematicCanvas.Children.Add(child);
            }
        }

        private void AddComponent_Click(object sender, RoutedEventArgs e)
        {
            if (_isCircuitLocked)
            {
                MessageBox.Show("Cannot add new components while circuit is wired.", "Circuit Locked");
                return;
            }

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

        private void PlaceNode_Click(object sender, RoutedEventArgs e)
        {
            var nodeControl = new NodeControl();
            nodeControl.Width = 10;
            nodeControl.Height = 10;

            Point position = new Point(SchematicCanvas.ActualWidth / 2, SchematicCanvas.ActualHeight / 2);
            double gridSize = 20.0;
            double snappedX = Math.Round(position.X / gridSize) * gridSize;
            double snappedY = Math.Round(position.Y / gridSize) * gridSize;

            Canvas.SetLeft(nodeControl, snappedX - nodeControl.Width / 2);
            Canvas.SetTop(nodeControl, snappedY - nodeControl.Height / 2);

            SchematicCanvas.Children.Add(nodeControl);
        }

        private void PlaceWire_Click(object sender, RoutedEventArgs e)
        {
            if (!_isWiringMode)
            {
                EnterWiringMode();
            }
            else
            {
                ExitWiringMode();
            }
        }

        private void EnterWiringMode()
        {
            _isWiringMode = true;
            _isCircuitLocked = true;
            WireMenuItem.IsChecked = true;
            SchematicCanvas.Cursor = Cursors.Cross;
            SchematicCanvas.MouseLeftButtonDown += Canvas_Wiring_MouseDown;
            SchematicCanvas.MouseRightButtonDown += Canvas_Wiring_MouseRightButtonDown;
            SchematicCanvas.MouseMove += Canvas_Wiring_MouseMove;
        }

        private void ExitWiringMode()
        {
            _isWiringMode = false;
            WireMenuItem.IsChecked = false;
            if (_currentWire != null)
            {
                // If the wire is just a single point, remove it
                if (_currentWire.EndPoint == _currentWire.StartPoint)
                {
                    SchematicCanvas.Children.Remove(_currentWire);
                }
                _currentWire = null;
            }
            SchematicCanvas.Cursor = Cursors.Arrow;
            SchematicCanvas.MouseLeftButtonDown -= Canvas_Wiring_MouseDown;
            SchematicCanvas.MouseRightButtonDown -= Canvas_Wiring_MouseRightButtonDown;
            SchematicCanvas.MouseMove -= Canvas_Wiring_MouseMove;
        }

        private void Canvas_Wiring_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (_isWiringMode)
            {
                ExitWiringMode();
            }
        }

        private void Canvas_Wiring_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (!_isWiringMode) return;

            Point clickPoint = e.GetPosition(SchematicCanvas);
            double gridSize = 20.0;
            Point snappedPoint = new Point(
                Math.Round(clickPoint.X / gridSize) * gridSize,
                Math.Round(clickPoint.Y / gridSize) * gridSize
            );

            if (_currentWire == null)
            {
                _currentWire = new Wire();
                _currentWire.StartPoint = snappedPoint;
                _currentWire.AddPoint(snappedPoint);
                SchematicCanvas.Children.Add(_currentWire);
            }
            else
            {
                _currentWire.AddPoint(snappedPoint);
            }

            if (FindNearestConnectionPoint(clickPoint) != null && _currentWire.StartPoint != snappedPoint)
            {
                ExitWiringMode();
            }
        }

        private void Canvas_Wiring_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isWiringMode && _currentWire != null)
            {
                Point currentPoint = e.GetPosition(SchematicCanvas);
                double gridSize = 20.0;
                Point snappedPoint = new Point(
                    Math.Round(currentPoint.X / gridSize) * gridSize,
                    Math.Round(currentPoint.Y / gridSize) * gridSize
                );
                _currentWire.UpdatePreview(snappedPoint);
            }
        }

        private void DeselectAll()
        {
            foreach (var child in SchematicCanvas.Children)
            {
                if (child is Wire wire)
                {
                    wire.IsSelected = false;
                }
            }
        }
        
        private Point? FindNearestConnectionPoint(Point clickPoint)
        {
            double tolerance = 10.0; 

            foreach (var child in SchematicCanvas.Children)
            {
                if (child is ComponentControl component)
                {
                    Point leftConnectorCenter = component.LeftConnector.TransformToAncestor(SchematicCanvas).Transform(new Point(component.LeftConnector.ActualWidth / 2, component.LeftConnector.ActualHeight / 2));
                    if ((clickPoint - leftConnectorCenter).Length < tolerance)
                    {
                        return leftConnectorCenter;
                    }

                    Point rightConnectorCenter = component.RightConnector.TransformToAncestor(SchematicCanvas).Transform(new Point(component.RightConnector.ActualWidth / 2, component.RightConnector.ActualHeight / 2));
                    if ((clickPoint - rightConnectorCenter).Length < tolerance)
                    {
                        return rightConnectorCenter;
                    }
                }
                else if (child is NodeControl node)
                {
                    Point nodeCenter = new Point(Canvas.GetLeft(node) + node.Width / 2, Canvas.GetTop(node) + node.Height / 2);
                    if ((clickPoint - nodeCenter).Length < tolerance)
                    {
                        return nodeCenter;
                    }
                }
            }
            return null;
        }

        private void PlaceComponent_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Component Library window would open here.", "Place Component");
        }

        private void EditSimulationCmd_Click(object sender, RoutedEventArgs e)
        {
            var settingsWindow = new SimulationSettingsWindow();
            settingsWindow.Owner = this;
            
            // Show the dialog and wait for it to close
            bool? result = settingsWindow.ShowDialog();

            if (result == true)
            {
                // If the user clicked OK, retrieve the parameters
                // This requires adding public properties to SimulationSettingsWindow.xaml.cs
                // For now, we'll just store a placeholder.
                // _simulationParameters = settingsWindow.Parameters;
                MessageBox.Show("Simulation settings saved!");
            }
        }

        private void RunAnalysis_Click(object sender, RoutedEventArgs e)
        {
            // 1. Generate the netlist from the canvas
            List<string> netlistCommands = NetlistGenerator.Generate(SchematicCanvas);

            if (!netlistCommands.Any())
            {
                MessageBox.Show("The circuit is empty. Add some components to simulate.", "Empty Circuit");
                return;
            }

            // For debugging: display the generated netlist
            var netlistText = string.Join("\n", netlistCommands);
            MessageBox.Show("Generated Netlist:\n\n" + netlistText, "Netlist");

            // 2. Create a new simulator instance and load the circuit
            using (var simulator = new CircuitSimulatorService())
            {
                // This is a simplified loading process. You'll need to parse each command.
                foreach (var command in netlistCommands)
                {
                    var parts = command.Split(' ');
                    string type = parts[0];
                    if (type == "GND")
                    {
                        simulator.SetGroundNode(parts[1]);
                    }
                    else
                    {
                        // Example for a resistor. You need to expand this for all component types.
                        if (type == "R")
                        {
                            simulator.AddResistor(parts[1], parts[2], parts[3], double.Parse(parts[4]));
                        }
                        else if (type == "V")
                        {
                             simulator.AddVoltageSource(parts[1], parts[2], parts[3], double.Parse(parts[4]));
                        }
                        // ... add other component types here ...
                    }
                }

                // 3. Run the selected analysis with the stored parameters
                bool success = false;
                switch (_simulationParameters.CurrentAnalysis)
                {
                    case SimulationParameters.AnalysisType.Transient:
                        success = simulator.RunTransientAnalysis(_simulationParameters.StopTime, _simulationParameters.MaxTimestep);
                        break;
                    case SimulationParameters.AnalysisType.ACSweep:
                        // You will need to get the source name for the sweep
                        string acSourceName = "V1"; // Placeholder
                        success = simulator.RunACAnalysis(acSourceName, _simulationParameters.StartFrequency, _simulationParameters.StopFrequency, _simulationParameters.NumberOfPoints);
                        break;
                    // Add Phase Sweep case here
                }

                if (success)
                {
                    // 4. If successful, open the plot window and pass the results
                    var plotWindow = new PlotWindow();
                    plotWindow.Owner = this;
                    
                    // Example: Get results for all nodes and pass them to the plot window
                    var nodeNames = simulator.GetNodeNames();
                    // You would create a method in PlotWindow to accept this data
                    // plotWindow.LoadData(simulator, nodeNames);

                    plotWindow.Show();
                }
                else
                {
                    MessageBox.Show("The simulation failed to run. Please check the circuit.", "Simulation Error");
                }
            }
        }
    }
}
