// mohsenm22zz/proj-mz-rjb/proj-mz-rjb-1b949d5aa204b9f590a1c5f0644f3424cf2a70ce/wpfUI/MainWindow.xaml.cs

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
            SchematicCanvas.Cursor = Cursors.Arrow;
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
            SchematicCanvas.Cursor = Cursors.Arrow;
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
            List<string> netlistCommands = NetlistGenerator.Generate(SchematicCanvas);
            if (!netlistCommands.Any())
            {
                MessageBox.Show("The circuit is empty.", "Empty Circuit");
                return;
            }

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
                    switch (_simulationParameters.CurrentAnalysis)
                    {
                        case SimulationParameters.AnalysisType.Transient:
                            success = simulator.RunTransientAnalysis(_simulationParameters.MaxTimestep, _simulationParameters.StopTime);
                            break;
                        case SimulationParameters.AnalysisType.ACSweep:
                            string acSource = netlistCommands.FirstOrDefault(c => c.StartsWith("ACV"))?.Split(' ')[1] ?? "";
                            if (string.IsNullOrEmpty(acSource))
                            {
                                MessageBox.Show("AC Sweep requires an ACV component in the circuit.", "Simulation Error");
                                return;
                            }
                            success = simulator.RunACAnalysis(acSource, _simulationParameters.StartFrequency, _simulationParameters.StopFrequency, _simulationParameters.NumberOfPoints, _simulationParameters.SweepType);
                            break;
                    }

                    if (success)
                    {
                        var plotWindow = new PlotWindow();
                        plotWindow.Owner = this;
                        var itemsToPlot = _probedItems.Any() ? _probedItems.ToArray() : simulator.GetNodeNames();
                        plotWindow.LoadData(simulator, itemsToPlot);
                        plotWindow.Show();
                    }
                    else
                    {
                        MessageBox.Show("The simulation failed to run.", "Simulation Error");
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"An error occurred during simulation setup: {ex.Message}", "Error");
                }
            }
        }
    }
}
