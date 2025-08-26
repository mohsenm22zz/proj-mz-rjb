namespace wpfUI
{
    public class SimulationParameters
    {
        // --- MODIFIED: Added DCAnalysis enum member ---
        public enum AnalysisType { DCAnalysis, Transient, ACSweep, PhaseSweep }

        public AnalysisType CurrentAnalysis { get; set; }

        // Transient Parameters
        public double StopTime { get; set; }
        public double TimeToStartSavingData { get; set; }
        public double MaxTimestep { get; set; }

        // AC Sweep Parameters
        public string SweepType { get; set; }
        public double StartFrequency { get; set; }
        public double StopFrequency { get; set; }
        public int NumberOfPoints { get; set; }

        // Phase Sweep Parameters
        public double BaseFrequency { get; set; }
        public double StartPhase { get; set; }
        public double StopPhase { get; set; }
    }
}
