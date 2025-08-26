using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;

namespace wpfUI
{
    public class NetlistGenerator
    {
        private class NetlistComponentInfo
        {
            public string Name { get; set; }
            public string Node1 { get; set; } // Positive terminal for V, From terminal for I
            public string Node2 { get; set; } // Negative terminal for V, To terminal for I
            public double Value { get; set; }
            public double AcPhase { get; set; } 
        }

        public static List<string> Generate(Canvas canvas)
        {
            var commands = new List<string>();
            var connectionPoints = new List<Point>();
            var componentInfos = new List<NetlistComponentInfo>();
            var nodeMap = new Dictionary<Point, string>();
            int nodeCounter = 1;

            foreach (var child in canvas.Children.OfType<FrameworkElement>())
            {
                if (child is ComponentControl component)
                {
                    Point leftConnector = component.LeftConnector.TransformToAncestor(canvas).Transform(new Point(component.LeftConnector.ActualWidth / 2, component.LeftConnector.ActualHeight / 2));
                    Point rightConnector = component.RightConnector.TransformToAncestor(canvas).Transform(new Point(component.RightConnector.ActualWidth / 2, component.RightConnector.ActualHeight / 2));
                    connectionPoints.Add(leftConnector);
                    connectionPoints.Add(rightConnector);
                }
                else if (child is NodeControl node)
                {
                    Point nodeCenter = new Point(Canvas.GetLeft(node) + node.Width / 2, Canvas.GetTop(node) + node.Height / 2);
                    connectionPoints.Add(nodeCenter);
                }
            }
            
            var distinctPoints = connectionPoints.Distinct().ToList();
            foreach(var point in distinctPoints)
            {
                if (!nodeMap.ContainsKey(point))
                {
                    nodeMap[point] = $"N{nodeCounter++}";
                }
            }

            foreach (var child in canvas.Children.OfType<ComponentControl>())
            {
                Point leftConnector = child.LeftConnector.TransformToAncestor(canvas).Transform(new Point(child.LeftConnector.ActualWidth / 2, child.LeftConnector.ActualHeight / 2));
                Point rightConnector = child.RightConnector.TransformToAncestor(canvas).Transform(new Point(child.RightConnector.ActualWidth / 2, child.RightConnector.ActualHeight / 2));

                string leftNode = nodeMap.ContainsKey(leftConnector) ? nodeMap[leftConnector] : "UNCONNECTED";
                string rightNode = nodeMap.ContainsKey(rightConnector) ? nodeMap[rightConnector] : "UNCONNECTED";
                
                var info = new NetlistComponentInfo
                {
                    Name = child.ComponentName,
                    Value = child.Value,
                    AcPhase = child.AcPhase
                };

                string type = new string(info.Name.TakeWhile(char.IsLetter).ToArray());
                if (type == "V" || type == "ACV")
                {
                    info.Node1 = rightNode; 
                    info.Node2 = leftNode;
                }
                else if (type == "I")
                {
                    info.Node1 = leftNode;
                    info.Node2 = rightNode;
                }
                else
                {
                    info.Node1 = leftNode;
                    info.Node2 = rightNode;
                }
                
                componentInfos.Add(info);
            }

            foreach (var info in componentInfos)
            {
                string type = new string(info.Name.TakeWhile(char.IsLetter).ToArray());
                string command;
                if (type == "ACV")
                {
                    command = $"{type} {info.Name} {info.Node1} {info.Node2} {info.Value} {info.AcPhase}";
                }
                else
                {
                    command = $"{type} {info.Name} {info.Node1} {info.Node2} {info.Value}";
                }
                commands.Add(command);
            }

            if (nodeMap.Values.Any())
            {
                string groundNode = nodeMap.OrderBy(kvp => kvp.Key.Y).ThenBy(kvp => kvp.Key.X).FirstOrDefault().Value;
                if(groundNode != null)
                {
                    commands.Add($"GND {groundNode}");
                }
            }

            return commands;
        }
    }
}
