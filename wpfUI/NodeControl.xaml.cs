using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace wpfUI
{
    public partial class NodeControl : UserControl
    {
        public bool HasBeenPlaced { get; set; } = false;

        public NodeControl()
        {
            InitializeComponent();
        }

        private Point startPoint;
        private bool isDragging = false;

        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonDown(e);
            if (!HasBeenPlaced && Parent is Canvas)
            {
                startPoint = e.GetPosition(Parent as IInputElement);
                this.CaptureMouse();
                isDragging = true;
            }
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            if (isDragging)
            {
                Point currentPoint = e.GetPosition(Parent as IInputElement);
                double newX = Canvas.GetLeft(this) + (currentPoint.X - startPoint.X);
                double newY = Canvas.GetTop(this) + (currentPoint.Y - startPoint.Y);

                Canvas.SetLeft(this, newX);
                Canvas.SetTop(this, newY);

                startPoint = currentPoint;
            }
        }

        protected override void OnMouseLeftButtonUp(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonUp(e);
            if (isDragging)
            {
                this.ReleaseMouseCapture();
                isDragging = false;

                double gridSize = 20.0;
                Point currentPosition = new Point(Canvas.GetLeft(this) + this.Width / 2, Canvas.GetTop(this) + this.Height / 2);
                
                double snappedX = Math.Round(currentPosition.X / gridSize) * gridSize;
                double snappedY = Math.Round(currentPosition.Y / gridSize) * gridSize;

                Canvas.SetLeft(this, snappedX - this.Width / 2);
                Canvas.SetTop(this, snappedY - this.Height / 2);

                HasBeenPlaced = true;
            }
        }
    }
}
