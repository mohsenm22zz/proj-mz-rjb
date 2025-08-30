using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;

namespace wpfUI
{
    public partial class MainWindow : Window
    {
        private readonly Dictionary<string, int> _componentCounts = new Dictionary<string, int>
        {
            {"R", 1}, {"C", 1}, {"L", 1}, {"D", 1}, {"V", 1}, {"ACV", 1}, {"I", 1}
        };

        private bool _isWiringMode = false;
        private bool _isProbingMode = false;
        private bool _isCircuitLocked = false;
        private Wire _currentWire = null;
        private SimulationParameters _simulationParameters;
        private readonly List<string> _probedItems = new List<string>();

        public MainWindow()
        {
            InitializeComponent();
            _simulationParameters = new SimulationParameters { CurrentAnalysis = SimulationParameters.AnalysisType.Transient, StopTime = 1, MaxTimestep = 0.001 }; // Default
            this.Loaded += (s, e) => DrawGrid();
            this.KeyDown += MainWindow_KeyDown;
        }

        private void MainWindow_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Escape)
            {
                ExitWiringMode();
                ExitProbingMode();
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
            var nodeControl = new NodeControl { Width = 10, Height = 10 };
            Point position = new Point(SchematicCanvas.ActualWidth / 2, SchematicCanvas.ActualHeight / 2);
            double gridSize = 20.0;
            double snappedX = Math.Round(position.X / gridSize) * gridSize;
            double snappedY = Math.Round(position.Y / gridSize) * gridSize;
            Canvas.SetLeft(nodeControl, snappedX - nodeControl.Width / 2);
            Canvas.SetTop(nodeControl, snappedY - nodeControl.Height / 2);

            MessageBoxResult result = MessageBox.Show("Set this node as ground?", "Ground Node", MessageBoxButton.YesNo, MessageBoxImage.Question);
            if (result == MessageBoxResult.Yes)
            {
                foreach (var existingNode in SchematicCanvas.Children.OfType<NodeControl>())
                {
                    existingNode.IsGround = false;
                }
                nodeControl.IsGround = true;
            }

            SchematicCanvas.Children.Add(nodeControl);
        }

        private void PlaceWire_Click(object sender, RoutedEventArgs e)
        {
            if (!_isWiringMode) EnterWiringMode();
            else ExitWiringMode();
        }

        private void PlaceProbe_Click(object sender, RoutedEventArgs e)
        {
            if (!_isProbingMode) EnterProbingMode();
            else ExitProbingMode();
        }

        private void EnterWiringMode()
        {
            ExitProbingMode();
            _isWiringMode = true;
            _isCircuitLocked = true;
            WireMenuItem.IsChecked = true;
            ProbeMenuItem.IsChecked = false;
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
                if (_currentWire.EndPoint == _currentWire.StartPoint)
                    SchematicCanvas.Children.Remove(_currentWire);
                _currentWire = null;
            }
            if (!_isProbingMode)
            {
                 SchematicCanvas.Cursor = Cursors.Arrow;
            }
            SchematicCanvas.MouseLeftButtonDown -= Canvas_Wiring_MouseDown;
            SchematicCanvas.MouseRightButtonDown -= Canvas_Wiring_MouseRightButtonDown;
            SchematicCanvas.MouseMove -= Canvas_Wiring_MouseMove;
        }

        private void EnterProbingMode()
        {
            ExitWiringMode();
            _isProbingMode = true;
            ProbeMenuItem.IsChecked = true;
            SchematicCanvas.Cursor = Cursors.Help; 
            SchematicCanvas.MouseLeftButtonDown += Canvas_Probing_MouseDown;
        }

        private void ExitProbingMode()
        {
            _isProbingMode = false;
            ProbeMenuItem.IsChecked = false;
            if(!_isWiringMode)
            {
                SchematicCanvas.Cursor = Cursors.Arrow;
            }
            SchematicCanvas.MouseLeftButtonDown -= Canvas_Probing_MouseDown;
        }
        
        private void Canvas_Wiring_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (_isWiringMode) ExitWiringMode();
        }

        private void Canvas_Wiring_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (!_isWiringMode) return;
            Point clickPoint = e.GetPosition(SchematicCanvas);
            
            Point? connectionPoint = FindNearestConnectionPoint(clickPoint);
            Point snappedPoint = connectionPoint ?? new Point(Math.Round(clickPoint.X / 20.0) * 20.0, Math.Round(clickPoint.Y / 20.0) * 20.0);

            if (_currentWire == null)
            {
                _currentWire = new Wire { StartPoint = snappedPoint };
                _currentWire.AddPoint(snappedPoint);
                SchematicCanvas.Children.Add(_currentWire);
            }
            else
            {
                _currentWire.AddPoint(snappedPoint);
            }
            
            if (connectionPoint.HasValue && _currentWire.StartPoint != snappedPoint)
            {
                ExitWiringMode();
            }
        }

        private void Canvas_Probing_MouseDown(object sender, MouseButtonEventArgs e)
        {
            Point clickPoint = e.GetPosition(SchematicCanvas);
            string itemToProbe = NetlistGenerator.FindProbeTarget(SchematicCanvas, clickPoint);

            if (!string.IsNullOrEmpty(itemToProbe))
            {
                if (_probedItems.Contains(itemToProbe))
                {
                    _probedItems.Remove(itemToProbe);
                    MessageBox.Show($"Removed '{itemToProbe}' from plot list.");
                }
                else
                {
                    _probedItems.Add(itemToProbe);
                    MessageBox.Show($"Added '{itemToProbe}' to plot list.");
                }
            }
        }

        private void Canvas_Wiring_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isWiringMode && _currentWire != null)
            {
                Point currentPoint = e.GetPosition(SchematicCanvas);
                Point snappedPoint = new Point(Math.Round(currentPoint.X / 20.0) * 20.0, Math.Round(currentPoint.Y / 20.0) * 20.0);
                _currentWire.UpdatePreview(snappedPoint);
            }
        }

        private void DeselectAll()
        {
            foreach (var child in SchematicCanvas.Children.OfType<Wire>())
                child.IsSelected = false;
        }

        private Point? FindNearestConnectionPoint(Point clickPoint)
        {
            double tolerance = 10.0;
            foreach (var child in SchematicCanvas.Children)
            {
                if (child is ComponentControl component)
                {
                    Point left = component.LeftConnector.TransformToAncestor(SchematicCanvas).Transform(new Point(component.LeftConnector.ActualWidth / 2, component.LeftConnector.ActualHeight / 2));
                    if ((clickPoint - left).Length < tolerance) return left;
                    Point right = component.RightConnector.TransformToAncestor(SchematicCanvas).Transform(new Point(component.RightConnector.ActualWidth / 2, component.RightConnector.ActualHeight / 2));
                    if ((clickPoint - right).Length < tolerance) return right;
                }
                else if (child is NodeControl node)
                {
                    Point center = new Point(Canvas.GetLeft(node) + node.Width / 2, Canvas.GetTop(node) + node.Height / 2);
                    if ((clickPoint - center).Length < tolerance) return center;
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
            if (settingsWindow.ShowDialog() == true)
            {
                _simulationParameters = settingsWindow.Parameters;
                MessageBox.Show("Simulation settings updated!");
            }
        }

        private void RunAnalysis_Click(object sender, RoutedEventArgs e)
        {
            var netlistResult = NetlistGenerator.Generate(SchematicCanvas);
            List<string> netlistCommands = netlistResult.Item1;

            if (!netlistCommands.Any())
            {
                MessageBox.Show("The circuit is empty.", "Empty Circuit");
                return;
            }

            var netlistWindow = new NetlistWindow(netlistCommands);
            netlistWindow.Owner = this;
            netlistWindow.ShowDialog();

            using (var simulator = new CircuitSimulatorService())
            {
                try
                {
                    foreach (var command in netlistCommands)
                    {
                        var parts = command.Split(' ');
                        string type = parts[0];
                        if (type == "GND")
                        {
                            simulator.SetGroundNode(parts[1]);
                        }
                        else if (type == "R")
                        {
                            simulator.AddResistor(parts[1], parts[2], parts[3], double.Parse(parts[4]));
                        }
                        else if (type == "V")
                        {
                            simulator.AddVoltageSource(parts[1], parts[2], parts[3], double.Parse(parts[4]));
                        }
                        else if (type == "ACV")
                        {
                            simulator.AddACVoltageSource(parts[1], parts[2], parts[3], double.Parse(parts[4]), double.Parse(parts[5]));
                        }
                    }

                    bool success = false;
                    var plotWindow = new PlotWindow { Owner = this };

                    switch (_simulationParameters.CurrentAnalysis)
                    {
                        case SimulationParameters.AnalysisType.DCOperatingPoint:
                            success = simulator.RunDCAnalysis();
                            if(success)
                            {
                                var dcResultsDict = simulator.GetAllDCResults();
                                var resultsList = new List<string>();
                                foreach (var kvp in dcResultsDict)
                                {
                                    resultsList.Add($"{kvp.Key} = {kvp.Value:G4}");
                                }
                                var resultsWindow = new DCResultsWindow(resultsList);
                                resultsWindow.Owner = this;
                                resultsWindow.Show();
                            }
                            break;

                        case SimulationParameters.AnalysisType.Transient:
                            success = simulator.RunTransientAnalysis(_simulationParameters.MaxTimestep, _simulationParameters.StopTime);
                             if (success)
                            {
                                var itemsToPlot = _probedItems.Any() ? _probedItems.ToArray() : simulator.GetNodeNames();
                                plotWindow.LoadTransientData(simulator, itemsToPlot);
                                plotWindow.Show();
                            }
                            break;

                        case SimulationParameters.AnalysisType.ACSweep:
                            string acSource = netlistCommands.FirstOrDefault(c => c.StartsWith("ACV"))?.Split(' ')[1] ?? "";
                            if (string.IsNullOrEmpty(acSource))
                            {
                                MessageBox.Show("AC Sweep requires an ACV component in the circuit.", "Simulation Error");
                                return;
                            }
                            success = simulator.RunACAnalysis(acSource, _simulationParameters.StartFrequency, _simulationParameters.StopFrequency, _simulationParameters.NumberOfPoints, _simulationParameters.SweepType);
                            if(success)
                            {
                                var itemsToPlot = _probedItems.Any() ? _probedItems.Where(p => !p.StartsWith("I(")).ToArray() : simulator.GetNodeNames();
                                if (!itemsToPlot.Any())
                                {
                                     MessageBox.Show("Please probe at least one node to plot for AC analysis.", "Plot Error");
                                     return;
                                }
                                plotWindow.LoadACData(simulator, itemsToPlot);
                                plotWindow.Show();
                            }
                            break;
                        
                        case SimulationParameters.AnalysisType.PhaseSweep:
                            string phaseSource = netlistCommands.FirstOrDefault(c => c.StartsWith("ACV"))?.Split(' ')[1] ?? "";
                            if (string.IsNullOrEmpty(phaseSource))
                            {
                                MessageBox.Show("Phase Sweep requires an ACV component in the circuit.", "Simulation Error");
                                return;
                            }
                            // Fix: RunPhaseAnalysis returns an int, not a bool
                            int result = simulator.RunPhaseAnalysis(phaseSource, _simulationParameters.BaseFrequency, _simulationParameters.StartPhase, _simulationParameters.StopPhase, _simulationParameters.NumberOfPoints);
                            success = result > 0; // Consider success if result > 0
                            if(success)
                            {
                                var itemsToPlot = _probedItems.Any() ? _probedItems.Where(p => !p.StartsWith("I(")).ToArray() : simulator.GetNodeNames();
                                if (!itemsToPlot.Any())
                                {
                                     MessageBox.Show("Please probe at least one node to plot for Phase analysis.", "Plot Error");
                                     return;
                                }
                                plotWindow.LoadPhaseData(simulator, itemsToPlot);
                                plotWindow.Show();
                            }
                            break;
                    }

                    if (!success && _simulationParameters.CurrentAnalysis != SimulationParameters.AnalysisType.DCOperatingPoint)
                    {
                        MessageBox.Show("The simulation failed to run.", "Simulation Error");
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"An error occurred during simulation: {ex.Message}", "Error");
                }
            }
        }
    }
}

