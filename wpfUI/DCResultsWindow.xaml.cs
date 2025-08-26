// mohsenm22zz/proj-mz-rjb/proj-mz-rjb-e850e6c0f7d11e5661819e4f80ff5ef06a6db456/wpfUI/DCResultsWindow.xaml.cs

using System.Collections.Generic;
using System.Windows;

namespace wpfUI
{
    public partial class DCResultsWindow : Window
    {
        public DCResultsWindow(List<string> results)
        {
            InitializeComponent();
            // This now works because 'ResultsListBox' is defined in the XAML
            ResultsListBox.ItemsSource = results;
        }

        // --- FIX: Added the missing OkButton_Click event handler ---
        private void OkButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}