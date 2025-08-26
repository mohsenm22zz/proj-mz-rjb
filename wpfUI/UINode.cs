using System.Windows;
using System.Windows.Shapes;
using System.Collections.Generic;

namespace wpfUI
{
    /// <summary>
    /// Represents a node in the UI for connecting components and wires.
    /// </summary>
    public class UINode
    {
        /// <summary>
        /// The position of the node on the canvas.
        /// </summary>
        public Point Position { get; set; }

        /// <summary>
        /// The visual representation of the node (e.g., a small rectangle).
        /// </summary>
        public Shape Visual { get; set; }

        /// <summary>
        /// A list of UI elements (wires, components) connected to this node.
        /// </summary>
        public List<UIElement> Connections { get; private set; }

        public UINode(Point position)
        {
            Position = position;
            Connections = new List<UIElement>();
            Visual = new Rectangle
            {
                Width = 6,
                Height = 6,
                Fill = System.Windows.Media.Brushes.Black,
                Margin = new Thickness(-3, -3, 0, 0) // Center the visual on the position
            };
        }
    }
}