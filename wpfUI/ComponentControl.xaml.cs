using System.Windows.Controls;
using System.Windows.Input;
using System.Windows;

namespace wpfUI
{
    /// <summary>
    /// Interaction logic for ComponentControl.xaml
    /// </summary>
    public partial class ComponentControl : UserControl
    {
        // Dependency property to make the component name bindable
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

        // Basic drag-and-drop functionality
        private Point startPoint;
        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonDown(e);
            startPoint = e.GetPosition(Parent as IInputElement);
            this.CaptureMouse();
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            if (this.IsMouseCaptured)
            {
                Point currentPoint = e.GetPosition(Parent as IInputElement);
                var dragDelta = currentPoint - startPoint;

                Canvas.SetLeft(this, Canvas.GetLeft(this) + dragDelta.X);
                Canvas.SetTop(this, Canvas.GetTop(this) + dragDelta.Y);

                startPoint = currentPoint;
            }
        }

        protected override void OnMouseLeftButtonUp(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonUp(e);
            this.ReleaseMouseCapture();
        }
    }
}
