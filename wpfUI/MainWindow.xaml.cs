using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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
        private Wire _previewWire = null;
        private const double GridSize = 20.0;

        public MainWindow()
        {
            InitializeComponent();
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
            var existingChildren = SchematicCanvas.Children.OfType<UIElement>()
                                        .Where(c => !(c is Line && ((Line)c).StrokeThickness == 1))
                                        .ToList();
            SchematicCanvas.Children.Clear();

            for (double x = 0; x < SchematicCanvas.ActualWidth; x += GridSize)
            {
                var line = new Line { X1 = x, Y1 = 0, X2 = x, Y2 = SchematicCanvas.ActualHeight, Stroke = new SolidColorBrush(Color.FromArgb(50, 80, 80, 80)), StrokeThickness = 1 };
                Panel.SetZIndex(line, -10);
                SchematicCanvas.Children.Add(line);
            }
            for (double y = 0; y < SchematicCanvas.ActualHeight; y += GridSize)
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
                
                // Position component in the center of the canvas
                double centerX = SchematicCanvas.ActualWidth / 2;
                double centerY = SchematicCanvas.ActualHeight / 2;
                Canvas.SetLeft(componentControl, centerX - componentControl.Width / 2);
                Canvas.SetTop(componentControl, centerY - componentControl.Height / 2);
                
                SchematicCanvas.Children.Add(componentControl);
                _componentCounts[type]++;
            }
        }

        private void PlaceNode_Click(object sender, RoutedEventArgs e)
        {
            if (_isCircuitLocked)
            {
                MessageBox.Show("Cannot add new nodes while circuit is wired.", "Circuit Locked");
                return;
            }

            var nodeControl = new NodeControl();
            
            // Position node in the center of the canvas
            double centerX = SchematicCanvas.ActualWidth / 2;
            double centerY = SchematicCanvas.ActualHeight / 2;
            Canvas.SetLeft(nodeControl, centerX - nodeControl.Width / 2);
            Canvas.SetTop(nodeControl, centerY - nodeControl.Height / 2);
            
            SchematicCanvas.Children.Add(nodeControl);
        }

        private void PlaceWire_Click(object sender, RoutedEventArgs e)
        {
            _isWiringMode = !_isWiringMode;
            WireMenuItem.IsChecked = _isWiringMode;
            if (_isWiringMode)
            {
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
            Point snappedPoint = new Point(
                Math.Round(clickPoint.X / GridSize) * GridSize,
                Math.Round(clickPoint.Y / GridSize) * GridSize);

            if (_previewWire == null)
            {
                _previewWire = new Wire { StartPoint = snappedPoint };
                _previewWire.UpdatePath(snappedPoint);
                SchematicCanvas.Children.Add(_previewWire);
            }
            else
            {
                _previewWire.UpdatePath(snappedPoint);
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
            foreach (var child in SchematicCanvas.Children.OfType<Wire>())
                child.IsSelected = false;
        }

        private void RunAnalysis_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var (components, nodes) = BuildNetlistFromCanvas();
                if (components.Count == 0)
                {
                    MessageBox.Show("The circuit is empty or invalid.", "Netlist Error");
                    return;
                }

                using (var sim = new CircuitSimulatorService())
                {
                    foreach (var node in nodes)
                        sim.AddNode($"N{node.Id}");
                    
                    sim.SetGroundNode("N0");

                    foreach (var component in components)
                    {
                        if (component.Type.StartsWith("R"))
                            sim.AddResistor(component.Name, $"N{component.Node1.Id}", $"N{component.Node2.Id}", 1000.0);
                        else if (component.Type.StartsWith("V"))
                            sim.AddVoltageSource(component.Name, $"N{component.Node1.Id}", $"N{component.Node2.Id}", 5.0);
                    }

                    bool success = sim.RunDCAnalysis();
                    if (success)
                    {
                        StringBuilder result = new StringBuilder("DC Analysis Complete:\n");
                        foreach (var node in nodes.Where(n => n.Id != 0))
                        {
                            double voltage = sim.GetNodeVoltage($"N{node.Id}");
                            result.AppendLine($"Voltage at Node {node.Id}: {voltage:F3} V");
                        }
                        MessageBox.Show(result.ToString(), "Analysis Results");
                    }
                    else
                    {
                        MessageBox.Show("Analysis failed. Check circuit connections.", "Error");
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error building or running circuit: {ex.Message}", "Error");
            }
        }

        private (List<NetlistComponent> components, List<NetlistNode> nodes) BuildNetlistFromCanvas()
        {
            var componentsOnCanvas = SchematicCanvas.Children.OfType<ComponentControl>().ToList();
            var nodesOnCanvas = SchematicCanvas.Children.OfType<NodeControl>().ToList();
            var wiresOnCanvas = SchematicCanvas.Children.OfType<Wire>().ToList();

            var connectionPoints = new Dictionary<Point, UIElement>();
            
            foreach (var comp in componentsOnCanvas)
            {
                Point leftPos = comp.LeftConnector.TransformToAncestor(SchematicCanvas).Transform(new Point(comp.LeftConnector.ActualWidth / 2, comp.LeftConnector.ActualHeight / 2));
                Point rightPos = comp.RightConnector.TransformToAncestor(SchematicCanvas).Transform(new Point(comp.RightConnector.ActualWidth / 2, comp.RightConnector.ActualHeight / 2));
                connectionPoints[leftPos] = comp.LeftConnector;
                connectionPoints[rightPos] = comp.RightConnector;
            }

            foreach (var node in nodesOnCanvas)
            {
                Point nodePos = new Point(Canvas.GetLeft(node) + node.Width / 2, Canvas.GetTop(node) + node.Height / 2);
                connectionPoints[nodePos] = node;
            }

            var pointList = connectionPoints.Keys.ToList();
            var dsu = new DisjointSet(pointList.Count);

            foreach (var wire in wiresOnCanvas)
            {
                var connectedIndices = new List<int>();
                for (int i = 0; i < pointList.Count; i++)
                {
                    if (IsPointOnWire(pointList[i], wire))
                    {
                        connectedIndices.Add(i);
                    }
                }
                for (int i = 0; i < connectedIndices.Count - 1; i++)
                {
                    dsu.Union(connectedIndices[i], connectedIndices[i + 1]);
                }
            }

            var rootToNodeId = new Dictionary<int, int>();
            int nextNodeId = 1;
            var pointToFinalNodeId = new Dictionary<Point, int>();

            for (int i = 0; i < pointList.Count; i++)
            {
                int root = dsu.Find(i);
                if (!rootToNodeId.ContainsKey(root))
                {
                    rootToNodeId[root] = nextNodeId++;
                }
                pointToFinalNodeId[pointList[i]] = rootToNodeId[root];
            }

            var netlistComponents = new List<NetlistComponent>();
            var logicalNodes = new List<NetlistNode>();

            foreach (var comp in componentsOnCanvas)
            {
                var leftPos = comp.LeftConnector.TransformToAncestor(SchematicCanvas).Transform(new Point(comp.LeftConnector.ActualWidth / 2, comp.LeftConnector.ActualHeight / 2));
                var rightPos = comp.RightConnector.TransformToAncestor(SchematicCanvas).Transform(new Point(comp.RightConnector.ActualWidth / 2, comp.RightConnector.ActualHeight / 2));
                
                int leftNodeId = pointToFinalNodeId.ContainsKey(leftPos) ? pointToFinalNodeId[leftPos] : 0;
                int rightNodeId = pointToFinalNodeId.ContainsKey(rightPos) ? pointToFinalNodeId[rightPos] : 0;

                var netComp = new NetlistComponent(comp.ComponentName, comp.ComponentName)
                {
                    Node1 = GetOrCreateNode(logicalNodes, leftNodeId),
                    Node2 = GetOrCreateNode(logicalNodes, rightNodeId)
                };
                netlistComponents.Add(netComp);
            }
            
            GetOrCreateNode(logicalNodes, 0);
            return (netlistComponents, logicalNodes.OrderBy(n => n.Id).ToList());
        }

        private NetlistNode GetOrCreateNode(List<NetlistNode> nodes, int id)
        {
            var node = nodes.FirstOrDefault(n => n.Id == id);
            if (node == null)
            {
                node = new NetlistNode(id);
                nodes.Add(node);
            }
            return node;
        }

        private bool IsPointOnWire(Point p, Wire w)
        {
            double tolerance = 1.0;
            var start = w.StartPoint;
            var path = w.Content as Path;
            if (path?.Data is not PathGeometry geometry) return false;
            var figure = geometry.Figures.FirstOrDefault();
            if (figure?.Segments.FirstOrDefault() is not PolyLineSegment polySegment) return false;

            Point current = start;
            foreach (var point in polySegment.Points)
            {
                var minX = Math.Min(current.X, point.X) - tolerance;
                var maxX = Math.Max(current.X, point.X) + tolerance;
                var minY = Math.Min(current.Y, point.Y) - tolerance;
                var maxY = Math.Max(current.Y, point.Y) + tolerance;

                if (p.X >= minX && p.X <= maxX && p.Y >= minY && p.Y <= maxY)
                {
                    if (Math.Abs(current.X - point.X) < tolerance)
                    {
                        if (Math.Abs(p.X - current.X) < tolerance) return true;
                    }
                    else if (Math.Abs(current.Y - point.Y) < tolerance)
                    {
                        if (Math.Abs(p.Y - current.Y) < tolerance) return true;
                    }
                }
                current = point;
            }
            return false;
        }

        private void PlaceComponent_Click(object sender, RoutedEventArgs e) { MessageBox.Show("Component Library window would open here.", "Place Component"); }
        private void EditSimulationCmd_Click(object sender, RoutedEventArgs e) { MessageBox.Show("Simulation Settings dialog would open here.", "Edit Simulation Command"); }
    }

    public class DisjointSet
    {
        private int[] parent;
        public DisjointSet(int size)
        {
            parent = new int[size];
            for (int i = 0; i < size; i++)
                parent[i] = i;
        }

        public int Find(int i)
        {
            if (parent[i] == i)
                return i;
            return parent[i] = Find(parent[i]);
        }

        public void Union(int i, int j)
        {
            int rootI = Find(i);
            int rootJ = Find(j);
            if (rootI != rootJ)
                parent[rootI] = rootJ;
        }
    }
}
