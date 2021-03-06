# Requirements

Objective: Interprocess communication and basic coordination using message queue[^2]

* ProbeA
* ProbeB
* ProbeC
* DataHub

The Hub receives data from the 3 probes. __It outputs the `PID` of the probe and the data__ that was sent from its probe. The Hub needs to execute __without unnessessary blocking__.

## Software Requirements

### Behavior of Each Probe

Each probe produces its own reading[^4] to send to the DataHub, until the probe terminates. While each __probe sends the Hub its [valid reading](#valid-reading)__ (see below) it produces, each probe behaves differently: For each reading ProbeA sends to the Hub, it waits to __receive an acknowledgement__ (return message) from the Hub, before it continues to generate its next reading. ProbeB and ProbeC __must not receive any acknowledgement__ from the Hub.

### Valid Reading

A valid reading is defined as: *A random integer[^5] that is divisible by its magic_seed*[^6]. That is, any random integer which is not divisible by any magic_seed is discarded. The 3 magic_seeds (_α_, _β_, _ρ_) must satisfy this mathematical condition: _α_ > 3 * _β_ > _ρ_. ProbeA would use _α_ as its magic_seed, _β_ is used in ProbB and _ρ_ goes with ProbeC. Furthermore, each magic_seed needs to be chosen to be sufficiently large, depending on the speed of your CPU[^7]. You may try 997, 257, 251 initially, although there may require some experimentation in choosing them.

### Probe Termination

Each probe terminates differently. ProbeA terminates when it produces a random integer smaller than 100. After the Hub had received a total of 10,000 messages, the Hub forces ProbeB to exit. This requires using the __`force_patch` file__ provided by your instructor. Lastly, the user uses a `kill` command in a separate terminal to kill ProbeC. This can be accomplished with the __`kill_patch` file__ provided by your instructor. Finally, the DataHub terminates after all 3 probes had exited.

## Programming Notes

* [ ] Construct as 4 separate programs. Each program needs to execute with the maximum degree of concurrency without delay[^9].

  * [ ] Initialize a `magic_seed` from either hard-coding or user input.

* [ ] Design message `struct` and communication protocol.

* [ ] Assume lossless communication (?)

* [ ] State and document additional constraints and assumptions in design and implementation.

* [ ] There is no set order in which the applications execute

* [ ] All interprocess communications must be accomplished with a __single message queue__.

* [ ] Do not halt the program whatsoever[^10].

* [ ] Properly handle race conditions.

* [ ] Ideally each program executes in their own terminal/window.

## Deliverables

* [ ] Source code w/ documentation
* [ ] Cover page w/ description (`README.md`)
* [ ] Names of people in group, date, course #, email address (`README.md`)
* [ ] Live demo

## Sample

```bash
$ ipcs
------ Shared Memory Segments --------
key shmid owner perms bytes nattch status
------ Semaphore Arrays --------
key semid owner perms nsems
------ Message Queues --------
key msqid owner perms used-bytes messages

$
```

[^1]: This project can be completed after the lab lecture on the sample program on message queue.
[^4]: Simulated by a random integer value.
[^5]: Use the full range of random values generated by function rand(), i.e., do not truncate with a mod operator.
[^6]: Each probe is assigned its own magic_seed: alpha, beta or rho. It’s best to chose alpha, beta and rho to be prime numbers.
[^7]: When the magic_seed is too small, a fast CPU would fill the message queue instantly before the next probe gets a chance to execute.
[^9]: Always allowing the highest number of processes to execute concurrently.
[^10]: Typically present when the programmer uses the prohibited IPC_NOWAIT option.
