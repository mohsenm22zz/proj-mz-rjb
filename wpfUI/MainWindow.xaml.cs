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
        private readonly CircuitSimulatorService _simulatorService;
        private readonly Dictionary<string, int> _componentCounts = new Dictionary<string, int>
        {
            {"R", 1}, {"C", 1}, {"L", 1}, {"D", 1}, {"V", 1}, {"ACV", 1}, {"I", 1}
        };

        private bool _isWiringMode = false;
        private bool _isCircuitLocked = false;
        private Wire _currentWire = null;

        public MainWindow()
        {
            InitializeComponent();
            _simulatorService = new CircuitSimulatorService();
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
            // Finalize the current wire on right-click
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
                // Start a new wire
                _currentWire = new Wire();
                _currentWire.StartPoint = snappedPoint;
                _currentWire.AddPoint(snappedPoint);
                SchematicCanvas.Children.Add(_currentWire);
            }
            else
            {
                // Add a new segment to the existing wire
                _currentWire.AddPoint(snappedPoint);
            }

            // If the click point is on a connection point, end the wire
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
            settingsWindow.ShowDialog();
        }

        private void RunAnalysis_Click(object sender, RoutedEventArgs e)
        {
            var plotWindow = new PlotWindow();
            plotWindow.Owner = this;
            plotWindow.Show();
        }
    }
}
