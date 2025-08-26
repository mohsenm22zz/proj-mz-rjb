using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace wpfUI
{
    public class NetlistGenerator
    {
        private class NetlistComponentInfo
        {
            public string Name { get; set; }
            public string Node1 { get; set; }
            public string Node2 { get; set; }
            public double Value { get; set; }
            public double AcPhase { get; set; }
        }

        public static Tuple<List<string>, Dictionary<Point, string>> Generate(Canvas canvas)
        {
            var commands = new List<string>();
            var allPoints = new HashSet<Point>();
            var adjacencyList = new Dictionary<Point, List<Point>>();
            var componentInfos = new List<NetlistComponentInfo>();
            var nodeMap = new Dictionary<Point, string>();
            Point? groundPoint = null;

            foreach (var child in canvas.Children.OfType<FrameworkElement>())
            {
                if (child is ComponentControl component)
                {
                    Point left = component.LeftConnector.TransformToAncestor(canvas).Transform(new Point(component.LeftConnector.ActualWidth / 2, component.LeftConnector.ActualHeight / 2));
                    Point right = component.RightConnector.TransformToAncestor(canvas).Transform(new Point(component.RightConnector.ActualWidth / 2, component.RightConnector.ActualHeight / 2));
                    allPoints.Add(left);
                    allPoints.Add(right);
                }
                else if (child is NodeControl node)
                {
                    Point center = new Point(Canvas.GetLeft(node) + node.Width / 2, Canvas.GetTop(node) + node.Height / 2);
                    allPoints.Add(center);
                    if (node.Tag as string == "Ground")
                    {
                        groundPoint = center;
                    }
                }
            }

            foreach (var wire in canvas.Children.OfType<Wire>())
            {
                var pathFigure = (wire.Content as Grid)?.Children.OfType<Path>().FirstOrDefault()?.Data.GetFlattenedPathGeometry().Figures.FirstOrDefault();
                if (pathFigure != null)
                {
                    var wirePoints = new List<Point> { pathFigure.StartPoint };
                    foreach (var segment in pathFigure.Segments.OfType<LineSegment>())
                    {
                        wirePoints.Add(segment.Point);
                    }
                    
                    for (int i = 0; i < wirePoints.Count - 1; i++)
                    {
                        Point p1 = wirePoints[i];
                        Point p2 = wirePoints[i + 1];
                        allPoints.Add(p1);
                        allPoints.Add(p2);

                        if (!adjacencyList.ContainsKey(p1)) adjacencyList[p1] = new List<Point>();
                        if (!adjacencyList.ContainsKey(p2)) adjacencyList[p2] = new List<Point>();
                        adjacencyList[p1].Add(p2);
                        adjacencyList[p2].Add(p1);
                    }
                }
            }

            var visited = new HashSet<Point>();
            int nodeCounter = 1;
            foreach (var startPoint in allPoints)
            {
                if (!visited.Contains(startPoint))
                {
                    string nodeName = $"N{nodeCounter++}";
                    var queue = new Queue<Point>();
                    queue.Enqueue(startPoint);
                    visited.Add(startPoint);

                    while (queue.Count > 0)
                    {
                        var currentPoint = queue.Dequeue();
                        nodeMap[currentPoint] = nodeName;

                        if (adjacencyList.ContainsKey(currentPoint))
                        {
                            foreach (var neighbor in adjacencyList[currentPoint])
                            {
                                if (!visited.Contains(neighbor))
                                {
                                    visited.Add(neighbor);
                                    queue.Enqueue(neighbor);
                                }
                            }
                        }
                    }
                }
            }
            
            if (groundPoint.HasValue && nodeMap.ContainsKey(groundPoint.Value))
            {
                string groundNodeName = nodeMap[groundPoint.Value];
                var pointsToUpdate = nodeMap.Where(kvp => kvp.Value == groundNodeName).Select(kvp => kvp.Key).ToList();
                foreach (var point in pointsToUpdate)
                {
                    nodeMap[point] = "0";
                }
            }

            foreach (var child in canvas.Children.OfType<ComponentControl>())
            {
                Point leftConnector = child.LeftConnector.TransformToAncestor(canvas).Transform(new Point(child.LeftConnector.ActualWidth / 2, child.LeftConnector.ActualHeight / 2));
                Point rightConnector = child.RightConnector.TransformToAncestor(canvas).Transform(new Point(child.RightConnector.ActualWidth / 2, child.RightConnector.ActualHeight / 2));

                string leftNode = nodeMap.ContainsKey(leftConnector) ? nodeMap[leftConnector] : "UNCONNECTED";
                string rightNode = nodeMap.ContainsKey(rightConnector) ? nodeMap[rightConnector] : "UNCONNECTED";
                
                var info = new NetlistComponentInfo { Name = child.ComponentName, Value = child.Value, AcPhase = child.AcPhase };
                string type = new string(info.Name.TakeWhile(char.IsLetter).ToArray());

                if (type == "V" || type == "ACV") { info.Node1 = rightNode; info.Node2 = leftNode; }
                else { info.Node1 = leftNode; info.Node2 = rightNode; }
                
                componentInfos.Add(info);
            }

            foreach (var info in componentInfos)
            {
                string type = new string(info.Name.TakeWhile(char.IsLetter).ToArray());
                string command = (type == "ACV")
                    ? $"{type} {info.Name} {info.Node1} {info.Node2} {info.Value} {info.AcPhase}"
                    : $"{type} {info.Name} {info.Node1} {info.Node2} {info.Value}";
                commands.Add(command);
            }

            if (nodeMap.ContainsValue("0"))
            {
                commands.Add($"GND 0");
            }

            return new Tuple<List<string>, Dictionary<Point, string>>(commands, nodeMap);
        }
    }
}
