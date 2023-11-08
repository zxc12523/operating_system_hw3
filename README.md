# operating_system_hw3

- CPU Scheduling
    - Scheduling of processes/work is done to finish the work on time. CPU Scheduling is a process that allows one process to use the CPU while another process is delayed (in standby) due to unavailability of any resources such as I / O etc, thus making full use of the CPU.
    
- Goal
    - Implemet context switch and serveral schduling algorithms
        - First Come First Serverd (FCFS)
        - Round Robin (RR)
        - Non-preemptive Shortest Job First (SJF)
        - Shortest Remaining Time First (Preemptive SJF)

## Launching
```shell
docker pull ntuos/mp3
docker run -it -v $(pwd)/xv6:/home/xv6-riscv/ ntuos/mp3 /bin/bash
```

## Testing
1. test all testcase

```shell
python3 grade-mp3
```

2. compile xv6 by running this command in docker
```shell
POLICY="THREAD_SCHEDULER_FCFS"
make qemu SCHEDPOLICY=${POLICY}
```