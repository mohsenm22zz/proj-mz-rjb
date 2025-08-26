using System.Windows;
using System.Windows.Controls;

namespace wpfUI
{
    public partial class SimulationSettingsWindow : Window
    {
        public SimulationParameters Parameters { get; private set; }

        public SimulationSettingsWindow()
        {
            InitializeComponent();
            Parameters = new SimulationParameters();
        }

        private void OkButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (DcOpPntTab.IsSelected)
                {
                    Parameters.CurrentAnalysis = SimulationParameters.AnalysisType.DCAnalysis;
                }
                else if (TransientTab.IsSelected)
                {
                    Parameters.CurrentAnalysis = SimulationParameters.AnalysisType.Transient;
                    Parameters.StopTime = double.Parse(StopTimeTextBox.Text.Replace("s", ""));
                    Parameters.MaxTimestep = double.Parse(MaxTimestepTextBox.Text.Replace("ms", "")) / 1000.0;
                }
                else if (AcSweepTab.IsSelected)
                {
                    Parameters.CurrentAnalysis = SimulationParameters.AnalysisType.ACSweep;
                    Parameters.SweepType = ((ComboBoxItem)SweepTypeComboBox.SelectedItem).Content.ToString();
                    Parameters.StartFrequency = double.Parse(StartFrequencyTextBox.Text.Replace("k", "000"));
                    Parameters.StopFrequency = double.Parse(StopFrequencyTextBox.Text.Replace("k", "000"));
                    Parameters.NumberOfPoints = int.Parse(NumPointsAcTextBox.Text);
                }
                else if (PhaseSweepTab.IsSelected)
                {
                    Parameters.CurrentAnalysis = SimulationParameters.AnalysisType.PhaseSweep;
                    Parameters.BaseFrequency = double.Parse(BaseFrequencyTextBox.Text.Replace("k", "000"));
                    Parameters.StartPhase = double.Parse(StartPhaseTextBox.Text);
                    Parameters.StopPhase = double.Parse(StopPhaseTextBox.Text);
                    Parameters.NumberOfPoints = int.Parse(NumPointsPhaseTextBox.Text);
                }

                DialogResult = true;
            }
            catch (System.Exception ex)
            {
                MessageBox.Show($"Invalid input. Please check the values.\nError: {ex.Message}", "Input Error");
            }
        }
    }
}
