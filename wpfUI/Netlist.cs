using System.Collections.Generic;
using System.Windows;

namespace wpfUI
{
    // Represents a logical node in the circuit (a point of connection).
    public class NetlistNode
    {
        public int Id { get; }
        public List<UIElement> ConnectedElements { get; } = new List<UIElement>();

        public NetlistNode(int id)
        {
            Id = id;
        }
    }

    // Represents a logical component in the circuit.
    public class NetlistComponent
    {
        public string Name { get; }
        public string Type { get; }
        public NetlistNode Node1 { get; set; }
        public NetlistNode Node2 { get; set; }
        // public double Value { get; set; } // You would add this for resistors, etc.

        public NetlistComponent(string name, string type)
        {
            Name = name;
            Type = type;
        }
    }
}