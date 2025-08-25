using System;
using System.Runtime.InteropServices;
using System.Text;

namespace wpfUI
{
    /// <summary>
    /// C# wrapper for the native C++ Circuit Simulator DLL
    /// </summary>
    public class CircuitSimulatorService : IDisposable
    {
        private const string DllName = "CircuitSimulator.dll";

        // --- Core Circuit Management ---
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreateCircuit();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void DestroyCircuit(IntPtr circuit);

        // --- Component Addition ---
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void AddNode(IntPtr circuit, string name);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void AddResistor(IntPtr circuit, string name, string node1, string node2, double value);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void AddVoltageSource(IntPtr circuit, string name, string node1, string node2, double voltage);

        // --- NEW ---
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void AddACVoltageSource(IntPtr circuit, string name, string node1, string node2, double magnitude, double phase);


        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void SetGroundNode(IntPtr circuit, string nodeName);

        // --- Analysis Execution ---
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool RunDCAnalysis(IntPtr circuit);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool RunTransientAnalysis(IntPtr circuit, double stepTime, double stopTime);

        // --- NEW ---
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern bool RunACAnalysis(IntPtr circuit, string sourceName, double startFreq, double stopFreq, int numPoints, string sweepType);


        // --- Result Retrieval ---
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern double GetNodeVoltage(IntPtr circuit, string nodeName);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int GetNodeNames(IntPtr circuit, [Out] IntPtr[] nodeNames, int maxCount);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int GetNodeVoltageHistory(IntPtr circuit, string nodeName,
            [Out] double[] timePoints, [Out] double[] voltages, int maxCount);
            
        // --- NEW ---
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int GetNodeSweepHistory(IntPtr circuit, string nodeName, 
            [Out] double[] frequencies, [Out] double[] magnitudes, int maxCount);


        private IntPtr circuitHandle;
        private bool disposed = false;

        public CircuitSimulatorService()
        {
            circuitHandle = CreateCircuit();
        }

        // --- Public Wrapper Methods ---

        public void AddNode(string name)
        {
            AddNode(circuitHandle, name);
        }

        public void AddResistor(string name, string node1, string node2, double value)
        {
            AddResistor(circuitHandle, name, node1, node2, value);
        }

        public void AddVoltageSource(string name, string node1, string node2, double voltage)
        {
            AddVoltageSource(circuitHandle, name, node1, node2, voltage);
        }
        
        // --- NEW ---
        public void AddACVoltageSource(string name, string node1, string node2, double magnitude, double phase)
        {
            AddACVoltageSource(circuitHandle, name, node1, node2, magnitude, phase);
        }

        public void SetGroundNode(string nodeName)
        {
            SetGroundNode(circuitHandle, nodeName);
        }

        public bool RunDCAnalysis()
        {
            return RunDCAnalysis(circuitHandle);
        }

        public bool RunTransientAnalysis(double stepTime, double stopTime)
        {
            return RunTransientAnalysis(circuitHandle, stepTime, stopTime);
        }

        // --- NEW ---
        public bool RunACAnalysis(string sourceName, double startFreq, double stopFreq, int numPoints, string sweepType = "Linear")
        {
            return RunACAnalysis(circuitHandle, sourceName, startFreq, stopFreq, numPoints, sweepType);
        }

        public double GetNodeVoltage(string nodeName)
        {
            return GetNodeVoltage(circuitHandle, nodeName);
        }

        public string[] GetNodeNames()
        {
            // This approach to string marshalling is simplified. For robust applications,
            // a more sophisticated method would be needed to handle memory management.
            const int maxNodes = 100;
            IntPtr[] nodeNamePtrs = new IntPtr[maxNodes];
            int count = GetNodeNames(circuitHandle, nodeNamePtrs, maxNodes);
            
            string[] nodeNames = new string[count];
            for (int i = 0; i < count; i++)
            {
                nodeNames[i] = Marshal.PtrToStringAnsi(nodeNamePtrs[i]);
                // In a real app, you might need to free the memory allocated by C++ if it's not static.
            }
            
            return nodeNames;
        }

        public Tuple<double[], double[]> GetNodeVoltageHistory(string nodeName)
        {
            const int maxPoints = 2000; // Assuming a max number of history points
            double[] timePoints = new double[maxPoints];
            double[] voltages = new double[maxPoints];
            
            int count = GetNodeVoltageHistory(circuitHandle, nodeName, timePoints, voltages, maxPoints);
            
            // Trim the arrays to the actual data size
            Array.Resize(ref timePoints, count);
            Array.Resize(ref voltages, count);
            
            return Tuple.Create(timePoints, voltages);
        }
        
        // --- NEW ---
        public Tuple<double[], double[]> GetNodeSweepHistory(string nodeName)
        {
            const int maxPoints = 2000; // Assuming a max number of sweep points
            double[] frequencies = new double[maxPoints];
            double[] magnitudes = new double[maxPoints];

            int count = GetNodeSweepHistory(circuitHandle, nodeName, frequencies, magnitudes, maxPoints);

            // Trim the arrays to the actual data size
            Array.Resize(ref frequencies, count);
            Array.Resize(ref magnitudes, count);

            return Tuple.Create(frequencies, magnitudes);
        }

        // --- IDisposable Implementation for proper cleanup ---
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
