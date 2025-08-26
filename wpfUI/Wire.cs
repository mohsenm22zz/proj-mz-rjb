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
        private PolyLineSegment _polyLineSegment;

        public Point StartPoint
        {
            get { return _pathFigure.StartPoint; }
            set { _pathFigure.StartPoint = value; }
        }

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
            _polyLineSegment = new PolyLineSegment();
            _pathFigure = new PathFigure { IsClosed = false };
            _pathFigure.Segments.Add(_polyLineSegment);
            _pathGeometry = new PathGeometry();
            _pathGeometry.Figures.Add(_pathFigure);
            _wirePath = new Path
            {
                Data = _pathGeometry,
                StrokeThickness = 2,
                Stroke = Brushes.Cyan
            };

            this.Content = new Grid { Children = { _wirePath } };
            Panel.SetZIndex(this, -1);
        }

        public void AddPoint(Point newPoint)
        {
            Point lastPoint = EndPoint;

            double deltaX = System.Math.Abs(newPoint.X - lastPoint.X);
            double deltaY = System.Math.Abs(newPoint.Y - lastPoint.Y);
            if (_polyLineSegment.Points.Count > 0)
            {
                 if (deltaX > deltaY)
                {
                    _polyLineSegment.Points.Add(new Point(newPoint.X, lastPoint.Y));
                }
                else
                {
                    _polyLineSegment.Points.Add(new Point(lastPoint.X, newPoint.Y));
                }
            }
            _polyLineSegment.Points.Add(newPoint);
        }

        public void UpdatePreview(Point previewPoint)
        {
            if (_polyLineSegment.Points.Count > 1 && _polyLineSegment.Points[_polyLineSegment.Points.Count - 2] == _polyLineSegment.Points[_polyLineSegment.Points.Count - 1])
            {
                 _polyLineSegment.Points.RemoveAt(_polyLineSegment.Points.Count - 1);
            }
            
            Point lastPoint = EndPoint;
            double deltaX = System.Math.Abs(previewPoint.X - lastPoint.X);
            double deltaY = System.Math.Abs(previewPoint.Y - lastPoint.Y);

            Point elbowPoint;
            if (deltaX > deltaY)
            {
                elbowPoint = new Point(previewPoint.X, lastPoint.Y);
            }
            else
            {
                elbowPoint = new Point(lastPoint.X, previewPoint.Y);
            }

            if (_polyLineSegment.Points.Count > 0)
            {
                _polyLineSegment.Points[_polyLineSegment.Points.Count - 1] = elbowPoint;
                _polyLineSegment.Points.Add(previewPoint);
            }
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