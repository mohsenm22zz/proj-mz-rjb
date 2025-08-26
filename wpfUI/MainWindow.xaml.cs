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
        private readonly CircuitSimulatorService _simulatorService;
        private readonly Dictionary<string, int> _componentCounts = new Dictionary<string, int>
        {
            {"R", 1}, {"C", 1}, {"L", 1}, {"D", 1}, {"V", 1}, {"ACV", 1}, {"I", 1}
        };

        // --- State Variables ---
        private bool _isWiringMode = false;
        private bool _isCircuitLocked = false;
        private Wire _previewWire = null;

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
            var existingChildren = SchematicCanvas.Children.OfType<UIElement>()
                                        .Where(c => !(c is Line && ((Line)c).StrokeThickness == 1))
                                        .ToList();

            SchematicCanvas.Children.Clear();

            for (double x = 0; x < SchematicCanvas.ActualWidth; x += gridSize)
            {
                var line = new Line { X1 = x, Y1 = 0, X2 = x, Y2 = SchematicCanvas.ActualHeight, Stroke = new SolidColorBrush(Color.FromArgb(50, 80, 80, 80)), StrokeThickness = 1 };
                Panel.SetZIndex(line, -10);
                SchematicCanvas.Children.Add(line);
            }

            for (double y = 0; y < SchematicCanvas.ActualHeight; y += gridSize)
            {
                var line = new Line { X1 = 0, Y1 = y, X2 = SchematicCanvas.ActualWidth, Y2 = y, Stroke = new SolidColorBrush(Color.FromArgb(50, 80, 80, 80)), StrokeThickness = 1 };
                Panel.SetZIndex(line, -10);
                SchematicCanvas.Children.Add(line);
            }

            foreach (var child in existingChildren)
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

        // --- FIXED: Now uses the proper NodeControl ---
        private void PlaceNode_Click(object sender, RoutedEventArgs e)
        {
            var nodeControl = new NodeControl();

            Point position = Mouse.GetPosition(SchematicCanvas);
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
                _isWiringMode = true;
                _isCircuitLocked = true;
                WireMenuItem.IsChecked = true;
                SchematicCanvas.Cursor = Cursors.Cross;
                SchematicCanvas.MouseLeftButtonDown += Canvas_Wiring_MouseDown;
                SchematicCanvas.MouseMove += Canvas_Wiring_MouseMove;
            }
            else
            {
                ExitWiringMode();
            }
        }

        private void ExitWiringMode()
        {
            _isWiringMode = false;
            WireMenuItem.IsChecked = false;
            if (_previewWire != null)
            {
                SchematicCanvas.Children.Remove(_previewWire);
                _previewWire = null;
            }
            SchematicCanvas.Cursor = Cursors.Arrow;
            SchematicCanvas.MouseLeftButtonDown -= Canvas_Wiring_MouseDown;
            SchematicCanvas.MouseMove -= Canvas_Wiring_MouseMove;
        }

        private void Canvas_Wiring_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (!_isWiringMode) return;

            Point clickPoint = e.GetPosition(SchematicCanvas);
            Point? connectionPoint = FindNearestConnectionPoint(clickPoint);

            if (connectionPoint == null) return;

            if (_previewWire == null)
            {
                _previewWire = new Wire
                {
                    StartPoint = connectionPoint.Value,
                };
                _previewWire.UpdatePath(connectionPoint.Value);
                SchematicCanvas.Children.Add(_previewWire);
            }
            else
            {
                _previewWire.UpdatePath(connectionPoint.Value);
                _previewWire = null;
            }
        }

        private void Canvas_Wiring_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isWiringMode && _previewWire != null)
            {
                Point currentPoint = e.GetPosition(SchematicCanvas);
                _previewWire.UpdatePath(currentPoint);
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

        // --- FIXED: Now looks for NodeControl instead of Ellipse ---
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
                else if (child is NodeControl node) // Check for the correct control type
                {
                    Point nodeCenter = new Point(Canvas.GetLeft(node) + node.ActualWidth / 2, Canvas.GetTop(node) + node.ActualHeight / 2);
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
            MessageBox.Show("Simulation Settings dialog would open here.", "Edit Simulation Command");
        }

        private void RunAnalysis_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Analysis would run based on the schematic on the canvas.", "Run Analysis");
        }
    }
}
