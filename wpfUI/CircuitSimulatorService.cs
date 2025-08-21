using System;
using System.Runtime.InteropServices;
using System.Text;

namespace wpfUI
{
    /// <summary>
    /// C# wrapper for the native C++ Circuit Simulator DLL
    /// </summary>
    public class CircuitSimulatorService
    {
        private const string DllName = "CircuitSimulator.dll";

        // DLL imports for the circuit simulator functions
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreateCircuit();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void DestroyCircuit(IntPtr circuit);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void AddNode(IntPtr circuit, string name);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void AddResistor(IntPtr circuit, string name, string node1, string node2, double value);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void AddVoltageSource(IntPtr circuit, string name, string node1, string node2, double voltage);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetGroundNode(IntPtr circuit, string nodeName);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool RunDCAnalysis(IntPtr circuit);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool RunTransientAnalysis(IntPtr circuit, double stepTime, double stopTime);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern double GetNodeVoltage(IntPtr circuit, string nodeName);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetNodeNames(IntPtr circuit, [Out] IntPtr[] nodeNames, int maxCount);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetNodeVoltageHistory(IntPtr circuit, string nodeName, 
            [Out] double[] timePoints, [Out] double[] voltages, int maxCount);

        private IntPtr circuitHandle;

        public CircuitSimulatorService()
        {
            circuitHandle = CreateCircuit();
        }

        ~CircuitSimulatorService()
        {
            if (circuitHandle != IntPtr.Zero)
            {
                DestroyCircuit(circuitHandle);
                circuitHandle = IntPtr.Zero;
            }
        }

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

        public double GetNodeVoltage(string nodeName)
        {
            return GetNodeVoltage(circuitHandle, nodeName);
        }

        public string[] GetNodeNames()
        {
            // For simplicity, we'll assume a maximum of 100 nodes
            IntPtr[] nodeNamesPtr = new IntPtr[100];
            int count = GetNodeNames(circuitHandle, nodeNamesPtr, 100);
            
            string[] nodeNames = new string[count];
            for (int i = 0; i < count; i++)
            {
                nodeNames[i] = Marshal.PtrToStringAnsi(nodeNamesPtr[i]);
            }
            
            return nodeNames;
        }

        public Tuple<double[], double[]> GetNodeVoltageHistory(string nodeName)
        {
            // For simplicity, we'll assume a maximum of 1000 history points
            double[] timePoints = new double[1000];
            double[] voltages = new double[1000];
            
            int count = GetNodeVoltageHistory(circuitHandle, nodeName, timePoints, voltages, 1000);
            
            // Return only the actual data
            double[] actualTimePoints = new double[count];
            double[] actualVoltages = new double[count];
            
            Array.Copy(timePoints, actualTimePoints, count);
            Array.Copy(voltages, actualVoltages, count);
            
            return Tuple.Create(actualTimePoints, actualVoltages);
        }
    }
}