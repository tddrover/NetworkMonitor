# Network Monitor

The NetworkMonitor module is designed to monitor network activity on a Linux system by tracking IP addresses and their occurrence counts. This module operates by intercepting network packets and recording information about the source IP addresses.

Includes:
- Monitors network activity in real-time.
- Tracks source IP addresses and their occurrence counts.
- Designed to integrate seamlessly with Linux kernel modules.


Screenshots: 

<div display="flex">
<img src="/pictures/Screenshot_1.png" alt="Image 1" width="50%" />
</div>


How to Run the App:
- Navigate to the directory with the files and compile the module with "make"
- Load the module into the kernel with "sudo insmod NetworkMonitor.ko"
- Once the module is loaded, it will start monitoring network activity. You can view the recorded IP addresses and their occurrence counts by checking the system logs or using tools like running "sudo dmesg"


Unload the Kernel Module
- Use "sudo rmmod NetworkMonitor" to remove the NetworkMonitor module from the kernel


Important Notes
- This module is designed for use on Linux systems.
- Ensure that you have the necessary permissions to load kernel modules (insmod requires root privileges).
- Use caution when loading out-of-tree kernel modules, as they may not be officially supported by your distribution.
