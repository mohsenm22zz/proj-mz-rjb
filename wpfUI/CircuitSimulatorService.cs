// mohsenm22zz/proj-mz-rjb/proj-mz-rjb-1b949d5aa204b9f590a1c5f0644f3424cf2a70ce/wpfUI/CircuitSimulatorService.cs

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace wpfUI
{
    /// <summary>
    /// C# wrapper for the native C++ Circuit Simulator DLL.
    /// Manages the lifecycle of the native circuit object and provides a safe interface.
    /// </summary>
    public class CircuitSimulatorService : IDisposable
    {
        private const string DllName = "CircuitSimulator.dll";

        // --- P/Invoke Signatures for the C++ DLL ---

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreateCircuit();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void DestroyCircuit(IntPtr circuit);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void AddNode(IntPtr circuit, string name);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void AddResistor(IntPtr circuit, string name, string node1, string node2, double value);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void AddVoltageSource(IntPtr circuit, string name, string node1, string node2, double voltage);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void AddACVoltageSource(IntPtr circuit, string name, string node1, string node2, double magnitude, double phase);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void SetGroundNode(IntPtr circuit, string nodeName);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool RunDCAnalysis(IntPtr circuit);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool RunTransientAnalysis(IntPtr circuit, double stepTime, double stopTime);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern bool RunACAnalysis(IntPtr circuit, string sourceName, double startFreq, double stopFreq, int numPoints, string sweepType);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern double GetNodeVoltage(IntPtr circuit, string nodeName);

        // --- FIX: Changed signature to receive a pre-allocated buffer for the names for safe string marshalling ---
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int GetNodeNames(IntPtr circuit, StringBuilder nodeNamesBuffer, int bufferSize);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int GetNodeVoltageHistory(IntPtr circuit, string nodeName, [Out] double[] timePoints, [Out] double[] voltages, int maxCount);
            
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int GetNodeSweepHistory(IntPtr circuit, string nodeName, [Out] double[] frequencies, [Out] double[] magnitudes, int maxCount);

        private IntPtr circuitHandle;
        private bool disposed = false;

        public CircuitSimulatorService()
        {
            circuitHandle = CreateCircuit();
        }
        
        // --- Public Wrapper Methods ---

        public void AddNode(string name) => AddNode(circuitHandle, name);
        public void AddResistor(string name, string node1, string node2, double value) => AddResistor(circuitHandle, name, node1, node2, value);
        public void AddVoltageSource(string name, string node1, string node2, double voltage) => AddVoltageSource(circuitHandle, name, node1, node2, voltage);
        public void AddACVoltageSource(string name, string node1, string node2, double magnitude, double phase) => AddACVoltageSource(circuitHandle, name, node1, node2, magnitude, phase);
        public void SetGroundNode(string nodeName) => SetGroundNode(circuitHandle, nodeName);
        public bool RunDCAnalysis() => RunDCAnalysis(circuitHandle);
        public bool RunTransientAnalysis(double stepTime, double stopTime) => RunTransientAnalysis(circuitHandle, stepTime, stopTime);
        public bool RunACAnalysis(string sourceName, double startFreq, double stopFreq, int numPoints, string sweepType = "Linear") => RunACAnalysis(circuitHandle, sourceName, startFreq, stopFreq, numPoints, sweepType);
        public double GetNodeVoltage(string nodeName) => GetNodeVoltage(circuitHandle, nodeName);

        // --- FIX: Implemented safe string marshalling by receiving a single comma-separated string ---
        public string[] GetNodeNames()
        {
            const int maxBufferSize = 2048; // A reasonable buffer size for all node names
            StringBuilder buffer = new StringBuilder(maxBufferSize);
            int length = GetNodeNames(circuitHandle, buffer, buffer.Capacity);

            if (length > 0)
            {
                // Split the comma-separated string into an array
                return buffer.ToString().Split(new[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
            }
            return new string[0]; // Return empty array if no nodes
        }

        public Tuple<double[], double[]> GetNodeVoltageHistory(string nodeName)
        {
            const int maxPoints = 2000; // Max points for a single simulation plot
            double[] timePoints = new double[maxPoints];
            double[] voltages = new double[maxPoints];
            int count = GetNodeVoltageHistory(circuitHandle, nodeName, timePoints, voltages, maxPoints);
            
            // Trim arrays to the actual number of points returned by the simulation
            Array.Resize(ref timePoints, count);
            Array.Resize(ref voltages, count);
            
            return Tuple.Create(timePoints, voltages);
        }

        public Tuple<double[], double[]> GetNodeSweepHistory(string nodeName)
        {
            const int maxPoints = 2000;
            double[] frequencies = new double[maxPoints];
            double[] magnitudes = new double[maxPoints];
            int count = GetNodeSweepHistory(circuitHandle, nodeName, frequencies, magnitudes, maxPoints);

            Array.Resize(ref frequencies, count);
            Array.Resize(ref magnitudes, count);

            return Tuple.Create(frequencies, magnitudes);
        }
        
        // --- IDisposable Implementation for proper native resource cleanup ---
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (circuitHandle != IntPtr.Zero)
                {
                    DestroyCircuit(circuitHandle);
                    circuitHandle = IntPtr.Zero;
                }
                disposed = true;
            }
        }

        ~CircuitSimulatorService()
        {
            Dispose(false);
        }
    }
}
