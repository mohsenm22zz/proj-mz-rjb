using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;

namespace wpfUI
{
    /// <summary>
    /// Generates a netlist from the components on the SchematicCanvas.
    /// </summary>
    public class NetlistGenerator
    {
        private class NetlistComponentInfo
        {
            public string Name { get; set; }
            public string Node1 { get; set; }
            public string Node2 { get; set; }
            public double Value { get; set; } // Simplified value
        }

        public static List<string> Generate(Canvas canvas)
        {
            var commands = new List<string>();
            var connectionPoints = new List<Point>();
            var componentInfos = new List<NetlistComponentInfo>();
            var nodeMap = new Dictionary<Point, string>();
            int nodeCounter = 1;

            // 1. Find all connection points from components and nodes
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
            
            // 2. Group connection points that are connected by wires into logical nodes
            // (This is a simplified version. A full implementation would use graph traversal.)
            var distinctPoints = connectionPoints.Distinct().ToList();
            foreach(var point in distinctPoints)
            {
                if (!nodeMap.ContainsKey(point))
                {
                    // For now, treat every unique connection point as a unique node.
                    // A ground node should be explicitly handled (e.g., named "0").
                    nodeMap[point] = $"N{nodeCounter++}";
                }
            }

            // 3. Create component info with mapped node names
            foreach (var child in canvas.Children.OfType<ComponentControl>())
            {
                Point leftConnector = child.LeftConnector.TransformToAncestor(canvas).Transform(new Point(child.LeftConnector.ActualWidth / 2, child.LeftConnector.ActualHeight / 2));
                Point rightConnector = child.RightConnector.TransformToAncestor(canvas).Transform(new Point(child.RightConnector.ActualWidth / 2, child.RightConnector.ActualHeight / 2));

                string node1 = nodeMap.ContainsKey(leftConnector) ? nodeMap[leftConnector] : "UNCONNECTED";
                string node2 = nodeMap.ContainsKey(rightConnector) ? nodeMap[rightConnector] : "UNCONNECTED";
                
                // This is a placeholder for getting component values (e.g., from a dialog)
                double value = 1; // Default value
                if (child.ComponentName.StartsWith("R")) value = 1000; // 1k Ohm
                if (child.ComponentName.StartsWith("V")) value = 5;    // 5 Volts

                componentInfos.Add(new NetlistComponentInfo
                {
                    Name = child.ComponentName,
                    Node1 = node1,
                    Node2 = node2,
                    Value = value
                });
            }

            // 4. Build the final command strings
            foreach (var info in componentInfos)
            {
                string type = new string(info.Name.TakeWhile(char.IsLetter).ToArray());
                string command = $"{type} {info.Name} {info.Node1} {info.Node2} {info.Value}";
                commands.Add(command);
            }

            // Add a ground node command (assuming one node is ground)
            if (nodeMap.Values.Any())
            {
                 // Heuristic: find the lowest node and call it ground.
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
