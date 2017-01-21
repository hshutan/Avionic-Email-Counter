using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using OutLook = Microsoft.Office.Interop.Outlook;
using System.IO.Ports;

namespace TGT_OutlookSerial
{
    public partial class Form1 : Form
    {

        int lastNumUnread = 0;
        int lastNumTotal = 0;
        bool shouldBeRunning = false;
        bool resetCalledFor = false;
        bool portFailed = false;
        string myPort = "";
        int failSafeCounter = 0;

        public Form1()
        {
            InitializeComponent();

            try
            {
                cboComPort.Items.Clear();
                string[] ports = SerialPort.GetPortNames();
                foreach (string port in ports)
                {
                    cboComPort.Items.Add(port);
                    cboComPort.Text = port;
                }

            }
            catch (Exception)
            {
                
            }
        }

        private void btnOpen_Click(object sender, EventArgs e)
        {
            try
            {
                btnReset.Enabled = true;
                btnOpen.Enabled = false;
                btnClose.Enabled = true;
                shouldBeRunning = true;
                serialPort1.PortName = cboComPort.Text;
                myPort = cboComPort.Text;
                serialPort1.BaudRate = 9600;
                serialPort1.Open();
                backgroundWorker1.RunWorkerAsync();
                backgroundWorker2.RunWorkerAsync();
            }
            catch (Exception)
            {
                lastNumUnread = -1;
                lastNumTotal = -1;
                // Error starting.
            }


        }


        private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {

            if (serialPort1.IsOpen)
            {
                try
                {
                    if (resetCalledFor)
                    {
                        serialPort1.WriteLine("r");
                        resetCalledFor = false;
                    }
                    else
                    {

                        // Connect to Microsoft Outlook

                        OutLook.Application oApp;
                        OutLook._NameSpace oNS;

                        oApp = new OutLook.Application();
                        oNS = (OutLook._NameSpace)oApp.GetNamespace("MAPI");

                        // Get the number of unread Inbox items
                        int numUnread = oNS.GetDefaultFolder(Microsoft.Office.Interop.Outlook.OlDefaultFolders.olFolderInbox).UnReadItemCount;
                        // Get the total number of Inbox items
                        int numTotal = oNS.GetDefaultFolder(Microsoft.Office.Interop.Outlook.OlDefaultFolders.olFolderInbox).Items.Count;

                        if (numUnread > 10)
                        {
                            numUnread = 10;
                        }

                        if (numTotal > 999)
                        {
                            numTotal = 999;
                        }

                        string myAPI = numUnread.ToString() + "," + numTotal.ToString() + ".";



                        if ((lastNumUnread != numUnread) || (lastNumTotal != numTotal))
                        {
                            // We need to update the display.
                            // Tell the device what to display
                            serialPort1.WriteLine(myAPI);
                            // Remember what we just sent, as to not re-send when numbers have not changed
                            lastNumUnread = numUnread;
                            lastNumTotal = numTotal;
                            // Prevent high CPU usage with delay
                            System.Threading.Thread.Sleep(1000);
                        }
                        else
                        {
                            // We don't need to update the display.
                            failSafeCounter++;
                            // Prevent high CPU usage with delay
                            System.Threading.Thread.Sleep(500);
                        }

                        if (failSafeCounter > 5)
                        {
                            // Cause program to re-send unread and total items every so often even when data has not changed
                            lastNumUnread = -1;
                            lastNumTotal = -1;
                            failSafeCounter = 0;
                        }
                    }
                }
                catch (Exception)
                {
                    System.Threading.Thread.Sleep(5000);
                    lastNumUnread = -1;
                    lastNumTotal = -1;
                }
            }
        }

        private void backgroundWorker1_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            if (shouldBeRunning)
            {
                backgroundWorker1.RunWorkerAsync();
            }
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            lastNumUnread = -1;
            lastNumTotal = -1;
            try
            {
                serialPort1.WriteLine("0,0.");
                serialPort1.WriteLine("b");
            }
            catch (Exception)
            {
            }
            try
            {
                btnReset.Enabled = false;
                btnOpen.Enabled = true;
                btnClose.Enabled = false;
                shouldBeRunning = false;
                serialPort1.Close();
            }
            catch (Exception)
            {
                
            }

        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            resetCalledFor = true;
        }

        private void backgroundWorker2_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {
                string[] ports = SerialPort.GetPortNames();
                if (!ports.Contains(myPort))
                {
                    portFailed = true;
                    shouldBeRunning = false;
                    lastNumUnread = -1;
                    lastNumTotal = -1;
                }
                if (ports.Contains(myPort))
                {
                    portFailed = false;
                    shouldBeRunning = true;
                }
            }
            catch (Exception)
            {
            }

            try
            {
                if (portFailed)
                {
                    lastNumUnread = -1;
                    lastNumTotal = -1;
                    System.Threading.Thread.Sleep(1000);
                    serialPort1.Open();
                }
            }
            catch (Exception)
            {
            }

            try
            {
                if (serialPort1.IsOpen)
                {
                    portFailed = false;
                    shouldBeRunning = true;
                }
            }
            catch (Exception)
            {
            }

            System.Threading.Thread.Sleep(1000);
        }

        private void backgroundWorker2_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            backgroundWorker2.RunWorkerAsync();
        }
    }
}
