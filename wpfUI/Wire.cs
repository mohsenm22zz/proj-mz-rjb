using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace wpfUI
{
    public class Wire : ContentControl
    {
        private Path _wirePath;
        private PathGeometry _pathGeometry;
        private PathFigure _pathFigure;
        // --- MODIFIED: Use a PolyLineSegment for multiple points ---
        private PolyLineSegment _polyLineSegment;

        public Point StartPoint
        {
            get { return _pathFigure.StartPoint; }
            set { _pathFigure.StartPoint = value; }
        }

        // The EndPoint is now the last point in our collection
        public Point EndPoint
        {
            get { return _polyLineSegment.Points.Count > 0 ? _polyLineSegment.Points[_polyLineSegment.Points.Count - 1] : StartPoint; }
        }

        private bool _isSelected;
        public bool IsSelected
        {
            get { return _isSelected; }
            set
            {
                _isSelected = value;
                UpdateVisualState();
            }
        }

        public Wire()
        {
            // --- MODIFIED: Setup for PolyLineSegment ---
            _polyLineSegment = new PolyLineSegment();
            _pathFigure = new PathFigure(new Point(0, 0), new[] { _polyLineSegment }, false);
            _pathGeometry = new PathGeometry(new[] { _pathFigure });
            _wirePath = new Path
            {
                Data = _pathGeometry,
                StrokeThickness = 2,
                Stroke = Brushes.Cyan
            };

            this.Content = new Grid { Children = { _wirePath } };
            Panel.SetZIndex(this, -1);
        }

        // --- NEW: Method to update the wire's shape for right-angled drawing ---
        public void UpdatePath(Point newEndPoint)
        {
            Point start = this.StartPoint;
            Point elbowPoint;

            // Determine if the first segment is horizontal or vertical
            double deltaX = System.Math.Abs(newEndPoint.X - start.X);
            double deltaY = System.Math.Abs(newEndPoint.Y - start.Y);

            if (deltaX > deltaY) // Horizontal segment first
            {
                elbowPoint = new Point(newEndPoint.X, start.Y);
            }
            else // Vertical segment first
            {
                elbowPoint = new Point(start.X, newEndPoint.Y);
            }

            // Update the points for the polyline
            _polyLineSegment.Points.Clear();
            _polyLineSegment.Points.Add(elbowPoint);
            _polyLineSegment.Points.Add(newEndPoint);
        }


        private void UpdateVisualState()
        {
            if (IsSelected)
            {
                _wirePath.Stroke = Brushes.Yellow;
                _wirePath.StrokeThickness = 4;
            }
            else
            {
                _wirePath.Stroke = Brushes.Cyan;
                _wirePath.StrokeThickness = 2;
            }
        }

        protected override void OnMouseDown(System.Windows.Input.MouseButtonEventArgs e)
        {
            base.OnMouseDown(e);
            IsSelected = !IsSelected;
        }
    }
}
