// mohsenm22zz/proj-mz-rjb/proj-mz-rjb-e850e6c0f7d11e5661819e4f80ff5ef06a6db456/wpfUI/SimulationParameters.cs

using System;

namespace wpfUI
{
    public class SimulationParameters
    {
        // --- FIX: Added DCOperatingPoint to the enum ---
        public enum AnalysisType { Transient, ACSweep, PhaseSweep, DCOperatingPoint }

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
        // NumberOfPoints is shared with AC Sweep
    }
}
