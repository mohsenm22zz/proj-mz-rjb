using System.Windows.Controls;
using System.Windows.Input;
using System.Windows;
using System;

namespace wpfUI
{
    /// <summary>
    /// Interaction logic for ComponentControl.xaml
    /// </summary>
    public partial class ComponentControl : UserControl
    {
        public static readonly DependencyProperty ComponentNameProperty =
            DependencyProperty.Register("ComponentName", typeof(string), typeof(ComponentControl), new PropertyMetadata("Cmp", OnComponentNameChanged));

        public string ComponentName
        {
            get { return (string)GetValue(ComponentNameProperty); }
            set { SetValue(ComponentNameProperty, value); }
        }

        private static void OnComponentNameChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var control = d as ComponentControl;
            if (control != null)
            {
                control.ComponentNameLabel.Text = e.NewValue.ToString();
            }
        }

        public ComponentControl()
        {
            InitializeComponent();
        }

        // --- Drag-and-drop with Grid Snapping ---
        private Point startPoint;
        private bool isDragging = false;

        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonDown(e);
            if (Parent is Canvas)
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

                // --- NEW: Snap to Grid Logic ---
                double gridSize = 20.0; // The size of our grid cells
                double currentLeft = Canvas.GetLeft(this);
                double currentTop = Canvas.GetTop(this);

                // Calculate the nearest grid point for the center of the component
                double centerX = currentLeft + this.Width / 2;
                double centerY = currentTop + this.Height / 2;

                double snappedCenterX = Math.Round(centerX / gridSize) * gridSize;
                double snappedCenterY = Math.Round(centerY / gridSize) * gridSize;

                // Adjust the top-left position based on the snapped center
                Canvas.SetLeft(this, snappedCenterX - this.Width / 2);
                Canvas.SetTop(this, snappedCenterY - this.Height / 2);
            }
        }
    }
}
